// AbstractBinaryOperator.h: interface for the AbstractBinaryOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABSTRACTBINARYOPERATOR_H__8F69F7C7_3C44_11D2_AF8D_000000000000__INCLUDED_)
#define AFX_ABSTRACTBINARYOPERATOR_H__8F69F7C7_3C44_11D2_AF8D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Individual.h"

class AbstractBinaryOperator  
{
public:
	virtual bool apply(Individual &m, Individual &f, Individual &b, Individual &s) = 0;
	AbstractBinaryOperator();
	virtual ~AbstractBinaryOperator();

};

#endif // !defined(AFX_ABSTRACTBINARYOPERATOR_H__8F69F7C7_3C44_11D2_AF8D_000000000000__INCLUDED_)
 
