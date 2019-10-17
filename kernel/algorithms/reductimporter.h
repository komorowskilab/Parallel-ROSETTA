//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTIMPORTER_H__
#define __REDUCTIMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/importer.h>

//-------------------------------------------------------------------
// Class.........: ReductImporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms dealing with importing
//                 alien format reduct sets.
// Revisions.....:
//===================================================================

class ReductImporter : public Importer {
public:

  //- Constructors/destructor........................................
  ReductImporter();
  virtual ~ReductImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual bool IsApplicable(const Structure &structure, bool warn = true) const;
	virtual ReductImporter	*Clone() = 0;

};

#endif 
