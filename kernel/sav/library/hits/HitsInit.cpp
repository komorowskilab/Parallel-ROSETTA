// HitsInit.cpp: implementation of the HitsInit class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "HitsInit.h"
#include "HitsMessage.h"
#include "../ea/Rand.h"
#include <kernel/system/assert.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HitsInit::HitsInit(Context &context) : c(context)
{

}

HitsInit::~HitsInit()
{

}

Population & HitsInit::init()
{
	for(int i = 0; i < c.parameters.population_size; i++){
		EABitset * v = new EABitset(c.elems_used);
		assert(v->size() == c.elems_used);
		for(int j = 0; j < c.nsets; j++){
			if(c.setsize[j] == 0){
				continue;
			}
			int e = Rand::i() % c.setsize[j];
			// pick the e'th element in that set
			const Bits * set = c.sets[j];
			int l = 0;
			int elm;
			int real_index = 0;
			for(int k = 0; k < set->GetSize(); k++){		
				if(set->GetState(k))
					if(l++ == e){
						elm = real_index;
						break;
					}		
				if(c.elements_used.GetState(k))
					real_index++;
			}
			v->set(elm, true);
		}
		assert(v->size() == c.elems_used);
		p.population.push_back(v);
	}
	
	return p;
}
 
