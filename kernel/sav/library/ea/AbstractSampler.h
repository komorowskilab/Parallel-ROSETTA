/* -*-C-*-
********************************************************************************
*
* File:         AbstractSampler.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/AbstractSampler.h,v 1.1.1.1 2000/11/01 21:33:51 aleksander Exp $
* Description:  
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 14:36:39 1998
* Modified:     Thu Aug 20 13:22:23 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_ABSTRACTSAMPLER_H
#define INCLUDED_ABSTRACTSAMPLER_H

#include "macros.h"
#include "Rand.h"
#include <kernel/system/math.h>
#include <kernel/system/iostream.h>

class AbstractSampler
{
public:
  virtual ~AbstractSampler()
    {
    }
  
 public:
  virtual int sample(Vector(double) &cumdist,
                     Vector(int) &indices, int elements);
  virtual int sampleNR(Vector(double) &cumdist,
                       Vector(int) &indices, int elements);
  static int distToCumdist(Vector(double) &dist, Vector(double) &cumdist);
  static int cumdistToDist(Vector(double) &cumdist, Vector(double) &dist);
};


#endif
 
