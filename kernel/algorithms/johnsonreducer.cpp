//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/johnsonreducer.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/discernibilitymatrix.h>

#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Static helpers.
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticGetMaximum
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns index with highest weight.
// Comments......: In case of ties, returns the lowest index.
// Revisions.....:
//===================================================================

static int
StaticGetMaximum(const Vector(float) &weights) {

	float maximum_w = weights[0];
	int   maximum_i = 0, i;

	for (i = 1; i < weights.size(); i++) {
		if (weights[i] > maximum_w) {
			maximum_w = weights[i];
	    maximum_i = i;
		}
	}

	return maximum_i;

}

//-------------------------------------------------------------------
// Methods for class JohnsonReducer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

JohnsonReducer::JohnsonReducer() {
	ApproximateSolutions(true);
	SetHittingFraction(0.95f);
}

JohnsonReducer::~JohnsonReducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(JohnsonReducer, JOHNSONREDUCER, Reducer)

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
JohnsonReducer::GetParameters() const {

	String parameters = Reducer::GetParameters() + Keyword::Separator();

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
JohnsonReducer::SetParameter(const String &keyword, const String &value) {

	if (Reducer::SetParameter(keyword, value))
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
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
JohnsonReducer::Apply(Structure &structure) const {
	return Reducer::Apply(structure);
}


//-------------------------------------------------------------------
// Methods inherited from Reducer.
//===================================================================

//-------------------------------------------------------------------
// Method........: CreateFunction
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Possible bug: If the matrix is precomputed and
//                 the simplify flag is set to false, then it has been
//                 observed that the computed attribute set is
//                 a superset of the attribute set computed with the
//                 simplify flag set to true. Intuitively, they should
//                 be minimal either way.
//
//                 See also other CreateFunction method.
//
// Revisions.....: Aš 990226: Added PrecomputeMatrix() stuff.
//                 Aš 990726: Moved weight stuff to BooleanFunction.
//                 Aš 990728: Set simplify flag to true.
//===================================================================

bool
JohnsonReducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const {

	bool  copy      = false;
	bool  reduce    = true;
	bool  simplify  = true;
	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();
	bool  weighted  = ApproximateSolutions();

	const GeneralizedDecision::Handles *decisions = NULL;

	if (PrecomputeMatrix())
		return function.Create(matrix, copy, reduce, simplify, weighted);
	else
		return function.CreateSimplified(table, masked, ModuloDecision(), discerner, decisions, precision, &matrix, weighted);

}

//-------------------------------------------------------------------
// Method........: CreateFunction
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: PrecomputeMatrix() stuff is assumed done in
//                 Apply method.
//
//                 See also other CreateFunction method.
//
// Revisions.....: Aš 990226: Added PrecomputeMatrix() stuff.
//                 Aš 990726: Moved weight stuff to BooleanFunction.
//                 Aš 990728: Set simplify flag to true.
//===================================================================

bool
JohnsonReducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const {

	bool  copy      = false;
	bool  reduce    = true;
	bool  simplify  = true;
	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();
	bool  weighted  = ApproximateSolutions();

	if (PrecomputeMatrix())
		return function.Create(matrix, object_no, copy, reduce, simplify, weighted);
	else
		return function.CreateSimplified(table, object_no, masked, ModuloDecision(), discerner, decisions, precision, &matrix, weighted);

}

//-------------------------------------------------------------------
// Method........: ComputePrimeImplicants
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Should be possible to optimize this using
//                 priority queues or something like that.
// Revisions.....: Aš 990726: Moved weight stuff to BooleanFunction.
//                 Aš 990728: Simplified somewhat.
//===================================================================

bool
JohnsonReducer::ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const {

	int i, j;

	int no_sums       = function.GetNoSums();
	int no_attributes = function.GetArity(false);

	primeimplicants.RemoveAllProducts();

	// Empty function?
	if (no_sums == 0)
		return true;

	Vector(float) attributeweights; attributeweights.reserve(no_attributes); // For each attribute, contains accumulated weights of the uncovered sums the attribute occurs in.
	Vector(int)   uncovered;        uncovered.reserve(no_sums);              // Contains indices of yet uncovered sums.

	float totalweight = 0.0;

	// Initialize working structures.
	for (i = 0; i < no_sums; i++) {
		uncovered.push_back(i);
		float weight = function.GetWeight(i);
		if (weight <= 0.0) {
			Message::Error("One or more of the function's sums have non-positive weights.");
			return false;
		}
		totalweight += weight;
	}

	for (j = 0; j < no_attributes; j++)
		attributeweights.push_back(0.0);

	float requiredweight = ApproximateSolutions() ? (totalweight * GetHittingFraction()) : (totalweight);
	float coveredweight  = 0.0;

	Bits  primeimplicant(no_attributes, false);

	// Cover all or "enough" sums.
	do {

		// Reset accumulated weights per attribute.
		if (coveredweight > 0.0) {
			for (j = 0; j < no_attributes; j++)
				attributeweights[j] = 0.0;
		}

#ifdef _DEBUG
		Message::Debug(String::Format(uncovered.size()) + " still uncovered.");
#endif

		// Recompute accumulated weights.
		for (i = uncovered.size() - 1; i >= 0; i--) {

			// Get basic data.
			int         index  = uncovered[i];
			const Bits *sum    = function.GetSum(index);
			float       weight = function.GetWeight(index);

			// Accumulate.
			for (j = 0; j < no_attributes; j++) {
				if (sum->GetState(j))
					attributeweights[j] += weight;
			}

		}

#ifdef _DEBUG
		for (j = 0; j < no_attributes; j++)
			Message::Debug("w[" + String::Format(j) + "] = " + String::Format(attributeweights[j]));
#endif

		// Get attribute with the largest weight.
		int attribute = StaticGetMaximum(attributeweights);

#ifdef _DEBUG
		Message::Debug("Selected w[" + String::Format(attribute) + "] = " + String::Format(attributeweights[attribute]));
#endif

		// Add this to the Johnson reduct.
		primeimplicant.SetState(attribute, true);

		int last = uncovered.size() - 1;

		// Remove all newly covered sums.
		for (i = last; i >= 0; i--) {

			// Get basic data.
			int         index  = uncovered[i];
			const Bits *sum    = function.GetSum(index);
			float       weight = function.GetWeight(index);

			// Skip still uncovered sums.
			if (!sum->GetState(attribute))
				continue;

			// Update accumulated weight of covered sums.
			coveredweight += weight;

			// Move tail upwards to current position.
			uncovered[i] = uncovered[last--];

		}

		// Remove moved indices at tail
		uncovered.erase(uncovered.begin() + last + 1, uncovered.end());

		// Are "enough" sums covered?
		if (coveredweight >= requiredweight)
			break;

	} while (!uncovered.empty());

	// Prepare output.
	primeimplicants.AppendProduct(&primeimplicant, true, 1.0);

	supports.erase(supports.begin(), supports.end());
	supports.reserve(1);
	supports.push_back(MathKit::Round(100 * static_cast(float, coveredweight) / totalweight));

	return true;

}

JohnsonReducer *
JohnsonReducer::Clone() {
	return new JohnsonReducer;
}
