//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005 
// Description...: Rule tuning. This group of algorithms changes a
//				   set of rules
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/algorithms/rulegeneralizer.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/structures/structures.h>
#include <kernel/basic/message.h>
#include <kernel/structures/rules.h>
#include <kernel/utilities/creator.h>
#include <kernel/basic/algorithm.h>
#include <kernel/utilities/discerner.h>
#include <kernel/structures/decisiontable.h>


//-------------------------------------------------------------------
// Methods for class RuleGeneralizer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RuleGeneralizer::RuleGeneralizer() {}

RuleGeneralizer::~RuleGeneralizer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleGeneralizer, RULEGENERALIZER, RuleTuner)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
RuleGeneralizer::GetParameters() const {
	return RuleTuner::GetParameters();
}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleGeneralizer::SetParameter(const String &keyword, const String &value) {
	return RuleTuner::SetParameter(keyword, value);
}


//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Ewa Makosa
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleGeneralizer::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return RuleTuner::IsApplicable(structure);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Ewa Makosa
// Date..........:
// Description...: Takes a structure set as input and tunes it according
//                 to some (overloaded) filtering scheme.  By tuning is
//                 meant change or removal of individual member structures.
//
// Comments......: 
// Revisions.....: 
//===================================================================

Structure *
RuleGeneralizer::Apply(Structure &structure) const {
	
/*	// Check input.
	if (!IsApplicable(structure))
		return NULL;
	
	Handle<Structures> structures = dynamic_cast(Structures *, &structure);
	// Tune.
	Structure *result = RuleTuner::Apply(structure);
*/	
	return  RuleTuner::Apply(structure);
}

//-------------------------------------------------------------------
// Methods inherited from RuleTuner.
//===================================================================

//-------------------------------------------------------------------
// Method........: CreateRuleSubsets
// Author........: Ewa Makosa
// Date..........:
// Description...: Groups rules according to their generalized
//					decision. For every rule, finds its DecisionSet 
//					and assigns it to appropriate subset.
//					
// Comments......: 
// Revisions.....: 
//===================================================================

void		
RuleGeneralizer::CreateRuleSubsets(const Handle<GeneralRules> rules,
										   DecisionSubsets &decisionSubsets)const{
	
	int i, d, decisionsNo, rulesNo = rules->GetNoStructures();
	
	for (i = (rulesNo - 1); i >= 0 ; i--) {

		Handle<GeneralRule> rule = rules->GetRule(i);
		DecisionSet ds;
		decisionsNo = rule->GetNoDecisionValues();
		
		for (d = (decisionsNo - 1); d >= 0 ; d--){
			ds.Insert(rule->GetDecisionValue(d));
		}
		AssignToDecisionSubset(decisionSubsets, ds, rule);
	}
}

//-------------------------------------------------------------------
// Method........: AssignToDecisionSubset
// Author........: Ewa Makosa
// Date..........:
// Description...: Adds a rule to its subset according to its 
//					decision set.
//					
// Comments......: 
// Revisions.....: 
//===================================================================

void 
RuleGeneralizer::AssignToDecisionSubset(DecisionSubsets &decisionSubsets,
											  const DecisionSet &decisionSet,
											  const Handle<GeneralRule> rule)const{
	
	DecisionSubsets::iterator it;
	it = decisionSubsets.find(decisionSet);
	RuleHandle grh;
	
	grh.ruleHandle_ = rule;
	
	if (it != decisionSubsets.end()){ 
		(it->second).push_back(grh);
//		*(const_cast<bool*>( &((it->first).changed_) ) ) =true;
	}
	else {
		RuleGroup rg;
		rg.push_back(grh);
//		*(const_cast<bool*>( &(decisionSet.changed_) ) ) =true;
		Pair(const DecisionSet, RuleGroup) p(decisionSet, rg);
		decisionSubsets.insert(p);
	}
}


//-------------------------------------------------------------------
// Method........: RemoveDuplicateRules
// Author........: Ewa Makosa
// Date..........:
// Description...: Removes the given rule if it was in the set 
//					already. Duplicates may appear after 
//					generalization -> The base rule evolves into
//					some existing one. Returns true if a rule was
//					removed.
// Comments......: Needs to be called for each generalized rule
// Revisions.....:
//===================================================================

bool 
RuleGeneralizer::RemoveDuplicateRules(Handle<GeneralRule> rule,
									  Handle<GeneralRules> rules)const{

	//check if there are two same rules
	bool firstFound = false;

	int rulesNo = rules->GetNoStructures();

	for (int j = 0; j < rulesNo; j++) {

		if (*rule == *(rules->GetRule(j))){

			if (firstFound){
				rules->RemoveStructure(j);
				return true;
			}else
				firstFound = true;
		}
	}
	return false;
}

 
