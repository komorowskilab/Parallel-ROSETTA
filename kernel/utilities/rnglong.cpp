
#include <kernel/utilities/rnglong.h>

LongGenerator::LongGenerator(long key){

	seed = key;
	init_seed = seed;
	idum2 = 123456789;
	iy = 0;

};


float LongGenerator::Get()
{
	if (seed <= 0) { //Initialize.

		if (-(seed) < 1) seed=1; // Be sure to prevent idum = 0.
			else seed = -(seed);

		idum2=(seed);

		for (j=NTAB+7;j>=0;j--) { // Load the shuffle table (after 8 warm-ups).
		
			k=(seed)/IQ1;
			seed=IA1*(seed-k*IQ1)-k*IR1;
			if (seed < 0) seed += IM1;
		
			if (j < NTAB) iv[j] = seed;
		}
		
		iy=iv[0];
	
	};

	k=(seed)/IQ1; //Start here when not initializing.
	seed=IA1*(seed-k*IQ1)-k*IR1; // Compute idum=(IA1seed) % IM1 without overflows by Schrages method. if (seed < 0) seed += IM1;

	k=idum2/IQ2;
	idum2=IA2*(idum2-k*IQ2)-k*IR2; // Compute idum2=(IA2seed) % IM2 likewise.

	if (idum2 < 0) idum2 += IM2;

	j=iy/NDIV; // Will be in the range 0..NTAB-1.
	iy=iv[j]-idum2; // Here idum is shuffled, idum and idum2 are combined to generate output. iv[j] = seed;

	if (iy < 1) iy += IMM1;

	if ((temp=AM*iy) > RNMX) return RNMX; // Because users dont expect endpoint values.
	else return temp;
};
