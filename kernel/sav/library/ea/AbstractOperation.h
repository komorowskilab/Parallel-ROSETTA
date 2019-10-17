/* -*-C-*-
********************************************************************************
*
* File:         AbstractOperation.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/AbstractOperation.h,v 1.1.1.1 2000/11/01 21:33:48 aleksander Exp $
* Description:  
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 13:57:56 1998
* Modified:     Tue Aug 18 14:58:51 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_ABSTRACTOPERATION_H
#define INCLUDED_ABSTRACTOPERATION_H

#include "Population.h"

class AbstractOperation
{
public:
  virtual ~AbstractOperation()
    {
    }
  
 public:
  virtual Population & apply(Population &p, int generation = 0) = 0;
};


#endif
 
