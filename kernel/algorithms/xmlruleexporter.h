//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __XMLRULEEXPORTER_H__
#define __XMLRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/ruleexporter.h>

//-------------------------------------------------------------------
// Class.........: XMLRuleExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of rules as XML.
// Revisions.....:
//===================================================================

class XMLRuleExporter : public RuleExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportPrologue(ofstream &stream, const Structure &structure) const;
	virtual bool ExportEpilogue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from RuleExporter............................
	virtual bool ExportRule(ofstream &stream, int rule_no, const Rule &rule) const;

public:

	//- Constructors/destructor........................................
	XMLRuleExporter();
	virtual ~XMLRuleExporter();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

   virtual XMLRuleExporter   *Clone();

	//- Methods inherited from Algorithm...............................
	virtual bool	IsApplicable(const Structure &structure, bool warn = true) const;

};

#endif 
