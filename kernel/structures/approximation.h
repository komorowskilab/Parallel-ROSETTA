//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __APPROXIMATION_H__
#define __APPROXIMATION_H__

#include <copyright.h>

#include <kernel/structures/annotatedstructure.h>
#include <kernel/structures/equivalenceclasses.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Approximation
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents the rough approximation of a set of
//                 objects.
// Revisions.....:
//===================================================================

class Approximation : public AnnotatedStructure {
private:

  //- Set approximations.............................................
	Handle<EquivalenceClasses> universe_;  // Universe, i.e. all objects.
	Handle<EquivalenceClasses> lower_;     // Lower approximation, i.e. those definitely inside.
	Handle<EquivalenceClasses> upper_;     // Upper approximation, i.e. those definitely or possibly inside
	Handle<EquivalenceClasses> boundary_;  // Upper approximation minus lower approximation, i.e. those possibly inside.
	Handle<EquivalenceClasses> outside_;   // Universe minus upper approximation, i.e. those definitely outside.

	int                        decision_;  // The decision class we've approximated.

protected:

	//- Constructors...................................................
  Approximation(const Approximation &in);

public:

  //- Constructor and destructor....................................
	Approximation();
  virtual ~Approximation();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool              Load(ifstream &stream);
  virtual bool              Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure        *Duplicate() const;
  virtual void              Clear();

  //- Operators.....................................................
	Approximation            &operator=(const Approximation &in);
	bool                      operator==(const Approximation &in) const;
	bool                      operator!=(const Approximation &in) const;

  //- Accessors.....................................................
	int                       GetDecisionValue() const {return decision_;}
  bool                      SetDecisionValue(int decision) {decision_ = decision; return true;}

	const EquivalenceClasses *GetUniverse() const {return universe_.GetPointer();}
	bool                      SetUniverse(const EquivalenceClasses *universe) {universe_ = universe; return true;}

	const EquivalenceClasses *GetLowerApproximation() const {return lower_.GetPointer();}
	bool                      SetLowerApproximation(const EquivalenceClasses *lower) {lower_ = lower; return true;}

	const EquivalenceClasses *GetUpperApproximation() const {return upper_.GetPointer();}
	bool                      SetUpperApproximation(const EquivalenceClasses *upper) {upper_ = upper; return true;}

	const EquivalenceClasses *GetBoundaryRegion() const {return boundary_.GetPointer();}
	bool                      SetBoundaryRegion(const EquivalenceClasses *boundary) {boundary_ = boundary; return true;}

	const EquivalenceClasses *GetOutsideRegion() const {return outside_.GetPointer();}
	bool                      SetOutsideRegion(const EquivalenceClasses *outside) {outside_ = outside; return true;}

	//- Querying methods..............................................
	bool                      IsRough() const;
	bool                      IsCrisp() const;

	//- Numerical methods.............................................
	float                     GetSensitivity(int *no_correct = NULL, int *no_total = NULL) const;
	float                     GetSpecificity(int *no_correct = NULL, int *no_total = NULL) const;
	float                     GetAccuracy(int *no_correct = NULL, int *no_total = NULL) const;

};

#endif 
