//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __XMLREPORTER_H__
#define __XMLREPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/reporter.h>

//-------------------------------------------------------------------
// Class.........: XMLReporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Generates a report in XML format.
// Revisions.....:
//===================================================================

class XMLReporter : public Reporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Report methods..................................................
	virtual bool ExportData(ofstream &stream, const Structure &structure, const String &indent) const;

public:

  //- Constructors/destructor........................................
  XMLReporter();
  virtual ~XMLReporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// Algorithms
	virtual XMLReporter	*Clone();

};

#endif 
