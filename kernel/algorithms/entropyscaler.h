//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ENTROPYSCALER_H__
#define __ENTROPYSCALER_H__

#include <copyright.h>

#include <kernel/algorithms/orthogonalscaler.h>

//-------------------------------------------------------------------
// Class.........: EntropyScaler
// Author........: Aleksander Ûhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class EntropyScaler : public OrthogonalScaler {
protected:

	//- Local methods..................................................
	float        FindBestCut(const Vector(OVPair) &objects, int begin, int end, const DecisionTable &table, int decision_attribute, bool masked, const Vector(int) &decision_classes, int &begin_l, int &end_l, int &begin_r, int &end_r, float &entropy_t, float &entropy_l, float &entropy_r, int &no_decision_classes_l, int &no_decision_classes_r) const;
  void         RecursiveFindCuts(const Vector(OVPair) &objects, int begin, int end, const DecisionTable &table, int decision_attribute, bool masked, const Vector(int) &decision_classes, Cuts &cuts) const;

public:

  //- Constructors/destructor........................................
  EntropyScaler();
  virtual ~EntropyScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from OrthogonalScaler........................
  virtual bool FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

	//- Methods inherited from Algorithms........................
	virtual EntropyScaler *Clone();
};

#endif 
