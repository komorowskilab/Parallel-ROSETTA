// RecombineReplaceParents.h: interface for the RecombineReplaceParents class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECOMBINEREPLACEPARENTS_H__82C5F8C1_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_RECOMBINEREPLACEPARENTS_H__82C5F8C1_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"

class RecombineReplaceParents : public AbstractOperation
{
public:
	Population & apply(Population &p, int generation = 0);
	RecombineReplaceParents();
	virtual ~RecombineReplaceParents();

};

#endif // !defined(AFX_RECOMBINEREPLACEPARENTS_H__82C5F8C1_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
