//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005 
// Description...: Rule tuning. This group of algorithms changes a
//				   set of rules to make the model smaller and more 
//					general
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/ruletuner.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/structures/structures.h>
#include <kernel/structures/reducts.h>
#include <kernel/utilities/creator.h>
#include <kernel/basic/message.h>
#include <kernel/basic/set.h>

//-------------------------------------------------------------------
// Methods for class ruletuner.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RuleTuner::RuleTuner() {
}

RuleTuner::~RuleTuner() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleTuner, RULETUNER, Algorithm)

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
RuleTuner::GetParameters() const {
	
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
RuleTuner::SetParameter(const String &keyword, const String &value) {
	
	//Threshold
	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetThreshold(value.GetFloat());

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());
	
	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);
	
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
RuleTuner::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(RULES);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Ewa Makosa
// Date..........:
// Description...: Takes a structure set as input and tunes it according
//                 to some (overloaded) tuning scheme.  By tuning is
//                 meant change or removal of individual member structures.
//
// Comments......: 
// Revisions.....: 
//===================================================================

Structure *
RuleTuner::Apply(Structure &structure) const {
	
	// Check input.
	if (!IsApplicable(structure))
		return NULL;
	Handle<Structures> structures = dynamic_cast(Structures *, &structure);
	return Tune(*structures);
}

// 
//-------------------------------------------------------------------
// Method........: ObtainRuleAccuracy
// Author........: Ewa Makosa
// Date..........:
// Description...: Obtains best decision rule accuracy
// Comments......: 
// Revisions.....: 
//===================================================================

float 
RuleTuner::ObtainRuleAccuracy(const Rule &rule)const{
	int no_decision_values = rule.GetNoDecisionValues();
	float best_accuracy = -1.0f;
	float tmp_accuracy;

	for (int j = 0; j < no_decision_values; j++) {
		tmp_accuracy = rule.GetAccuracy(j);
		if (tmp_accuracy > best_accuracy) 
			best_accuracy = tmp_accuracy;
	}
	return best_accuracy;
}

//-------------------------------------------------------------------
// Method........: InitializeRule
// Author........: Ewa Makosa
// Date..........:
// Description...: Creates a general rule basing on a given Rule
// Comments......: 
// Revisions.....: 
//===================================================================

Handle<GeneralRule>
RuleTuner::InitializeRule(const Handle<Rule> rule)const{
	
	int desc, val;
	Handle <GeneralRule> initRule = Creator::GeneralRule();
	
	// initialize the general rule with values of the rule given 
	initRule->ReserveLHS(rule->GetNoConditionDescriptors());
	initRule->ReserveRHS(rule->GetNoDecisionValues());
	
	// Copy the condition attributes and their values.
	for (desc = 0; desc < rule->GetNoConditionDescriptors(); desc++){
		GeneralRule::ValueSet disjunction;
		//	disjunction.reserve(in.GetNoConditionValues(desc));
		int v = rule->GetConditionValue(desc);
		disjunction.insert(v);

		initRule->AppendConditionDescriptor(rule->GetConditionAttribute(desc), disjunction);
	}
	
	// Copy the decision attribute.
	initRule->SetDecisionAttribute(rule->GetDecisionAttribute());
	
	// Copy all decision values.
	for (val = 0; val < rule->GetNoDecisionValues(); val++)
		initRule->AppendDecisionValue(rule->GetDecisionValue(val), rule->GetSupport(val));
	
	initRule->SetSupport(rule->GetSupport(), 1.0);
	
	return initRule;
}

//-------------------------------------------------------------------
// Method........: InitializeRule
// Author........: Ewa Makosa
// Date..........:
// Description...: Creates a general rule basing on a given general rule
// Comments......: 
// Revisions.....: 
//===================================================================

Handle<GeneralRule>
RuleTuner::InitializeRule(const Handle<GeneralRule> rule)const{
	
	int desc, val;
	Handle <GeneralRule> initRule = Creator::GeneralRule();
	
	/* initialize the general rule with values of the rule given */
	initRule->ReserveLHS(rule->GetNoConditionDescriptors());
	initRule->ReserveRHS(rule->GetNoDecisionValues());
	
	// Copy the condition attributes and their values.
	for (desc = 0; desc < rule->GetNoConditionDescriptors(); desc++){
		GeneralRule::ValueSet disjunction;
		//	disjunction.reserve(in.GetNoConditionValues(desc));
		STLSet(int)::const_iterator it = rule->GetConditionValueIterator(desc);
		
		for (val = 0; val < rule->GetNoConditionValues(desc); val++){
			disjunction.insert(*it);
			it++;
		}
		initRule->AppendConditionDescriptor(rule->GetConditionAttribute(desc), disjunction);
	}
	
	// Copy the decision attribute.
	initRule->SetDecisionAttribute(rule->GetDecisionAttribute());
	
	// Copy all decision values.
	for (val = 0; val < rule->GetNoDecisionValues(); val++)
		initRule->AppendDecisionValue(rule->GetDecisionValue(val), rule->GetSupport(val));
	
	initRule->SetSupport(rule->GetSupport(), 1.0);
	
	return initRule;
}

//-------------------------------------------------------------------
// Method........: ConvertRules
// Author........: Ewa Makosa
// Date..........:
// Description...: Creates a set of general rules basing on a given 
//					set of rules
// Comments......: 
// Revisions.....: 
//===================================================================

Handle<GeneralRules>
RuleTuner::ConvertRules(const Structures &structures) const{
	
	if (structures.IsA(GENERALRULES)){//We do not need to convert
		Handle<GeneralRules> oldRules = dynamic_cast(GeneralRules *, const_cast(Structures *, &structures));
		return oldRules;
	}

	if (!structures.IsA(RULES))
		return NULL;
	
	Handle<Rules> oldRules = dynamic_cast(Rules *, const_cast(Structures *, &structures));
	Handle<GeneralRules> newRules = Creator::GeneralRules();
	
	int no_rules = oldRules->GetNoRules();

	int  i, j;
	
	for (i = 0; i < no_rules; i++) {
		
		Handle<Rule> rule = oldRules->GetRule(i);
		Handle <GeneralRule> convertedRule = InitializeRule(rule);
		
		newRules->AppendStructure(convertedRule.GetPointer());
	}
	
	/* replace old rules with the new ones */
	
	// Does a parent exist?
//	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, oldRules->FindParent(DECISIONTABLE));
//	if (table == NULL) {
//		Message::Error("Rules to be tuned have no parent decision table.");
//		return NULL;
//	}
	

//	for (i = 0; i < oldRules->GetNoChildren(); i++) {
//		if (!newRules->AppendChild(oldRules->GetChild(i))) {                              // Shit, undo all previous appends and break.
//			Message::Error("An error occurred while reassigning/appending the children."); // Notify the user.
//			for (j = 0; j < i; j++) {                                                      // Undo.
//				if (!oldRules->RemoveChild(newRules->GetNoChildren() - 1)) {                     // Shit, undo failed.
//					Message::Error("Undo failed!");
//					return NULL;
//				}
//			}
//			return NULL;
//		}
//	}
	
	
//	Handle<Reducts> reducts = dynamic_cast(Reducts *, oldRules->FindParent(REDUCTS));
	
	// Remove the structure.  Handles take care of the physical deletion if needed.
//	if (!reducts->RemoveChild(reducts->FindChild(dynamic_cast(Structure *, oldRules.GetPointer())))) {
//		Message::Error("An error occurred while removing " + oldRules->GetName() + " from reducts.");

//	}
	


	newRules->SetName("Tuned"+oldRules->GetName());
	
	// Append rule set as a child to the old rules set.
	if (!oldRules->IsParent(newRules.GetPointer(), true))
		oldRules->AppendChild(newRules.GetPointer());

	Handle<DecisionTable> newtable = dynamic_cast(DecisionTable *, newRules->FindParent(DECISIONTABLE));
	if (newtable == NULL)
		Message::Warning("Convert: cant find parent table for new rules");
	//Message::Warning("Convert: cant find parent table for new rules");
	//Handle<DecisionTable> table = dynamic_cast(DecisionTable *, oldRules->FindParent(DECISIONTABLE));
	return newRules;
};

//-------------------------------------------------------------------
// Method........: UseIDG
// Author........: Aleksander hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleTuner::UseIDG() const {
	return idg_;
}

bool
RuleTuner::UseIDG(bool idg) {
	idg_ = idg;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetIDGFilename/SetIDGFilename
// Author........: Aleksander hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
RuleTuner::GetIDGFilename() const {
	return filename_idg_;
}

bool
RuleTuner::SetIDGFilename(const String &filename) {
	filename_idg_ = filename;
	return true;
}

 
