// MutateOperator.h: interface for the MutateOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTATEOPERATOR_H__82C5F8C5_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_MUTATEOPERATOR_H__82C5F8C5_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractUnaryOperator.h"

class MutateOperator : public AbstractUnaryOperator
{
	int times;
public:
	bool apply(Individual &in, Individual &out);
	MutateOperator(int rounds = 1);
	virtual ~MutateOperator();

};

#endif // !defined(AFX_MUTATEOPERATOR_H__82C5F8C5_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
