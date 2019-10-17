//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RNG_H__
#define __RNG_H__

#include <copyright.h>

//-------------------------------------------------------------------
// Class.........: RNG
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Random number generator, uniform distribution.
//
//                 Based on code taken from the book "Numerical Recipes
//                 in C".
//
// Comments......: Consider either renaming this to UniformRNG, and to
//                 let RNG be an abstract base class for all
//                 distribution types.
// Revisions.....:
//===================================================================

class RNG {
protected:

	//- Private member variables.......................................
	long  seed;  // Initial seed.
	long  idum_;  // Current state, mutable.

public:

	//- Constructors/destructor........................................
	RNG();
	RNG(long nSeed);
	~RNG();

	//- Seed management................................................
	long  GetSeed() const;
	void  SetSeed(long nSeed);

	//- Draw uniform deviates..........................................
	float DrawFloat(float lower = 0.0, float upper = 1.0) const;
	int   DrawInteger(int lower, int upper) const;

};

#endif 
