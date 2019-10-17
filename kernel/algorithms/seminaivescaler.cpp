//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/seminaivescaler.h>

#include <kernel/basic/algorithm.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticIsIncluded
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if all elements of vector a are also
//                 present in vector b.
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticIsIncluded(const Vector(int) &a, const Vector(int) &b) {

	int size_a = a.size();
	int size_b = b.size();

	if (size_a > size_b)
		return false;

	int i, j;

	for (i = 0; i < size_a; i++) {
		for (j = 0; j < size_b; j++) {
			if (b[j] == a[i])
				break;
		}
		if (j == size_b)
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Methods for class SemiNaiveScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

SemiNaiveScaler::SemiNaiveScaler() {
}

SemiNaiveScaler::~SemiNaiveScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(SemiNaiveScaler, SEMINAIVESCALER, NaiveScaler)

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
SemiNaiveScaler::FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const {
	return NaiveScaler::FindCuts(table, attribute_no, masked, cuts);
}

//-------------------------------------------------------------------
// Methods inherited from NaiveScaler.
//===================================================================

//-------------------------------------------------------------------
// Method........: SkipCut
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if a cut between objects (i, i + 1)
//                 should be skipped.
// Comments......:
// Revisions.....:
//===================================================================

bool
SemiNaiveScaler::SkipCut(const Vector(OVPair) &/*objects*/, int /*i*/, const Vector(int) &decisions_l, const Vector(int) &cardinalities_l, const Vector(int) &decisions_r, const Vector(int) &cardinalities_r) const {

	int highest_cardinality_l = 0;
	int highest_cardinality_r = 0;

	int j;

	// Determine the highest cardinalities.
	for (j = 0; j < cardinalities_l.size(); j++)
		highest_cardinality_l = std::max(cardinalities_l[j], highest_cardinality_l);

	for (j = 0; j < cardinalities_r.size(); j++)
		highest_cardinality_r = std::max(cardinalities_r[j], highest_cardinality_r);

	Vector(int) dominating_decisions_l; dominating_decisions_l.reserve(decisions_l.size());
	Vector(int) dominating_decisions_r; dominating_decisions_r.reserve(decisions_r.size());

	// Determine the dominating decisions (several in case of ties).
	for (j = 0; j < cardinalities_l.size(); j++) {
		if (cardinalities_l[j] == highest_cardinality_l)
			dominating_decisions_l.push_back(decisions_l[j]);
	}

	for (j = 0; j < cardinalities_r.size(); j++) {
		if (cardinalities_r[j] == highest_cardinality_r)
			dominating_decisions_r.push_back(decisions_r[j]);
	}

	// Aš: Don't add a cut if the sets of dominating decisions define an inclusion.
	if (StaticIsIncluded(dominating_decisions_l, dominating_decisions_r) || StaticIsIncluded(dominating_decisions_r, dominating_decisions_l))
		return true;

	return false;

}
 
