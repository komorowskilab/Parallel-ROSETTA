#ifndef __RNGLONG_H__
#define __RNGLONG_H__

//#include <kernel/utilities/rng.h>
#include <kernel/utilities/rngxxx.h>

using namespace std;

class LongGenerator: public RNGX {
private:
/*
	static const long IM1 = 2147483563;
	static const long IM2 = 2147483399;
	static const int IA1 = 40014;
	static const int IA2 = 40692;
	static const int IQ1 = 53668;
	static const int IQ2 = 52774;
	static const int IR1 = 12211;
	static const int IR2 = 3791;
	static const int NTAB = 32;
	static const long IMM1 = IM1-1;

	static const long NDIV=1+IMM1/NTAB;
	static const double AM=(1.0/IM1);
	static const double EPS= 1.2e-7;
	static const double RNMX =1.0-1.2e-7;
*/

	/*UPDATE 06/10/14 NB -- Swapped to constexpr with C++11 for compile-time calculations rather than runtime.*/
	static constexpr long IM1 = 2147483563;
	static constexpr long IM2 = 2147483399;
	static constexpr int IA1 = 40014;
	static constexpr int IA2 = 40692;
	static constexpr int IQ1 = 53668;
	static constexpr int IQ2 = 52774;
	static constexpr int IR1 = 12211;
	static constexpr int IR2 = 3791;
	static constexpr int NTAB = 32;
	static constexpr long IMM1 = IM1-1;

	static constexpr long NDIV=1+IMM1/NTAB;
	static constexpr double AM=(1.0/IM1);
	static constexpr double EPS= 1.2e-7;
	static constexpr double RNMX =1.0-1.2e-7;

	int j;
	long k;
	long idum2;
	long iy;
	float temp;
	long iv[NTAB];

public:

	LongGenerator(long key);
	float Get();
};

#endif
