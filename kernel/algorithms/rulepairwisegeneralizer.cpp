//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005 
// Description...: Rule tuning. This group of algorithms changes a
//				   set of rules
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/algorithms/rulepairwisegeneralizer.h>
#include <kernel/algorithms/simplecoverfilter.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/structures/structures.h>
#include <kernel/basic/message.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/reducts.h>
#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/basic/algorithm.h>

//-------------------------------------------------------------------
// Methods for class RulePairwiseGeneralizer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================


RulePairwiseGeneralizer::RulePairwiseGeneralizer() {
	SetThreshold(0.9f);
	UseIDG(false);
	SetIDGFilename(Undefined::String());
	SetAttributeDifference(0);
}

RulePairwiseGeneralizer::~RulePairwiseGeneralizer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RulePairwiseGeneralizer, RULEPAIRWISEGENERALIZER, RuleGeneralizer)

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
RulePairwiseGeneralizer::GetParameters() const {
	
	String parameters;
	// Threshold
	parameters += Keyword::Separator();
	parameters += Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetThreshold());

	// Use IDG?
	parameters += Keyword::Separator();
	parameters += Keyword::IDG();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseIDG());
	
	// IDG filename.
	if (UseIDG()) {
		parameters += Keyword::Separator();
		parameters += Keyword::IDG() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIDGFilename();
	}
	// AttrDifference
	parameters += Keyword::Separator();
	parameters += Keyword::AttrDifference();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAttributeDifference());
	return parameters;
	
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
RulePairwiseGeneralizer::SetParameter(const String &keyword, const String &value) {
	
	//Threshold
	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetThreshold(value.GetFloat());

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());
	
	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);
	
	// AttrDifference
	if (keyword == Keyword::AttrDifference() && value.IsInteger())
		return this->SetAttributeDifference(value.GetInteger());
	
	return false;
	
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
RulePairwiseGeneralizer::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return RuleGeneralizer::IsApplicable(structure);
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
RulePairwiseGeneralizer::Apply(Structure &structure) const {
	
/*	// Check input.
	if (!IsApplicable(structure))
		return NULL;
	
	Handle<Structures> structures = dynamic_cast(Structures *, &structure);
	// Tune.
	Structure *result = RuleTuner::Apply(structure);
*/	
	return RuleGeneralizer::Apply(structure);
}

//-------------------------------------------------------------------
// Methods inherited from RuleTuner.
//===================================================================

//-------------------------------------------------------------------
// Method........: Tune
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure* 
RulePairwiseGeneralizer::Tune(const Structures &structures) const {
	
	Handle<GeneralRules> rules = ConvertRules(structures);
	if (rules == NULL)
		return NULL;
	
	// This method is conceptually const only.
	Handle<RulePairwiseGeneralizer> self = const_cast(RulePairwiseGeneralizer *, this);
	
	DecisionSubsets decisionSubsets;
	CreateRuleSubsets(rules, decisionSubsets);

	GeneralizeRules(decisionSubsets, rules);

	return rules.Release();
}

//-------------------------------------------------------------------
// Method........: GeneralizeRules
// Author........: Ewa Makosa
// Date..........:
// Description...: Tries to "improve" each rule set by generalizing
//					pairs of rules within it, until no pairs are 
//					suitable. Generalized rule can move to other group
//					and start. 
// Comments......:
// Revisions.....:
//===================================================================

void 
RulePairwiseGeneralizer::GeneralizeRules(DecisionSubsets &decisionSubsets,
									   Handle<GeneralRules> rules)const{
	
	DecisionSubsets::iterator it;
	bool baseImprovement, improvement = true;
	int i, j, changedNo = decisionSubsets.size(); 

	Handle<DecisionTable> decisionTable = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE));
	
	// Initialize discerner.
	bool masked = true;
	Discerner discerner;

	if (UseIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *decisionTable, masked)) {
			Message::Error("Failed to load IDGs from " + GetIDGFilename() + ".");
			return;
		}
	}

//	while (changedNo > 0){//while there were some exchanges between rule subsets
//		changedNo = 0;
		
		for (it = decisionSubsets.begin(); it != decisionSubsets.end(); it ++){

		//	if ((it->first).changed_){//something new in this group, chance for some generalization
	//			changedNo++;
				std::sort((it->second).begin(), (it->second).end());
	//			improvement = true;
				
		//		while (improvement){//generalize pairwise till it's possible
		//			improvement = false;
					i = 0;
					
					while (i < (it->second).size() - 1){//go rule by rule trying to generalize it
						baseImprovement = Improve(*decisionTable, discerner, rules, decisionSubsets, it->first, it->second, i);				
						/*if (baseImprovement){
							improvement = true;
						}
						else{ 
							i++;
						}*/
						if (!baseImprovement){
							 
							i++;
						}
					}
			//	}
			//	*(const_cast<bool*>( &((it->first).changed_) ) ) = false;

//			}
		}
	//}
}

//-------------------------------------------------------------------
// Method........: Improve
// Author........: Ewa Makosa
// Date..........:
// Description...: Tries to "improve" each rule set by generalizing
//					pairs of rules within it, until no pairs are 
//					suitable. Generalized rule can move to other group
//					and start. 
// Comments......:
// Revisions.....:
//===================================================================

bool 
RulePairwiseGeneralizer::Improve(const DecisionTable &decisionTable,
								const Discerner &discerner,
								Handle<GeneralRules> rules,
								DecisionSubsets &decisionSubsets,
								const DecisionSet &ds_base,
								RuleGroup &ruleGroup,
								const int baseInd)const{

	
	int indGen = -1;
	
	bool improvement	= false;
	bool generalized	= false;
	bool removed		= false;
	
	generalized = GeneralizeRule(decisionTable, discerner, ruleGroup, baseInd, indGen);

	if (generalized){
		//put rules in order
		Handle<GeneralRule> ruleGen = (ruleGroup[indGen]).ruleHandle_;
		removed = RemoveDuplicateRules(ruleGen, rules);

		//take away the base from rules
		Handle<GeneralRule> ruleBase = (ruleGroup[baseInd]).ruleHandle_;
		int baseRuleInd = rules->FindMember(ruleBase.GetPointer(), false);
		
		if (baseRuleInd != Undefined::Integer()){
			rules->RemoveStructure(baseRuleInd);
		}

		//take away the base from ruleGroup
		ruleGroup.erase(ruleGroup.begin() + baseInd);
		
		//was there a change in the decision?
		DecisionSet ds_gen;
		if (DecisionChanged(ds_base, ruleGen, ds_gen)){

			//now the generalized rule stays in the wrong group
			ruleGroup.erase(ruleGroup.begin() + indGen-1);
			
			if (!removed)//this rule hasn't got a copy in the right place, we need to move it there
				AssignToDecisionSubset(decisionSubsets, ds_gen, ruleGen);
		
		}else{
		
			if (removed){//if the decision hasn't changed and there was a duplicate in rules, then now it is in rule group
				ruleGroup.erase(ruleGroup.begin() + indGen-1);
			}
			else
				improvement = true;
		}
	}

	return improvement;
}
	

//-------------------------------------------------------------------
// Method........: GeneralizeRule
// Author........: Ewa Makosa
// Date..........:
// Description...: If the improvement is possible, this function
//					results with chosen rule generalized with
//					one of the other rules from the gropup
// Comments......:
// Revisions.....:
//===================================================================

bool 
RulePairwiseGeneralizer::GeneralizeRule(const DecisionTable &decisionTable,
										const Discerner &discerner,
										RuleGroup &ruleGroup,
										const int baseInd,
										int &maxInd)const{
	
	
	int ruleInd, ruleAttrNo;
	Handle<GeneralRule> rule, ruleTemp;
	float tempAcc, maxAcc = 0.0;
	bool baseImprovement = false;
	Message message;

	Handle<GeneralRule> base = (ruleGroup[baseInd]).ruleHandle_;
	int baseAttrNo = base->GetNoConditionDescriptors();

	for (ruleInd = baseInd + 1; ruleInd < ruleGroup.size(); ruleInd++){
		
		rule = (ruleGroup[ruleInd]).ruleHandle_;
		ruleAttrNo = rule->GetNoConditionDescriptors();
		
		/*  it's no use checking the rules any further since they're sorted by length */
		if ((baseAttrNo - ruleAttrNo) > attributeDifference_)
			break;
		
		if (SuitablePair(base, rule)){
			
			ruleTemp = InitializeRule(rule);
			tempAcc = GeneralizePair(decisionTable, discerner, base, ruleTemp);	
			
			if (tempAcc >= threshold_){
				baseImprovement = true;
				
				if (tempAcc > maxAcc){
					maxInd = ruleInd;
					if (tempAcc == 1.0)//won't get any better
						break;
					maxAcc = tempAcc;
				}
			}
			
		}
	}

	if (baseImprovement){//perform the generalization on chosen rule with base values
		
		rule = (ruleGroup[maxInd]).ruleHandle_;
		GeneralizePair(decisionTable, discerner, base, rule);
	}

	return baseImprovement;
}

//-------------------------------------------------------------------
// Method........: SuitablePair
// Author........: Ewa Makosa
// Date..........:
// Description...: Checks is base and rule are suitable to be 
//					generalized to together
// Comments......:
// Revisions.....:
//===================================================================

bool
RulePairwiseGeneralizer::SuitablePair(Handle<GeneralRule> base, Handle<GeneralRule> rule)const{
	
	float ruleAcc = ruleAcc = ObtainRuleAccuracy(*rule);
		
	if (ruleAcc < threshold_) 
		return false;

	int baseDecNo = base->GetNoDecisionValues();
	int ruleDecNo = rule->GetNoDecisionValues();

	//the decision sets must be the same
	if (baseDecNo != ruleDecNo) 
		return false;
	
	int j, baseDecVal;
	
	for (int i = 0; i < baseDecNo; i++){
		
		baseDecVal = base->GetDecisionValue(i);
		
		for (j = 0; j < ruleDecNo; j++){
	
			if (baseDecVal == rule->GetDecisionValue(j))
				break;
		}

		if (j == ruleDecNo)
			return false;
	}
	
	//the attribute set of rule must be a proper subset of base attributes
	return rule->AttrLHSSubsetOf(base);

};

//-------------------------------------------------------------------
// Method........: GeneralizePair
// Author........: Ewa Makosa
// Date..........:
// Description...: Generalizes the toGen descriptors with the 
//					corresponding fromGen values
// Comments......:
// Revisions.....:
//===================================================================

float
RulePairwiseGeneralizer::GeneralizePair(const DecisionTable &decisionTable,
										const Discerner &discerner,
									  const Handle<GeneralRule> fromGen,
									  Handle<GeneralRule> toGen) const{
	
	int i, j, a, v;
	
	bool rhs = toGen->HasRHS();
	bool masked = true;
	STLSet(int)::const_iterator it;
		
	int toGenDescriptorsNo		= toGen->GetNoConditionDescriptors();
	int fromGenDescriptorsNo	= fromGen->GetNoConditionDescriptors();
	
	for (i = 0; i < toGenDescriptorsNo; i++) {
		
		a = toGen->GetConditionAttribute(i);
		
		for (j = 0; j < fromGenDescriptorsNo; j++) {
			
			if (fromGen->GetConditionAttribute(j) == a) {
				//feed the descriptor with values from fromGen's same attribute
				it = fromGen->GetConditionValueIterator(j);
				
				for (v = 0; v < fromGen->GetNoConditionValues(j); v++){
					toGen->AddConditionValue(i, *it);
					it++;
				}
				break;
			}				
		}
	}

	toGen->Reevaluate(discerner, decisionTable, rhs);
	return ObtainRuleAccuracy(*toGen);
}


bool 
RulePairwiseGeneralizer::InsertToRuleGroup(RuleGroup &ruleGroup, 
										 Handle<GeneralRule> rule,
										 int ruleInd)const{
	
	// Check logical membership of some other rule equal to the generalized one.
	bool improvement = false;
	int i;
	
	for (i = 0; i < ruleGroup.size(); i++){
		if (*rule == *(ruleGroup[i].ruleHandle_))
			break;
	}
	
	if (i == ruleGroup.size()){
		//there doesn't already exist a rule like this
		improvement = true;
		RuleHandle grh;
		grh.ruleHandle_ = rule;
		ruleGroup.insert(ruleGroup.begin() + ruleInd, grh);
	}
	return improvement;							
}


//-------------------------------------------------------------------
// Method........: DecisionChanged
// Author........: Ewa Makosa
// Date..........:
// Description...: Checks if the old and new decision sets of the rule
//					are different.
//					
// Comments......: 
// Revisions.....: 
//===================================================================

bool
RulePairwiseGeneralizer::DecisionChanged(const DecisionSet &ds1,
									   const Handle<GeneralRule> rule,
									   DecisionSet &ds2)const{
	
	int decisionsNo = rule->GetNoDecisionValues();
	
	for (int d = (decisionsNo - 1); d >= 0; d--){
		ds2.Insert(rule->GetDecisionValue(d));
	}
	
	if (ds1 == ds2)
		return false;
	
	return true;
} 

RulePairwiseGeneralizer *
RulePairwiseGeneralizer::Clone() {
	return new RulePairwiseGeneralizer;
}
