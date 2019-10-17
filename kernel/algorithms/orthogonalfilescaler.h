//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn/Knut Magne Risvik
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ORTHOGONALFILESCALER_H__
#define __ORTHOGONALFILESCALER_H__

#include <copyright.h>

#include <kernel/algorithms/orthogonalscaler.h>

//-------------------------------------------------------------------
// Class.........: OrthogonalFileScaler
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Discretizes from a cut-file.
// Revisions.....:
//===================================================================

class OrthogonalFileScaler : public OrthogonalScaler {
public:

  //- Constructors/destructor........................................
  OrthogonalFileScaler();
  virtual ~OrthogonalFileScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Scaler..................................
  virtual bool SetMode(Scaler::Mode mode);

	//- Methods inherited from OrthogonalScaler........................
  virtual bool FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

	//- Methods inherited from Algorithms........................
	virtual OrthogonalFileScaler *Clone();

};

#endif 
