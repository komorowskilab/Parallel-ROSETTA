// InversionOperator.cpp: implementation of the InversionOperator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "EABitset.h"
#include "InversionOperator.h"
#include "Rand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InversionOperator::InversionOperator(int rounds) : times(rounds)
{

}

InversionOperator::~InversionOperator()
{

}

bool InversionOperator::apply(Individual & in, Individual & out)
{
	EABitset * i = (EABitset *)&in;
	EABitset * o = (EABitset *)&out;

	*o = *i;

	if(i->size() < 2)
		return false;

	for(int ii = 0; ii < times; ii++){

		int point1;
		int point2;
		do{
		   point1= Rand::i() % i->size();
			 point2= Rand::i() % i->size();
		}while(point1 == point2);
		o->transpose(point1, point2);
	}

	//o->reeval = true;
  o->age = 0; // newborn

	return true;
}
 
