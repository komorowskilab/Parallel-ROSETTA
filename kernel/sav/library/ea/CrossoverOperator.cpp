// rossoverOperator.cpp: implementation of the CrossoverOperator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "CrossoverOperator.h"
#include "EABitset.h"
#include "Rand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CrossoverOperator::CrossoverOperator(int points) : times(points)
{

}

CrossoverOperator::~CrossoverOperator()
{

}

bool CrossoverOperator::apply(Individual & m, Individual & f, Individual & s, Individual & b)
{
	// downcast
	EABitset * p1 = (EABitset*)&m;
  EABitset * p2 = (EABitset*)&f;
	EABitset * o1 = (EABitset*)&b;
  EABitset * o2 = (EABitset*)&s;

  int size = p1->size();

	if(size <= 2) // two genes only, no point in crossover
		return false;

  // who shall inherit the world..or at least the inversion of the parents
  int inherit = Rand::i() % 2;

  // this copies the inversion information
  if(inherit == 0){
    *o1 = *p1;
    *o2 = *p2;
  }
  else{
		*o1 = *p2;
    *o2 = *p1;
  }

  for(int ii = 0; ii < times; ii++){
    // start of second segment
    int point = (Rand::i() % (size - 2)) + 1;

    // disregard inversion information below
    for(int i = size - 1; i >= 0; i--){
      if(i < point){
				o1->setP(i, p1->getP(i));
				o2->setP(i, p2->getP(i));
			}
			else{
				o1->setP(i, p2->getP(i));
				o2->setP(i, p1->getP(i));
			}
		}
    // tell the evaluator that this needs to be reevaluated
    o1->reeval = true;
    o2->reeval = true;
    o1->age = 0; // newborn
    o2->age = 0;
	}
	return true;
}
 
