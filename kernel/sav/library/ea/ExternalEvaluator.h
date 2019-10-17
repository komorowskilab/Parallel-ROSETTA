// ExternalEvaluator.h: interface for the ExternalEvaluator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTERNALEVALUATOR_H__7A853F73_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_)
#define AFX_EXTERNALEVALUATOR_H__7A853F73_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <kernel/system/iostream.h>
#include <kernel/basic/string.h>
#include "AbstractEvaluator.h"
#include "Stats.h"

class ExternalEvaluator  : public AbstractEvaluator
{
	const char * cline;
	const char * fbase;
	Stats &stats;
public:
	void eval(Population &);
	virtual void hook(){};
	ExternalEvaluator(char * commandline, char * iofile, Stats &s);
	virtual ~ExternalEvaluator();

};

#endif // !defined(AFX_EXTERNALEVALUATOR_H__7A853F73_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_)
 
