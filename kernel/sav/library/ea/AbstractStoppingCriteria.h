// AbstractStoppingCriteria.h: interface for the AbstractStoppingCriteria class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABSTRACTSTOPPINGCRITERIA_H__909F6272_3B76_11D2_AF8A_000000000000__INCLUDED_)
#define AFX_ABSTRACTSTOPPINGCRITERIA_H__909F6272_3B76_11D2_AF8A_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Population.h"

class AbstractStoppingCriteria
{
public:
	virtual bool stop(Population &p, int generation = 0) = 0;
	AbstractStoppingCriteria();
	virtual ~AbstractStoppingCriteria();

};

#endif // !defined(AFX_ABSTRACTSTOPPINGCRITERIA_H__909F6272_3B76_11D2_AF8A_000000000000__INCLUDED_)
 
