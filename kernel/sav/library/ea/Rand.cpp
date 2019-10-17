// Rand.cpp: implementation of the Rand class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.
#include <kernel/basic/message.h>
#include "Rand.h"

RNG Rand::rngInstance = RNG();
RNG * Rand::rng = NULL;
long Rand::seed = 12345;
int Rand::MaxInt = 0x7fff;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Rand::Rand()
{
}

Rand::~Rand()
{
}

double Rand::d()
{
	if(rng == NULL){
		rng = &rngInstance;
		rng->SetSeed(Rand::seed);
	}
	return (double)rng->DrawFloat();
}

int Rand::i(int upper)
{
	if(rng == NULL){
		rng = &rngInstance;
		rng->SetSeed(Rand::seed);
	}
	return rng->DrawInteger(0, upper);
}

void Rand::setSeed(long seed)
{
	Rand::seed = seed;
	if(rng == NULL){
		rng = &rngInstance;
		rng->SetSeed(Rand::seed);
		Message::Debug(String("Setting random seed to ") + String::Format(Rand::seed));
	}
}
 
