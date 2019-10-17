// HitsFitness.cpp: implementation of the HitsFitness class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "HitsFitness.h"
#include <kernel/system/assert.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HitsFitness::HitsFitness(Context &c)
: context(c), costs(c.parameters.costs), fbias(c.parameters.bias)
{
	//sum_costs = std::accumulate(w.begin(), w.end(), 0.0);
	sum_costs = 0.0;
	for(unsigned int i = 0; i < context.elems_used; i++)
		sum_costs += costs[context.real_index[i]];
}

HitsFitness::~HitsFitness()
{

}

float HitsFitness::term1(EABitset * b)
{
	assert(b != NULL);
	float count = 0;
  for(int i = 0; i < b->size(); i++){
    if(b->get(i)){
      count += costs[context.real_index[i]];
    }
  }

  return b->term1 = (sum_costs - count)/sum_costs;
}

float HitsFitness::term2(EABitset * b)
{
  b->cover = context.covers(b);
  return (float)(b->term2 = b->cover/(context.sum_set_weights));
}

double HitsFitness::eval(Individual * ind)
{
	EABitset * b = (EABitset*)ind;

  double t1 = b->reeval ? term1(b) : b->term1;
  double t2 = b->reeval ? term2(b) : b->term2;

  if(b->reeval){
    eval_count++;
		b->reeval = false;
	}

  return (1-fbias) * t1 + fbias * (t2 > context.parameters.hit_fraction ? 
	  context.parameters.hit_fraction : t2);
}
 
