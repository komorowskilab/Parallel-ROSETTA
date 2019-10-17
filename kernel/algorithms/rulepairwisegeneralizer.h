//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEPAIRWISEGENERALIZER_H__
#define __RULEPAIRWISEGENERALIZER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/algorithms/rulegeneralizer.h>
#include <kernel/structures/rule.h>
#include <kernel/basic/set.h>
#include <kernel/basic/map.h>
//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structures;

//-------------------------------------------------------------------
// Class.........: RulePairwiseGeneralizer
// Author........: Ewa Makosa
// Date..........:
// Description...: A RulePairwiseGeneralizer is an algorithm that operates on a set of
//				   structures, and changes members of this set - 
//					generalizes rules in pairwise manner
// Revisions.....: 
//===================================================================


class RulePairwiseGeneralizer : public RuleGeneralizer {
protected:
	int			attributeDifference_;        // the number of attributes the two suitable rules differ on


	//- Tuning.............................................
	virtual Structure*	Tune(const Structures &structures) const;
	virtual void		GeneralizeRules(DecisionSubsets &decisionSubsets, Handle<GeneralRules> rules)const;
	bool				Improve(const DecisionTable &decisionTable, const Discerner &discerner, Handle<GeneralRules> rules, DecisionSubsets &decisionSubsets, const DecisionSet &ds_base, RuleGroup &ruleGroup, const int baseInd)const;
	bool				GeneralizeRule(const DecisionTable &decisionTable, const Discerner &discerner, RuleGroup &ruleGroup, const int baseInd, int &maxInd)const;
	bool				SuitablePair(Handle<GeneralRule> base, Handle<GeneralRule> rule)const;
	float				GeneralizePair(const DecisionTable &decisionTable, const Discerner &discerner, const Handle<GeneralRule> fromGen, Handle<GeneralRule> toGen) const;
	bool				InsertToRuleGroup(RuleGroup &ruleGroup, Handle<GeneralRule> rule, int ruleInd)const;
	bool				DecisionChanged(const DecisionSet &ds1, const Handle<GeneralRule> rule, DecisionSet &ds2)const;

public:

	//- Constructors/destructor........................................
	RulePairwiseGeneralizer();
	virtual ~RulePairwiseGeneralizer();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String				GetParameters() const;
	virtual bool				SetParameter(const String &keyword, const String &value);
	virtual bool				IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure			*Apply(Structure &structure) const;
	virtual RulePairwiseGeneralizer	*Clone();
	
	virtual	int		GetAttributeDifference()const {return attributeDifference_;}
	virtual	bool    SetAttributeDifference(int attributeDifference) {attributeDifference_ = attributeDifference; return true;}
		
};

#endif
 
