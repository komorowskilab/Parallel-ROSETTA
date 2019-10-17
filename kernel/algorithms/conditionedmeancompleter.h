//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CONDITIONEDMEANCOMPLETER_H__
#define __CONDITIONEDMEANCOMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/conditionedcompleter.h>

//-------------------------------------------------------------------
// Class.........: ConditionedMeanCompleter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Combines MeanCompleter and ConditionedCompleter.
// Revisions.....:
//===================================================================

class ConditionedMeanCompleter : public ConditionedCompleter {
public:

  //- Constructors/destructor........................................
  ConditionedMeanCompleter();
  virtual ~ConditionedMeanCompleter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual ConditionedMeanCompleter	*Clone();

};

#endif 
