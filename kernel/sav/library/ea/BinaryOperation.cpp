// BinaryOperation.cpp: implementation of the BinaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "BinaryOperation.h"
#include <math.h>
#include "Individual.h"

//debug
#include <kernel/basic/message.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



BinaryOperation::~BinaryOperation()
{
	
}

Population & BinaryOperation::apply(Population & p, int /*generation*/)
{
	//cerr << "Binop : indices size: " << p.parents_indices.size() << "\n";
	if(p.parents_indices.size() < 2)
		return p;
	
    // determine number of indices to process from the p.parentsInd list
    int number = (int)floor(frac * p.size());
		
    if(number == 0)
		return p;
	
    if(number > (int)p.parents_indices.size()) // make sure there are enough...
		number = p.parents_indices.size();
	
    if(number % 2 != 0) // check that it is even
		number--;
	
	//cerr << "Binop: number of parents to use: " << number << "\n";
	
	// get parents from p.parents_indices
	Vector(int) parents;
	parents.reserve(number);
	
	parents.insert(parents.end(), p.parents_indices.begin(),
		p.parents_indices.begin() + number);
	// remove the range from p.parents_indices
	p.parents_indices.erase(p.parents_indices.begin(),
		p.parents_indices.begin() + number);

    // mate
	number /= 2; // make number of pairs
    int i = 0;
    while(number-- > 0)
    {
		int father_index = parents[i];
		int mother_index = parents[i + 1];
		
		i += 2;
		
		// get parents from offspring if it is there, otherwise from population
		Individual * father = p.offspring[father_index];
		Individual * mother = p.offspring[mother_index];
		
		if(father == NULL){ // get from the population
			father = p.population[p.parents[father_index]];
			assert(father != NULL);
		}
		if(mother == NULL){ // get from population
			mother = p.population[p.parents[mother_index]];
			assert(mother != NULL);
		}
		
		// create offspring
		Individual * brother = father->clone();
		Individual * sister = mother->clone();
		
		brother->age = 0;
		sister->age = 0;
		
		operation.apply(*mother, *father, *sister, *brother);  // apply operator
		
		// insert into offspring at parent index in offspring
		p.offspring[father_index] = brother;
		p.offspring[mother_index] = sister;
	}
	
    return p;
}


BinaryOperation::BinaryOperation(AbstractBinaryOperator & op, double percent)
: frac(percent), operation(op)
{
	
}
 
