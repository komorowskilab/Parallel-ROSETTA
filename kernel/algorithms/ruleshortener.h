//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULESHORTENER_H__
#define __RULESHORTENER_H__

#include <copyright.h>

#include <kernel/algorithms/ruletuner.h>
#include <kernel/structures/generalrule.h>
#include <kernel/utilities/discerner.h>
#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structures;

//-------------------------------------------------------------------
// Class.........: RuleShortener
// Author........: Ewa Makosa
// Date..........:
// Description...: A RuleShortener is an algorithm that operates on a set of
//                 structures, and changes members of this set - preformes
//			       a fine tuning by removing a number of the descriptors
// Revisions.....: 
//===================================================================

class RuleShortener : public RuleTuner {

protected:

	//- Tuning.............................................
	virtual Structure*			Tune(const Structures &structures) const;
	virtual Handle<GeneralRule>	Shorten(const Discerner &discerner, const DecisionTable &decisiontable, Handle<GeneralRule> rule)const;

public:

  //- Constructors/destructor........................................
  RuleShortener();
  virtual ~RuleShortener();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String				GetParameters() const;
	virtual bool				SetParameter(const String &keyword, const String &value);
	virtual bool				IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure			*Apply(Structure &structure) const;
	virtual RuleShortener	*Clone();
};

#endif
 
