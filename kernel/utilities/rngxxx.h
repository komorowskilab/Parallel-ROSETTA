//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================
//


#ifndef __RNG_H__
#define __RNG_H__

//#include <kernel/utilities/mathkit.h> includes rng.h so we can not include it here until replacement


//-------------------------------------------------------------------
// Class.........: RNG
// Author........: Aleksander Ûhrn, Lukasz Ligowski
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

class RNGX {
protected:

	//- Private member variables.......................................
	long  seed;  // Current state.
	long  init_seed;  // Initial seed.

public:

	//- Constructors/destructor........................................
	virtual ~RNGX();

	//- Seed management................................................
	long  GetSeed() const;
	void  SetSeed(long key);


	//- Draw uniform deviates..........................................
	virtual float Get()=0;
	float DrawFloat(float lower = 0.0, float upper = 1.0);
//	int   DrawInteger(int lower, int upper) const;

};


#endif 
