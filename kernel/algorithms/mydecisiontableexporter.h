//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYDECISIONTABLEEXPORTER_H__
#define __MYDECISIONTABLEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>

//-------------------------------------------------------------------
// Class.........: MyDecisionTableExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a decision table to ROSETTA import format.
// Revisions.....:
//===================================================================

class MyDecisionTableExporter : public DecisionTableExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from DecisionTableExporter...................
	virtual bool ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const;

public:

  //- Constructors/destructor........................................
  MyDecisionTableExporter();
  virtual ~MyDecisionTableExporter();

	virtual MyDecisionTableExporter   *Clone();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

};

#endif 
