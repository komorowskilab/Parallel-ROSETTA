// AbstractInitializer.h: interface for the AbstractInitializer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABSTRACTINITIALIZER_H__909F6271_3B76_11D2_AF8A_000000000000__INCLUDED_)
#define AFX_ABSTRACTINITIALIZER_H__909F6271_3B76_11D2_AF8A_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Population.h"

class AbstractInitializer  
{
public:
	virtual Population & init() = 0;
	AbstractInitializer();
	virtual ~AbstractInitializer();

};

#endif // !defined(AFX_ABSTRACTINITIALIZER_H__909F6271_3B76_11D2_AF8A_000000000000__INCLUDED_)
 
