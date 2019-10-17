

#include <kernel/utilities/rngxxx.h>

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RNGX::~RNGX() {
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
RNGX::GetSeed() const {
	return init_seed;
}

//-------------------------------------------------------------------
// Method........: SetSeed
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
//

void
RNGX::SetSeed(long key) {

	init_seed = key;

	if (init_seed < 0)
		seed = init_seed;
	else if (init_seed > 0)
		seed = -init_seed;
	else
		seed = -1;

	Get();

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
RNGX::DrawFloat(float lower, float upper) {

	if (lower == upper)
		return lower;

	// Draw (0, 1) deviate.
	float deviate = Get();

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

/*int
RNGX::DrawInteger(int lower, int upper) const {
	return MathKit::Round(DrawFloat(lower - 0.49f, upper + 0.49f));
};*/


