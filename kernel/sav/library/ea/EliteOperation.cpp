// EliteOperation.cpp: implementation of the EliteOperation class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "EliteOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EliteOperation::EliteOperation() : elite(NULL)
{

}

EliteOperation::~EliteOperation()
{
	//RA 040910 Clean up pointers
	delete elite;
}

Population & EliteOperation::apply(Population & p, int /*generation*/)
{
	if(p.size() == 0)
    return p;

  Individual * max = p.population[p.fittest];
  if(elite == NULL || (getFitness(elite) <= getFitness(max))){ // first time
		if(elite == NULL)
			elite = max->clone();
		else
			*elite = *max;
    return p;
  }

  if(getFitness(elite) > getFitness(max)){ // lost elite, reinsert
	
    *p.population[p.worst] = *elite;
		p.population[p.worst]->age = 0;
    return stats.apply(p);
  }

  return p;
}

double EliteOperation::getFitness(Individual * i, bool scaled)
{
	return scaled ? i->sfitness : i->fitness;
}
 
