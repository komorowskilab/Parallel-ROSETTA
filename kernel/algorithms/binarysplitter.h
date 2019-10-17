//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BINARYSPLITTER_H__
#define __BINARYSPLITTER_H__

#include <copyright.h>

#include <kernel/algorithms/splitter.h>

//-------------------------------------------------------------------
// Class.........: BinarySplitter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Randomly splits a decision table into subtables.
// Revisions.....:
//===================================================================

class BinarySplitter : public Splitter {
protected:

  //- Parameters.....................................................
	int                seed_;             // Seed to the RNG.
	float              splitting_factor_; // Normalised splitting percentage.

public:

  //- Constructors/destructor........................................
  BinarySplitter();
  virtual ~BinarySplitter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual BinarySplitter	*Clone();

	//- Parameter management...........................................
	int                GetSeed() const {return seed_;}
	bool               SetSeed(int seed) {seed_ = seed; return true;}

	float              GetSplittingFactor() const {return splitting_factor_;}
	bool               SetSplittingFactor(float splitting_factor) {float f = splitting_factor; if (f < 0.0) f = 0.0; else if (f > 1.0) f = 1.0; splitting_factor_ = f; return true;}

};

#endif 
