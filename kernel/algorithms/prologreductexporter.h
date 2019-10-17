//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PROLOGREDUCTEXPORTER_H__
#define __PROLOGREDUCTEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductexporter.h>

//-------------------------------------------------------------------
// Class.........: PrologReductExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of reducts to Prolog format.
// Revisions.....:
//===================================================================

class PrologReductExporter : public ReductExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from ReductExporter..........................
	virtual bool ExportReduct(ofstream &stream, int reduct_no, const Reduct &reduct) const;

public:

  //- Constructors/destructor........................................
  PrologReductExporter();
  virtual ~PrologReductExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// Algorithms
	virtual PrologReductExporter	*Clone();
};

#endif 
