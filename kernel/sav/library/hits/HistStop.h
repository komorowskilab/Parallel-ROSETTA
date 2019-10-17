// HistStop.h: interface for the HistStop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTSTOP_H__F2ED4F82_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_HISTSTOP_H__F2ED4F82_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/AbstractStoppingCriteria.h"
#include "Context.h"

class HistStop : public AbstractStoppingCriteria  
{
	Context &c;
	bool fit_stop;
	bool keep_stop;
	bool use_fs;
	bool use_ks;
	int gen_gap;
	double max_fitness;
  int f_generation; 
	int k_generation;

public:
	bool stop(Population &p, int generation = 0);
	HistStop(Context &cont, int gap = 5, bool fStop = true, bool kStop = true);
	virtual ~HistStop();

};

#endif // !defined(AFX_HISTSTOP_H__F2ED4F82_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
