// Reaper.h: interface for the Reaper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REAPER_H__82C5F8C8_3CF1_11D2_AF8F_000000000000__INCLUDED_)
#define AFX_REAPER_H__82C5F8C8_3CF1_11D2_AF8F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "Stats.h"

class Reaper : public AbstractOperation
{
	int nu;
	int mu;
	int maxLT;
	int minLT;
	Stats stats;
public:
	int setAge(double fitness, double avgFit, double minFit, double maxFit);
	Reaper(int min = 1, int max = 6);
	Population & apply(Population &p, int generation = 0);
	virtual ~Reaper();

};

#endif // !defined(AFX_REAPER_H__82C5F8C8_3CF1_11D2_AF8F_000000000000__INCLUDED_)
 
