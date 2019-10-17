//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SCRIPTALGORITHM_H__
#define __SCRIPTALGORITHM_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class.........: ScriptAlgorithm
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms that are only intended
//                 used in script files.
// Revisions.....:
//===================================================================

class ScriptAlgorithm : public Algorithm {
public:

  //- Constructors/destructor........................................
  ScriptAlgorithm();
  virtual ~ScriptAlgorithm();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Identifier..............................
	virtual ScriptAlgorithm	*Clone() = 0;	
};

#endif 
