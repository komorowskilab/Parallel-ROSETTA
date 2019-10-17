// ontext.cpp: implementation of the Context class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Context.h"
#include <kernel/basic/message.h>
#include <kernel/system/assert.h>
#include "HitsMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Context::Context(const Vector(const Bits*) &bits, Parameters &p)
: sets(bits), parameters(p), nelems(0)
{

	// calculate mapping etos
	if (!sets.empty()) {
		nelems = sets[0]->GetSize();
		elements_used = Bits(nelems, false);
	}

	nsets = sets.size();	
	
	int i;
	for(i = 0; i < nelems; i++)
		etos.push_back(Bits(nsets,false));
	
	for(i = 0; i < sets.size(); i++){
		const Bits * b = sets[i];
		setsize.push_back(b->Count(true));
		for(int j = 0; j < b->GetSize(); j++){
			if(b->GetState(j)){
				elements_used.SetState(j, true);
				etos[j].SetState(i, true);
			}
		}
	}

	elems_used = elements_used.Count(true);

	// calculate real index
	real_index.reserve(elems_used);
	for(i = 0; i < nelems; i++){
		if(elements_used.GetState(i))
			real_index.push_back(i);
	}

	for(i = nelems - 1; i >= 0; i--)
		if(!elements_used.GetState(i)){
			etos.erase(etos.begin() + i);
		}
	
	// initialize default cost
	for(i = p.costs.size(); i < nelems; i++)
			p.costs.push_back(1.0);

	// initialize default set_weights
	for(i = p.set_weights.size();i < nsets; i++)
			p.set_weights.push_back(1.0);

	// calculate sum of sets_weights
	sum_set_weights = 0;
	for(i = 0; i < nsets; i++)
		sum_set_weights += p.set_weights[i];

  // calculate keepers requirement
	keep_limit = p.hit_fraction * sum_set_weights;
}

Context::~Context()
{
}

double Context::covers(EABitset * b)
{
	Bits su(nsets, false);
	double count = 0;
	int i;
	for(i = 0; i < b->size(); i++){
		if(b->get(i)){
			//Vector(int) * v  = &etos[i];
			Bits * v = &etos[i];
			assert(v != NULL);			
			su |= *v;

			if(su == true) // Really big optimization by AÜ/SAV. Possible since dimension of su == nsets.
				return sum_set_weights;
		}
	}
	for(i = 0; i < nsets; i++)
		if(su.GetState(i))
			count += parameters.set_weights[i];	

	return count;
}
 
