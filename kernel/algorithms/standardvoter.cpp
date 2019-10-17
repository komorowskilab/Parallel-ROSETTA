//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/standardvoter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/informationvector.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/rulebasedclassification.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

#include <omp.h>

//-------------------------------------------------------------------
// Methods for class StandardVoter.
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

StandardVoter::StandardVoter() {
	SetVotingStrategy(VOTING_SUPPORT);
	SetNormalizationStrategy(NORMALIZATION_FIRING);
	ExcludeGeneralizations(false);
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

StandardVoter::~StandardVoter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(StandardVoter, STANDARDVOTER, Voter)

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
StandardVoter::GetParameters() const {

	String parameters;

	// Get parameters higher up.
	parameters += Voter::GetParameters();

	parameters += Keyword::Separator();

	// Exclude generalizations (choose most specific rule).
	parameters += Keyword::Specific();
	parameters += Keyword::Assignment();
	parameters += String::Format(ExcludeGeneralizations());

	parameters += Keyword::Separator();

	// Voting strategy.
	parameters += Keyword::Voting();
	parameters += Keyword::Assignment();
	parameters += GetString(GetVotingStrategy());

	parameters += Keyword::Separator();

	// Normalization strategy.
	parameters += Keyword::Normalization();
	parameters += Keyword::Assignment();
	parameters += GetString(GetNormalizationStrategy());

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 000411: Call Voter::SetParameter instead of
//                            RuleBasedClassifier::SetParameter...
//===================================================================

bool
StandardVoter::SetParameter(const String &keyword, const String &value) {

	// Voting strategy.
	if (keyword == Keyword::Voting()) {
		if (value == GetString(VOTING_SUPPORT))
			return SetVotingStrategy(VOTING_SUPPORT);
		if (value == GetString(VOTING_SIMPLE))
			return SetVotingStrategy(VOTING_SIMPLE);
		return false;
	}

	// Exclude generalizations (choose most specific rule).
	if (keyword == Keyword::Specific() && value.IsBoolean())
		return ExcludeGeneralizations(value.GetBoolean());

	// Normalization strategy.
	if (keyword == Keyword::Normalization()) {
		if (value == GetString(NORMALIZATION_FIRING))
			return SetNormalizationStrategy(NORMALIZATION_FIRING);
		if (value == GetString(NORMALIZATION_ALL))
			return SetNormalizationStrategy(NORMALIZATION_ALL);
		return false;
	}

	// Perhaps higher up?
	return Voter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Methods inherited from Voter.
//===================================================================

//-------------------------------------------------------------------
// Method........: EliminateRules
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the indices of those rules that
//                 fire.
// Comments......:
// Revisions.....:
//===================================================================

bool
StandardVoter::EliminateRules(Vector(int) &indices, const InformationVector &inf, const Rules &rules, float fraction) const {

	if (!Voter::EliminateRules(indices, inf, rules, fraction))
		return false;

	// Only let the most specific rules fire?
	if (ExcludeGeneralizations())
		return ExcludeGeneralizations(indices, rules);

	return true;

}

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
StandardVoter::FillMaps(VoteMap &votemap, RuleMap &rulemap, const Vector(int) &indices, const Rules &rules) const {
//bool
//StandardVoter::FillMaps(VoteMap &votemap, RuleMap &rulemap, const Vector(int) &indices, const Rules rules) const {

	// Clear maps.
	votemap.erase(votemap.begin(), votemap.end());
	rulemap.erase(rulemap.begin(), rulemap.end());

	int decision_attribute = Undefined::Integer();

	int i, j;
	Message message;

	// Run through all rules that fired and gather votes.
	for (i = 0; i < indices.size(); i++) {

		// Get a firing rule. 
		Handle<Rule> rule = rules.GetRule(indices[i]);

    // Determine how many possible decision values the rule assigns.
		int no_decision_values = rule->GetNoDecisionValues();

		// Determine decision attribute, if not already done.
		if (decision_attribute == Undefined::Integer())
			decision_attribute = rule->GetDecisionAttribute();

		// Ensure that we do not mix apples and bananas.
		if (rule->GetDecisionAttribute() != decision_attribute)
			continue;

		// Loop over all possible decision values.
		for (j = 0; j < no_decision_values; j++) {

			// Get the current decision value.
			int value = rule->GetDecisionValue(j);

			// Determine how many votes the rule casts in favour of that value.
			int votes = GetNoVotes(*rule, j);

      // Update maps. (Insert into maps if not already present.)
			votemap.insert(Pair(const int, int)(value, 0));
			votemap[value] += votes;

			rulemap.insert(Pair(const int, Rule::Handles)(value, Rule::Handles()));
			rulemap[value].push_back(rule);

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
StandardVoter::FillClassification(RuleBasedClassification &classification, const VoteMap &votemap, const RuleMap &rulemap, const Rules &rules) const {

	// Verify map dimensions.
	if (votemap.size() != rulemap.size())
		return false;

	// Clear current contents.
	classification.Clear();

	// Set decision attribute. Assume it's the same for all rules.
	Handle<Rule> rule;
	
	rule = rules.GetRule(0);
	
	if (!classification.SetDecisionAttribute(rule->GetDecisionAttribute()))
		return false;

	VoteMap::const_iterator vit = votemap.begin();
	RuleMap::const_iterator rit = rulemap.begin();

	int i = 0, no_rules = rules.GetNoRules(), normalization = 0;

	// Compute normalization factor.
	switch (GetNormalizationStrategy()) {
		case NORMALIZATION_FIRING: // Consider only firing rules.
 			                         switch (GetVotingStrategy()) {
			                           case VOTING_SUPPORT:   while (!(vit == votemap.end()))
			                                                    normalization += (*vit++).second;
													                              break;
			                           case VOTING_SIMPLE:    while (!(rit == rulemap.end()))
			                                                    normalization += (*rit++).second.size();
													                              break;
				                         default:               return false;
															 }
															 break;
		case NORMALIZATION_ALL:    // Consider all rules.
			                         switch (GetVotingStrategy()) {
			                           case VOTING_SUPPORT:   for (i = 0; i < no_rules; i++)
			                                                    normalization += rules.GetRule(i)->GetSupport();
													                              break;
			                           case VOTING_SIMPLE:    normalization = no_rules;
													                              break;
				                         default:               return false;
															 }
															 break;
		default:                   return false;
	}

	// Was a normalization factor computed properly?
	if (normalization == 0)
		normalization = 1;

	vit = votemap.begin();
	rit = rulemap.begin();

	i = 0;

	// Normalize the election results.
	while (!(vit == votemap.end()) && !(rit == rulemap.end())) {

		// Get (value, accumulated votes) pair.
		int value = (*vit).first;
		int votes = (*vit++).second;

		// Update classification structure.
		if (!classification.AppendDecisionValue(value, static_cast(float, votes) / normalization))
			return false;

		if (!classification.SetRules((*rit++).second, i++))
			return false;

	}

	return true;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExcludeGeneralizations
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
StandardVoter::ExcludeGeneralizations(Vector(int) &indices, const Rules &rules) const {

	// Keep track of how many generalizations that were excluded.
	int no_excluded = 0;

	int i, j;

	// Check all pairs.
	for (i = indices.size() - 1; i >= 0; i--) {

		// Skip rules that have been tagged as excluded.
		if (indices[i] == Undefined::Integer())
				continue;

		for (j = i - 1; j >= 0; j--) {

			// Skip rules that have been tagged as excluded.
			if (indices[j] == Undefined::Integer())
				continue;

			// Get rules.
			Handle<Rule> rule_i = rules.GetRule(indices[i]);
			Handle<Rule> rule_j = rules.GetRule(indices[j]);

#if 0
			String formatted_i, formatted_j;

			// For debugging.
			rule_i->Format(formatted_i);
			rule_j->Format(formatted_j);
#endif

			// Is one rule a specialization of the other? If so, tag the generalization as excluded.
			if (*rule_i < *rule_j) {
				indices[j] = Undefined::Integer();
				no_excluded++;
			}
			else if (*rule_j < *rule_i) {
				indices[i] = Undefined::Integer();
				no_excluded++;
				break;
			}

		}

	}

	// No generalizations present?
	if (no_excluded == 0)
		return true;
#ifdef X86_64
	Message::Debug(String::Format((int)no_excluded) + " generalizations excluded out of " + String::Format((int)indices.size()) + " firing rules.");
#else
	Message::Debug(String::Format(no_excluded) + " generalizations excluded out of " + String::Format(indices.size()) + " firing rules.");
#endif

	Vector(int) specializations;
	specializations.reserve(indices.size() - no_excluded);

	// Remove all those tagged as generalizations.
	for (i = 0; i < indices.size(); i++) {
		if (indices[i] != Undefined::Integer())
			specializations.push_back(indices[i]);
	}

	indices = specializations;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNoVotes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a rule and a position index into the rule's
//                 decision value list, returns the number of votes
//                 that the rule casts for that decision class.
// Comments......:
// Revisions.....:
//===================================================================

int
StandardVoter::GetNoVotes(const Rule &rule, int i) const {

	switch (GetVotingStrategy()) {
		case VOTING_SUPPORT: return rule.GetSupport(i);
		case VOTING_SIMPLE:  return 1;
		default:             return 0;
	}

}

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
StandardVoter::GetString(Voting voting) {

	switch (voting) {
		case VOTING_SUPPORT: return "Support";
		case VOTING_SIMPLE:  return "Simple";
		default:             return Undefined::String();
	}

}

String
StandardVoter::GetString(Normalization normalization) {

	switch (normalization) {
		case NORMALIZATION_FIRING: return "Firing";
		case NORMALIZATION_ALL:    return "All";
		default:                   return Undefined::String();
	}

}

StandardVoter *
StandardVoter::Clone() {
	return new StandardVoter;
}
