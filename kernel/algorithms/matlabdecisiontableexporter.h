//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MATLABDECISIONTABLEEXPORTER_H__
#define __MATLABDECISIONTABLEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>

//-------------------------------------------------------------------
// Class.........: MatlabDecisionTableExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a decision table to Matlab format.
// Revisions.....:
//===================================================================

class MatlabDecisionTableExporter : public DecisionTableExporter {
protected:

  //- Methods inherited from DecisionTableExporter...................
	virtual bool ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const;

public:

  //- Constructors/destructor........................................
  MatlabDecisionTableExporter();
  virtual ~MatlabDecisionTableExporter();

	virtual MatlabDecisionTableExporter   *Clone();
	
  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

};

#endif 
