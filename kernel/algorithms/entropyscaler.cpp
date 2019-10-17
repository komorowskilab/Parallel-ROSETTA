//-------------------------------------------------------------------
// Author........: Aleksander šhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/entropyscaler.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/set.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/mathkit.h>

#include <kernel/system/math.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticLog2
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the the base-2 logarithm.
// Comments......: Invokes an extra function call, but..
// Revisions.....:
//===================================================================

static float
StaticLog2(float x) {

	// Define a nice constant.
	const double log2 = 0.301029995664;

	return (log(x) / log2);

}

//-------------------------------------------------------------------
// Method........: StaticComputeProportion
// Author........: Aleksander šhrn/Knut Magne Risvik
// Date..........:
// Description...: Computes the proportions of objects within the
//                 range [begin, end) that belong to the given decision
//                 classes.
//
// Comments......: Made static so as to not obfuscate the header file
//                 and make it less readable. Consider making it a
//                 member method if any classes are to be derived from
//                 EntropyScaler.
// Revisions.....:
//===================================================================

static bool
StaticComputeProportions(const Vector(OrthogonalScaler::OVPair) &objects,             // Input:  The objects (indices) and their attribute values (sorted by the latter).
                         int                                     begin,               // Input:  Start index (included).
                         int                                     end,                 // Input:  End index (excluded).
                         const DecisionTable                    &table,               // Input:  The table the objects belong to.
                         int                                     decision_attribute,  // Input:  Index of decision attribute.
                         bool                                    masked,              // Input:  Operate on a masked table?
                         const Vector(int)                      &decision_classes,    // Input:  The distinct decision classes.
												 Vector(float)                          &proportions) {       // Output: The proportions (estimated probabilities).

	// Verify input.
	if (begin >= end)
		return false;

	// How many decision classes are there?
	int no_decision_classes = decision_classes.size();

	Vector(int) counts;

	// Initialize count vector.
	counts.reserve(no_decision_classes);

	int i, j;

	for (i = 0; i < no_decision_classes; i++)
		counts.push_back(0);

	int no_total = 0;

	// Iterate over given range, and count.
	for (i = begin; i < end; i++) {

		// Find out which decision class the current object belongs to.
		int   object_no      = objects[i].first;
		int   decision_class = table.GetEntry(object_no, decision_attribute, masked);

		// Aš: If the current object has an undefined value, skip it.
		int value = objects[i].second;

		if (value == Undefined::Integer())
			continue;

		// Increment the correct counter.
		for (j = 0; j < no_decision_classes; j++) {
			if (decision_class == decision_classes[j])
				counts[j]++;
			break;
		}

		no_total++;

	}

	// Initialize in-place output.
	proportions.erase(proportions.begin(), proportions.end());
	proportions.reserve(no_decision_classes);

	// Compute proportions.
	for (i = 0; i < no_decision_classes; i++)
		proportions.push_back(static_cast(float, counts[i]) / no_total);

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticComputeEntropy
// Author........: Aleksander šhrn/Knut Magne Risvik
// Date..........:
// Description...: Calculates the entropy of the range [begin, end).
// Comments......: Returns Undefined::Float() on error.
//
//                 Made static so as to not obfuscate the header file
//                 and make it less readable. Consider making it a
//                 member method if any classes are to be derived from
//                 EntropyScaler.
// Revisions.....:
//===================================================================

static float
StaticComputeEntropy(const Vector(OrthogonalScaler::OVPair) &objects,            // The objects (indices) and their attribute values (sorted by the latter).
                     int                                     begin,              // Start index (included).
                     int                                     end,                // End index (excluded).
                     const DecisionTable                    &table,              // The table the objects belong to.
                     int                                     decision_attribute, // Index of decision attribute.
                     bool                                    masked,             // Operate on a masked table?
                     const Vector(int)                      &decision_classes) { // The distinct decision classes.

	// How many decision classes are there?
	int no_decision_classes = decision_classes.size();

	Vector(float) proportions;

	// Compute proportions (probabilities).
	if (!StaticComputeProportions(objects, begin, end, table, decision_attribute, masked, decision_classes, proportions))
		return Undefined::Float();

	double sum = 0.0;

	int i;

	// Iterate over all decision classes.
	for (i = 0; i < no_decision_classes; i++) {
		float p = proportions[i];
		if (p != 0.0)
			sum += p * log(p);
	}

	// Define a nice constant.
	const double log2 = 0.301029995664;

	return (-sum / log2);

}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(EntropyScaler, ENTROPYSCALER, OrthogonalScaler)

//-------------------------------------------------------------------
// Methods for class EntropyScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

EntropyScaler::EntropyScaler() {
}

EntropyScaler::~EntropyScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from OrthogonalScaler
//===================================================================

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
EntropyScaler::FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const {

	// Clear cut vector.
	cuts.erase(cuts.begin(), cuts.end());

	// Determine table size.
	int no_objects = table.GetNoObjects(masked);

	// Determine decision attribute.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	Vector(int) decision_classes;
	Vector(int) frequencies; // Dummy, not in use.

	// Get value set for decision attribute.
	if (!table.GetValueSet(decision_classes, frequencies, decision_attribute, masked)) {
		//corrected 040823 Robin Andersson
		//return NULL;
		return false;
	}

	Vector(OVPair) objects;
	objects.reserve(no_objects);

	int i;

	// Fill vector. Skip missing values.
	for (i = 0; i < no_objects; i++) {
		int value = table.GetEntry(i, attribute_no, masked);
		if (value != Undefined::Integer())
			objects.push_back(std::make_pair(i, value));
	}

	OVPairCompareSecond comparator;

	// Sort the (index, value) pairs according to value.
	std::sort(objects.begin(), objects.end(), comparator);

	// Find cuts recursively.
	RecursiveFindCuts(objects, 0, objects.size(), table, decision_attribute, masked, decision_classes, cuts);

	return true;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: FindBestCut
// Author........: Aleksander šhrn/Knut Magne Risvik
// Date..........:
// Description...: Using the entropy criterion, finds the best cut
//                 within the given set of objects.
//
//                 Also splits the object set into objects that are
//                 left and right of the cut, computes entropies and
//                 number of distinct decisions in these sets.
//
// Comments......: Returns Undefined::Float() on error.
//
//                 Fayyad and Irani (1992) prove that, for convex
//                 splitting criteria such as information gain, it is
//                 not necessary to examine all thresholds (cuts).
//                 If all cases with value v[i] and with adjacent value
//                 v[i + 1] belong to the same class, a threshold (cut)
//                 between them cannot lead to a partition that has the
//                 optimum value of the criterion.
//
// Revisions.....:
//===================================================================

float
EntropyScaler::FindBestCut(const Vector(OVPair) &objects,                       // Input:  The objects (indices) and their attribute values (sorted by the latter).
                           int                   begin,                         // Input:  Start index (included).
                           int                   end,                           // Input:  End index (excluded).
                           const DecisionTable  &table,                         // Input:  The table the objects belong to.
                           int                   decision_attribute,            // Input:  Index of decision attribute.
                           bool                  masked,                        // Input:  Operate on a masked table?
                           const Vector(int)    &decision_classes,              // Input:  The distinct decision classes.
                           int                  &begin_l,                       // Output: Start index (included) of partition left of cut.
                           int                  &end_l,                         // Output: End index (excluded) of partition left of cut.
                           int                  &begin_r,                       // Output: Start index (included) of partition right of cut.
                           int                  &end_r,                         // Output: End index (excluded) of partition right of cut.
                           float                &entropy_t,                     // Output: Total entropy of cut.
                           float                &entropy_l,                     // Output: Entropy of partition left of cut.
                           float                &entropy_r,                     // Output: Entropy of partition right of cut.
                           int                  &no_decision_classes_l,         // Output: Number of distinct decision classes in partition left of cut.
                           int                  &no_decision_classes_r) const { // Output: Number of distinct decision classes in partition right of cut.

	// We need at least two values to find a cut.
	if (end - begin < 2)
		return Undefined::Float();

	entropy_t = FLT_MAX;

	// Set partial [begin, end) range indices for object sets left/right of cut.
	begin_l = begin;
	end_r   = end;

	float cut = Undefined::Float();

	int i;

	// Iterate over neighbour pairs.
	for (i = begin; i < end - 1; i++) {

#if 0
		// Get indices of neighbours.
		int object_l = objects[i].first;
		int object_r = objects[i + 1].first;
#endif
		
		// Get values of neighbours.
		int value_l = objects[i].second;
		int value_r = objects[i + 1].second;

		// Aš: If the values are the same, don't consider a cut between them.
		if (value_l == value_r)
			continue;

		Vector(int) decision_classes_l;
		Vector(int) decision_classes_r;
		Vector(int) dummy_l; // Currently not in use.
		Vector(int) dummy_r; // Currently not in use.

		// Do the neighbouring objects belong to homogenous decision classes wrt the current attribute?
		bool is_consistent_l = IsConsistent(objects, i, begin, end, table, decision_attribute, masked, decision_classes_l, dummy_l);
		bool is_consistent_r = IsConsistent(objects, i + 1, begin, end, table, decision_attribute, masked, decision_classes_r, dummy_r);

		// Aš: If the objects are homogeneous wrt the decision class and the decision classes are the same,
		// don't consider a cut between them.
		if (is_consistent_l && is_consistent_r && (decision_classes_l[0] == decision_classes_r[0]))
			continue;

		// Compute cardinalities to left/right.
		int cardinality_l = begin - i + 1;
		int cardinality_r = (end - begin) - cardinality_l;

		// Compute entropies to the left and right of the current value.
		float e_l = StaticComputeEntropy(objects, begin, i + 1, table, decision_attribute, masked, decision_classes);
		float e_r	= StaticComputeEntropy(objects, i + 1, end, table, decision_attribute, masked, decision_classes);

		// Compute total entropy.
		float factor_l = static_cast(float, cardinality_l) / (end - begin);
		float factor_r = static_cast(float, cardinality_r) / (end - begin);

		float e_t = factor_l * e_l + factor_r * e_r;

		// Remember best entropies, best cut, and split index.
 		if (e_t < entropy_t) {
			entropy_l = e_l;
			entropy_r = e_r;
			entropy_t = e_t;
			end_l     = i + 1;
			begin_r   = i + 1;
			cut       = 0.5f * (value_l + value_r);
		}

	}

	// Was no cut found?
	if (cut == Undefined::Float())
		return cut;

	STLSet(int) dummy_l;
	STLSet(int) dummy_r;

	// Count the number of distinct decision values in the left/right partitions.
	for (i = begin_l; i < end_l; i++) {
		int object_no = objects[i].first;
		dummy_l.insert(table.GetEntry(object_no, decision_attribute, masked));
	}

	for (i = begin_r; i < end_r; i++) {
		int object_no = objects[i].first;
		dummy_r.insert(table.GetEntry(object_no, decision_attribute, masked));
	}

	no_decision_classes_l = dummy_l.size();
	no_decision_classes_r = dummy_r.size();

	// Return the value of the best found cut.
	return cut;

}

//-------------------------------------------------------------------
// Method........: RecursiveFindCuts
// Author........: Knut Magne Risvik/Aleksander šhrn
// Date..........:
// Description...: Recursive entrypoint for finding cuts.
//
//                 Determines a cut, and if gain is good enough
//                 (according to the MDL-principle), recursively finds
//                 cuts to the left and right of the found cut.
//
// Comments......:
// Revisions.....:
//===================================================================

void
EntropyScaler::RecursiveFindCuts(const Vector(OVPair) &objects,            // Input:  The objects (indices) and their attribute values (sorted by the latter).
                                 int                   begin,              // Input:  Start index (included).
                                 int                   end,                // Input:  End index (excluded).
                                 const DecisionTable  &table,              // Input:  The table the objects belong to.
                                 int                   decision_attribute, // Input:  Index of decision attribute.
                                 bool                  masked,             // Input:  Operate on a masked table?
                                 const Vector(int)    &decision_classes,   // Input:  The distinct decision classes.
												         Cuts                 &cuts) const {       // Output: A vector of cut values.

	int   begin_l;               // Indices defining left partition, [begin, end).
	int   end_l;                 //
	int   begin_r;               // Indices defining right partition, [begin, end).
	int   end_r;                 //
	float entropy_t;             // Total entropy of cut.
	float entropy_l;             // Entropy of left partition.
	float entropy_r;             // Entropy of right partition.
	int   no_decision_classes_l; // Number of distinct decision classes in left partition.
	int   no_decision_classes_r; // Number of distinct decision classes in right partition.

	// Find best cut.
	float cut = FindBestCut(objects, begin, end, table, decision_attribute, masked, decision_classes,
		                      begin_l, end_l, begin_r, end_r, entropy_t, entropy_l, entropy_r,
										      no_decision_classes_l, no_decision_classes_r);

	// No cut found?
	if (cut == Undefined::Float())
		return;

	// Compute entropy of input range.
	float entropy = StaticComputeEntropy(objects, begin, end, table, decision_attribute, masked, decision_classes);

	// Compute entropy gain of cut.
	float gain = entropy - entropy_t;

	// Calculate MDL stopping criterion threshold.
	int		 k_t		   = decision_classes.size();
	int		 k_l		   = no_decision_classes_l;
	int		 k_r		   = no_decision_classes_r;
	float  delta     = StaticLog2(MathKit::Power(3.0, k_t) - 2.0) - (k_t * entropy - k_l * entropy_l - k_r * entropy_r);
	int    n         = end - begin;
	float  threshold = (StaticLog2(n - 1)  + delta) / n;

	// Gain not good enough?
	if (gain <= threshold)
		return;

	// Call left side recursively.
	if (end_l - begin_l > 1)
		RecursiveFindCuts(objects, begin_l, end_l, table, decision_attribute, masked, decision_classes, cuts);

	// Store found cut.
	cuts.push_back(cut);

	// Call right side recursively.
	if (end_r - begin_r > 1)
		RecursiveFindCuts(objects, begin_r, end_r, table, decision_attribute, masked, decision_classes, cuts);

}

EntropyScaler *
EntropyScaler::Clone() {
	return new EntropyScaler;
}
