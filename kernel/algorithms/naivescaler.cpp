//-------------------------------------------------------------------
// Author........: Aleksander šhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/naivescaler.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class NaiveScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

NaiveScaler::NaiveScaler() {
}

NaiveScaler::~NaiveScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(NaiveScaler, NAIVESCALER, OrthogonalScaler)

//-------------------------------------------------------------------
// Methods inherited from OrthogonalScaler.
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
NaiveScaler::FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const {
	return FindCuts(table, attribute_no, masked, cuts, NULL, NULL);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SkipCut
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if a cut between objects (i, i + 1)
//                 should be skipped.
//
//                 Can be overloaded to cater for more advanced
//                 handling than a straight-forward approach.
// Comments......:
// Revisions.....:
//===================================================================

bool
NaiveScaler::SkipCut(const Vector(OVPair) &/*objects*/, int /*i*/, const Vector(int) &/*decisions_l*/, const Vector(int) &/*cardinalities_l*/, const Vector(int) &/*decisions_r*/, const Vector(int) &/*cardinalities_r*/) const {
	return false;
}

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Scans the objects (sorted by attribute value) from
//                 left to right in a single pass, and adds a cut
//                 between values for which the (generalized) decision
//                 for the objects to the immediate left and right of
//                 the cut differ.
//
// Comments......: The straight-forward naive approach may generate
//                 very many cuts, perhaps too many. In some cases it may
//                 be desirable to introduce inconsistencies.
//
// Revisions.....: Aš 0897 - Ignore missing entries.
//                 Aš 1297 - Moved "fancy" handling of inconsistent
//                           cases to SemiNaiveScaler.
//                 Aš 1098 - Introduced region stuff.
//===================================================================

bool
NaiveScaler::FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts, Vector(int) *regions, int *no_regions) const {

	// Clear vectors.
	cuts.erase(cuts.begin(), cuts.end());

	if (regions != NULL)
		regions->erase(regions->begin(), regions->end());

	// Determine table size.
	int no_objects = table.GetNoObjects(masked);

	// Determine decision attribute.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	Vector(OVPair) objects;
	objects.reserve(no_objects);

	int i;

	// Fill vectors. Skip missing values.
	for (i = 0; i < no_objects; i++) {
		int value = table.GetEntry(i, attribute_no, masked);
		if (value != Undefined::Integer())
			objects.push_back(std::make_pair(i, value));
	}

	if (regions != NULL) {
		regions->reserve(no_objects);
		for (i = 0; i < no_objects; i++)
			regions->push_back(Undefined::Integer());
	}

	OVPairCompareSecond comparator;

	// Sort the (object, value) pairs according to value.
	std::sort(objects.begin(), objects.end(), comparator);

	int region = 0;

	// Iterate over all (object, value) pairs.
	for (i = 0; i < objects.size() - 1; i++) {

		int object_l = objects[i].first;
		int value_l  = objects[i].second;
		int object_r = objects[i + 1].first;
		int value_r  = objects[i + 1].second;

		// Set region label of current object.
		if (regions != NULL)
			(*regions)[object_l] = region;

		// Aš: Don't add a cut if the neighbouring objects have the same value.
		if (value_l == value_r)
			continue;

		Vector(int) decisions_l;     // Decisions of eq. class to the immediate left of the potential cut.
		Vector(int) decisions_r;     // Decisions of eq. class to the immediate right of the potential cut.
		Vector(int) cardinalities_l; // Cardinalities of the respective decisions defined above. Currently not in use.
		Vector(int) cardinalities_r; // Cardinalities of the respective decisions defined above. Currently not in use.

		// Gather information about the eq. classes (wrt the current attribute and value) of the neighbouring objects.
		bool is_consistent_l = IsConsistent(objects, i, 0, objects.size(), table, decision_attribute, masked, decisions_l, cardinalities_l);
		bool is_consistent_r = IsConsistent(objects, i + 1, 0, objects.size(), table, decision_attribute, masked, decisions_r, cardinalities_r);

		// Aš: Don't add a cut if the the eq. classes (wrt the current attribute and value) of the
		// neighbouring objects are both homogenous and have the same decision.
		if (is_consistent_l && is_consistent_r && decisions_l[0] == decisions_r[0])
			continue;

		// Aš: Can be overloaded to leave room for more advanced handling.
		if (SkipCut(objects, i, decisions_l, cardinalities_l, decisions_r, cardinalities_r))
			continue;

		// Add a cut between the current neighbours.
		cuts.push_back(0.5f * (value_l + value_r));

		// Increase region label.
		region++;

	}

	// Make sure the rightmost object gets assigned a region label, too.
	if (regions != NULL)
		(*regions)[objects[objects.size() - 1].first] = region;

	if (no_regions != NULL)
		*no_regions = region + 1;

	return true;

}

NaiveScaler *
NaiveScaler::Clone() {
	return new NaiveScaler;
}
