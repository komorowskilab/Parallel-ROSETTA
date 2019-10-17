//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __APPROXIMATOR_H__
#define __APPROXIMATOR_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: Approximator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Calculates the rough approximation of a set.  A
//                 threshold can be set so as to control the
//                 precision of the approximation.
// Revisions.....:
//===================================================================

class Approximator : public Algorithm {
protected:

	//- Parameters.....................................................
	int                decision_;   // Approximate the set of all object with this decision value.
	String             attributes_; // Attribute subset to approximate with (e.g. "{bird, penguin}" or "{0, 3}")
	float              precision_;  // Membership value threshold, controls appr. precision.

public:

  //- Constructors/destructor........................................
  Approximator();
  virtual ~Approximator();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure *Apply(Structure &structure) const;
	virtual Approximator	*Clone();

	//- Parameter management...........................................
	int                GetDecisionValue() const {return decision_;}
	bool               SetDecisionValue(int decision) {decision_ = decision; return true;}

	const String      &GetAttributes() const {return attributes_;}
	bool               SetAttributes(const String &attributes) {attributes_ = attributes; return true;}

	float              GetPrecision() const {return precision_;}
	bool               SetPrecision(float precision) {precision_ = MathKit::Truncate(0.0f, precision, 0.5f); return true;}

};

#endif 
