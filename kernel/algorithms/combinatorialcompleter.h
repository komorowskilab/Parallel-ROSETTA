//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __COMBINATORIALCOMPLETER_H__
#define __COMBINATORIALCOMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/completer.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: CombinatorialCompleter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns a decision table with all missing entries
//                 filled, expanded so as to account for all possible
//                 combinations.
// Revisions.....:
//===================================================================

class CombinatorialCompleter : public Completer {
protected:

	//- Local methods..................................................
	bool               CalculateValueSets(Vector(Vector(int)) &values, const DecisionTable &table, bool masked) const;

public:

  //- Constructors/destructor........................................
  CombinatorialCompleter();
  virtual ~CombinatorialCompleter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual Structure *Apply(Structure &structure) const;
	virtual CombinatorialCompleter	*Clone();

};

#endif 
