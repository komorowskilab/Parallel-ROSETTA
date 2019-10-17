//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYREDUCTEXPORTER_H__
#define __MYREDUCTEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductexporter.h>

//-------------------------------------------------------------------
// Class.........: MyReductExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of reducts to plain "native" format.
// Revisions.....:
//===================================================================

class MyReductExporter : public ReductExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from ReductExporter..........................
	virtual bool ExportReduct(ofstream &stream, int reduct_no, const Reduct &reduct) const;

public:

  //- Constructors/destructor........................................
  MyReductExporter();
  virtual ~MyReductExporter();

	virtual MyReductExporter   *Clone();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

};

#endif 
