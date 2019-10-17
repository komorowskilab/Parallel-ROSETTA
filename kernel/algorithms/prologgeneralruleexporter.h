//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PROLOGGENERALRULEEXPORTER_H__
#define __PROLOGGENERALRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/generalruleexporter.h>

//-------------------------------------------------------------------
// Class.........: PrologGeneralRuleExporter
// Author........: Ewa Makosa
// Date..........:
// Description...: Exports a set of general rules to Prolog format.
// Revisions.....:
//===================================================================

class PrologGeneralRuleExporter : public GeneralRuleExporter {
protected:

  //- Methods inherited from Exporter ................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
  //- Methods inherited from RuleExporter............................
	virtual bool ExportRule(ofstream &stream, int rule_no, const GeneralRule &rule) const;

public:

  //- Constructors/destructor........................................
  PrologGeneralRuleExporter();
  virtual ~PrologGeneralRuleExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
  
	//- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual PrologGeneralRuleExporter	*Clone();

};

#endif 
