// HitsInit.h: interface for the HitsInit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HITSINIT_H__F2ED4F83_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_HITSINIT_H__F2ED4F83_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/AbstractInitializer.h"
#include "Context.h"

class HitsInit : public AbstractInitializer  
{
	Context &c;
	Population p;
public:
	Population & init();
	HitsInit(Context &context);
	virtual ~HitsInit();

};

#endif // !defined(AFX_HITSINIT_H__F2ED4F83_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
