// RecombineReplaceParents.cpp: implementation of the RecombineReplaceParents class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "RecombineReplaceParents.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RecombineReplaceParents::RecombineReplaceParents()
{

}

RecombineReplaceParents::~RecombineReplaceParents()
{

}

Population & RecombineReplaceParents::apply(Population & p, int /*generation*/)
{
	if(p.offspring.size() == 0)
		return p;

	for(unsigned int i = 0; i < p.offspring.size(); i++){
		if(p.offspring[i] != NULL){
			delete p.population[p.parents[i]]; // retire parent
			p.population[p.parents[i]] = p.offspring[i];
		}
	}

	p.offspring.erase(p.offspring.begin(), p.offspring.end());
	p.parents.erase(p.parents.begin(), p.parents.end());
	p.parents_indices.erase(p.parents_indices.begin(), p.parents_indices.end());

	return p;
}
 
