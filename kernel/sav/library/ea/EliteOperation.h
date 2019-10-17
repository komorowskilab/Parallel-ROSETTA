// EliteOperation.h: interface for the EliteOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELITEOPERATION_H__8A931B52_3C08_11D2_AF8B_000000000000__INCLUDED_)
#define AFX_ELITEOPERATION_H__8A931B52_3C08_11D2_AF8B_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractOperation.h"
#include "Stats.h"

class EliteOperation : public AbstractOperation
{
	Individual * elite;
	Stats stats;
public:
	virtual double getFitness(Individual * i, bool scaled = true);
	Population & apply(Population &p, int generation = 0);
	EliteOperation();
	virtual ~EliteOperation();
};

#endif // !defined(AFX_ELITEOPERATION_H__8A931B52_3C08_11D2_AF8B_000000000000__INCLUDED_)
 
