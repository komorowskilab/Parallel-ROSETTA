//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MEANCOMPLETER_H__
#define __MEANCOMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/completer.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: MeanCompleter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns a decision table with all missing entries
//                 filled with the mean (mode) for numeric (symbolic)
//                 attributes.
// Revisions.....:
//===================================================================

class MeanCompleter : public Completer {
protected:

	//- Local methods..................................................
	bool               CalculateMeans(Vector(int) &means, const DecisionTable &table, bool masked) const;

public:

  //- Constructors/destructor........................................
  MeanCompleter();
  virtual ~MeanCompleter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual Structure *Apply(Structure &structure) const;
	virtual MeanCompleter	*Clone();

};

#endif 
