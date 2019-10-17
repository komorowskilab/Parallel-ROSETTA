//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __EQUIVALENCECLASSES_H__
#define __EQUIVALENCECLASSES_H__

#include <copyright.h>

#include <kernel/structures/structures.h>
#include <kernel/structures/equivalenceclass.h>

//-------------------------------------------------------------------
// Class.........: EquivalenceClasses
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A set of equivalence classes.
// Revisions.....:
//===================================================================

class EquivalenceClasses : public Structures {
private:

	//- STL comparator.................................................
	struct Compare {
		bool operator()(const Handle<Structure> &a, const Handle<Structure> &b) const;
	};

protected:

  //- Constructors...................................................
  EquivalenceClasses(const EquivalenceClasses &in);

public:

  //- Constructors/destructor........................................
  EquivalenceClasses();
  virtual ~EquivalenceClasses();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual Structure        *Duplicate() const;

	//- Methods inherited from Structures..............................
	virtual bool              InsertStructure(Structure *structure, int i);

	//- Local methods..................................................
	int                       GetNoEquivalenceClasses() const;
	EquivalenceClass         *GetEquivalenceClass(int i) const;

	bool                      Sort(bool ascending);

	//- Query methods..................................................
	bool                      IsMember(int object_no) const;

	//- Total cardinality methods......................................
	int                       GetNoObjects() const;
	float                     GetRatio(const EquivalenceClasses &in) const;

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoEquivalenceClasses/GetEquivalenceClass
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Convenience methods.
// Comments......:
// Revisions.....:
//===================================================================

inline int
EquivalenceClasses::GetNoEquivalenceClasses() const {
	return GetNoStructures();
}

inline EquivalenceClass *
EquivalenceClasses::GetEquivalenceClass(int i) const {
	return dynamic_cast(EquivalenceClass *, GetStructure(i));
}

//-------------------------------------------------------------------
// Method........: IsMember
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Given an object index, returns true if the object
//                 is a member of any of the eq. classes.
// Comments......:
// Revisions.....:
//===================================================================

inline bool
EquivalenceClasses::IsMember(int object_no) const {

	int i, no_classes = GetNoEquivalenceClasses();

	for (i = 0; i < no_classes; i++) {
		if (GetEquivalenceClass(i)->IsMember(object_no))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Method........: GetNoObjects
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the accumulated cardinality of all the
//                 eq. classes.
// Comments......:
// Revisions.....:
//===================================================================

inline int
EquivalenceClasses::GetNoObjects() const {

	int i, no_classes = GetNoEquivalenceClasses(), cardinality = 0;

	for (i = 0; i < no_classes; i++)
		cardinality += GetEquivalenceClass(i)->GetNoObjects();

	return cardinality;

}

//-------------------------------------------------------------------
// Method........: GetRatio
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the ratio between the total cardinalities
//                 of this set of eq. classes and the input set.
//
//                 Example:
//
//                   M = m.GetNoObjects();
//                   N = n.GetNoObjects();
//
//                   m.GetRatio(n) returns M / N.
//
// Comments......:
// Revisions.....:
//===================================================================

inline float
EquivalenceClasses::GetRatio(const EquivalenceClasses &in) const {

	if (&in == this)
		return 1.0;

	int denominator = in.GetNoObjects();

	// Avoid division by zero.
	if (denominator == 0)
		return Undefined::Float();

	int counter = GetNoObjects();

	// Safe-guard to avoid numerical quirks.
	if (counter == denominator)
		return 1.0;

	return static_cast(float, counter) / denominator;

}

#endif 
