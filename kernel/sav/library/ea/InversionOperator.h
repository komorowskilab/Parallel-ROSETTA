// InversionOperator.h: interface for the InversionOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVERSIONOPERATOR_H__82C5F8C6_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_INVERSIONOPERATOR_H__82C5F8C6_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractUnaryOperator.h"

class InversionOperator : public AbstractUnaryOperator
{
	int times;
public:
	bool apply(Individual &in, Individual &out);
	InversionOperator(int rounds = 1);
	virtual ~InversionOperator();

};

#endif // !defined(AFX_INVERSIONOPERATOR_H__82C5F8C6_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
