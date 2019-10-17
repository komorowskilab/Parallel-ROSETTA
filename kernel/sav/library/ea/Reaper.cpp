// Reaper.cpp: implementation of the Reaper class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Reaper.h"
#include <kernel/system/assert.h>
#include <kernel/basic/message.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Reaper::~Reaper()
{

}

Population & Reaper::apply(Population & p, int generation)
{
	int killed = 0;

 	double maxsfitness = p.population[p.fittest]->sfitness;
	double minsfitness = p.population[p.worst]->sfitness;
	unsigned int i;
  for(i = 0; i < p.population.size(); i++)
  {
    Individual * e = p.population[i];
    // check if this is a newborn to be assigned a killdate to
    if(e->age == 0){
      e->kill_date = generation +
         setAge(e->sfitness, p.avg_scaled_fitness, minsfitness,
		            maxsfitness);
    }
    e->age++;
    if(e->kill_date <= generation){
      delete e;
			p.population[i] = NULL;
			killed++;
    }
  }

  Vector(Individual *) npop;
	npop.reserve(p.size() - killed);

	for(i = 0; i < p.population.size(); i++){
		if(p.population[i] != NULL)
			npop.push_back(p.population[i]);
	}

	p.population = npop;

  return stats.apply(p);
}

Reaper::Reaper(int min, int max) : maxLT(max), minLT(min)
{
//	Message::Progress(
//		"Reaper: (min lt, max lt) = (" + String(minLT) +","+String(maxLT)+")");
	nu = (max - min)/2;
  mu = (max + min)/2;
}

int Reaper::setAge(double fitness, double avgFit, double minFit, double maxFit)
{
	int lt = (int)(fitness >= avgFit ?
                 (minLT + nu*(fitness - minFit)/(avgFit - minFit)) :
                 (mu + nu*(fitness - avgFit)/(maxFit - avgFit)));
	if(lt < minLT)
		lt = minLT;
	if(lt > maxLT)
		lt = maxLT;
	return lt;
}
 
