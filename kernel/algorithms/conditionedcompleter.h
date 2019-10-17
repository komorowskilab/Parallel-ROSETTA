//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CONDITIONEDCOMPLETER_H__
#define __CONDITIONEDCOMPLETER_H__

#include <copyright.h>

#include <kernel/algorithms/completer.h>

//-------------------------------------------------------------------
// Class.........: ConditionedCompleter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A completer that conditions completion values on
//                 the decision classes.
// Revisions.....:
//===================================================================

class ConditionedCompleter : public Completer {
protected:

	//- The (unconditioned) completer algorithm........................
	Handle<Completer> completer_;

public:

  //- Constructors/destructor........................................
  ConditionedCompleter();
  virtual ~ConditionedCompleter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual ConditionedCompleter	*Clone();

	//- Completer management...........................................
	Completer         *GetCompleter() const {return const_cast(Completer *, completer_.GetPointer());}
	bool               SetCompleter(const Completer *completer) {completer_ = completer; return true;}

};

#endif 
