//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ADXDECISIONTABLEEXPORTER_H__
#define __ADXDECISIONTABLEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>

//-------------------------------------------------------------------
// Class.........: AdxDecisionTableExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a decision table to dmLab .adx format
// Revisions.....:
//===================================================================

class AdxDecisionTableExporter : public DecisionTableExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from DecisionTableExporter...................
	virtual bool ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const;

public:

  //- Constructors/destructor........................................
  AdxDecisionTableExporter();
  virtual ~AdxDecisionTableExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// Methods inherited from Algorithm ..........
	virtual AdxDecisionTableExporter		*Clone();

};

#endif 
