// rossoverOperator.h: interface for the CrossoverOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROSSOVEROPERATOR_H__82C5F8C4_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_ROSSOVEROPERATOR_H__82C5F8C4_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractBinaryOperator.h"

class CrossoverOperator  : public AbstractBinaryOperator
{
	int times;
public:
	bool apply(Individual &m, Individual &f, Individual &s, Individual &b);
	CrossoverOperator(int points = 1);
	virtual ~CrossoverOperator();

};

#endif // !defined(AFX_ROSSOVEROPERATOR_H__82C5F8C4_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
