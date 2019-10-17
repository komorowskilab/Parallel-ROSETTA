//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYGENERALRULEFILTER_H__
#define __MYGENERALRULEFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/myrulefilter.h>

//-------------------------------------------------------------------
// Class.........: MyGeneralRuleFilter
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Description...: Filters away rules based on a compound
//                 criterion of support, accuracy and decision.
// Comments......: For General Rules
// Revisions.....:
//===================================================================

class MyGeneralRuleFilter : public MyRuleFilter {

protected:

	//- Methods inherited from Filter..................................
	virtual bool            Remove(const Structures &structures, int i) const;

public:

  //- Constructors/destructor........................................
  MyGeneralRuleFilter();
  virtual ~MyGeneralRuleFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String          GetParameters() const;
	virtual bool            SetParameter(const String &keyword, const String &value);

  	//- Methods inherited from Algorithm...............................
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual MyGeneralRuleFilter	*Clone();

};

#endif 
