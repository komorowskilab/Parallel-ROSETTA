// ParentsSelector.cpp: implementation of the ParentsSelector class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "ParentsSelector.h"
#include <kernel/system/math.h> // Added 980901 by Aleksander Ührn.

#include "macros.h"
#include "Rand.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class MyRNG{  // used in random_shuffle for recreatibility
public: int operator()(int n){ return Rand::i() % n; }
};

ParentsSelector::~ParentsSelector()
{
}

Population & ParentsSelector::apply(Population & p, int /*generation*/)
{
	if(p.size() == 0)
		return p;

	if(p.parents.size() != 0)
		p.parents.erase(p.parents.begin(), p.parents.end());

	if(p.parents_indices.size() != 0)
		p.parents_indices.erase(p.parents_indices.begin(), p.parents_indices.end());

	int elements = (int)floor(frac * p.size());
	//cerr << "selecting " << elements << " parents\n";

	Vector(double) c;
	c.reserve(p.size());

	// find cumulative normalized scaled fitness value distribution
	double sum = 0.0;
	int i;
	for(i = 0; i < p.size(); i++){
		c.push_back((sum += p[i]->norm_sfitness));
	}
	
	Vector(int) isample;
	isample.reserve(elements);

	if(!(replacement ? sampler.sample(c, isample, elements) : sampler.sampleNR(c, isample, elements)))
		return p;

	//cerr << "isample size: " << isample.size() << "\n";

	Map(int, int) map;

	for(i = 0; i < (int)isample.size(); i++){
		//cerr << "isample[" << i << "] : " << isample[i] << "\n";
		if(map.find(isample[i]) == map.end()){
			p.parents.push_back(isample[i]);
			map[isample[i]] = p.parents.size() - 1;
		}
		//cerr << "map[isample[" << i << "]] : " << map[isample[i]] << "\n";
		p.parents_indices.push_back(map[isample[i]]);
	}

	//cerr << "p.parents size: " << p.parents.size() << "\n";
	//cerr << "p.parents_indices size: " << p.parents_indices.size() << "\n";

	// shuffle parents
	MyRNG rng;
	std::random_shuffle(p.parents_indices.begin(),
		p.parents_indices.end(),
		rng);

	// initiate p.offspring to be right size and full of NULL's
	if(p.offspring.size() != 0)
		p.offspring.erase(p.offspring.begin(), p.offspring.end());
	p.offspring.reserve(elements);
	//p.offspring.insert(p.offspring.end(), elements, NULL);
	p.offspring.insert(p.offspring.end(), elements, (Individual*) 0);

	return p;
}






ParentsSelector::ParentsSelector(AbstractSampler & s, double percent, bool r)
: sampler(s), frac(percent), replacement(r)
{

}
 
