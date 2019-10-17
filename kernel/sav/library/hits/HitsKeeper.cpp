// HitsKeeper.cpp: implementation of the HitsKeeper class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "HitsKeeper.h"
#include <kernel/system/float.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



HitsKeeper::HitsKeeper(Context &c) : Keeper(c.parameters.keep_size), context(c)
{
	float hf = context.parameters.hit_fraction;
	int levels = context.parameters.keep_steps;

	float dx = (1.0f - hf);
	if(levels > 0)
		dx /= (levels); 

	int the_size = limit/(levels + 1);
//	cerr << "HitsKeeper: " << the_size << " " << hf << " " << levels << " " << dx << "\n";

	for(int i = 0; i <= levels; i++){
//		cerr << i << " : " << hf + i*dx << " " << "\n";

		Keeper *k = new Keeper(the_size);
		k->crit = new HitsCriteria((hf + i*dx) * context.sum_set_weights); // gets deleted in ~Keeper()
		keepers.push_back(k);
	}
}

HitsKeeper::~HitsKeeper()
{
	for(int i = 0; i < keepers.size(); i++)
		delete keepers[i];

}

int HitsKeeper::size()
{
	int sum = 0;
	for(int i = 0; i < keepers.size(); i++)
		sum += keepers[i]->size();
	return sum;
}

int HitsKeeper::keep(Individual *i, double fitness)
{
	int r = 1;
	for(int j = keepers.size() - 1; j >= 0; j--){
		int r = keepers[j]->keep(i, fitness);
		switch(r){
		case 0: // inserted
			return r;
		case 1: // failed criteria
			break;
		case 2: // too low fitness to be inserted, let next take it
			return r;
		case 3: // already inserted
			return r;
		}
	}
	return r;
}

int HitsKeeper::kept(Vector ( Individual * ) & out)
{
	for(int i = 0; i < keepers.size(); i++)
		keepers[i]->kept(out);
	return out.size();
}

int HitsKeeper::keptList(Vector ( Vector ( Individual * ) ) & out)
{
	int i;
	for(i = out.size(); i < keepers.size(); i++){
		Vector(Individual *) dummy;
		out.push_back(dummy);
	}
	int sum = 0;
	for(i = 0; i < keepers.size(); i++)
		sum += keepers[i]->kept(out[i]);
	return sum;
}

bool HitsKeeper::contains(Individual *i, double fitness)
{
	for(int j = keepers.size() - 1; j >= 0; j--){
		if(keepers[j]->contains(i, fitness))
			return true;
	}
	return false;
}

double HitsKeeper::low()
{
	double lw = DBL_MAX;
	for(int i = 0; i < keepers.size(); i++){
		double l = keepers[i]->low();
		if(lw > l)
			lw = l;
	}
	return lw;
}
 
