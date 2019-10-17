// BitVRandomInitializer.h: interface for the BitVRandomInitializer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITVRANDOMINITIALIZER_H__1B52DB00_BC66_11D2_92A0_00A0C925E2B3__INCLUDED_)
#define AFX_BITVRANDOMINITIALIZER_H__1B52DB00_BC66_11D2_92A0_00A0C925E2B3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractInitializer.h"
#include "Population.h"	// Added by ClassView


class BitVRandomInitializer : public AbstractInitializer
{
	Population pop;
	int number;
	int bits;
	double p;
	bool oneShot;
public:
	virtual Population& init();
	BitVRandomInitializer(int size, int nbits, double prob = 0.5, bool vary = false);
	virtual ~BitVRandomInitializer();

};

#endif // !defined(AFX_BITVRANDOMINITIALIZER_H__1B52DB00_BC66_11D2_92A0_00A0C925E2B3__INCLUDED_)
 
