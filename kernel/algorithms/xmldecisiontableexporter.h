//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __XMLDECISIONTABLEEXPORTER_H__
#define __XMLDECISIONTABLEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>

//-------------------------------------------------------------------
// Class.........: XMLDecisionTableExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a decision table as XML.
// Revisions.....:
//===================================================================

class XMLDecisionTableExporter : public DecisionTableExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from DecisionTableExporter...................
	virtual bool ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const;

public:

  //- Constructors/destructor........................................
  XMLDecisionTableExporter();
  virtual ~XMLDecisionTableExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// Algorithms
	virtual XMLDecisionTableExporter	*Clone();

};

#endif 
