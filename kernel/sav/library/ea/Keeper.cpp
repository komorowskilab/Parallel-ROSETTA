#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Keeper.h"
#include <kernel/system/assert.h> // Added 980901 by Aleksander Ührn.

#include <kernel/basic/message.h> // Added 980901 by Aleksander Ührn.

int Keeper::keep(Individual *i, double fitness)
{
	assert(i != NULL);
	if(crit != NULL && crit->keep(i, fitness) == false)
		return 1;
	if(the_size >= limit && fitness <= low())
		return 2;
	if(contains(i, fitness))
		return 3;

//	assert(i->cover > 0);
	mymap::iterator p = the_list.find(fitness);
//	Vector(Individual*) * v;
	subcontainer * v;
	v = &the_list[fitness];
	v->insert(v->end(), i->clone()); // gets deleted in ~Keeper
	the_size++;

	// remove one of the elements that have the lowest fitness if
	// the_list.size() is larger than limit
	if(the_size > limit){ // remove one of the lowest fitness elements
		mymap::iterator it = the_list.begin();
		assert(it != the_list.end());
		v = &(*it).second;
		assert(v->size() != 0);
		Individual * ind = *(v->begin());
		v->erase(v->begin());
		the_size--;
		delete ind;
		if(v->empty()){
			the_list.erase(it);
		}
	}

	return 0;
}


// log n + k, n = number of different fitness values inserted,
//            k = number of elements having fitness fitness
bool Keeper::contains(Individual *i, double fitness)
{
	assert(i != NULL);
	mymap::iterator p = the_list.find(fitness);
	if(p == the_list.end())
		return false;

//	Vector(Individual *) * v = &(*p).second;
	subcontainer * v = &(*p).second;
	assert(!v->empty());
#ifdef USING_VECTOR
	for(unsigned int j = 0; j < v->size(); j++){
		if(*v->operator[](j) == *i)
			return true;
	}
	return false;
#else
	return v->find(i) != v->end();
#endif

}

int Keeper::size()
{
	return the_size;
}


int Keeper::kept(Vector(Individual *) & list)
{
	for(mymap::iterator it = the_list.begin(); it != the_list.end(); it++){
		for(subcontainer::iterator its = ((*it).second).begin();

		    its != ((*it).second).end(); its++)
					list.push_back(*its);
	}
//		list.insert(list.end(), ((*it).second).begin(), ((*it).second).end());
	return list.size();
}

double Keeper::low()
{
	if(the_list.empty())
		return 0.0;
	mymap::iterator it = the_list.begin();
	return (*it).first;
}

Keeper::~Keeper()
{
	// cleanup list
	for(mymap::iterator i = the_list.begin(); i != the_list.end(); i++){
//		Vector(Individual *) * v = &(*i).second;
		subcontainer * v = &(*i).second;
//		for(unsigned int j = 0; j < v->size(); j++) {
		for(subcontainer::iterator it = v->begin() ; it != v->end(); it++){
			Individual *dummy = *(it); // Added 980901 by Aleksander Ührn.
			delete dummy;
		}
	}
	if(crit != NULL)
		delete crit;
}
 
