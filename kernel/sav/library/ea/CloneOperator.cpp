// loneOperator.cpp: implementation of the CloneOperator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "CloneOperator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CloneOperator::CloneOperator()
{

}

CloneOperator::~CloneOperator()
{

}

bool CloneOperator::apply(Individual & in, Individual & out)
{
	*&out = *&in;
	return true;
}
 
