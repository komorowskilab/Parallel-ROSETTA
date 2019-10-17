//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ORTHOGONALSCALER_H__
#define __ORTHOGONALSCALER_H__

#include <copyright.h>

#include <kernel/algorithms/scaler.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Attribute;
class FloatAttribute;
class IntegerAttribute;
class StringAttribute;

//-------------------------------------------------------------------
// Class.........: OrthogonalScaler
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Algorithm class that discretizes decision tables by
//                 means of intervals, i.e. with hyperplanes that
//                 are orthogonal to the axes.
// Revisions.....:
//===================================================================

class OrthogonalScaler : public Scaler {
public:

	//- Some convenient type definitions...............................
	typedef Pair(int, int) OVPair;
	typedef Vector(float)  Cuts;

protected:

	//- STL stuff for internal use.....................................
	struct OVPairCompareSecond {
		bool operator () (const OVPair &x, const OVPair &y) const {
			return x.second < y.second;
		}
	};

protected:

  //- Dictionary creation methods....................................
	virtual bool             CreateDictionary(const DecisionTable &before, DecisionTable &after, const String &filename) const;

	virtual Attribute       *CreateDiscretizedAttribute(const Attribute &before, const Cuts &cuts) const;
	virtual Attribute       *CreateSubsetAttribute(const Attribute &before, const Cuts &cuts) const;
	virtual Attribute       *CreateIntervalAttribute(const Attribute &before, const Cuts &cuts) const;

	//- Discretization methods.........................................
	virtual DecisionTable   *Discretize(DecisionTable &table) const;
	virtual bool             UpdateTable(DecisionTable &table, int attribute_no, bool masked, const Cuts &cuts) const;

	virtual bool             IsConsistent(const Vector(OVPair) &objects, int i, int begin, int end, const DecisionTable &table, int decision_attribute, bool masked, Vector(int) &decisions, Vector(int) &cardinalities) const;

public:

  //- Constructors/destructor........................................
  OrthogonalScaler();
  virtual ~OrthogonalScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual Structure       *Apply(Structure &structure) const;
	virtual OrthogonalScaler	*Clone();

	//- New virtual methods............................................
  virtual bool             FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const;

};

#endif 
