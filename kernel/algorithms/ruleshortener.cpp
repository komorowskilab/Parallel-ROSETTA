//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005 
// Description...: Rule shortening. Algorithm for rule tuning. Iteratively
//					removes descriptors from each rule that have lowest
//					impact on accuracy, until acuracy goes down below a
//					given threshold (parameter)
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/algorithms/ruleshortener.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/structures/structures.h>
#include <kernel/basic/message.h>
#include <kernel/structures/reducts.h>
#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class RuleShortener.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RuleShortener::RuleShortener() {
	SetThreshold(0.9f);
	UseIDG(false);
	SetIDGFilename(Undefined::String());
}

RuleShortener::~RuleShortener() {}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleShortener, RULESHORTENER, RuleTuner)

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
RuleShortener::GetParameters() const {
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
RuleShortener::SetParameter(const String &keyword, const String &value) {
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
RuleShortener::IsApplicable(const Structure &structure, bool /*warn*/) const {
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
RuleShortener::Apply(Structure &structure) const {

	// Check input.
/*	if (!IsApplicable(structure))
		return NULL;

	Handle<Structures> structures = dynamic_cast(Structures *, &structure);
	// Tune.
	Structure *result = RuleTuner::Apply(structure);
*/
	return RuleTuner::Apply(structure);
}

//-------------------------------------------------------------------
// Methods inherited from RuleTuner.
//===================================================================

//-------------------------------------------------------------------
// Method........: Tune
// Author........: Ewa Makosa
// Date..........:
// Description...: Shortens each rule one by one
// Comments......:
// Revisions.....:
//===================================================================

Structure* 
RuleShortener::Tune(const Structures &structures) const {
	
	Handle<GeneralRules> rules = ConvertRules(structures);
	if (rules == NULL){
		Message::Error("Algorithm failed to convert rules into general ones");
		return NULL;
	}
	
	// This method is conceptually const only.
	Handle<RuleShortener> self = const_cast(RuleShortener *, this);
	
	Message message;
	Discerner discerner;
	
	Handle<DecisionTable> decisiontable = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE));
	bool masked = true;

	// Initialize discerner.	
	if (UseIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *decisiontable, masked)) {
			Message::Error("Failed to load IDGs from " + GetIDGFilename() + ".");
			return NULL;
		}
	}

	int descNoBefore, descNoAfter;
	int no_rules = rules->GetNoRules();

	// How often should we poll for cancellations? How many structures do, say, 5 percent correspond to?
	int pollsize = no_rules / 25, pollcount = 0;

	for (int i = (no_rules - 1); i >= 0; i--) {
	
		if (pollcount == pollsize || i == 0 || i == no_rules - 1) {
			if (!message.Progress("Shortening...", no_rules - 1 - i, no_rules))
				break;
			pollcount = 0;
		}
		else {
			pollcount++;
		}

		Handle<GeneralRule> rule = rules->GetRule(i);
		descNoBefore = rule->GetNoConditionDescriptors();
		Handle<GeneralRule> shorter = Shorten(discerner, *decisiontable, rule);
		descNoAfter = shorter->GetNoConditionDescriptors();
	
		if (descNoBefore > descNoAfter){
			//the rule has been shortened, remove if it doubles with some other now
			
			int rulesNoTemp = rules->GetNoStructures();
			// Check logical membership of some other rule equal to the shortened one.

			for (int j = 0; j < rulesNoTemp; j++) {
			
				if (j != i){
					if (*shorter == *(rules->GetRule(j))){
						rules->RemoveStructure(i);
						break;
					}
				}
			}
		}
	}
	return rules.Release();
}

//-------------------------------------------------------------------
// Method........: Shorten
// Author........: Ewa Makosa
// Date..........:
// Description...: Iteratively removes one descriptor from the rule 
//					until accuracy drops below a given threshold. In
//					each iteration step the best descriptor is chosen,
//					i.e. the one that couses the least loss on the 
//					rules accuracy
// Comments......:
// Revisions.....:
//===================================================================

Handle<GeneralRule>
RuleShortener::Shorten( const Discerner &discerner, const DecisionTable &decisionTable, Handle<GeneralRule> ruleShortened)const{

	// Get accuracy
	float accuracy = ObtainRuleAccuracy(*ruleShortened);
	
	if (accuracy < threshold_){
		return ruleShortened;
	}

	// now accuracy >= threshold_

	bool rhs = ruleShortened->HasRHS();	
	float accTemp;
	float accMax = accuracy;
	int j, posMax, d;
	Handle<GeneralRule> ruleTemp = NULL;
	
	while ((accMax >= threshold_)&&(ruleShortened->GetNoConditionDescriptors() > 1)){
		accMax = 0;
		posMax = 0;
		
		
		for (d = 0; d< ruleShortened->GetNoConditionDescriptors(); d++){
			ruleTemp = InitializeRule(ruleShortened);
			ruleTemp->RemoveConditionDescriptor(d);
		
			if (!ruleTemp->Reevaluate(discerner, decisionTable, rhs)){
				Message::Error("Failed to reevaluate the temporary shortened rule.");
				return NULL;
			}

			accTemp = ObtainRuleAccuracy(*ruleTemp);
			
			
			if (accTemp > accMax){
				accMax = accTemp;
				posMax = d;
			}
		}
		
		if (accMax >= threshold_){
			ruleShortened->RemoveConditionDescriptor(posMax);
		
			if (!ruleShortened->Reevaluate(discerner, decisionTable, rhs)){
				Message::Error("Failed to reevaluate the shortened rule.");
				return NULL;
			}
		}
		
	}
	return ruleShortened;
}


 
RuleShortener *
RuleShortener::Clone() {
	return new RuleShortener;
}
