//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __HTMLREPORTER_H__
#define __HTMLREPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/reporter.h>

//-------------------------------------------------------------------
// Class.........: HTMLReporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Generates a report in HTML format.
// Revisions.....:
//===================================================================

class HTMLReporter : public Reporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Report methods..................................................
	virtual bool ExportTree(ofstream &stream, const Structure &structure, const String &label) const;
	virtual bool ExportData(ofstream &stream, const Structure &structure, const String &label) const;

public:

  //- Constructors/destructor........................................
  HTMLReporter();
  virtual ~HTMLReporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	virtual HTMLReporter	*Clone();
};

#endif 
