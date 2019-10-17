// HistStop.cpp: implementation of the HistStop class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "HistStop.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HistStop::HistStop(Context &cont, int gap, bool fStop, bool kStop)
: c(cont), gen_gap(gap), use_fs(fStop), use_ks(kStop), max_fitness(0.0),
f_generation(0), k_generation(0), fit_stop(false), keep_stop(false)
{

}

HistStop::~HistStop()
{

}

bool HistStop::stop(Population & p, int generation)
{
	if(p.size() == 0)
      return true;
    
	if(use_fs){
		if(p.avg_fitness > max_fitness){
			max_fitness = p.avg_fitness;
			f_generation = generation;
			fit_stop = false;
		}

		if(generation - f_generation > gen_gap){
			fit_stop = true;
		}
	}

	if(use_ks){
		if(c.keep_changed){ // check if the keep list has changed...
			k_generation = generation;
			keep_stop = false;
		}

    // reset context's check for next run
		c.keep_changed = false;
  
		if(generation - k_generation > gen_gap){
   		keep_stop = true;
		}
	}
  return keep_stop && fit_stop;
}
 
