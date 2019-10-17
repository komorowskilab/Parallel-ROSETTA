//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __FILTER_H__
#define __FILTER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structures;

//-------------------------------------------------------------------
// Class.........: Filter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A filter is an algorithm that operates on a set of
//                 structures, and removes members of this set
//                 according to some filtering criterion.
//
// Revisions.....: Previously named Pruner
//===================================================================

class Filter : public Algorithm {
protected:

  //- Parameters.....................................................
	bool                   invert_logic_; // Invert filtering logic?

protected:

	//- Filtering decision.............................................
	virtual bool           Remove(const Structures &structures, int i) const = 0;

public:

  //- Constructors/destructor........................................
  Filter();
  virtual ~Filter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String         GetParameters() const;
	virtual bool           SetParameter(const String &keyword, const String &value);
	virtual bool           IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure     *Apply(Structure &structure) const;

	//- Parameter management...........................................
	bool                   InvertLogic() const {return invert_logic_;}
	bool                   InvertLogic(bool invert_logic) {invert_logic_ = invert_logic; return true;}

};

#endif 
