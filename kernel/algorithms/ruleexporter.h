//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEEXPORTER_H__
#define __RULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/exporter.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Rule;

//-------------------------------------------------------------------
// Class.........: RuleExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms dealing with exporting
//                 rules to alien formats.
// Revisions.....:
//===================================================================

class RuleExporter : public Exporter {
protected:

	//- Members (mutable) for internal use.............................
	Vector(int)             decisions_;     // Decision class for originating table, if any. For computing coverage.
	Vector(int)             cardinalities_; // Cardinalities for each decision class. For computing coverage.
	int                     no_objects_;    // Number of objects in originating decision table. For computing (pattern, RHS) coverage.

protected:

  //- Methods inherited from Exporter................................
	virtual bool            ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool            ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool            ExportEpilogue(ofstream &stream, const Structure &structure) const;
	//virtual bool		SetParameter(const String &keyword, const String &value);
	//virtual String		GetParameters() const;

protected:

	//- Overloadable for various formats...............................
	virtual bool             ExportRule(ofstream &stream, int rule_no, const Rule &rule) const = 0;

public:

  //- Constructors/destructor........................................
  RuleExporter();
  virtual ~RuleExporter();
  
  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual RuleExporter		*Clone() = 0;

};

#endif 
