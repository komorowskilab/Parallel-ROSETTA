//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DISCERNIBILITYEXPORTER_H__
#define __DISCERNIBILITYEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Discerner;
class IndiscernibilityGraph;

//-------------------------------------------------------------------
// Class.........: DiscernibilityExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Computes and exports discernibility related stuff
//                 from a decision table.
// Revisions.....:
//===================================================================

class DiscernibilityExporter : public DecisionTableExporter {
protected:

	//- Implementation structures......................................
	bool             idg_;          // Use user-defined indiscernibility definition graph (IDG)?
	String           filename_idg_; // Location of IDG.
	bool             name_;         // Use 1-1 attribute for object "naming"/identification?
	String           id_name_;      // Name of attribute.
	bool             modulo_;       // Discernibility modulo decision?

protected:

  //- Methods inherited from Exporter................................
	virtual bool     ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool     ExportData(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from DecisionTableExporter...................
	virtual bool     ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const;

  //- New methods....................................................
	virtual bool     ExportData(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const = 0;

public:

  //- Constructors/destructor........................................
  DiscernibilityExporter();
  virtual ~DiscernibilityExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String   GetParameters() const;
	virtual bool     SetParameter(const String &keyword, const String &value);
	virtual DiscernibilityExporter 	*Clone() = 0;

	//- Parameter management...........................................
	bool             UseIDG() const {return idg_;}
	bool             UseIDG(bool idg) {idg_ = idg; return true;}

	const String    &GetIDGFilename() const {return filename_idg_;}
	bool             SetIDGFilename(const String &filename) {filename_idg_ = filename; return true;}

	bool             UseIDAttribute() const {return name_;}
	bool             UseIDAttribute(bool name) {name_ = name; return true;}

	const String    &GetIDAttributeName() const {return id_name_;}
	bool             SetIDAttributeName(const String &id_name) {id_name_ = id_name; return true;}

	bool             ModuloDecision() const {return modulo_;}
	bool             ModuloDecision(bool modulo) {modulo_ = modulo; return true;}

};

#endif 
