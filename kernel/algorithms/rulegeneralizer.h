//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEGENERALIZER_H__
#define __RULEGENERALIZER_H__

#include <copyright.h>

#include <kernel/algorithms/ruletuner.h>
#include <kernel/basic/set.h>
#include <kernel/basic/map.h>
//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structures;

//-------------------------------------------------------------------
// Class.........: RuleGeneralizer
// Author........: Ewa Makosa
// Date..........:
// Description...: A RuleGeneralizer is an algorithm that operates on a set of
//				   structures, and changes members of this set - preformes
//				   a fine tuning by removing a number of the descriptors
// Revisions.....: 
//===================================================================

struct RuleHandle{

	Handle<GeneralRule> ruleHandle_;
	bool operator< (const RuleHandle &in) const{
		//to have it descending according to the length
		return ((in.ruleHandle_)->GetNoConditionDescriptors() < ruleHandle_->GetNoConditionDescriptors());
	}


};


class RuleGeneralizer : public RuleTuner {

public:
	typedef Vector(RuleHandle)					RuleGroup;
	typedef Map(const DecisionSet, RuleGroup)	DecisionSubsets;

protected:
	//- Tuning.............................................
	virtual Structure*	Tune(const Structures &structures) const = 0;
	virtual void		GeneralizeRules(DecisionSubsets &decisionSubsets, Handle<GeneralRules> rules)const = 0;
	virtual void		CreateRuleSubsets(const Handle<GeneralRules> rules, DecisionSubsets &decisionSubsets)const;
	void				AssignToDecisionSubset(DecisionSubsets &decisionSubsets, const DecisionSet &decisionSet, const Handle<GeneralRule> rule)const;
	bool				RemoveDuplicateRules(Handle<GeneralRule> rule, Handle<GeneralRules> rules)const;
public:

	//- Constructors/destructor........................................
	RuleGeneralizer();
	virtual ~RuleGeneralizer();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String		GetParameters() const;
	virtual bool		SetParameter(const String &keyword, const String &value);
	virtual bool		IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure	*Apply(Structure &structure) const = 0;

};

#endif
 
