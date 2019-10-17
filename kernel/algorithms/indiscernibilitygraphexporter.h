//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __INDISCERNIBILITYGRAPHEXPORTER_H__
#define __INDISCERNIBILITYGRAPHEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/discernibilityexporter.h>

//-------------------------------------------------------------------
// Class.........: IndiscernibilityGraphExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Computes and exports a graph of a user-defined
//                 indiscernibility relation.
//
// Revisions.....: AÛ 991110: Added low-cardinality stuff.
//
// Comments......: To do: Add a more general option than simply
//                        allowing low-cardinality entries to be ignored.
//                        Allow, e.g., general cost information, or even
//                        thresholding on weighted distances (i.e., Manhattan
//                        or city-block metric).
//===================================================================

class IndiscernibilityGraphExporter : public DiscernibilityExporter {
protected:

	//- Parameters.....................................................
	bool           reflexive_;   // Include reflexive information in output?
	bool           deg_;         // Include vertex degree data in output?
	bool           apsp_;        // Include all-pairs shortest path data in output?
	bool           ignore_;      // Ignore low-cardinality entries in M?
	int            cardinality_; // Add an edge iff |M(x, y)| is less or equal to this value.

protected:

  //- Methods inherited from Exporter................................
	virtual bool   ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from DiscernibilityExporter..................
	virtual bool   ExportData(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const;

  //- New methods....................................................
	virtual bool   ExportGraph(ofstream &stream, const IndiscernibilityGraph &graph, const DecisionTable &table, bool masked) const;
	virtual bool   ExportDegreeData(ofstream &stream, const IndiscernibilityGraph &graph, const DecisionTable &table, bool masked) const;
	virtual bool   ExportAPSPData(ofstream &stream, const IndiscernibilityGraph &graph, const DecisionTable &table, bool masked) const;

public:

  //- Constructors/destructor........................................
  IndiscernibilityGraphExporter();
  virtual ~IndiscernibilityGraphExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual IndiscernibilityGraphExporter	*Clone();

	//- Parameter management...........................................
	bool           IncludeReflexiveData() const {return reflexive_;}
	bool           IncludeReflexiveData(bool reflexive) {reflexive_ = reflexive; return true;}

	bool           IncludeDegreeData() const {return deg_;}
	bool           IncludeDegreeData(bool deg) {deg_ = deg; return true;}

	bool           IncludeAPSPData() const {return apsp_;}
	bool           IncludeAPSPData(bool apsp) {apsp_ = apsp; return true;}

	bool           IgnoreLowCardinalityEntries() const {return ignore_;}
	bool           IgnoreLowCardinalityEntries(bool ignore) {ignore_ = ignore; return true;}

	int            GetMaximumCardinality() const {return cardinality_;}
	bool           SetMaximumCardinality(int cardinality) {cardinality_ = cardinality; return (cardinality_ >= 0);}

};

#endif 
