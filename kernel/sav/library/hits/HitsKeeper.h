// HitsKeeper.h: interface for the HitsKeeper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HITSKEEPER_H__6F1C52E1_4961_11D2_AF9F_000000000000__INCLUDED_)
#define AFX_HITSKEEPER_H__6F1C52E1_4961_11D2_AF9F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/Keeper.h"
#include "Context.h"

class HitsCriteria : public Criteria
{
	float limit;
	public:
	HitsCriteria(float threshold) : limit(threshold) {}
  int keep(Individual * ind, double /*fitness*/)
  {
    return ind->cover >= (double)limit;
  }
};

class HitsKeeper : public Keeper
{
	Vector(Keeper *) keepers;
	Context &context;
public:
	int keptList(Vector(Vector(Individual *)) &out);
	int kept(Vector(Individual *) &out);
	int keep(Individual *i, double fitness);
	int size();
	HitsKeeper(Context &c);
	virtual ~HitsKeeper();
	double low();
  bool contains(Individual *i, double fitness);
  
};

#endif // !defined(AFX_HITSKEEPER_H__6F1C52E1_4961_11D2_AF9F_000000000000__INCLUDED_)
 
