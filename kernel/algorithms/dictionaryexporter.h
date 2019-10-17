//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DICTIONARYEXPORTER_H__
#define __DICTIONARYEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/exporter.h>

//-------------------------------------------------------------------
// Class.........: DictionaryExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports the dictionary associated with a decision
//                 table.
// Revisions.....:
//===================================================================

class DictionaryExporter : public Exporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool       ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool       ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool       ExportEpilogue(ofstream &stream, const Structure &structure) const;

protected:

	//- Overloadable for various formats...............................
	virtual bool       ExportAttribute(ofstream &stream, const Dictionary &dictionary, int attribute_no) const;

public:

  //- Constructors/destructor........................................
  DictionaryExporter();
  virtual ~DictionaryExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
	virtual DictionaryExporter	*Clone();

};

#endif 
