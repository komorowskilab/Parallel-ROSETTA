// Hits.h: interface for the Hits class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HITS_H__F2ED4F86_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_HITS_H__F2ED4F86_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/CrossoverOperator.h"
#include "../ea/InversionOperator.h"
#include "../ea/MutateOperator.h"
#include "../ea/CloneOperator.h"
#include "../ea/BinaryOperation.h"
#include "../ea/UnaryOperation.h"
#include "../ea/EliteOperation.h"
#include "../ea/ParentsSelector.h"
#include "../ea/Reaper.h"
#include "../ea/Evaluator.h"
#include "../ea/Stats.h"
#include "../ea/RecombineReplaceSample.h"
#include "../ea/RecombineReplaceParents.h"
#include "../ea/RecombineAdd.h"
#include "../ea/Boltzscaler.h"



#include <kernel/basic/bits.h>
#include "Context.h"
#include "HitsEngine.h"
#include "HitsFitness.h"
#include "HistStop.h"
#include "HitsInit.h"

struct HittingSetInfo
{
	float cost;      // cost of set
	float cover;     // cover calculated by algorithm
	float acc_cover; // accumulated cover by absorbed sets

	HittingSetInfo(float c = 0.0f, float a = 0.0f, float o = 0.0f) : 
		cost(c), cover(a), acc_cover(o) {}
};

class Hits  
{
public:
	Parameters &params;
	const Vector(const Bits*) &sets;

	Context *context;
	HitsEngine *engine;
	HitsFitness *fitness;
	Keeper *keeper;
	HistStop *stop;
	AbstractInitializer *init;
	// AbstractEvaluator * eval;
	Evaluator * eval;
	
	CrossoverOperator * crsop;
	InversionOperator * invop;
	MutateOperator * mutop;
	CloneOperator * clnop;

	BinaryOperation * binop;
	UnaryOperation * unop1;
	UnaryOperation * unop2;
	UnaryOperation * unop3;
	EliteOperation * elite;

	AbstractOperation * recombinator;
	ParentsSelector * parents_selector;
	AbstractOperation * scaler;

	Reaper * reaper;
	AbstractSampler * sampler;
	Stats * stats;

	Vector(AbstractOperation *) ops;
        int stat_initialized; // 0 not (error) , 1 not init but ok (empty fun),2 ok

public:
	bool hard(Vector(Vector(Bits)) & solutions, Vector(Vector(HittingSetInfo)) &info, bool simp = true);
	bool hard(Vector(Bits) &solution, Vector(HittingSetInfo) &info, bool simp = true);
	Hits(const Vector(const Bits *) &bits, Parameters &parameters);
	virtual ~Hits();

};

#endif // !defined(AFX_HITS_H__F2ED4F86_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
