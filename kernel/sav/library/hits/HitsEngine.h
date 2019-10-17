// HitsEngine.h: interface for the HitsEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HITSENGINE_H__F2ED4F84_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_HITSENGINE_H__F2ED4F84_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/Engine.h"
#include "Context.h"
#include "../ea/Keeper.h"
#include "../ea/Evaluator.h"


class HitsEngine : public Engine  
{
  Keeper &keeper;
  Context &c;
  int lastfc;
 public:
  void hook(Population &p, int generation);
  HitsEngine(AbstractInitializer &i, Evaluator &e,
             Vector(AbstractOperation*) &o, 
             AbstractStoppingCriteria &s,
             Keeper &keep,
             Context &c);
  virtual ~HitsEngine();
  
};

#endif // !defined(AFX_HITSENGINE_H__F2ED4F84_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
