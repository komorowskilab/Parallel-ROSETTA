/* -*-C-*-
********************************************************************************
*
* File:         Population.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Population.h,v 1.1.1.1 2000/11/01 21:34:06 aleksander Exp $
* Description:  Population class
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 13:47:06 1998
* Modified:     Tue Aug 18 15:04:08 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_POPULATION_H
#define INCLUDED_POPULATION_H

#include "macros.h"
#include "Individual.h"
#include <kernel/system/iostream.h>

class Population
{
 public:
  Vector(Individual*) population;

  double sum_fitness;
  double avg_fitness;
  double stdd_fitness;
  double min_fitness;
  double max_fitness;

  double sum_scaled_fitness;
  double avg_scaled_fitness;
  double stdd_scaled_fitness;
  double min_scaled_fitness;
  double max_scaled_fitness;

  int fittest;
  int worst;

  Vector(int) parents;
  Vector(int) parents_indices;
  Vector(Individual *) offspring;

	int size() { return population.size(); }
	Individual * operator[](int i) { return population[i]; }

	virtual ~Population(){
		for(unsigned int i = 0; i < population.size(); i++)
					delete population[i];
	}

	ostream &print(ostream & out){
		out << max_fitness << " " << sum_fitness << "\n";
		for(unsigned int i = 0; i < population.size(); i++)
			population[i]->print(out);
		return out;
	}
};

#endif

 
