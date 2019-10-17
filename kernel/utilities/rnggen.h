#ifndef __RNGGEN_H__
#define __RNGGEN_H__

#include <kernel/utilities/rngxxx.h>
//#include <kernel/utilities/rng.h>
#include <kernel/utilities/rngshort.h>
#include <kernel/utilities/rnglong.h>


namespace Generator {
	static RNGX* GetGenerator(int type, long seed);
};


#endif
