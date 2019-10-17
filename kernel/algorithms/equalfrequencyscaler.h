//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __EQUALFREQUENCYSCALER_H__
#define __EQUALFREQUENCYSCALER_H__

#include <copyright.h>

#include <kernel/algorithms/orthogonalscaler.h>

//-------------------------------------------------------------------
// Class.........: EqualFrequencyScaler
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: For each attribute, constructs a fixed number of
//                 intervals, each containing approximately the same
//                 number of objects.
// Revisions.....:
//===================================================================

class EqualFrequencyScaler : public OrthogonalScaler {
protected:

	//- Member variables...............................................
	int            no_intervals_; // Number of intervals.

protected:

	//- New methods....................................................
	//...

public:

  //- Constructors/destructor........................................
  EqualFrequencyScaler();
  virtual ~EqualFrequencyScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual EqualFrequencyScaler	*Clone();

	//- Methods inherited from OrthogonalScaler........................
  virtual bool   FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

	//- Parameter methods..............................................
	int            GetNoIntervals() const {return no_intervals_;}
	bool           SetNoIntervals(int no_intervals) {no_intervals_ = no_intervals; return (no_intervals >= 1);}

};

#endif 
