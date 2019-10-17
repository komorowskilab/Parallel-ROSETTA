//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/objecttrackingvoter.h>

#include <kernel/structures/informationvector.h>
#include <kernel/structures/rulebasedclassification.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class ObjectTrackingVoter.
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

ObjectTrackingVoter::ObjectTrackingVoter() {
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ObjectTrackingVoter::~ObjectTrackingVoter() {
	Reset();
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ObjectTrackingVoter, OBJECTTRACKINGVOTER, Voter)

//-------------------------------------------------------------------
// Methods inherited from Classifier.
//===================================================================

//-------------------------------------------------------------------
// Method........: Initialize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the rules-to-objects mapping.
// Comments......:
// Revisions.....: Aš 990727: Added IDG stuff.
//===================================================================

bool
ObjectTrackingVoter::Initialize(const DecisionTable &/*table*/, bool /*masked*/) {

	if (GetRules() == NULL) {
		Message::Debug("Cannot classify, no classifying rules set.");
		return false;
	}

	int no_rules = GetRules()->GetNoRules();

	if (no_rules == 0) {
		Message::Warning("Empty rule set.", false);
		return false;
	}

	// Trace back to originating decision table.
  Handle<DecisionTable> parent = dynamic_cast(DecisionTable *, GetRules()->FindParent(DECISIONTABLE));

	if (parent.IsNULL()) {
		Message::Error("Unable to trace back to the rules' originating decision table.", false);
		return false;
	}

	bool masked = true;

	Message   message;
	Discerner discerner;

	// Initialize discerner.
	if (UseIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *parent, masked)) {
			Message::Error("Failed to load IDGs.");
			return false;
		}
	}

	message.Notify("Tracking back to originating objects...");

	tracked_.erase(tracked_.begin(), tracked_.end());
	tracked_.reserve(no_rules);

	int i, j, no_objects = parent->GetNoObjects(true);

	// Initialize mapping.
	for (i = 0; i < no_rules; i++)
		tracked_.push_back(new Bits(no_objects, false));

	Handle<InformationVector> inf = Creator::InformationVector();

	float fraction = 0.0;

	// Build tracking map.
	for (i = 0; i < no_objects; i++) {
		if (!inf->Create(*parent, i, true))
			return false;
		for (j = 0; j < no_rules; j++) {
			if (GetRules()->GetRule(j)->Matches(*inf, discerner, fraction))
				tracked_[j]->SetState(i, true);
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Reset
// Author........: Aleksander šhrn
// Date..........:
// Description...: Deletes the rules-to-objects mapping.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectTrackingVoter::Reset() {

	int i;

	for (i = 0; i < tracked_.size(); i++) {
		if (tracked_[i] != NULL)
			delete tracked_[i];
	}

	tracked_.erase(tracked_.begin(), tracked_.end());

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Voter.
//===================================================================

//-------------------------------------------------------------------
// Method........: FillMaps
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given the indices of those rules that fire, returns
//                 (in-place) filled maps for these.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectTrackingVoter::FillMaps(VoteMap &votemap, RuleMap &rulemap, const Vector(int) &indices, const Rules &rules) const {

	// Verify that the rules-to-objects map is computed.
	if (tracked_.size() != rules.GetNoRules())
		return false;

	// Clear maps.
	votemap.erase(votemap.begin(), votemap.end());
	rulemap.erase(rulemap.begin(), rulemap.end());

	if (indices.empty())
		return true;

	int i, j, decision_attribute = Undefined::Integer();

	Bits tracked;

	// Process all rules that fired.
	for (i = 0; i < indices.size(); i++) {

		// Compute the union of all objects from which the rules that fired were derived.
		if (i == 0)
			tracked = *(tracked_[indices[i]]);
		else
			tracked |= *(tracked_[indices[i]]);

		Handle<Rule> rule = rules.GetRule(indices[i]);

		// Fill rule map.
		int no_decision_values = rule->GetNoDecisionValues();

		// Determine decision attribute, if not already done.
		if (decision_attribute == Undefined::Integer())
			decision_attribute = rule->GetDecisionAttribute();

		// Ensure that we do not mix apples and bananas.
		if (rule->GetDecisionAttribute() != decision_attribute)
			continue;

		for (j = 0; j < no_decision_values; j++) {
			int value = rule->GetDecisionValue(j);
			rulemap.insert(Pair(const int, Rule::Handles)(value, Rule::Handles()));
			rulemap[value].push_back(rule);
		}

	}

	// Trace back to originating decision table.
  Handle<DecisionTable> parent = dynamic_cast(DecisionTable *, rules.FindParent(DECISIONTABLE));

	if (parent.IsNULL()) {
		Message::Error("Unable to trace back to the rules' originating decision table.", false);
		return false;
	}

	// Determine index of decision attribute in parent table.
	decision_attribute = parent->GetDecisionAttribute(true);

	if (decision_attribute == Undefined::Integer())
		return false;

	// Ensure that we do not mix apples and bananas.
	if (rules.GetRule(indices[0])->GetDecisionAttribute() != decision_attribute) {
		Message::Error("Mismatch between decision attribute indices of rules and parent table.", false);
		return false;
	}

	// Fill vote map.
	for (i = tracked.GetSize() - 1; i >= 0; i--) {
		if (tracked[i]) {
			int value = parent->GetEntry(i, decision_attribute, true);
			votemap.insert(Pair(const int, int)(value, 0));
			votemap[value]++;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: FillClassification
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectTrackingVoter::FillClassification(RuleBasedClassification &classification, const VoteMap &votemap, const RuleMap &rulemap, const Rules &rules) const {

	// Clear current contents.
	classification.Clear();

	// Set decision attribute. Assume it's the same for all rules.
	if (!classification.SetDecisionAttribute(rules.GetRule(0)->GetDecisionAttribute()))
		return false;

	VoteMap::const_iterator vit;
	RuleMap::const_iterator rit;

	long normalization = 0;

	// Compute normalization factor.
	for (vit = votemap.begin(); vit != votemap.end(); vit++) {
		normalization += (*vit).second;
	}

	// Was a normalization factor computed properly?
	if (normalization == 0)
		normalization = 1;

	vit = votemap.begin();
	rit = rulemap.begin();

	int i = 0;

	// Compute normalized certainty factors.
	while (!(vit == votemap.end())) {
		int value = (*vit).first;
		int votes = (*vit++).second;
		if (!classification.AppendDecisionValue(value, static_cast(float, votes) / normalization))
			return false;
		RuleMap::const_iterator rit = rulemap.find(value);
		if (rit != rulemap.end()) {
			if (!classification.SetRules((*rit).second, i))
				return false;
		}
		i++;
	}

	return true;

}

ObjectTrackingVoter *
ObjectTrackingVoter::Clone() {
	return new ObjectTrackingVoter;
}
