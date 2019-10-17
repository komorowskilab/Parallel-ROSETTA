//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CPPGENERALRULEEXPORTER_H__
#define __CPPGENERALRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/cppruleexporter.h>

//-------------------------------------------------------------------
// Class.........: CPPGeneralRuleExporter
// Author........: Ewa Makosa
// Date..........:
// Description...: Exports a set of rules to C++ format.
// Revisions.....:
//===================================================================

class CPPGeneralRuleExporter : public CPPRuleExporter {
protected:

  //- Methods inherited from Exporter................................
	virtual bool ExportData(ofstream &stream, const Structure &structure) const;

public:

  //- Constructors/destructor........................................
  CPPGeneralRuleExporter();
  virtual ~CPPGeneralRuleExporter();

  //- Methods inherited from Identifier..............................
  DECLAREIDMETHODS()
	
  //- Methods inherited from Algorithm...............................
  virtual bool	IsApplicable(const Structure &structure, bool warn = true) const;
  virtual CPPGeneralRuleExporter		*Clone();

};

#endif 
