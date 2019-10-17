/* -*-C-*-
********************************************************************************
*
* File:         Evaluator.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Evaluator.h,v 1.1.1.1 2000/11/01 21:33:58 aleksander Exp $
* Description:
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 14:09:03 1998
* Modified:     Tue Aug 18 15:09:29 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_EVALUATOR_H
#define INCLUDED_EVALUATOR_H

#include "Population.h"
#include "AbstractFitness.h"
#include "AbstractEvaluator.h"
#include "Stats.h"

class Evaluator : public AbstractEvaluator
{

  Stats &stats;
public:
  Evaluator(AbstractFitness *fitness_, Stats &stats_)
    : fitness(fitness_), stats(stats_)
    {
    }

  virtual ~Evaluator()
    {
    }


	 AbstractFitness *fitness;
  void eval(Population &p);

  virtual void hook()
    {
    }
};



#endif
 
