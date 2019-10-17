//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PROLOGRULEEXPORTER_H__
#define __PROLOGRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/ruleexporter.h>

//-------------------------------------------------------------------
// Class.........: PrologRuleExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of rules to Prolog format.
// Revisions.....:
//===================================================================

class PrologRuleExporter : public RuleExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from RuleExporter............................
	virtual bool ExportRule(ofstream &stream, int rule_no, const Rule &rule) const;

public:

  //- Constructors/destructor........................................
  PrologRuleExporter();
  virtual ~PrologRuleExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
//- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual PrologRuleExporter	*Clone();

};

#endif 
