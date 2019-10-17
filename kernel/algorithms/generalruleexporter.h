//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __GENERALRULEEXPORTER_H__
#define __GENERALRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/ruleexporter.h>
#include <kernel/structures/generalrule.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Rule;

//-------------------------------------------------------------------
// Class.........: GeneralRuleExporter
// Author........: Ewa Makosa
// Date..........:
// Description...: Base class for algorithms dealing with exporting
//                 rules to alien formats.
// Revisions.....:
//===================================================================

class GeneralRuleExporter : public RuleExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool             ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool             ExportData(ofstream &stream, const Structure &structure) const;
	virtual bool             ExportEpilogue(ofstream &stream, const Structure &structure) const;

protected:

	//- Overloadable for various formats...............................
	virtual bool             ExportRule(ofstream &stream, int rule_no, const GeneralRule &rule) const = 0;
	virtual bool             ExportRule(ofstream &stream, int rule_no, const Rule &rule) const;
public:

  //- Constructors/destructor........................................
  GeneralRuleExporter ();
  virtual ~GeneralRuleExporter ();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual GeneralRuleExporter 	*Clone() = 0;

};

#endif 
