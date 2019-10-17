//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEBASEDCLASSIFIER_H__
#define __RULEBASEDCLASSIFIER_H__

#include <copyright.h>

#include <kernel/algorithms/classifier.h>

#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Class.........: RuleBasedClassifier
// Author........: Aleksander Ûhrn
// Date..........: 9608/9609
// Description...: Base class for rule-based classifiers.
// Revisions.....:
//===================================================================

class RuleBasedClassifier : public Classifier {
protected:

	//- Classifying rules..............................................
  	Handle<Rules> rules_;

public:

  //- Constructors/destructor........................................
	RuleBasedClassifier();
  virtual ~RuleBasedClassifier();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const = 0;
	virtual bool       SetParameter(const String &keyword, const String &value) = 0;
	virtual RuleBasedClassifier	*Clone() = 0;

	//- Methods inherited from Classifier..............................
	virtual bool       IsCompatible(const DecisionTable &table, bool masked) const;
	virtual bool       SetRules(const Rules *rules);
	// -N 24/05/13
//	virtual bool		SetRules(const Rules rules);

  //- Parameter management...........................................
	virtual Rules     *GetRules() const;
};

#endif

 
