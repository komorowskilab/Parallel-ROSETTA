// RecombineAdd.h: interface for the RecombineAdd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECOMBINEADD_H__82C5F8C2_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_RECOMBINEADD_H__82C5F8C2_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"

class RecombineAdd : public AbstractOperation
{
	int limit;
public:
	Population & apply(Population &p, int generation = 0);
	RecombineAdd(int max_pop_size = 1500);
	virtual ~RecombineAdd();

};

#endif // !defined(AFX_RECOMBINEADD_H__82C5F8C2_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
