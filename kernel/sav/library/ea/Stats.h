/* -*-C-*-
********************************************************************************
*
* File:         Stats.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Stats.h,v 1.1.1.1 2000/11/01 21:34:08 aleksander Exp $
* Description:
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 14:17:13 1998
* Modified:     Tue Aug 18 14:19:44 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_STATS_H
#define INCLUDED_STATS_H

#include "AbstractOperation.h"
#include "Population.h"

class Stats : public AbstractOperation
{
 public:
  Population & apply(Population &p, int generation = 0);
};


#endif
 
