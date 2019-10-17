#ifndef __GENERATOR_H__
#define __GENERATOR_H__
/** Internal random number generator. It is separate
 * from Rosetta's one because we need to improwe Rosetta's
 * one.
 * \sa Numerical Recipes in C, Chapter 7
 */

class Generator {

private:
/** @name Algorithm constants.
 *  UPDATE: 06/10/14 NB -- Swapped to constexpr with C++11 to calculate at compiletime rather than runtime. 
 */
//@{
/*
	static const long   IA     = 16807;
        static const long   IM     = 2147483647;
#ifdef WIN32 
	static const double AM;
#else 
	static const double AM     = (1.0 / IM);
#endif
        static const long   IQ     = 127773;
        static const long   IR     = 2836;
        static const long   NTAB   = 32;
        static const long   NDIV   = (1 + (IM - 1) / NTAB);
*/

	static constexpr long   IA     = 16807;
        static constexpr long   IM     = 2147483647;
#ifdef WIN32 
	static constexpr double AM;
#else 
	static constexpr double AM     = (1.0 / IM);
#endif
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
	long int seed;
//@}

public:
	/** Constructor takes the seed which is a key.
	 * \param key seed for random number algorithm
         */	
	Generator(long int key);

	/** Draws a long int.
	 * \return long int random number
	 */
	long int Get();

};

#endif

