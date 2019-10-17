//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REMOVALCOMPLETER_H__
#define __REMOVALCOMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/completer.h>

//-------------------------------------------------------------------
// Class.........: RemovalCompleter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns a decision table with all objects with
//                 missing entries removed.
// Revisions.....:
//===================================================================

class RemovalCompleter : public Completer {
public:

  //- Constructors/destructor........................................
  RemovalCompleter();
  virtual ~RemovalCompleter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual Structure *Apply(Structure &structure) const;
	virtual RemovalCompleter	*Clone();

};

#endif 
