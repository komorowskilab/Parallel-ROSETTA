// RecombineReplaceSample.cpp: implementation of the RecombineReplaceSample class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "RecombineReplaceSample.h"
#include <kernel/system/assert.h> // Added 980901 by Aleksander Ührn.

#include "AbstractSampler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RecombineReplaceSample::RecombineReplaceSample(AbstractSampler &s)
: sampler(s)
{

}

RecombineReplaceSample::~RecombineReplaceSample()
{

}

Population & RecombineReplaceSample::apply(Population & p, int /*generation*/)
{
	if(p.offspring.size() == 0)
		return p;

	if(p.size() == 0)
		return p;

	int elements = p.size() - p.offspring.size();

	//cerr << "replacing sample of size " << elements << "\n";

	// set up distribution
	Vector(double) c;
	c.reserve(p.size());

	// find cumulative normalized scaled fitness value distribution
	double sum = 0.0;
	int i;
	for(i = 0; i < p.size(); i++){
		c.push_back((sum += p[i]->norm_sfitness));
	}

	Vector(int) isample;  // the survivors
	isample.reserve(elements);

	if(!sampler.sampleNR(c, isample, elements))
		return p; // something is wrong....

	//cerr << "recomb: sampled " << isample.size() << "\n";
	// ascending sort
	std::sort(isample.begin(), isample.end());

	// retire non-survivors and replace with offspring
	int j = 0;
	int k = 0;
	for(i = 0; i < (int)isample.size(); i++){
		while(j < isample[i]){
			delete p.population[j];
			p.population[j++] = p.offspring[k++];
		}
		j++;
	}
	// retire the remainders
	while(j < p.size()){
		delete p.population[j];
		p.population[j++] = p.offspring[k++];
	}

	assert(k == (int)p.offspring.size());

	p.offspring.erase(p.offspring.begin(), p.offspring.end());
	p.parents.erase(p.parents.begin(), p.parents.end());
	p.parents_indices.erase(p.parents_indices.begin(), p.parents_indices.end());

	return p;
}

 
