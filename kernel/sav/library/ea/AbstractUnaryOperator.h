// AbstractUnaryOperator.h: interface for the AbstractUnaryOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABSTRACTUNARYOPERATOR_H__8F69F7C6_3C44_11D2_AF8D_000000000000__INCLUDED_)
#define AFX_ABSTRACTUNARYOPERATOR_H__8F69F7C6_3C44_11D2_AF8D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Individual.h"

class AbstractUnaryOperator
{
public:
	virtual bool apply(Individual &in, Individual &out) = 0;
	AbstractUnaryOperator();
	virtual ~AbstractUnaryOperator();

};

#endif // !defined(AFX_ABSTRACTUNARYOPERATOR_H__8F69F7C6_3C44_11D2_AF8D_000000000000__INCLUDED_)
 
