// HitsEngine.cpp: implementation of the HitsEngine class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "HitsEngine.h"
#include "HitsMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HitsEngine::HitsEngine(AbstractInitializer &i, Evaluator &e,
                       Vector(AbstractOperation*) &o, 
                       AbstractStoppingCriteria &s, 
                       Keeper &keep, Context &context) 
  : Engine(i, e, o, s), keeper(keep), c(context),
    lastfc(0)
{
}

HitsEngine::~HitsEngine()
{
}

void HitsEngine::hook(Population & p, int generation)
{
  bool changed = false;
  int nchanged = 0;
  
  Evaluator * meval = dynamic_cast(Evaluator*, &eval);
    
  // update keeplist
  //    SDebug.p("doing keep...");
  for(int i = 0; i < p.size(); i++){
    Individual * ind = p[i];
    bool ins = keeper.keep(ind,ind->fitness) == 0;
    changed |= ins;
    if(ins)
      nchanged++;
  }
  c.keep_changed = changed;
  if(c.parameters.report_gen){
    if(c.parameters.debug)
      p.print(cerr);
    char buf[BUFSIZ];
    sprintf(buf, "%d %f %f %f %d %d %d %d %f %d",
            generation, 
            p.avg_fitness, 
            p.min_fitness,
            p.max_fitness,
	    nchanged,
	    p.size(),
	    ((*meval).fitness->eval_count - lastfc),
            (*meval).fitness->eval_count, keeper.low(), keeper.size());
    HitsMessage::warn(buf);
  }
  lastfc = (*meval).fitness->eval_count;
  // + "\n population:\n" + p);
#ifdef _SAVHACK
  if(c.parameters.is_interruptible) {
#endif
		if (generation % 10 == 0) { // Added 990526 by Aleksander šhrn.
			Message message;          // Added 990526 by Aleksander šhrn.
			interrupted = !message.Progress("Calculating, at generation " + String::Format(generation) + "...");
		}
#ifdef _SAVHACK
  }
#endif
}
 
