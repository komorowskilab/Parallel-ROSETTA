//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DECISIONTABLEIMPORTER_H__
#define __DECISIONTABLEIMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/importer.h>

//-------------------------------------------------------------------
// Class.........: DecisionTableImporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms dealing with importing
//                 alien format decision tables.
// Revisions.....:
//===================================================================

class DecisionTableImporter : public Importer {
public:

  //- Constructors/destructor........................................
  DecisionTableImporter();
  virtual ~DecisionTableImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual bool IsApplicable(const Structure &structure, bool warn = true) const;

};

#endif 
