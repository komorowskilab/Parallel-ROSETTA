//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DECISIONTABLEEXPORTER_H__
#define __DECISIONTABLEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/exporter.h>

//-------------------------------------------------------------------
// Class.........: DecisionTableExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for exporting decision tables to alien
//                 formats.
// Revisions.....:
//===================================================================

class DecisionTableExporter : public Exporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool       ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool       ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool       ExportEpilogue(ofstream &stream, const Structure &structure) const;

protected:

	//- Overloadable for various formats...............................
	virtual bool       ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const = 0;

public:

  //- Constructors/destructor........................................
  DecisionTableExporter();
  virtual ~DecisionTableExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
	virtual DecisionTableExporter	*Clone() = 0;

};

#endif 
