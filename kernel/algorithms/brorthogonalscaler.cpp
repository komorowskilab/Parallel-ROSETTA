//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/brorthogonalscaler.h>
#include <kernel/algorithms/naivescaler.h>
#include <kernel/algorithms/johnsonreducer.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/discernibilityfunction.h>
#include <kernel/structures/booleansopfunction.h>

#include <kernel/basic/set.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticCopySetToVector
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticCopySetToVector(const STLSet(int) &s, Vector(int) &v) {

	v.erase(v.begin(), v.end());
	v.reserve(s.size());

	STLSet(int)::const_iterator i = s.begin();

	while (!(i == s.end())) {
		v.push_back(*i);
		i++;
	}

	return true;

}

//-------------------------------------------------------------------
// Methods for class BROrthogonalScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

BROrthogonalScaler::BROrthogonalScaler() {
	ApproximateSolutions(false);
	SetHittingFraction(0.95f);
}

BROrthogonalScaler::~BROrthogonalScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BROrthogonalScaler, BRORTHOGONALSCALER, OrthogonalScaler)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
BROrthogonalScaler::GetParameters() const {

	String parameters = OrthogonalScaler::GetParameters() + Keyword::Separator();

	// Approximate solutions.
	parameters += Keyword::Approximate();
	parameters += Keyword::Assignment();
	parameters += String::Format(ApproximateSolutions());

	if (ApproximateSolutions()) {

		// Hitting fraction.
		parameters += Keyword::Separator();
		parameters += Keyword::Fraction();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetHittingFraction());

	}

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::SetParameter(const String &keyword, const String &value) {

	if (OrthogonalScaler::SetParameter(keyword, value))
		return true;

	// Approximate solutions.
	if (keyword == Keyword::Approximate() && value.IsBoolean())
		return ApproximateSolutions(value.GetBoolean());

	// Hitting fraction.
	if (keyword == Keyword::Fraction() && value.IsFloat())
		return SetHittingFraction(value.GetFloat());

	return false;

}

//-------------------------------------------------------------------
// Methods inherited from OrthogonalScaler.
//===================================================================

//-------------------------------------------------------------------
// Method........: Discretize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the cuts and prepares the stage for
//                 calls to the FindCuts method from the
//                 OrthogonalScaler::Discretize method.
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable *
BROrthogonalScaler::Discretize(DecisionTable &table) const {

	Message message;

	message.Notify("Computing candidate cuts and regions...");

	Vector(ACPair) candidates;
	Vector(Region) regions;

	// Find all candidate cuts.
	if (!FindCuts(table, candidates, regions))
		return NULL;

#ifdef _DEBUG
	Message::Debug("Found " + String::Format(candidates.size()) + " candidates.");
	Message::Debug("Found " + String::Format(regions.size()) + " regions.");
#endif

	message.Notify("Building discernibility function...");

	DiscernibilityFunction function;

	// Build a suitable discernibility function.
	if (!BuildFunction(table, candidates, regions, function))
		return NULL;

#ifdef _DEBUG
	Message::Debug("Function has " + String::Format(function.GetNoSums()) + " sums.");
#endif

	message.Notify("Computing prime implicant...");

	BooleanSOPFunction primeimplicant;

	// Compute the "optimal" candidate subset.
	if (!ComputePrimeImplicant(function, primeimplicant))
		return NULL;

	message.Notify("Filtering candidate set...");

	// This method is conceptually const only.
	BROrthogonalScaler *self = const_cast(BROrthogonalScaler *, this);

	// Select the winner cuts from the set of candidates.
	if (!self->FilterCuts(table, candidates, primeimplicant))
		return NULL;

	// Do the actual discretization.
	return OrthogonalScaler::Discretize(table);

}

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the cuts for the specified attribute.
//                 The cuts were computed in the Discretize method.
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::FindCuts(const DecisionTable &/*table*/, int attribute_no, bool masked, Cuts &cuts) const {

	// Masking has been assumed.
	if (!masked)
		return false;

#ifdef _DEBUG
	if (attribute_no < 0 || attribute_no >= cuts_.size()) {
		Message::Error("Index passed to FindCuts method outside allowed range.");
		return false;
	}
#endif

	cuts = cuts_[attribute_no];

	return true;

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes and returns (in-place) all candidate cuts
//                 in a flattened list of (attribute, cut) pairs.
//
//                 Also returns (in-place) a set of representative
//                 objects per region, along with the regions'
//                 (generalized) decision values.
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::FindCuts(const DecisionTable &table, Vector(ACPair) &candidates, Vector(Region) &regions) const {

	int i, j;

	// Operate on a masked table.
	bool masked = true;

	int no_attributes = table.GetNoAttributes(masked);
	int no_objects    = table.GetNoObjects(masked);

	// Initialize candidate container. Make a crude guess.
	candidates.erase(candidates.begin(), candidates.end());
	candidates.reserve(no_attributes * 50);

	NaiveScaler scaler;

	Cuts        cuts;              // Contains cuts for an attribute.
	Vector(int) labels;            // Contains region labels per object for an attribute.

	Vector(Vector(int)) cartesian; // Contains all region labels per object.
	cartesian.reserve(no_objects);

	// Initialize container.
	for (i = 0; i < no_objects; i++) {
		cartesian.push_back(Vector(int)());
		cartesian[i].reserve(no_attributes - 1);
	}

	for (i = 0; i < no_attributes; i++) {

		// Don't consider cuts for the decision attribute.
		if (table.IsDecision(i, masked))
			continue;

		// Find cuts for current attribute.
		if (!scaler.FindCuts(table, i, masked, cuts, &labels, NULL))
			return false;

		for (j = 0; j < cuts.size(); j++)
			candidates.push_back(std::make_pair(i, cuts[j]));

		for (j = 0; j < labels.size(); j++)
			cartesian[j].push_back(labels[j]);

	}

	// Find one representative object per region.
	return FindRegions(table, cartesian, regions);

}

//-------------------------------------------------------------------
// Method........: FindRegions
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes and returns (in-place) a vector
//                 of regions, defined through the cartesian vector.
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::FindRegions(const DecisionTable &table, const Vector(Vector(int)) &cartesian, Vector(Region) &regions) const {

	regions.erase(regions.begin(), regions.end());
	regions.reserve(cartesian.size());

	// Operate on a masked table.
	bool masked = true;

	// Get table information.
	int no_objects         = table.GetNoObjects(masked);
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	if (no_objects == 0)
		return true;

#ifdef _DEBUG
	if (cartesian.size() != no_objects) {
		Message::Error("Vector of wrong dimension passed to FindRegions method.");
		return false;
	}
#endif

	Vector(int) indices;
	indices.reserve(cartesian.size());

	int i;

	// Initialize index vector to sort.
	for (i = 0; i < no_objects; i++)
		indices.push_back(i);

	Compare compare(&cartesian);

	// Sort index vector.
	std::sort(indices.begin(), indices.end(), compare);

	STLSet(int) generalized;
	int         cardinality = 0;

	// Scan the set of sorted labels and update the region vector.
	for (i = 0; i < indices.size(); i++) {
		generalized.insert(table.GetEntry(indices[i], decision_attribute, masked));
		cardinality++;
		if ((i == indices.size() - 1) || (cartesian[indices[i]] != cartesian[indices[i + 1]])) {
			regions.push_back(Region());
			regions[regions.size() - 1].object_ = indices[i];
			regions[regions.size() - 1].cardinality_ = cardinality;
			StaticCopySetToVector(generalized, regions[regions.size() - 1].decisions_);
			generalized.erase(generalized.begin(), generalized.end());
			cardinality = 0;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: BuildFunction
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a decision table and a set of candidate
//                 cuts, builds a discernibility function that expresses
//                 how the discernibility of the system can be preserved
//                 using the candidate cuts.
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::BuildFunction(const DecisionTable &table, const Vector(ACPair) &candidates, const Vector(Region) &regions, DiscernibilityFunction &function) const {

	// Operate on a masked table.
	bool masked = true;

	int no_regions = regions.size();

	// Compute dimensions of function.
	int no_sums_max  = ((no_regions * (no_regions - 1)) / 2); // Maximally, one Boolean sum per region pair.
	int no_variables = candidates.size();                     // One Boolean variable per cut.

	function.RemoveAllSums();
	function.Reserve(no_sums_max);

	Bits sum(no_variables, false);

	int i, j, k;

	// Process all object (region) pairs and build function.
	for (i = 0; i < no_regions; i++) {
		for (j = i + 1; j < no_regions; j++) {

			// Don't discern between regions with the same decisions.
			if (regions[i].decisions_ == regions[j].decisions_)
				continue;

			// Determine which cuts that discern between the regions.
			for (k = 0; k < no_variables; k++) {
				int value_i   = table.GetEntry(regions[i].object_, candidates[k].first, masked);
				int value_j   = table.GetEntry(regions[j].object_, candidates[k].first, masked);
				float cut     = candidates[k].second;
				bool  between = (std::min(value_i, value_j) <= cut) && (cut < std::max(value_i, value_j));
				sum.SetState(k, between);
			}

			// Associate a weight with this sum.
			float weight = regions[i].cardinality_ * regions[j].cardinality_;

			// Append sum to function.
			function.AppendSum(&sum, true, weight);

		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ComputePrimeImplicant
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a Boolean function which expresses how a set
//                 of candidate cuts maintains discernibility in a
//                 decision system, computes a set of "optimal" cuts
//                 (encoded as Boolean variables).
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::ComputePrimeImplicant(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicant) const {

	JohnsonReducer reducer;

	reducer.ApproximateSolutions(approximate_);
	reducer.SetHittingFraction(hitting_fraction_);

	Vector(int) supports;

	return reducer.ComputePrimeImplicants(function, primeimplicant, supports);

}

//-------------------------------------------------------------------
// Method........: FilterCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Initializes the vector of cuts per attribute with
//                 the "optimal" cuts computed from the set of cut
//                 candidates.
// Comments......:
// Revisions.....:
//===================================================================

bool
BROrthogonalScaler::FilterCuts(const DecisionTable &table, const Vector(ACPair) &candidates, const BooleanSOPFunction &primeimplicant) {

	if (primeimplicant.GetNoProducts() != 1) {
		Message::Error("Exactly one prime implicant expected passed to FilterCuts method.");
		return false;
	}

	// Operate on a masked table.
	bool masked = true;

	int no_attributes = table.GetNoAttributes(masked);

	cuts_.erase(cuts_.begin(), cuts_.end());
	cuts_.reserve(no_attributes);

	int i;

	for (i = 0; i < no_attributes; i++) {
		cuts_.push_back(Cuts());
		cuts_[i].reserve(10);
	}

	int no_candidates = candidates.size();

#ifdef _DEBUG
	if (no_candidates != primeimplicant.GetArity(false)) {
		Message::Error("Dimension mismatch encountered in FilterCandidates method.");
		return false;
	}
#endif

	const Bits *optimal = primeimplicant.GetProduct(0);

	for (i = 0; i < no_candidates; i++) {
		if (optimal->GetState(i)) {
			int   a = candidates[i].first;
			float c = candidates[i].second;
			cuts_[a].push_back(c);
		}
	}

	return true;

}
 
BROrthogonalScaler *
BROrthogonalScaler::Clone() {
	return new BROrthogonalScaler;
}
