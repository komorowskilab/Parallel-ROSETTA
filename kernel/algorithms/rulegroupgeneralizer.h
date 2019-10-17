//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEGROUPGENERALIZER_H__
#define __RULEGROUPGENERALIZER_H__

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
// Class.........: RuleGroupGeneralizer
// Author........: Ewa Makosa
// Date..........:
// Description...: A RuleGroupGeneralizer is an algorithm that operates on a set of
//				   structures, and changes members of this set - preformes
//				   a fine tuning by generalizing a number of the descriptors
// Revisions.....: 
//===================================================================


class RuleGroupGeneralizer : public RuleGeneralizer {
 
protected:
	int coverage_;

	typedef Pair(int, int) AttrPos;
	typedef Vector(AttrPos) ToGeneralize;

public:
  
	typedef Vector(RuleGroup)					RuleGroups;

protected:
	//- Tuning.............................................
	virtual Structure*			Tune(const Structures &structures) const;
	virtual void 				GroupRules(DecisionSubsets &decisionSubsets, RuleGroups& ruleGroups)const;
	virtual void				GeneralizeRules(DecisionSubsets &decisionSubsets, Handle<GeneralRules> rules)const;
	virtual Handle<GeneralRule>	Generalize(const Discerner &discerner, const DecisionTable &decisiontable, Handle<GeneralRule> base, const RuleGroup &ruleGroup)const;
	float						GeneralizeDescriptor(const Discerner &discerner, const DecisionTable &decisionTable, int attribute, int attributePos, const RuleGroup &ruleGroup, Handle<GeneralRule> ruleGen)const;

public:

	//- Constructors/destructor........................................
	RuleGroupGeneralizer();
	virtual ~RuleGroupGeneralizer();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String				GetParameters() const;
	virtual bool				SetParameter(const String &keyword, const String &value);
	virtual bool				IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure			*Apply(Structure &structure) const;
	virtual RuleGroupGeneralizer	*Clone();

	virtual	int		GetCoverageThreshold()const {return coverage_;}
	virtual	bool    SetCoverageThreshold(int coverage) {coverage_ = coverage; return true;}

};

#endif

 
