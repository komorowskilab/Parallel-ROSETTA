#include <kernel/utilities/generator.h>

#ifdef WIN32
	const double Generator::AM = (1.0/IM);
#endif

Generator::Generator(long int key){

	seed = key;

	iy = 0;	

#ifdef DEBUG
	if (seed > 0){
		cout << "Random seed greater than zero... aborting." << endl;
		exit(0);
	};
#endif

        if (-(seed) < 1)
        	seed = 1;                       // Be sure to prevent idum = 0.
        else
        	seed = -(seed);

        for (j = NTAB + 7; j >=0; j--) {      	// Load the shuffle table (after 8 warm-ups).

        	k = (seed) / IQ;
        	seed = IA * (seed - k * IQ) - IR * k;

        	if (seed < 0) seed += IM;

        	if (j < NTAB) iv[j] = seed;
        }

        iy = iv[0];

};

long int Generator::Get(){

        k = (seed) / IQ;                       	// Start here when not initializing.

        seed = IA * (seed - k * IQ) - IR * k; 	// Compute idum = (IA * idum) % IM without 
						// overflows by Schrage's method.
        if (seed < 0) seed += IM;

        j = iy / NDIV;                          // Will be in the range 0..NTAB-1.

        iy = iv[j];                             // Output previously stored value 
						// and refill the shuffle table.
        iv[j] = seed;

	return iy;

};

