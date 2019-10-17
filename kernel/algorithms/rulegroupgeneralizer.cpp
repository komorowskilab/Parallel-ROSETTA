//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005 
// Description...: Rule tuning algorithm for rule generalization. 
//					Generalizes groups of similar rules of the same 
//					decision and then performs simple coverage filtering
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/algorithms/rulegroupgeneralizer.h>
#include <kernel/algorithms/simplecoverfilter.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/structures/structures.h>
#include <kernel/basic/message.h>
#include <kernel/utilities/discerner.h>
#include <kernel/structures/decisiontable.h>
#include <common/objectmanager.h>
#include <kernel/basic/algorithm.h>


//-------------------------------------------------------------------
// Methods for class RuleGroupGeneralizer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================


RuleGroupGeneralizer::RuleGroupGeneralizer() {
	SetThreshold(0.9f);
	UseIDG(false);
	SetIDGFilename(Undefined::String());
	SetCoverageThreshold(2);
}

RuleGroupGeneralizer::~RuleGroupGeneralizer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleGroupGeneralizer, RULEGROUPGENERALIZER, RuleGeneralizer)

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
RuleGroupGeneralizer::GetParameters() const {
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

	// CoverageThreshold
	parameters += Keyword::Separator();
	parameters += Keyword::Coverage();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetCoverageThreshold());
	
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
RuleGroupGeneralizer::SetParameter(const String &keyword, const String &value) {
	
	if (keyword == Keyword::Coverage() && value.IsInteger())
		return SetCoverageThreshold(value.GetInteger());

	return RuleGeneralizer::SetParameter(keyword, value);
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
RuleGroupGeneralizer::IsApplicable(const Structure &structure, bool /*warn*/) const {
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
RuleGroupGeneralizer::Apply(Structure &structure) const {
	
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
RuleGroupGeneralizer::Tune(const Structures &structures) const {

	Handle<GeneralRules> rules = ConvertRules(structures);
	if (rules == NULL){
		Message::Error("Algorithm failed to convert rules into general ones");
		return NULL;
	}

	// This method is conceptually const only.
	Handle<RuleGroupGeneralizer> self = const_cast(RuleGroupGeneralizer *, this);
	
	DecisionSubsets decisionSubsets;
	CreateRuleSubsets(rules, decisionSubsets);
	GeneralizeRules(decisionSubsets, rules);

	return rules.Release();
}

//-------------------------------------------------------------------
// Method........: Generalize Rules
// Author........: Ewa Makosa
// Date..........:
// Description...: Groups the rules. For each of the groups
//					generalizes each of the rules within them. At the
//					end performs simple coverage filtering.
// Comments......:
// Revisions.....:
//===================================================================

void 
RuleGroupGeneralizer::GeneralizeRules(DecisionSubsets &decisionSubsets,
									  Handle<GeneralRules> rules)const{

	RuleGroups ruleGroups;
	GroupRules(decisionSubsets, ruleGroups);
	
	Discerner discerner;
	Handle<DecisionTable> decisiontable = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE));
	
	// Initialize discerner.
	bool masked = true;
	
	if (UseIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *decisiontable, masked)) {
			Message::Error("Failed to load IDGs from " + GetIDGFilename() + ".");
			return;
		}
	}

	int rulesNo = rules->GetNoStructures();
	int i, j, r = 0;
	int pollsize = rulesNo / 20, pollcount = 0;
	Message message;
	float accuracy;
	Handle<GeneralRule> genRule;

	//for all groups do
	for (i = 0; i < ruleGroups.size(); i++){
	
		//for all rules in the Rule Group -- generalize them
		for (j = ruleGroups[i].size() - 1; j >= 0; j--){
			
			if (pollcount == pollsize || r == 0 || r == rulesNo - 1) {
				if (!message.Progress("Generalizing...", r, rulesNo))
					break;
				pollcount = 0;
			}
			else {
				pollcount++;
			}
			r++;

			Handle<GeneralRule> base = ((ruleGroups[i])[j]).ruleHandle_;//get the base rule
			// Get accuracy
			accuracy = ObtainRuleAccuracy(*base);

			if (accuracy >= threshold_){
				//initialise the general rule
				genRule = Generalize(discerner, *decisiontable, base, ruleGroups[i]);
				RemoveDuplicateRules(genRule, rules);
			}
		}
	}

	Handle<Algorithm> filter = ObjectManager::GetIdentifiedAlgorithm(SIMPLECOVERFILTER);
	if (filter==NULL)
		Message::Error("ups i can't find filter");
	
	String parameters = Keyword::Threshold()+"="+String::Format(GetCoverageThreshold())+"; "+Keyword::IDG() +"=";
	
	if (UseIDG())
		parameters += "T; "+Keyword::IDG() + Keyword::Dot() + Keyword::Filename()+"="+GetIDGFilename()+";";
	else
		parameters += "F;";
	
	filter->SetParameters(parameters);
	
	rules = dynamic_cast(GeneralRules *,const_cast(Structure *, rules->Apply(*filter)));

}


//-------------------------------------------------------------------
// Method........: Group Rules
// Author........: Ewa Makosa
// Date..........:
// Description...: Groups the rules so that they are ordered by subset of
//					attributes relation.
// Comments......:
// Revisions.....:
//===================================================================

void
RuleGroupGeneralizer::GroupRules(DecisionSubsets &decisionSubsets, 
							RuleGroups& ruleGroups)const{
	
	//S - map of subsets according to their decision
	int i, j;
	Handle<GeneralRule> longest, rule;

	for (DecisionSubsets::iterator it = decisionSubsets.begin(); it != decisionSubsets.end(); it++){
		i = 0;
		while (i < (it->second).size()){
			RuleGroup rg;
			longest = ((it->second)[i]).ruleHandle_;
			rg.push_back((it->second)[i]);
			(it->second).erase((it->second).begin() + i);
			//add others to the new group
			j = i;
			while (j < (it->second).size()){
				rule = ((it->second)[j]).ruleHandle_;
				//if curresntRule.AttributeSet() is conatined in longest.AttributeSet
				if (rule->AttrLHSSubsetOf(longest)){
					rg.push_back((it->second)[j]);
					longest = rule;
					(it->second).erase((it->second).begin() + j);
				}else
					j++;
			}
			//RuleHandle compare;
			std::sort(rg.begin(), rg.end()/* compare*/);
			ruleGroups.push_back(rg);
		}	
	}
	
};

//-------------------------------------------------------------------
// Method........: Generalize
// Author........: Ewa Makosa
// Date..........:
// Description...: Generalizes the attributes of base rule,
//					(one descriptor a time, iteratively) with values 
//					of attributes from other rules in the group. In 
//					each iteration one descriptor is chosen to be 
//					generalized, depending on the loss of accuracy. 
//					Iteration proceeds until the drop on accuracy 
//					exceeds a given threshold.
// Comments......:
// Revisions.....:
//===================================================================

Handle<GeneralRule>
RuleGroupGeneralizer::Generalize(const Discerner &discerner,
							const DecisionTable &decisionTable,
							Handle<GeneralRule> base,
							const RuleGroup &ruleGroup)const{
	
	int posMax, baseDescNo, ruleDescNo, g, r, a, aPos;
	GeneralRule::ValueSet valuesMax;
	ToGeneralize toGeneralize;
	Handle<GeneralRule> ruleGen, rule;
	GeneralRule::ValueSet::iterator it;
	float accGen, accMax = ObtainRuleAccuracy(*base);
	bool rhs = base->HasRHS();
	baseDescNo = base->GetNoConditionDescriptors();
	
	for (int b = 0; b < baseDescNo; b++){
		AttrPos p(base->GetConditionAttribute(b), b);
		toGeneralize.push_back(p);
	}

	while ((accMax >= threshold_)&&(toGeneralize.size() > 0)){
		
		accMax = 0;
		posMax = 0;
		
		//Try to generalize each of the descriptors and evaluate those transformations
		for (g = 0; g < toGeneralize.size(); g++){

			ruleGen = InitializeRule(base);

			a = toGeneralize[g].first;
			aPos = toGeneralize[g].second;
			
			accGen = GeneralizeDescriptor(discerner, decisionTable, a, aPos, ruleGroup, ruleGen);
		
			if (accGen > accMax){
				accMax = accGen;
				posMax = aPos;
				valuesMax = ruleGen->GetConditionValues(aPos);
			}
		}
	
		if (accMax >= threshold_){
			toGeneralize.erase(toGeneralize.begin() + posMax);
			
			for (it = valuesMax.begin(); it != valuesMax.end(); it++){
				base->AddConditionValue(posMax, *it);
			}
			
			if (!base->Reevaluate(discerner, decisionTable, rhs)){
				Message::Error("Failed to reevaluate the generalized rule.");
				return NULL;
			}
		}
	}
	return base;
}


//-------------------------------------------------------------------
// Method........: GeneralizeDescriptor
// Author........: Ewa Makosa
// Date..........:
// Description...: Add all the values for corresponding attribute 
//					from other rules
// Comments......:
// Revisions.....:
//===================================================================

float 
RuleGroupGeneralizer::GeneralizeDescriptor(const Discerner &discerner,
									  const DecisionTable &decisionTable,
									  int attribute, 
									  int attributePos,
									  const RuleGroup &ruleGroup,
									  Handle<GeneralRule> ruleGen
									  )const{
	
	Handle<GeneralRule> rule;
	int ruleDescNo, j, v;
	STLSet(int)::const_iterator it;
	float accuracy;
	bool rhs = ruleGen->HasRHS();

	for (int i = 0; i < ruleGroup.size(); i++){
		rule = (ruleGroup[i]).ruleHandle_;
		ruleDescNo = rule->GetNoConditionDescriptors();
		
		for (j = 0; j < ruleDescNo; j++) {
			
			if (rule->GetConditionAttribute(j) == attribute) {
				it = rule->GetConditionValueIterator(j);
				
				for (v = 0; v < rule->GetNoConditionValues(j); v++){
					ruleGen->AddConditionValue(attributePos, *it);
					it++;
				}	
				break;
			}
		}
	}

	if (!ruleGen->Reevaluate(discerner, decisionTable, rhs)){
		Message::Error("Rule Group Generalizer failed to reevaluate the generalized rule.");
	}

	return ObtainRuleAccuracy(*ruleGen);
}
 
RuleGroupGeneralizer *
RuleGroupGeneralizer::Clone() {
	return new RuleGroupGeneralizer;
}
