// UnaryOperation.cpp: implementation of the UnaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include <kernel/system/math.h> // Added 980901 by Aleksander Ührn.
#include "UnaryOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


UnaryOperation::~UnaryOperation()
{

}

UnaryOperation::UnaryOperation(AbstractUnaryOperator & op, double percent)
: operation(op), frac(percent)
{
}

Population & UnaryOperation::apply(Population & p, int /*generation*/)
{
	 if(p.parents_indices.size() < 1)
      return p;

    // determine number of indices to process from the p.parentsInd list
    int number = (int)floor(frac * p.size());

    if(number == 0)
      return p;

    if(number > (int)p.parents_indices.size()) // make sure there are enough...
      number = p.parents_indices.size();

	  //cerr << "Unop: using " << number << " parents\n";

		// get parents from p.parents_indices
		Vector(int) parents;
		parents.reserve(number);

		parents.insert(parents.end(), p.parents_indices.begin(),
			                            p.parents_indices.begin() + number);
		// remove the range from p.parents_indices
		p.parents_indices.erase(p.parents_indices.begin(),
			                      p.parents_indices.begin() + number);

    // mate
	  int i = 0;
    while(number-- > 0)
    {

			int mother_index = parents[i];

      i ++;

      // get parents from offspring if it is there, otherwise from population

      Individual * mother = p.offspring[mother_index];

			if(mother == NULL){ // get from population
				mother = p.population[p.parents[mother_index]];
			}

			// create offspring

			Individual * sister = mother->clone();
			sister->age = 0;

      operation.apply(*mother, *sister);  // apply operator

      // insert into offspring at parent index in offspring
      p.offspring[mother_index] = sister;
		}

    return p;
}
 
