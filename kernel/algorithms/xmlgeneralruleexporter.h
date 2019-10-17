//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __XMLGENERALRULEEXPORTER_H__
#define __XMLGENERALRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/generalruleexporter.h>
#include <kernel/structures/generalrule.h>

//-------------------------------------------------------------------
// Class.........: XMLGeneralRuleExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of rules as XML.
// Revisions.....:
//===================================================================

class XMLGeneralRuleExporter : public GeneralRuleExporter {
protected:

  //- Methods inherited from GeneralRuleExporter............................
	virtual bool ExportRule(ofstream &stream, int rule_no, const GeneralRule &rule) const;

public:

  //- Constructors/destructor........................................
  XMLGeneralRuleExporter();
  virtual ~XMLGeneralRuleExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual XMLGeneralRuleExporter	*Clone();
};

#endif 
