//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTFILTER_H__
#define __REDUCTFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/filter.h>

//-------------------------------------------------------------------
// Class.........: ReductFilter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Filters a set of reducts.  By filtering is meant
//                 complete removal of some individual reducts.  This
//                 in contrast to reduct shortening, which keeps the
//                 reducts in the set, but modifies them.
//
// Revisions.....: Previously named ReductPruner.
//===================================================================

class ReductFilter : public Filter {
public:

  //- Constructors/destructor........................................
  ReductFilter();
  virtual ~ReductFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual bool   IsApplicable(const Structure &structure, bool warn = true) const;

};

#endif 
