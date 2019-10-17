//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEBASEDCLASSIFICATION_H__
#define __RULEBASEDCLASSIFICATION_H__

#include <copyright.h>

#include <kernel/structures/classification.h>
#include <kernel/structures/rule.h>

//-------------------------------------------------------------------
// Class.........: RuleBasedClassification
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Structure returned by (single object) rule-based
//                 classifiers.  A specialization of a more general
//                 classification structure, but with the addition of
//                 being able to access the rules supporting each
//                 possible decision class.
// Comments......:
// Revisions.....:
//===================================================================

class RuleBasedClassification : public Classification {
private:

	//- Private member variables.......................................
	Vector(Rule::Handles)     rules_; // Rules supporting each possible decision.

protected:

  //- Constructors...................................................
  RuleBasedClassification(const RuleBasedClassification &in);

  //- Methods inherited from Classification..........................
	virtual void              SwapData(int i, int j);

public:

  //- Constructors/destructor........................................
  RuleBasedClassification();
  virtual ~RuleBasedClassification();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual void              Clear();
	virtual Structure        *Duplicate() const;

  //- Methods inherited from Classification..........................
	virtual bool              InsertDecisionValue(int value, float certainty, int i);
	virtual bool              RemoveDecisionValue(int i);

	virtual int               GetNoRules(int i) const;

  //- Supporting rule management.....................................
	const Rule::Handles      &GetRules(int i) const;
	bool                      SetRules(const Rule::Handles &rules, int i);

};

#endif
 
