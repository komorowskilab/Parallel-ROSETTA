//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __NAIVESCALER_H__
#define __NAIVESCALER_H__

#include <copyright.h>

#include <kernel/algorithms/orthogonalscaler.h>

//-------------------------------------------------------------------
// Class.........: NaiveScaler
// Author........: Aleksander Ûhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class NaiveScaler : public OrthogonalScaler {
protected:

	//- New methods....................................................
  virtual bool SkipCut(const Vector(OVPair) &objects, int i, const Vector(int) &decisions_l, const Vector(int) &cardinalities_l, const Vector(int) &decisions_r, const Vector(int) &cardinalities_r) const;

public:

  //- Constructors/destructor........................................
  NaiveScaler();
  virtual ~NaiveScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from OrthogonalScaler........................
  virtual bool FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

	//- Methods inherited from Algorithms........................
	virtual NaiveScaler	*Clone();

	//- New methods....................................................
  virtual bool FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts, Vector(int) *regions, int *no_regions) const;

};

#endif 
