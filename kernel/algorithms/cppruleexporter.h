//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CPPRULEEXPORTER_H__
#define __CPPRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/ruleexporter.h>

//-------------------------------------------------------------------
// Class.........: CPPRuleExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of rules to C++ format.
// Revisions.....:
//===================================================================

class CPPRuleExporter : public RuleExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportData(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from RuleExporter................................
	virtual bool ExportRule(ofstream &stream, int rule_no, const Rule &rule) const;

public:

  //- Constructors/destructor........................................
  CPPRuleExporter();
  virtual ~CPPRuleExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
	
	//- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual CPPRuleExporter		*Clone();

};

#endif 
