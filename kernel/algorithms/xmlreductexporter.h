//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __XMLREDUCTEXPORTER_H__
#define __XMLREDUCTEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductexporter.h>

//-------------------------------------------------------------------
// Class.........: XMLReductExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of reducts as XML.
// Revisions.....:
//===================================================================

class XMLReductExporter : public ReductExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from ReductExporter..........................
	virtual bool ExportReduct(ofstream &stream, int reduct_no, const Reduct &reduct) const;

public:

  //- Constructors/destructor........................................
  XMLReductExporter();
  virtual ~XMLReductExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// Algorithms
	virtual XMLReductExporter	*Clone();

};

#endif 
