//-------------------------------------------------------------------
// Author........:
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CLASSIFIER_H__
#define __CLASSIFIER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Rules;

//-------------------------------------------------------------------
// Class.........: Classifier
// Author........:
// Date..........:
// Description...: Base class for classifiers.  Can be subclassed to
//                 allow for various different classification schemes,
//                 e.g. rule-based classifiers, neural networks,
//                 statistical classifiers etc.
// Revisions.....:
//===================================================================

class Classifier : public Algorithm {
public:

  //- Constructors/destructor........................................
  Classifier();
  virtual ~Classifier();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Classifier		*Clone() = 0;

  //- Classifier methods.............................................
	virtual bool SetRules(const Rules *rules);
	virtual bool IsCompatible(const DecisionTable &table, bool masked) const;
	virtual bool Initialize(const DecisionTable &table, bool masked);
	virtual bool Reset();

};

#endif 
