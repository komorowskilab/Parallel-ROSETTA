// HitsFitness.h: interface for the HitsFitness class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HITSFITNESS_H__F2ED4F81_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_HITSFITNESS_H__F2ED4F81_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/AbstractFitness.h"
#include "../ea/EABitset.h"
#include "../ea/macros.h"
#include "Context.h"

class HitsFitness  : public AbstractFitness
{
protected:
	Vector(float) &costs;
	float fbias;
	Context &context;
	float sum_costs;
public:
	virtual double eval(Individual * ind);
	float term2(EABitset * b);
	float term1(EABitset *b);
	HitsFitness(Context &c);
	virtual ~HitsFitness();

};

#endif // !defined(AFX_HITSFITNESS_H__F2ED4F81_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
