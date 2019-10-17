#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Stats.h"
#include <kernel/system/math.h>  // Added 980901 by Aleksander Ührn.
#include <kernel/system/float.h> // Added 980901 by Aleksander Ührn.


Population &Stats::apply(Population &p, int /*generation*/)
{

	int size = (int)p.population.size();
  if(size < 1)
    return p;

  double sum = 0.0;
  double sumsq = 0.0;
  double sums = 0.0;
  double sumssq = 0.0;
  double fittestFit = 0.0;
  double worstFit = DBL_MAX;
  double fittestSFit = 0.0;
  double worstSFit = DBL_MAX;

	int i;
  for(i = 0; i < size; i++){
    Individual * ind = p.population[i];

    sum += ind->fitness;
    sumsq += ind->fitness * ind->fitness;
    sums += ind->sfitness;
    sumssq += ind->sfitness * ind->sfitness;

    if(ind->fitness > fittestFit){
      p.fittest = i;
      fittestFit = ind->fitness;
      fittestSFit = ind->sfitness;
    }
    if(ind->fitness < worstFit){
      worstFit = ind->fitness;
      worstSFit = ind->sfitness;
			p.worst = i;
    }
  }


  double svar = size > 1 ? (size*sumsq - sum*sum)/(size*(size - 1)) : 0;
  double ssvar = size > 1 ? (size*sumssq - sums*sums)/(size*(size - 1)): 0;

  p.stdd_fitness = fabs(sqrt(svar));
  p.stdd_scaled_fitness = fabs(sqrt(ssvar));
  p.sum_scaled_fitness = sums;
  p.avg_scaled_fitness = sums/size;
  p.avg_fitness = sum/size;
  p.min_scaled_fitness = worstSFit;
  p.max_scaled_fitness = fittestSFit;
  p.min_fitness = worstFit;
  p.max_fitness = fittestFit;

  for(i = 0; i < size; i++){
    Individual * ind = p.population[i];
    ind->norm_sfitness = sums == 0 ? 0 : ind->sfitness/sums;
    ind->norm_fitness = sums == 0 ? 0 : ind->fitness/sum;
  }

  return p;
}
 
