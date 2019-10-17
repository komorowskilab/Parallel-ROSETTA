// AbstractEvaluator.h: interface for the AbstractEvaluator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABSTRACTEVALUATOR_H__7A853F74_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_)
#define AFX_ABSTRACTEVALUATOR_H__7A853F74_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Population.h"

class AbstractEvaluator  
{
public:
	virtual void hook() = 0;
	virtual void eval(Population &) = 0;
	AbstractEvaluator();
	virtual ~AbstractEvaluator();

};

#endif // !defined(AFX_ABSTRACTEVALUATOR_H__7A853F74_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_)
 
