//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEFILTER_H__
#define __RULEFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/filter.h>

//-------------------------------------------------------------------
// Class.........: RuleFilter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Filters a set of rules, i.e. physically removes
//                 selected rules from the rule set.
// Revisions.....:
//===================================================================

class RuleFilter : public Filter {
protected:

	//- Members variables for internal use.............................
	Vector(int)        decisions_;     // Decision class for originating table, if any. Mutable, for computing coverage.
	Vector(int)        cardinalities_; // Cardinalities for each decision class. Mutable, for computing coverage.

public:

  //- Constructors/destructor........................................
  RuleFilter();
  virtual ~RuleFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure *Apply(Structure &structure) const;
	virtual RuleFilter	*Clone() = 0;

};

#endif 
