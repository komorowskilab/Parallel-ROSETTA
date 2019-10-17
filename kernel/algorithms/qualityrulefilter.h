//-------------------------------------------------------------------
// Author........: Thomas Êgotnes/Aleksander Ûhrn
// Date..........: 981110
// Description...:
// Revisions.....:
//===================================================================

#ifndef __QUALITYRULEFILTER_H__
#define __QUALITYRULEFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/rulefilter.h>
#include <kernel/algorithms/ruleevaluator.h>

#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Class.........: QualityRuleFilter
// Author........: Thomas Êgotnes/Aleksander Ûhrn
// Date..........: 981110
// Description...: Filters away rules according to selected
//                 quality measures.
// Comments......:
// Revisions.....:
//===================================================================

class QualityRuleFilter : public RuleFilter {
public:

	//- Enumerated types...............................................
	enum Filtering  {FILTERING_MICHALSKI, FILTERING_TORGO,   FILTERING_BRAZDIL,
		               FILTERING_PEARSON,   FILTERING_G2,      FILTERING_J,
									 FILTERING_COHEN,     FILTERING_COLEMAN, FILTERING_KONONENKO};

	//- Textual representations........................................
	static String                       GetString(Filtering filtering);

protected:

  //- Parameters.....................................................
	Filtering                           filtering_;     // Pruning strategy/criterion.
	float										            weight_;        // For Michalski measure.
	ColemanRuleEvaluator::Normalization normalization_; // For Coleman measure.
	bool										            use_upper_;
	bool										            use_lower_;
	float										            upper_;
	float										            lower_;
	bool                                remove_undefined_;
	Handle<RuleEvaluator>               evaluator_;

protected:

	//- Methods inherited from Filter..................................
	virtual bool                        Remove(const Structures &structures, int i) const;

public:

  //- Constructors/destructor........................................
  QualityRuleFilter();
  virtual ~QualityRuleFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String                      GetParameters() const;
	virtual bool                        SetParameter(const String &keyword, const String &value);
	virtual Structure                  *Apply(Structure &structure) const;
	virtual QualityRuleFilter				*Clone();

	//- Parameter management...........................................
	Filtering                           GetFilteringStrategy() const {return filtering_;}
	bool                                SetFilteringStrategy(Filtering filtering) {filtering_ = filtering; return true;}

	float										            GetMichalskiWeight() const {return weight_;}
	bool                                SetMichalskiWeight(float weight) {weight_ = weight; return true;}

	ColemanRuleEvaluator::Normalization GetColemanNormalization() const {return normalization_;}
	bool                                SetColemanNormalization(ColemanRuleEvaluator::Normalization normalization) {normalization_ = normalization; return true;}

	bool										            UseUpper() const {return use_upper_;}
	bool                                UseUpper(bool use) {use_upper_ = use; return true;}

	bool										            UseLower() const {return use_lower_;}
	bool                                UseLower(bool use) {use_lower_ = use; return true;}

	float										            GetUpper() const {return upper_;}
	bool                                SetUpper(float value) {upper_ = value; return true;}

	float										            GetLower() const {return lower_;}
	bool                                SetLower(float value) {lower_ = value; return true;}

	bool										            RemoveUndefinedRules() const {return remove_undefined_;}
	bool                                RemoveUndefinedRules(bool remove) {remove_undefined_ = remove; return true;}

	//- Evaluator creation.............................................
	Handle<RuleEvaluator>               CreateEvaluator(const Structure &structure) const;

};

#endif 
