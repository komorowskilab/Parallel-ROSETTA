// Boltzscaler.cpp: implementation of the Boltzscaler class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Boltzscaler.h"
#include <kernel/system/math.h> // Added 980901 by Aleksander Ührn.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Boltzscaler::Boltzscaler() : startTmp(6.0), endTmp(1.0), deltaTmp(0.02)
{
	t = startTmp;
}

Boltzscaler::~Boltzscaler()
{

}

Population & Boltzscaler::apply(Population & p, int /*generation*/)
{
	if(p.size() == 0)
		return p;

	double sum = 0.0;
  // calc e^(f(i)/T), and avg for population
	int i;
  for(i = 0; i < p.size(); i++)
  {
    Individual * ind = p.population[i];
    ind->sfitness = exp(ind->fitness/t);
    sum += ind->sfitness;
  }
  double avg = sum/p.size();

  // normalize with avg.
  for(i = 0; i < p.size(); i++)
  {
    Individual * ind = p.population[i];
    ind->sfitness /= avg;
  }

	// recompute statistics....might be done more efficiently..
  p = stats.apply(p);

  // compute new temperature
  t = newTmp(t);

  return p;
}


double Boltzscaler::newTmp(double t)
{
	if(t > endTmp)
    t -= deltaTmp;
  if(t < endTmp)
    t = endTmp;
  return t;
}

Boltzscaler::Boltzscaler(double start, double end, double delta)
	: startTmp(start), endTmp(end), deltaTmp(delta)
{
	t = startTmp;
}
 
