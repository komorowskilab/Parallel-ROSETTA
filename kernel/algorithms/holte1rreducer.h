//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __HOLTE1RREDUCER_H__
#define __HOLTE1RREDUCER_H__

#include <copyright.h>

#include <kernel/algorithms/reducer.h>

//-------------------------------------------------------------------
// Class.........: Holte1RReducer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns singleton attribute sets and univariate
//                 rules. Inspired by Holte's 1993 paper.
//
// Comments......: For now, this algorithm does not take any parameters.
//                 Consider adding IDG support, though.
// Revisions.....:
//===================================================================

class Holte1RReducer : public Reducer {
public:

  //- Constructors/destructor........................................
  Holte1RReducer();
  virtual ~Holte1RReducer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
  virtual Structure *Apply(Structure &structure) const;
  virtual Holte1RReducer	*Clone();

};

#endif
 
