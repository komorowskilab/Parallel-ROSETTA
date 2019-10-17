//-------------------------------------------------------------------
// Author........:
// Date..........:
// Description...:
// Revisions.....: AÛ 000324: Added an #include for the DecisionTable
//                            class. Otherwise, breaks under egcs on
//                            Sun.
//===================================================================

#ifndef __NAIVEBAYESCLASSIFIER_H__
#define __NAIVEBAYESCLASSIFIER_H__

#include <copyright.h>

#include <kernel/algorithms/classifier.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/pair.h>
#include <kernel/basic/map.h>

//-------------------------------------------------------------------
// Class.........: NaiveBayesClassifier
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simple naive Bayes classifier.
// Revisions.....:
//===================================================================

class NaiveBayesClassifier : public Classifier {
protected:

	//- Type definition................................................
	typedef Pair(int, int)      IIPair;              // Represents a (condition value | decision value) pair.
	typedef Map(IIPair, double) CPMap;               // Maps each (condition value | decision value) pair to a probability.
	typedef Map(int, double)    PPMap;               // Maps each decision value to to prior probability.

	//- Member variables...............................................
	Handle<DecisionTable>       table_;              // Table to estimate probabilities from.
	int                         decision_attribute_; // Index of decision attribute.
  Vector(CPMap)               conditionals_;       // The conditional probabilities, one map per attribute.
	PPMap                       priors_;             // The prior probabilities.

public:

  //- Constructors/destructor........................................
  NaiveBayesClassifier();
  virtual ~NaiveBayesClassifier();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String              GetParameters() const;
	virtual bool                SetParameter(const String &keyword, const String &value);
	virtual Structure          *Apply(Structure &structure) const;
	virtual	NaiveBayesClassifier	*Clone();

  //- Methods inherited from Classifier..............................
	virtual bool                SetRules(const Rules *rules);
	virtual bool                IsCompatible(const DecisionTable &table, bool masked) const;
	virtual bool                Initialize(const DecisionTable &table, bool masked);

  //- Local methods..................................................
	Handle<DecisionTable>       GetDecisionTable() const {return table_;}
	bool                        SetDecisionTable(Handle<DecisionTable> table) {table_ = table; return !table_.IsNULL();}

};

#endif
 
