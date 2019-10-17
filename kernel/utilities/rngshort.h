#ifndef __RNGSHORT_H__
#define __RNGSHORT_H__

#include <kernel/utilities/rngxxx.h>
//#include <kernel/utilities/rng.h>
#include <cstddef>

class ShortGenerator:public RNGX {

private:
/** @name Algorithm constants.
 *  UDATE: 06/10/14 NB -- Changed consts to constexpr with C++11 to allow computations at compile-time
 *  	rather than runtime.
 */
//@{
//	static const long   IA     = 16807;
//      static const long   IM     = 2147483647;
	static constexpr long   IA     = 16807;
        static constexpr long   IM     = 2147483647;
#ifdef WIN32 
//	static const double AM;
//	static const double EPS;    
//	static const double RNMX; 
	static constexpr double AM;
	static constexpr double EPS;    
	static constexpr double RNMX; 
#endif
#ifdef LINUX
//	static const double AM = 1.0/IM;;
	static constexpr double AM = 1.0/IM;;
//	static const double EPS = 1.2e-7;    
	static constexpr double EPS = 1.2e-7;    
//	static const double RNMX = 1.0 - 1.2e-7; 
	static constexpr double RNMX = 1.0 - 1.2e-7; 
#endif
//      static const long   IQ     = 127773;
//      static const long   IR     = 2836;
//      static const long   NTAB   = 32;
//      static const long   NDIV   = (1 + (IM - 1) / NTAB);
        static constexpr long   IQ     = 127773;
        static constexpr long   IR     = 2836;
        static constexpr long   NTAB   = 32;
        static constexpr long   NDIV   = (1 + (IM - 1) / NTAB);
//@}
/** @name Algorithm variables.
 *
 */
//@{
	int         j;
        long        k;
        long int iy;
        long int iv[NTAB];
	float temp;
//@}

public:
	ShortGenerator(long key);
	float Get();

};

#endif

