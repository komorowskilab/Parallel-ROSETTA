//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __EXPORTER_H__
#define __EXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Exporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms dealing with exporting
//                 structural objects to alien formats.
// Revisions.....:
//===================================================================

class Exporter : public Algorithm {
private:

  //- Private member variables.......................................
	String                     filename_; // Export location.
	Handle<DecisionTable>      table_;    // Dictionary to use (if any).

protected:

  //- Export methods.................................................
	virtual bool               ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool               ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool               ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Table (dictionary) access......................................
	const DecisionTable       *GetDecisionTable() const;
	bool                       SetDecisionTable(const DecisionTable *table);

public:

  //- Constructors/destructor........................................
  Exporter();
  virtual ~Exporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String             GetParameters() const;
	virtual bool               GetOutputFilenames(Vector(String) &filenames) const;
	virtual bool               SetParameter(const String &keyword, const String &value);
  virtual Structure         *Apply(Structure &structure) const;
  virtual Exporter 			*Clone();

	//- Parameter management...........................................
	virtual bool               SetFilename(const String &filename);
	virtual const String      &GetFilename() const;

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

inline const DecisionTable *
Exporter::GetDecisionTable() const {
	return table_.GetPointer();
}

inline bool
Exporter::SetDecisionTable(const DecisionTable *table) {
	table_ = table;
	return true;
}

#endif 
