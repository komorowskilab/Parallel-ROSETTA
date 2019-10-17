// ExternalFitness.h: interface for the ExternalFitness class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTERNALFITNESS_H__7A853F71_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_)
#define AFX_EXTERNALFITNESS_H__7A853F71_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AbstractFitness.h"

class ExternalFitness : public AbstractFitness
{
public:
	ExternalFitness();
	virtual ~ExternalFitness();

};

#endif // !defined(AFX_EXTERNALFITNESS_H__7A853F71_BD1A_11D2_92A0_00A0C925E2B3__INCLUDED_)
 
