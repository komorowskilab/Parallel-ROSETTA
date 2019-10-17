//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/myrulefilter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/rule.h>
#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Methods for class MyRuleFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyRuleFilter::MyRuleFilter() {
	SetFilteringStrategy(0);
	SetFilteringConnective(CONNECTIVE_OR);
	SetDecision(Undefined::Integer());
	IsDominatingDecision(false);
	SetLowerRHSSupportThreshold(0);
	SetUpperRHSSupportThreshold(4);
	SetLowerRHSAccuracyThreshold(0.0f);
	SetUpperRHSAccuracyThreshold(0.75f);
	SetLowerRHSCoverageThreshold(0.0f);
	SetUpperRHSCoverageThreshold(0.05f);
	SetLowerRHSStabilityThreshold(0.0);
	SetUpperRHSStabilityThreshold(0.5);
	SetLowerLHSLengthThreshold(5);
	SetUpperLHSLengthThreshold(1000);
	SetAttribute(Undefined::Integer());
	SetAttributeValue(Undefined::Integer());
}

MyRuleFilter::~MyRuleFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyRuleFilter, MYRULEFILTER, RuleFilter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Allow compound filtering.
//===================================================================

String
MyRuleFilter::GetParameters() const {

	String parameters;

	// Filtering strategy (compound).
	parameters += Keyword::Filtering();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetFilteringStrategy());

	parameters += Keyword::Separator();

	// Filtering connective.
	parameters += Keyword::Connective();
	parameters += Keyword::Assignment();
	parameters += GetString(GetFilteringConnective());

	// Support parameters.
	if (GetFilteringStrategy() & FILTERING_RHS_SUPPORT) {

		parameters += Keyword::Separator();

		parameters += Keyword::Support() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerRHSSupportThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Support() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperRHSSupportThreshold());

	}

	// Accuracy parameters.
	if (GetFilteringStrategy() & FILTERING_RHS_ACCURACY) {

		parameters += Keyword::Separator();

		parameters += Keyword::Accuracy() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerRHSAccuracyThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Accuracy() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperRHSAccuracyThreshold());

	}

	// Coverage parameters.
	if (GetFilteringStrategy() & FILTERING_RHS_COVERAGE) {

		parameters += Keyword::Separator();

		parameters += Keyword::Coverage() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerRHSCoverageThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Coverage() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperRHSCoverageThreshold());

	}

	// Stability parameters.
	if (GetFilteringStrategy() & FILTERING_RHS_STABILITY) {

		parameters += Keyword::Separator();

		parameters += Keyword::Stability() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerRHSStabilityThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Stability() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperRHSStabilityThreshold());

	}

	// Decision parameters.
	if (GetFilteringStrategy() & FILTERING_DECISION) {

		parameters += Keyword::Separator();

		parameters += Keyword::Decision();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetDecision());

		parameters += Keyword::Separator();

		parameters += Keyword::Dominate();
		parameters += Keyword::Assignment();
		parameters += String::Format(IsDominatingDecision());

	}

	// Length parameters.
	if (GetFilteringStrategy() & FILTERING_LHS_LENGTH) {

		parameters += Keyword::Separator();

		parameters += Keyword::Length() + Keyword::Dot() + Keyword::LHS() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerLHSLengthThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Length() + Keyword::Dot() + Keyword::LHS() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperLHSLengthThreshold());

	}

	// Attribute value parameters.
	if (GetFilteringStrategy() & FILTERING_CONDITION) {

		parameters += Keyword::Separator();

		parameters += Keyword::Attribute();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetAttribute());

		parameters += Keyword::Separator();

		parameters += Keyword::Value();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetAttributeValue());

	}

	parameters += Keyword::Separator();

	return parameters + RuleFilter::GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Allow compound filtering.
//===================================================================

bool
MyRuleFilter::SetParameter(const String &keyword, const String &value) {

	// Filtering strategy.
	if (keyword == Keyword::Filtering() && value.IsInteger())
		return SetFilteringStrategy(value.GetInteger());

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_DECISION))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_DECISION);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_RHS_SUPPORT))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_RHS_SUPPORT);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_RHS_ACCURACY))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_RHS_ACCURACY);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_RHS_COVERAGE))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_RHS_COVERAGE);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_RHS_STABILITY))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_RHS_STABILITY);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_LHS_LENGTH))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_LHS_LENGTH);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_CONDITION))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_CONDITION);

	// Filtering connective.
	if (keyword == Keyword::Connective() && value == GetString(CONNECTIVE_OR))
		return SetFilteringConnective(CONNECTIVE_OR);

	if (keyword == Keyword::Connective() && value == GetString(CONNECTIVE_AND))
		return SetFilteringConnective(CONNECTIVE_AND);

	// Lower support threshold.
	if ((keyword == Keyword::Support() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerRHSSupportThreshold(value.GetInteger());

	// Lower support threshold (backwards compatibility).
	if ((keyword == Keyword::Support() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerRHSSupportThreshold(value.GetInteger());

	// Lower support threshold (backwards compatibility).
	if (keyword == Keyword::LSThreshold() && value.IsInteger())
		return SetLowerRHSSupportThreshold(value.GetInteger());

	// Upper support threshold.
	if ((keyword == Keyword::Support() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperRHSSupportThreshold(value.GetInteger());

	// Upper support threshold (backwards compatibility).
	if ((keyword == Keyword::Support() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperRHSSupportThreshold(value.GetInteger());

	// Upper support threshold (backwards compatibility).
	if (keyword == Keyword::USThreshold() && value.IsInteger())
		return SetUpperRHSSupportThreshold(value.GetInteger());

	// Lower accuracy threshold.
	if ((keyword == Keyword::Accuracy() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower()) && value.IsFloat())
		return SetLowerRHSAccuracyThreshold(value.GetFloat());

	// Upper accuracy threshold.
	if ((keyword == Keyword::Accuracy() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper()) && value.IsFloat())
		return SetUpperRHSAccuracyThreshold(value.GetFloat());

	// Lower stability threshold.
	if ((keyword == Keyword::Stability() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower()) && value.IsFloat())
		return SetLowerRHSStabilityThreshold(value.GetFloat());

	// Lower stability threshold (backwards compatibility).
	if ((keyword == Keyword::Stability() + Keyword::Dot() + Keyword::Lower()) && value.IsFloat())
		return SetLowerRHSStabilityThreshold(value.GetFloat());

	// Upper stability threshold.
	if ((keyword == Keyword::Stability() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper()) && value.IsFloat())
		return SetUpperRHSStabilityThreshold(value.GetFloat());

	// Upper stability threshold (backwards compatibility).
	if ((keyword == Keyword::Stability() + Keyword::Dot() + Keyword::Upper()) && value.IsFloat())
		return SetUpperRHSStabilityThreshold(value.GetFloat());

	// Lower coverage threshold.
	if ((keyword == Keyword::Coverage() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Lower()) && value.IsFloat())
		return SetLowerRHSCoverageThreshold(value.GetFloat());

	// Lower coverage threshold (backwards compatibility)
	if ((keyword == Keyword::Coverage() + Keyword::Dot() + Keyword::Lower()) && value.IsFloat())
		return SetLowerRHSCoverageThreshold(value.GetFloat());

	// Upper coverage threshold.
	if ((keyword == Keyword::Coverage() + Keyword::Dot() + Keyword::RHS() + Keyword::Dot() + Keyword::Upper()) && value.IsFloat())
		return SetUpperRHSCoverageThreshold(value.GetFloat());

	// Upper coverage threshold (backwards compatibility).
	if ((keyword == Keyword::Coverage() + Keyword::Dot() + Keyword::Upper()) && value.IsFloat())
		return SetUpperRHSCoverageThreshold(value.GetFloat());

	// Decision value.
	if (keyword == Keyword::Decision() && value.IsInteger())
		return SetDecision(value.GetInteger());

	// Domination flag.
	if (keyword == Keyword::Dominate() && value.IsBoolean())
		return IsDominatingDecision(value.GetBoolean());

	// Lower length threshold.
	if ((keyword == Keyword::Length() + Keyword::Dot() + Keyword::LHS() + Keyword::Dot() + Keyword::Lower())  && value.IsInteger())
		return SetLowerLHSLengthThreshold(value.GetInteger());

	// Lower length threshold (backwards compatibility).
	if ((keyword == Keyword::Length() + Keyword::Dot() + Keyword::Lower())  && value.IsInteger())
		return SetLowerLHSLengthThreshold(value.GetInteger());

	// Lower length threshold (backwards compatibility).
	if (keyword == Keyword::LLThreshold() && value.IsInteger())
		return SetLowerLHSLengthThreshold(value.GetInteger());

	// Upper length threshold.
	if ((keyword == Keyword::Length() + Keyword::Dot() + Keyword::LHS() + Keyword::Dot() + Keyword::Upper())  && value.IsInteger())
		return SetUpperLHSLengthThreshold(value.GetInteger());

	// Upper length threshold (backwards compatibility).
	if ((keyword == Keyword::Length() + Keyword::Dot() + Keyword::Upper())  && value.IsInteger())
		return SetUpperLHSLengthThreshold(value.GetInteger());

	// Upper length threshold (backwards compatibility).
	if (keyword == Keyword::ULThreshold() && value.IsInteger())
		return SetUpperLHSLengthThreshold(value.GetInteger());

	// Attribute.
	if (keyword == Keyword::Attribute() && value.IsInteger())
		return SetAttribute(value.GetInteger());

	// Value.
	if (keyword == Keyword::Value() && value.IsInteger())
		return SetAttributeValue(value.GetInteger());

	return RuleFilter::SetParameter(keyword, value);

}
//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Ewa Makosa
// Date..........: 03.06.2005
// Description...:
// Comments......: added to obtain compatibility with general rules
// Revisions.....:
//===================================================================

bool
MyRuleFilter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	if (structure.IsA(GENERALRULES))
		return false;
	return structure.IsA(RULES);
}

//-------------------------------------------------------------------
// Methods inherited from Filter.
//===================================================================

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true is the specified rule should be removed
//                 from the rule set.
// Comments......:
// Revisions.....: Aš   0797: Allow compound filtering.
//                 Aš 071197: Introduced dominating decision.
//                 Aš 020198: Introduced stability filtering.
//                 Aš 230198: Introduced connective.
//===================================================================

bool
MyRuleFilter::Remove(const Structures &structures, int i) const {

	if (!structures.IsA(RULES))
		return false;

	Handle<Rules> rules = dynamic_cast(Rules *, const_cast(Structures *, &structures));
	Handle<Rule>  rule  = rules->GetRule(i);

	bool is_or  = (GetFilteringConnective() == CONNECTIVE_OR);
	bool is_and = (GetFilteringConnective() == CONNECTIVE_AND);

	bool prune = false;

	// Get rule dimensions.
	int no_conditions = rule->GetNoConditionDescriptors();
	int no_decisions  = rule->GetNoDecisionValues();

	int j;

	// Filter according to decision?
	if (GetFilteringStrategy() & FILTERING_DECISION) {
		prune = true;
		int max_support = -1;
		int decision_support = 0;
		for (j = 0; j < no_decisions; j++) {
			int decision = rule->GetDecisionValue(j);
			if (!IsDominatingDecision()) { // If any other decision values are present, don't remove.
				if (decision != GetDecision()) {
					prune = false;
					break;
				}
			}
			else {                         // Remove if it's the dominating decision value. Collect supports.
				int support = rule->GetSupport(j);
				if (decision == GetDecision())
					decision_support = support;
				if (support > max_support)
					max_support = support;
			}
		}
		if (IsDominatingDecision())      // Remove if it's the dominating decision value. Compare supports.
			prune = (decision_support == max_support);
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	// Filter according to rule RHS support?
	if (GetFilteringStrategy() & FILTERING_RHS_SUPPORT) {
		prune = true;
		int upper = GetUpperRHSSupportThreshold();
		int lower = GetLowerRHSSupportThreshold();
		for (j = 0; j < no_decisions; j++) {
			int support = rule->GetSupport(j);
			if ((support > upper) || (support < lower)) {
				prune = false;
				break;
			}
		}
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	// Filter according to rule RHS accuracy?
	if (GetFilteringStrategy() & FILTERING_RHS_ACCURACY) {
		prune = true;
		float upper = GetUpperRHSAccuracyThreshold();
		float lower = GetLowerRHSAccuracyThreshold();
		for (j = 0; j < no_decisions; j++) {
			float accuracy = rule->GetAccuracy(j);
			if ((accuracy > upper) || (accuracy < lower)) {
				prune = false;
				break;
			}
		}
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	// Filter according to rule RHS coverage?
	if (GetFilteringStrategy() & FILTERING_RHS_COVERAGE) {
		prune = true;
		float upper = GetUpperRHSCoverageThreshold();
		float lower = GetLowerRHSCoverageThreshold();
		for (j = 0; j < no_decisions; j++) {
			float coverage = rule->GetCoverage(j, decisions_, cardinalities_);
			if ((coverage > upper) || (coverage < lower)) {
				prune = false;
				break;
			}
		}
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	// Filter according to rule RHS stability?
	if (GetFilteringStrategy() & FILTERING_RHS_STABILITY) {
		prune = true;
		double upper = GetUpperRHSStabilityThreshold();
		double lower = GetLowerRHSStabilityThreshold();
		for (j = 0; j < no_decisions; j++) {
			double stability = rule->GetStability(j);
			if ((stability > upper) || (stability < lower)) {
				prune = false;
				break;
			}
		}
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	// Filter according to rule LHS length?
	if (GetFilteringStrategy() & FILTERING_LHS_LENGTH) {
		int upper  = GetUpperLHSLengthThreshold();
		int lower  = GetLowerLHSLengthThreshold();
		int length = no_conditions;
		prune = (length <= upper) && (length >= lower);
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	// Filter according to value of condition attribute?
	if (GetFilteringStrategy() & FILTERING_CONDITION) {
		int attribute = GetAttribute();
		int value     = GetAttributeValue();
		for (j = 0; j < no_conditions; j++) {
			if (rule->GetConditionAttribute(j) != attribute)
				continue;
			if (rule->GetConditionValue(j) == value) {
				prune = true;
				break;
			}
		}
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	return prune;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
MyRuleFilter::GetString(MyRuleFilter::Filtering filtering) {

	switch (filtering) {
		case FILTERING_DECISION:           return "Decision";
		case FILTERING_RHS_SUPPORT:        return "RHS Support";
		case FILTERING_RHS_ACCURACY:       return "RHS Accuracy";
		case FILTERING_RHS_COVERAGE:       return "RHS Coverage";
		case FILTERING_RHS_STABILITY:      return "RHS Stability";
		case FILTERING_LHS_LENGTH:         return "LHS Length";
		case FILTERING_CONDITION:          return "Condition";
		default:                           return Undefined::String();
	}

}

String
MyRuleFilter::GetString(MyRuleFilter::Connective connective) {

	switch (connective) {
		case CONNECTIVE_OR:  return "Or";
		case CONNECTIVE_AND: return "And";
		default:             return Undefined::String();
	}

}
 
MyRuleFilter *
MyRuleFilter::Clone() {
	return new MyRuleFilter;
}
