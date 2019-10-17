// Boltzscaler.h: interface for the Boltzscaler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOLTZSCALER_H__8F69F7C0_3C44_11D2_AF8D_000000000000__INCLUDED_)
#define AFX_BOLTZSCALER_H__8F69F7C0_3C44_11D2_AF8D_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "Stats.h"

class Boltzscaler  : public AbstractOperation
{
	Stats stats;
	double startTmp;
	double endTmp;
	double deltaTmp;
	double t;
public:
	Boltzscaler(double start, double end, double delta);
	double newTmp(double t);
	Population & apply(Population &p, int generation = 0);
	Boltzscaler();
	virtual ~Boltzscaler();

};

#endif // !defined(AFX_BOLTZSCALER_H__8F69F7C0_3C44_11D2_AF8D_000000000000__INCLUDED_)
 
