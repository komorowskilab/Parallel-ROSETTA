// Rand.h: interface for the Rand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAND_H__45312C57_3B33_11D2_AF89_000000000000__INCLUDED_)
#define AFX_RAND_H__45312C57_3B33_11D2_AF89_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <stdlib.h>
#include <kernel/utilities/rng.h>


class Rand
{

	static RNG rngInstance;
	static RNG * rng;
	static long seed;
	static int MaxInt;
	static int count;
public:
	static void setSeed(long seed);
	static int i(int upper = Rand::MaxInt);
	static double d();
	Rand();
	virtual ~Rand();

};



#endif // !defined(AFX_RAND_H__45312C57_3B33_11D2_AF89_000000000000__INCLUDED_)
 
