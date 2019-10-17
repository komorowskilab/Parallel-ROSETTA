// BitVRandomInitializer.cpp: implementation of the BitVRandomInitializer class.
//
//////////////////////////////////////////////////////////////////////

#include "BitVRandomInitializer.h"
#include "Rand.h"
#include "EABitset.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitVRandomInitializer::BitVRandomInitializer(int size, int nbits, double prob, bool vary)
:	number(size), bits(nbits), p(prob), oneShot(vary)
{

}

BitVRandomInitializer::~BitVRandomInitializer()
{

}

Population& BitVRandomInitializer::init()
{
	cerr << "Init, size: " << number << ", bits: " << bits <<"...\n";
	for(int i = 0; i < number; i++){
    EABitset * v = new EABitset(bits);
		if(!oneShot)
			p = Rand::d();
    for(int j = 0; j < bits; j++){
			if(Rand::d() < p)
				v->set(j, true);
		}
    pop.population.push_back(v);
	}
	cerr << "return from init\n";
	return pop;
}
 
