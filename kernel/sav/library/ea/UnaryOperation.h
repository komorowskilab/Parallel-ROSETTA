// UnaryOperation.h: interface for the UnaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNARYOPERATION_H__8F69F7C5_3C44_11D2_AF8D_000000000000__INCLUDED_)
#define AFX_UNARYOPERATION_H__8F69F7C5_3C44_11D2_AF8D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "AbstractUnaryOperator.h"

class UnaryOperation : public AbstractOperation
{
	AbstractUnaryOperator &operation;
	double frac;
public:
	Population & apply(Population &p, int generation = 0);
	 UnaryOperation(AbstractUnaryOperator & op, double percent);

	virtual ~UnaryOperation();

};

#endif // !defined(AFX_UNARYOPERATION_H__8F69F7C5_3C44_11D2_AF8D_000000000000__INCLUDED_)
 
