//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/rulebasedclassification.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class RuleBasedClassification.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

RuleBasedClassification::RuleBasedClassification(const RuleBasedClassification &in) : Classification(in) {
	rules_ = in.rules_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

RuleBasedClassification::RuleBasedClassification() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

RuleBasedClassification::~RuleBasedClassification() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleBasedClassification, RULEBASEDCLASSIFICATION, Classification)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
RuleBasedClassification::Clear() {
	Classification::Clear();
	rules_.erase(rules_.begin(), rules_.end());
}

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
RuleBasedClassification::Duplicate() const {
	return new RuleBasedClassification(*this);
}

//-------------------------------------------------------------------
// Methods inherited from Classification.
//===================================================================

//-------------------------------------------------------------------
// Method........: InsertDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleBasedClassification::InsertDecisionValue(int value, float certainty, int i) {

	// Insert stuff higher up.
	if (!Classification::InsertDecisionValue(value, certainty, i))
		return false;

	// Insert an empty rule vector.
  rules_.insert(rules_.begin() + i, Rule::Handles());

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleBasedClassification::RemoveDecisionValue(int i) {

	// Remove stuff higher up.
	if (!Classification::RemoveDecisionValue(i))
		return false;

	// Erase rule vector.
  rules_.erase(rules_.begin() + i);

	return true;

}

//-------------------------------------------------------------------
// Method........: SwapData
// Author........: Aleksander šhrn
// Date..........:
// Description...: Swaps decision value data, used during sorting.
// Comments......:
// Revisions.....:
//===================================================================

void
RuleBasedClassification::SwapData(int i, int j) {

	// Swap stuff higher up.
	Classification::SwapData(i, j);

	// Swap rule vectors.
	std::swap(rules_[i], rules_[j]);

}

//-------------------------------------------------------------------
// Method........: GetNoRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
RuleBasedClassification::GetNoRules(int i) const {
	return rules_[i].size();
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const Rule::Handles &
RuleBasedClassification::GetRules(int i) const {

#ifdef _DEBUG
 	// Index in range?
	if ((i < 0) || (i >= GetNoDecisionValues())) {
		Message::Error("Index out of range.");
	  static Rule::Handles undefined;
		return undefined;
	}
#endif

	return rules_[i];

}

//-------------------------------------------------------------------
// Method........: SetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleBasedClassification::SetRules(const Rule::Handles &rules, int i) {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoDecisionValues())) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	rules_[i] = rules;

	return true;

}
 
