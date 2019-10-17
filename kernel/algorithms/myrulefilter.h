//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYRULEFILTER_H__
#define __MYRULEFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/rulefilter.h>

//-------------------------------------------------------------------
// Class.........: MyRuleFilter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Description...: Filters away rules based on a compound
//                 criterion of support, accuracy and decision.
// Comments......:
// Revisions.....:
//===================================================================

class MyRuleFilter : public RuleFilter {
public:

	//- Enumerated types...............................................
	enum Filtering {FILTERING_RHS_SUPPORT = 1, FILTERING_RHS_ACCURACY = 2,
		              FILTERING_DECISION = 4, FILTERING_LHS_LENGTH = 8,
	                FILTERING_CONDITION = 16, FILTERING_RHS_STABILITY = 32,
									FILTERING_RHS_COVERAGE = 64};
	enum Connective {CONNECTIVE_OR, CONNECTIVE_AND};

	//- Textual representations........................................
	static String           GetString(Filtering filtering);
	static String           GetString(Connective connective);

protected:

  //- Parameters.....................................................
	int                     filtering_;                   // Pruning strategy/criterion.
	Connective              connective_;                  // Connective between terms.

	int                     decision_;                    // Remove rules with this decision value.
	bool                    is_dominating_;               // The decision value must be the dominating one.

	int                     lower_rhs_support_threshold_; // Remove rule if support not above this threshold and..
	int                     upper_rhs_support_threshold_; //                ...not below this threshold.

	float                   lower_rhs_acc_threshold_;     // Remove rule if accuracy not above this threshold and..
	float                   upper_rhs_acc_threshold_;     //                ...not below this threshold.

	float                   lower_rhs_cov_threshold_;     // Remove rule if coverage not above this threshold and..
	float                   upper_rhs_cov_threshold_;     //                ...not below this threshold.

	double                  lower_rhs_stab_threshold_;    // Remove rule if stability not above this threshold and..
	double                  upper_rhs_stab_threshold_;    //                ...not below this threshold.

	int                     lower_lhs_length_threshold_;  // Remove rule if length not above this threshold and..
	int                     upper_lhs_length_threshold_;  //                ...not below this threshold.

	int                     value_;                       // Remove rule if it has this value...
	int                     attribute_;                   //                ...for this condition attribute.

protected:

	//- Methods inherited from Filter..................................
	virtual bool            Remove(const Structures &structures, int i) const;

public:

  //- Constructors/destructor........................................
  MyRuleFilter();
  virtual ~MyRuleFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String          GetParameters() const;
	virtual bool            SetParameter(const String &keyword, const String &value);
	virtual	MyRuleFilter	*Clone();

	//- Ewa Makosa 03.06.2005
	// Added to obtain compatibility with General Rules
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;

	//- Parameter management...........................................
	int                     GetFilteringStrategy() const {return filtering_;}
	bool                    SetFilteringStrategy(int filtering) {filtering_ = filtering; return true;}

	Connective              GetFilteringConnective() const {return connective_;}
	bool                    SetFilteringConnective(Connective connective) {connective_ = connective; return true;}

	int                     GetDecision() const {return decision_;}
	bool                    SetDecision(int decision) {decision_ = decision; return true;}

	bool                    IsDominatingDecision() const {return is_dominating_;}
	bool                    IsDominatingDecision(bool is_dominating) {is_dominating_ = is_dominating; return true;}

	int                     GetLowerRHSSupportThreshold() const {return lower_rhs_support_threshold_;}
	bool                    SetLowerRHSSupportThreshold(int lower_rhs_support_threshold) {lower_rhs_support_threshold_ = lower_rhs_support_threshold; return true;}

	int                     GetUpperRHSSupportThreshold() const {return upper_rhs_support_threshold_;}
	bool                    SetUpperRHSSupportThreshold(int upper_rhs_support_threshold) {upper_rhs_support_threshold_ = upper_rhs_support_threshold; return true;}

	float                   GetLowerRHSAccuracyThreshold() const {return lower_rhs_acc_threshold_;}
	bool                    SetLowerRHSAccuracyThreshold(float lower_rhs_acc_threshold) {lower_rhs_acc_threshold_ = lower_rhs_acc_threshold; return true;}

	float                   GetUpperRHSAccuracyThreshold() const {return upper_rhs_acc_threshold_;}
	bool                    SetUpperRHSAccuracyThreshold(float upper_rhs_acc_threshold) {upper_rhs_acc_threshold_ = upper_rhs_acc_threshold; return true;}

	float                   GetLowerRHSCoverageThreshold() const {return lower_rhs_cov_threshold_;}
	bool                    SetLowerRHSCoverageThreshold(float lower_rhs_cov_threshold) {lower_rhs_cov_threshold_ = lower_rhs_cov_threshold; return true;}

	float                   GetUpperRHSCoverageThreshold() const {return upper_rhs_cov_threshold_;}
	bool                    SetUpperRHSCoverageThreshold(float upper_rhs_cov_threshold) {upper_rhs_cov_threshold_ = upper_rhs_cov_threshold; return true;}

	double                  GetLowerRHSStabilityThreshold() const {return lower_rhs_stab_threshold_;}
	bool                    SetLowerRHSStabilityThreshold(double lower_rhs_stab_threshold) {lower_rhs_stab_threshold_ = lower_rhs_stab_threshold; return true;}

	double	                GetUpperRHSStabilityThreshold() const {return upper_rhs_stab_threshold_;}
	bool                    SetUpperRHSStabilityThreshold(double upper_rhs_stab_threshold) {upper_rhs_stab_threshold_ = upper_rhs_stab_threshold; return true;}

	int                     GetLowerLHSLengthThreshold() const {return lower_lhs_length_threshold_;}
	bool                    SetLowerLHSLengthThreshold(int lower_lhs_length_threshold) {lower_lhs_length_threshold_ = lower_lhs_length_threshold; return true;}

	int                     GetUpperLHSLengthThreshold() const {return upper_lhs_length_threshold_;}
	bool                    SetUpperLHSLengthThreshold(int upper_lhs_length_threshold) {upper_lhs_length_threshold_ = upper_lhs_length_threshold; return true;}

	int                     GetAttribute() const {return attribute_;}
	bool                    SetAttribute(int attribute) {attribute_ = attribute; return true;}

	int                     GetAttributeValue() const {return value_;}
	bool                    SetAttributeValue(int value) {value_ = value; return true;}

};

#endif 
