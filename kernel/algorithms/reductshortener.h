//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960419
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTSHORTENER_H__
#define __REDUCTSHORTENER_H__

#include <copyright.h>

#include <kernel/algorithms/reductfilter.h>

//-------------------------------------------------------------------
// Class.........: ReductShortener
// Author........: Aleksander Ûhrn
// Date..........: 950420
// Description...:
// Revisions.....: Inherits from ReductFilter instead of Algorithm.
//===================================================================

class ReductShortener : public ReductFilter {
protected:

  //- Methods inherited from Filter..................................
	virtual bool       Remove(const Structures &structures, int i) const;

public:

  //- Constructors/destructor........................................
  ReductShortener();
  virtual ~ReductShortener();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;

};

#endif 
