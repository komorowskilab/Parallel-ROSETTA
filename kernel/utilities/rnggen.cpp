#include <kernel/utilities/rnggen.h>

static RNGX* Generator::GetGenerator(int type, long seed){

	switch (type){
		case 0: return new ShortGenerator(seed);
		case 1: return new LongGenerator(seed);
	};

};

