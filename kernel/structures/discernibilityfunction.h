//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DISCERNIBILITYFUNCTION_H__
#define __DISCERNIBILITYFUNCTION_H__

#include <copyright.h>

#include <kernel/structures/booleanposfunction.h>
#include <kernel/structures/generalizeddecision.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Discerner;
class DiscernibilityMatrix;

//-------------------------------------------------------------------
// Class.........: DiscernibilityFunction
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....: AÛ 990629: Added BRT stuff.
//===================================================================

class DiscernibilityFunction : public BooleanPOSFunction {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<DiscernibilityFunction>) Handles;

protected:

	//- Constructors...................................................
  DiscernibilityFunction(const DiscernibilityFunction &in);

  //- Creation helpers...............................................
	virtual bool   Create(Vector(const Bits *) &entries, bool copy, bool simplify, const Vector(float) *weights);
	virtual bool   CreateSimplified(bool full, const DecisionTable &table, int object_no, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, const DiscernibilityMatrix *matrix, bool weighted);
	virtual bool   AppendSimplified(const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, bool weighted, int i, int j, int k, int &counter, int denominator, int total);

public:

  //- Constructors and destructor....................................
	DiscernibilityFunction();
  virtual ~DiscernibilityFunction();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Creation methods...............................................
	virtual bool   Create(const DiscernibilityMatrix &matrix, bool copy, bool reduce, bool simplify, bool weighted);
	virtual bool   Create(const DiscernibilityMatrix &matrix, int object_no, bool copy, bool reduce, bool simplify, bool weighted);

	virtual bool   CreateSimplified(const DecisionTable &table, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, const DiscernibilityMatrix *matrix, bool weighted);
	virtual bool   CreateSimplified(const DecisionTable &table, int object_no, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, const DiscernibilityMatrix *matrix, bool weighted);

};

#endif 
