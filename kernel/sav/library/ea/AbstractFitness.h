/* -*-C-*-
********************************************************************************
*
* File:         AbstractFitness.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/AbstractFitness.h,v 1.1.1.1 2000/11/01 21:33:48 aleksander Exp $
* Description:  
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 14:14:24 1998
* Modified:     Tue Aug 18 15:07:14 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_ABSTRACTFITNESS_H
#define INCLUDED_ABSTRACTFITNESS_H

#include "Individual.h"

class AbstractFitness
{

 
 public:
  int eval_count;
  virtual double eval(Individual * o) = 0;
 AbstractFitness() : eval_count(0)
    {
    }

  virtual ~AbstractFitness() 
    {
    }
  
};


#endif
 
