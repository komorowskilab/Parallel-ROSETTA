/* -*-C-*-
********************************************************************************
*
* File:         Keeper.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Keeper.h,v 1.1.1.1 2000/11/01 21:34:04 aleksander Exp $
* Description:
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 15:17:48 1998
* Modified:     Tue Aug 18 15:56:01 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDED_KEEPER_H
#define INCLUDED_KEEPER_H

#include "macros.h"
#include "Individual.h"
#include <kernel/basic/message.h>

class Criteria
{
public:
  virtual int keep(Individual * /*ind*/, double /*fitness*/)
  {
    return 1;
  }
};

struct ltip
{
  bool operator()(Individual *s1, Individual *s2) const
  {
    return (*s1 < *s2);
  }
};

// define USING_VECTOR until order has been defined on Individuals
#define USING_VECTOR

class Keeper
{
 protected:
//  typedef Map(double, Vector(Individual *)) mymap;
#ifdef USING_VECTOR
	 typedef Vector(Individual *) subcontainer;
#else
	 typedef std::set<Individual *, ltip> subcontainer;
#endif
	 typedef std::map<double, subcontainer, std::less<double> > mymap;
 public:

  mymap the_list;     // mymap::iterator it = list.begin()
  int limit;
  int changed;
  Criteria *crit;
  int the_size;


  Keeper(int size) : limit(size), changed(0), crit(NULL), the_size(0)
    {
    }

  virtual ~Keeper();

	virtual double low();
  virtual int keep(Individual *i, double fitness);
  virtual bool contains(Individual *i, double fitness);
  virtual int size();
  virtual int kept(Vector(Individual *) & list);
};


#endif
 
