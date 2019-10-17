// loneOperator.h: interface for the CloneOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LONEOPERATOR_H__82C5F8C7_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_LONEOPERATOR_H__82C5F8C7_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractUnaryOperator.h"

class CloneOperator : public AbstractUnaryOperator
{
public:
	bool apply(Individual &in, Individual &out);
	CloneOperator();
	virtual ~CloneOperator();

};

#endif // !defined(AFX_LONEOPERATOR_H__82C5F8C7_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
