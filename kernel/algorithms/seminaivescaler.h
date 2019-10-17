//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SEMINAIVESCALER_H__
#define __SEMINAIVESCALER_H__

#include <copyright.h>

#include <kernel/algorithms/naivescaler.h>

//-------------------------------------------------------------------
// Class.........: SemiNaiveScaler
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Similar to NaiveScaler, but with some additional
//                 logic to handle inconsistent cases.
// Revisions.....:
//===================================================================

class SemiNaiveScaler : public NaiveScaler {
protected:

	//- Methods inherited from NaiveScaler.............................
  virtual bool SkipCut(const Vector(OVPair) &objects, int i, const Vector(int) &decisions_l, const Vector(int) &cardinalities_l, const Vector(int) &decisions_r, const Vector(int) &cardinalities_r) const;

public:

  //- Constructors/destructor........................................
  SemiNaiveScaler();
  virtual ~SemiNaiveScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from OrthogonalScaler........................
  virtual bool FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

};

#endif 
