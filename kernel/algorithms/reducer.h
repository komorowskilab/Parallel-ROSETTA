//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCER_H__
#define __REDUCER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/algorithms/objectselector.h>

#include <kernel/structures/discernibilityfunction.h>
#include <kernel/structures/booleansopfunction.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/generalizeddecision.h>

//-------------------------------------------------------------------
// Class.........: Reducer
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

class Reducer : public Algorithm {
protected:

	//- Algorithm parameters...........................................
	Reduct::Discernibility   discernibility_;      // Type of reducts to calculate (full or object related).
	bool                     modulo_decision_;     // Compute reducts modulo decision attribute?
	ObjectSelector           selector_;            // Handles object selection.
	bool                     idg_;                 // Use indiscernibility definition graph (IDG)?
	String                   filename_idg_;        // IDG location, if relevant.
	bool                     precompute_;          // Precompute all disc. matrix entries?
	bool                     brt_;                 // Use boundary region thinning (BRT)?
	float                    precision_brt_;       // BRT precision, if relevant;

protected:

	//- Helper methods.................................................
	virtual bool             CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const;
	virtual bool             CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const;

	virtual bool             AppendReduct(Reducts &reducts) const;
	virtual bool             AppendReducts(const BooleanSOPFunction &primeimplicants, const Vector(int) &supports, Reducts &reducts) const;
	virtual bool             AppendReducts(const BooleanSOPFunction &primeimplicants, const DecisionTable &table, int object_no, bool masked, const Vector(int) &supports, Reducts &reducts, bool lock=false) const;

public:

  //- Constructors/destructor........................................
  Reducer();
  virtual ~Reducer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String           GetParameters() const;
	virtual bool             SetParameter(const String &keyword, const String &value);
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure       *Apply(Structure &structure) const = 0;
	virtual Reducer			*Clone() = 0;

	//- New virtual methods............................................
	virtual bool             ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const;

	//- Parameter methods..............................................
	Reduct::Discernibility   GetDiscernibility() const {return discernibility_;}
	bool                     SetDiscernibility(Reduct::Discernibility discernibility) {discernibility_ = discernibility; return true;}

	bool                     ModuloDecision() const {return modulo_decision_;}
	bool                     ModuloDecision(bool modulo_decision) {modulo_decision_ = modulo_decision; return true;}

	ObjectSelector          &GetObjectSelector() {return selector_;}
	const ObjectSelector    &GetObjectSelector() const {return selector_;}
	bool                     SetObjectSelector(ObjectSelector &selector) {selector_ = selector; return true;}

	virtual bool             UseIDG() const;
	virtual bool             UseIDG(bool idg);

	virtual const String    &GetIDGFilename() const;
	virtual bool             SetIDGFilename(const String &filename);

	virtual bool             PrecomputeMatrix() const;
	virtual bool             PrecomputeMatrix(bool precompute);

	virtual bool             UseBRT() const;
	virtual bool             UseBRT(bool brt);

	virtual float            GetBRTPrecision() const;
	virtual bool             SetBRTPrecision(float precision);

	//- Parameter methods, for internal use only.......................
	virtual bool             IsDynamicComponent() const;
	virtual bool             IsDynamicComponent(bool is_dynamic);

};

#endif 
