//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTEXPORTER_H__
#define __REDUCTEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/exporter.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Reduct;

//-------------------------------------------------------------------
// Class.........: ReductExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms dealing with exporting
//                 reducts to alien formats.
// Revisions.....:
//===================================================================

class ReductExporter : public Exporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool       ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool       ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool       ExportEpilogue(ofstream &stream, const Structure &structure) const;

protected:

	//- Overloadable for various formats...............................
	virtual bool       ExportReduct(ofstream &stream, int reduct_no, const Reduct &reduct) const = 0;

public:

  //- Constructors/destructor........................................
  ReductExporter();
  virtual ~ReductExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
	virtual ReductExporter	*Clone() =  0;

};

#endif 
