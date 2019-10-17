//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ARFFDECISIONTABLEEXPORTER_H__
#define __ARFFDECISIONTABLEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>

//-------------------------------------------------------------------
// Class.........: ArffDecisionTableExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a decision table to ROSETTA import format.
// Revisions.....:
//===================================================================

class ArffDecisionTableExporter : public DecisionTableExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from DecisionTableExporter...................
	virtual bool ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const;

public:

  //- Constructors/destructor........................................
  ArffDecisionTableExporter();
  virtual ~ArffDecisionTableExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// Algorithm
	virtual ArffDecisionTableExporter	*Clone();

};

#endif 
