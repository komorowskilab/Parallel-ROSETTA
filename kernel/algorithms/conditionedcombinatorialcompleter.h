//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CONDITIONEDCOMBINATORIALCOMPLETER_H__
#define __CONDITIONEDCOMBINATORIALCOMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/conditionedcompleter.h>

//-------------------------------------------------------------------
// Class.........: ConditionedCombinatoralCompleter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Combines CombinatoralCompleter and ConditionedCompleter.
// Revisions.....:
//===================================================================

class ConditionedCombinatorialCompleter : public ConditionedCompleter {
public:

  //- Constructors/destructor........................................
  ConditionedCombinatorialCompleter();
  virtual ~ConditionedCombinatorialCompleter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual ConditionedCombinatorialCompleter	*Clone();

};

#endif 
