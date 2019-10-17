#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Evaluator.h"
#include <kernel/system/float.h> // Added 980901 by Aleksander Ührn.


void Evaluator::eval(Population &p)
{
	double sum = 0.0;
  double sumsq = 0.0;
  double fittestFit = 0.0;
  double worstFit = DBL_MAX;

    //    SDebug.p("Evaluator.eval: popsize: " + p.population.size());
  for(unsigned int i = 0; i < p.population.size(); i++){
    Individual * ind = p.population[i];
    ind->fitness = fitness->eval(ind);
    ind->sfitness = ind->fitness; // this will be scaled
  }
  p = stats.apply(p);
  hook();
} 
