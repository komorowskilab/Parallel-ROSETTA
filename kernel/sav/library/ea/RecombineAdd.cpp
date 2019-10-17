// RecombineAdd.cpp: implementation of the RecombineAdd class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "RecombineAdd.h"
#include "RecombineReplaceParents.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RecombineAdd::RecombineAdd(int max_pop_size) : limit(max_pop_size)
{

}

RecombineAdd::~RecombineAdd()
{

}

Population & RecombineAdd::apply(Population & p, int /*generation*/)
{
	if(p.offspring.size() == 0)
		return p;

	if(p.population.size() > limit){
		RecombineReplaceParents r;
		return r.apply(p);
	}

	for(unsigned int i = 0; i < p.offspring.size(); i++){
		if(p.offspring[i] != NULL)
			p.population.push_back(p.offspring[i]);
	}

	p.offspring.erase(p.offspring.begin(), p.offspring.end());
	p.parents.erase(p.parents.begin(), p.parents.end());
	p.parents_indices.erase(p.parents_indices.begin(), p.parents_indices.end());

	return p;
}
 
