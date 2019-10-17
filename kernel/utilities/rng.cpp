//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================


#ifndef __RNG_CPP__
#define __RNG_CPP__

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/rng.h>
#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: ran1
// Author........:
// Date..........:
// Description...: "Minimal" random number generator of Park and Miller
//                 with Bays-Durham shuffle and added safeguards.
//                 Returns a uniform random deviate between 0.0 and 1.0
//                 (exclusive of the endpoint values). Call with idum
//                 a negative integer to initialize; thereafter, do not
//                 alter idum between successive deviates in a sequence.
//                 RNMX should approximate the largest floating value that
//                 is less than 1.
//
// Comments......: Taken from the book "Numerical Recipes in C".
// Revisions.....:
//===================================================================

static float
ran1(long *idum) {

	const long   IA     = 16807;
	const long   IM     = 2147483647;
	const double AM     = (1.0 / IM);
	const long   IQ     = 127773;
	const long   IR     = 2836;
	const long   NTAB   = 32;
  const long   NDIV   = (1 + (IM - 1) / NTAB);
	const double EPS    = 1.2e-7;
	const double RNMX   = (1.0 - EPS);

	int         j;
	long        k;
	static long iy = 0;
	static long iv[NTAB];
	float       temp;

	if (*idum <= 0 || !iy) {                // Initialize.
		if (-(*idum) < 1)
			*idum = 1;                          // Be sure to prevent idum = 0.
		else
			*idum = -(*idum);
		for (j = NTAB + 7; j >=0; j--) {      // Load the shuffle table (after 8 warm-ups).
			k = (*idum) / IQ;
			*idum = IA * (*idum - k * IQ) - IR * k;
			if (*idum < 0)
				*idum += IM;
			if (j < NTAB)
				iv[j] = *idum;
		}
		iy = iv[0];
	}
	k = (*idum) / IQ;                       // Start here when not initializing.
	*idum = IA * (*idum - k * IQ) - IR * k; // Compute idum = (IA * idum) % IM without overflows by Schrage's method.
	if (*idum < 0)
		*idum += IM;
	j = iy / NDIV;                          // Will be in the range 0..NTAB-1.
	iy = iv[j];                             // Output previously stored value and refill the shuffle table.
	iv[j] = *idum;
	if ((temp = AM * iy) > RNMX)
		return RNMX;                          // Because users don't expect endpoint values.
	else
		return temp;
}

//-------------------------------------------------------------------
// Methods for class RNG.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RNG::RNG() {
	SetSeed(-1);
}

RNG::RNG(long nSeed) {
	SetSeed(nSeed);
}

RNG::~RNG() {
}

//-------------------------------------------------------------------
// Method........: GetSeed
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

long
RNG::GetSeed() const {
	return seed;
}

//-------------------------------------------------------------------
// Method........: SetSeed
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
RNG::SetSeed(long nSeed) {

	#pragma omp critical(rng)
	{
	seed = nSeed;

	if (nSeed < 0)
		idum_ = nSeed;
	else if (nSeed > 0)
		idum_ = -nSeed;
	else
		idum_ = -1;

	ran1(&idum_);
	}
}

//-------------------------------------------------------------------
// Method........: DrawFloat
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a float in the range (lower, upper),
//                 sampled from a uniform distribution.
// Comments......:
// Revisions.....:
//===================================================================

float
RNG::DrawFloat(float lower, float upper) const {

	if (lower == upper)
		return lower;

	// This method is conceptually const only.
	RNG *self = const_cast(RNG *, this);

	// Draw (0, 1) deviate.
	float deviate;
	#pragma omp critical(rng)
	{
	deviate = ran1(&(self->idum_));
	}

	// Swap order?
	if (lower > upper) {
		float tmp = lower;
		lower = upper;
		upper = tmp;
	}

	if (lower == 0.0 && upper == 1.0)
		return deviate;

	// Rescale.
	deviate *= upper - lower;
	deviate += lower;

	return deviate;

}

//-------------------------------------------------------------------
// Method........: DrawInteger
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns an integer in the range [lower, upper],
//                 sampled from a uniform distribution.
// Comments......: Expanded range with 0.49 in both ends so that the
//                 endpoints will appear with the expected frequency.
// Revisions.....:
//===================================================================

int
RNG::DrawInteger(int lower, int upper) const {
	return MathKit::Round(DrawFloat(lower - 0.49f, upper + 0.49f));
}



#endif

 
