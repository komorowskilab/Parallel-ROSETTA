/* -*-C-*-
********************************************************************************
*
* File:         Engine.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Engine.h,v 1.1.1.1 2000/11/01 21:33:58 aleksander Exp $
* Description:
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 14:21:14 1998
* Modified:     Tue Aug 18 15:08:33 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_ENGINE_H
#define INCLUDED_ENGINE_H

#include "macros.h"
#include "Population.h"
#include "AbstractInitializer.h"
#include "AbstractStoppingCriteria.h"
#include "AbstractEvaluator.h"
#include "AbstractOperation.h"


class Engine
{
 public:
  Population *pop;
  AbstractInitializer &init;
  AbstractEvaluator &eval;
  Vector(AbstractOperation*) &opers;
  AbstractStoppingCriteria &stop;

  int first_time;
  int generation;
  int interrupted;

  Engine(AbstractInitializer &i, AbstractEvaluator &e,
         Vector(AbstractOperation*) &o, AbstractStoppingCriteria &s):
    init(i), eval(e), opers(o), stop(s), first_time(1), generation(0),
    interrupted(0)
    {
    }

  void run();
  virtual void hook(Population &/*p*/, int /*generation*/)
    {
    }
};


#endif
 
