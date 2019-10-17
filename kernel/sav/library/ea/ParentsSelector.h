// ParentsSelector.h: interface for the ParentsSelector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARENTSSELECTOR_H__1101AA40_3CC2_11D2_AF8E_000000000000__INCLUDED_)
#define AFX_PARENTSSELECTOR_H__1101AA40_3CC2_11D2_AF8E_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "AbstractSampler.h"

class ParentsSelector : public AbstractOperation
{
	AbstractSampler & sampler;
	double frac;
	bool replacement;

public:
	ParentsSelector(AbstractSampler &s, double percent, bool r = true);
	Population & apply(Population &p, int generation = 0);

	virtual ~ParentsSelector();

};

#endif // !defined(AFX_PARENTSSELECTOR_H__1101AA40_3CC2_11D2_AF8E_000000000000__INCLUDED_)
 
