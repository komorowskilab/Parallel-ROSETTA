// BinaryOperation.h: interface for the BinaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINARYOPERATION_H__8F69F7C3_3C44_11D2_AF8D_000000000000__INCLUDED_)
#define AFX_BINARYOPERATION_H__8F69F7C3_3C44_11D2_AF8D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "AbstractBinaryOperator.h"

class BinaryOperation : public AbstractOperation
{
	double frac;
	AbstractBinaryOperator &operation;


public:
	BinaryOperation(AbstractBinaryOperator & op, double percent);
	Population & apply(Population &p, int generation = 0);

	virtual ~BinaryOperation();

};

#endif // !defined(AFX_BINARYOPERATION_H__8F69F7C3_3C44_11D2_AF8D_000000000000__INCLUDED_)
 
