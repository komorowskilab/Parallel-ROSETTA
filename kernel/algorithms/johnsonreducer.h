//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __JOHNSONREDUCER_H__
#define __JOHNSONREDUCER_H__

#include <copyright.h>

#include <kernel/algorithms/reducer.h>

//-------------------------------------------------------------------
// Class.........: JohnsonReducer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simple greedy heuristic for computing a single
//                 prime implicant (reduct) only. Based on the
//                 set covering heuristic of Johnson:
//
//                 @ARTICLE{johnson:approximation74,
//                    author = "David S. Johnson",
//                    title = "Approximation Algorithms for Combinatorial Problems",
//                    journal = "Journal of Computer and System Sciences",
//                    year = 1974,
//                    volume = 9,
//                    pages = "256--278",
//                 }
//
// Revisions.....: AÛ 990726: Moved weight stuff to BooleanFunction.
//===================================================================

class JohnsonReducer : public Reducer {
protected:

	//- Algorithm parameters...........................................
	bool               approximate_;       // Compute approximate solutions?
	float              hitting_fraction_;  // Minimal hitting fraction, for approximate solutions.

protected:

	//- Methods inherited from Reducer.................................
	virtual bool       CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const;
	virtual bool       CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const;

public:

  //- Constructors/destructor........................................
  JohnsonReducer();
  virtual ~JohnsonReducer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual JohnsonReducer *Clone();

	//- Methods inherited from Reducer.................................
	virtual bool       ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const;

	//- Parameter methods..............................................
	bool               ApproximateSolutions() const {return approximate_;}
	bool               ApproximateSolutions(bool approximate) {approximate_ = approximate; return true;}

	float              GetHittingFraction() const {return hitting_fraction_;}
	bool               SetHittingFraction(float fraction) {hitting_fraction_ = fraction; return true;}

};

#endif 
