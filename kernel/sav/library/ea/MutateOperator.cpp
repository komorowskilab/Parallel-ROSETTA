// MutateOperator.cpp: implementation of the MutateOperator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "MutateOperator.h"
#include "EABitset.h"
#include "Rand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MutateOperator::MutateOperator(int rounds) :times(rounds)
{

}

MutateOperator::~MutateOperator()
{

}

bool MutateOperator::apply(Individual & in, Individual & out)
{
	EABitset * i = (EABitset *)&in;
	EABitset * o = (EABitset *)&out;

	*o = *i;

	if(i->size() <= 0)
		return false;

	for(int ii = 0; ii < times; ii++){
		int point = Rand::i() % i->size();
		o->setP(point, o->getP(point) ? false : true);
	}

	o->reeval = true;
  o->age = 0; // newborn

	return true;
}
 
