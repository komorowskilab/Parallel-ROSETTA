//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REPORTER_H__
#define __REPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/exporter.h>

//-------------------------------------------------------------------
// Class.........: Reporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Generates a report (history/comments) to a given
//                 file, of a given structure and its children.
//
//                 Serves as a base class for report generators to
//                 different formats.
// Revisions.....:
//===================================================================

class Reporter : public Exporter {
public:

  //- Constructors/destructor........................................
  Reporter();
  virtual ~Reporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Reporter 	*Clone();

};

#endif 
