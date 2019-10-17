//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...: implementation of MyRuleFilter for general rules
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/mygeneralrulefilter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/generalrule.h>
#include <kernel/structures/generalrules.h>

//-------------------------------------------------------------------
// Methods for class MyGeneralRuleFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyGeneralRuleFilter::MyGeneralRuleFilter() {
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

MyGeneralRuleFilter::~MyGeneralRuleFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyGeneralRuleFilter, MYGENERALRULEFILTER, RuleFilter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: 
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

String
MyGeneralRuleFilter::GetParameters() const {

	return MyRuleFilter::GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: 
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

bool
MyGeneralRuleFilter::SetParameter(const String &keyword, const String &value) {

	return MyRuleFilter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: EwaMakosa
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
MyGeneralRuleFilter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(GENERALRULES);
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
// Comments......: Ewa Makosa:  Filtering according to value of condition 
//					attribute adjusted to general rules -- if one of the
//					values matches the criterion, remove
//
// Revisions.....: Aš   0797: Allow compound filtering.
//                 Aš 071197: Introduced dominating decision.
//                 Aš 020198: Introduced stability filtering.
//                 Aš 230198: Introduced connective.
//===================================================================

bool
MyGeneralRuleFilter::Remove(const Structures &structures, int i) const {

	if (!structures.IsA(GENERALRULES))
		return false;

	Handle<GeneralRules> rules = dynamic_cast(GeneralRules *, const_cast(Structures *, &structures));
	Handle<GeneralRule>  rule  = rules->GetRule(i);

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
			
			GeneralRule::ValueSet::const_iterator value_it = rule->GetConditionValueIterator(j);
		
			int no_values = rule->GetNoConditionValues(j);
			int tmp_value, v;
	
			for (v = 0; v < no_values; v++){
				tmp_value = *value_it;
				if (tmp_value == value) {
					prune = true;
					break;
				}
				value_it++;
			}

		}
		if (is_or && prune)		           // Short-circuit evaluation.
			return true;
		if (is_and && !prune)		         // Short-circuit evaluation.
			return false;
	}

	return prune;

}

 
MyGeneralRuleFilter *
MyGeneralRuleFilter::Clone() {
	return new MyGeneralRuleFilter;
}
