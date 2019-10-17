/* -*-C++-*-
********************************************************************************
*
* File:         Engine.cpp
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/Engine.cpp,v 1.1.1.1 2000/11/01 21:33:58 aleksander Exp $
* Description:
* Author:       Staal Vinterbo
* Created:      Tue Aug 18 15:47:20 1998
* Modified:     Tue Aug 18 15:57:56 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C++
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Engine.h"
#include <kernel/system/assert.h>

void Engine::run()
{
  if (first_time) { // initialize stuff
    first_time = 0;

    pop = &init.init();
    generation = 1;
//cerr << "entering eval.eval for the first time..\n";
    eval.eval(*pop);

    hook(*pop, generation);
  }

  while ((!interrupted) && !stop.stop(*pop, generation)) {

    for (unsigned int i = 0; i < opers.size(); i++){
			assert(opers[i] != NULL);
      opers[i]->apply(*pop, generation);
		}

    generation++;

    eval.eval(*pop);

    hook(*pop, generation);
  }
}



 
