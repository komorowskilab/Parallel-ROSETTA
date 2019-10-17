/* -*-C++-*-
********************************************************************************
*
* File:         Individual.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Individual.h,v 1.1.1.1 2000/11/01 21:34:01 aleksander Exp $
* Description:  The individual abstract class
* Author:       Staal Vinterbo
* Created:      Fri Jul 10 16:42:11 1998
* Modified:     Tue Aug 18 15:41:32 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C++
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef EA_INDIVIDUAL_H
#define EA_INDIVIDUAL_H

#include "macros.h"
#include <kernel/system/iostream.h>

class Individual
{
public:

  bool reeval;
  double fitness;
  double norm_fitness;
  double norm_sfitness;
  double sfitness;

	// should not really be here...
	double term1;
	double term2;
	double cover;

  int age;
  int kill_date;

  virtual void copyInto(Individual& p) const
  {
    p.reeval = reeval;
    p.fitness = fitness;
    p.norm_fitness = norm_fitness;
    p.norm_sfitness = norm_sfitness;
    p.sfitness = sfitness;
    p.age = age;
    p.kill_date = kill_date;
		p.term1 = term1;
		p.term2 = term2;
		p.cover = cover;
	}

  Individual() :
    reeval(true),
    fitness(0.0),
    norm_fitness(0.0),
    norm_sfitness(0.0),
    sfitness(0.0),
	term1(0.0),
	term2(0.0),
	cover(0),
	age(0),
    kill_date(0)

  {
  }

  virtual ~Individual()
  {
  }

	virtual Individual * clone() = 0;
	virtual bool operator==(Individual & i) = 0;
	virtual bool operator<(Individual & i) = 0;
	virtual ostream &print(ostream& out) = 0;
	virtual Individual &operator=(Individual &p){
		p.copyInto(*this);
		return *this;
	}

protected:

  Individual(const Individual &p)
  {
    p.copyInto(*this);
  }
};

#endif





 
