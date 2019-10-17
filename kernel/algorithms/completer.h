//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __COMPLETER_H__
#define __COMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class.........: Completer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Takes a decision table (presumably with missing
//                 values), and produces a complete table (i.e. a table
//                 with no missing values) according to some scheme.
// Revisions.....:
//===================================================================

class Completer : public Algorithm {
public:

  //- Constructors/destructor........................................
  Completer();
  virtual ~Completer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual bool IsApplicable(const Structure &structure, bool warn = true) const;

};

#endif 
