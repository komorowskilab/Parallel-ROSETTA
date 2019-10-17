//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BRORTHOGONALSCALER_H__
#define __BRORTHOGONALSCALER_H__

#include <copyright.h>

#include <kernel/algorithms/orthogonalscaler.h>

#include <kernel/basic/pair.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class BooleanSOPFunction;
class DiscernibilityFunction;

//-------------------------------------------------------------------
// Class.........: BROrthogonalScaler
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: An algorithm for discretizing a decision table
//                 using Boolean reasoning techniques.
// Revisions.....:
//===================================================================

class BROrthogonalScaler : public OrthogonalScaler {
protected:

	//- Type definitions...............................................
	typedef Pair(int, float) ACPair; // An (attribute, cut) pair.

	struct Region {
		int         object_;           // Index of the object that represents the region.
		int         cardinality_;      // Number of objects in the region.
		Vector(int) decisions_;        // Generalized decision of the region.
	};

protected:

  //- STL helper, lexicographical comparison of integer vectors......
	struct Compare {
	private:
		const Vector(Vector(int)) *v_;
    Compare() {v_ = NULL;}
	public:
		bool operator()(int i, int j) const {
			int k, size = (*v_)[i].size();
			for (k = 0; k < size; k++) {
				if ((*v_)[i][k] != (*v_)[j][k])
					return ((*v_)[i][k] < (*v_)[j][k]);
			}
			return false;
		}
    Compare(const Vector(Vector(int)) *v) {v_ = v;}
	};

protected:

	//- Algorithm parameters...........................................
	bool                   approximate_;       // Compute approximate solutions with Johnson algorithm?
	float                  hitting_fraction_;  // Minimal hitting fraction, for approximate solutions.

protected:

	//- Methods inherited from OrthogonalScaler........................
	virtual DecisionTable *Discretize(DecisionTable &table) const;

	//- Helper methods.................................................
	virtual bool           FindCuts(const DecisionTable &table, Vector(ACPair) &candidates, Vector(Region) &regions) const;
	virtual bool           FindRegions(const DecisionTable &table, const Vector(Vector(int)) &cartesian, Vector(Region) &regions) const;
	virtual bool           BuildFunction(const DecisionTable &table, const Vector(ACPair) &candidates, const Vector(Region) &regions, DiscernibilityFunction &function) const;
	virtual bool           ComputePrimeImplicant(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicant) const;
	virtual bool           FilterCuts(const DecisionTable &table, const Vector(ACPair) &candidates, const BooleanSOPFunction &primeimplicant);

protected:

	//- Parameters.....................................................
	Vector(Cuts)           cuts_;

public:

  //- Constructors/destructor........................................
  BROrthogonalScaler();
  virtual ~BROrthogonalScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String         GetParameters() const;
	virtual bool           SetParameter(const String &keyword, const String &value);
	virtual BROrthogonalScaler	*Clone();

	//- Methods inherited from OrthogonalScaler........................
  virtual bool           FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

	//- Parameter methods..............................................
	bool                   ApproximateSolutions() const {return approximate_;}
	bool                   ApproximateSolutions(bool approximate) {approximate_ = approximate; return true;}

	float                  GetHittingFraction() const {return hitting_fraction_;}
	bool                   SetHittingFraction(float fraction) {hitting_fraction_ = fraction; return true;}

};

#endif 
