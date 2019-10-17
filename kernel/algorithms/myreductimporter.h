//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYREDUCTIMPORTER_H__
#define __MYREDUCTIMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductimporter.h>

//-------------------------------------------------------------------
// Class.........: MyReductImporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class MyReductImporter : public ReductImporter {
public:

  //- Constructors/destructor........................................
  MyReductImporter();
  virtual ~MyReductImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual Structure *Apply(Structure &structure) const;
	virtual MyReductImporter	*Clone();
};

#endif 
