// RecombineReplaceSample.h: interface for the RecombineReplaceSample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECOMBINEREPLACESAMPLE_H__82C5F8C3_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_RECOMBINEREPLACESAMPLE_H__82C5F8C3_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "AbstractSampler.h"

class RecombineReplaceSample : public AbstractOperation
{
	AbstractSampler &sampler;
public:
	Population & apply(Population & p, int generation = 0);
	RecombineReplaceSample(AbstractSampler &s);
	virtual ~RecombineReplaceSample();

};

#endif // !defined(AFX_RECOMBINEREPLACESAMPLE_H__82C5F8C3_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
