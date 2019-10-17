//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __EQUIVALENCECLASS_H__
#define __EQUIVALENCECLASS_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/generalizeddecision.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/basic/message.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: EquivalenceClass
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents a single equivalence class.  Viewed as
//                 a set of object indices into some ancestral decision
//                 table.
//
// Comments......: To do: Introduce a common base class for this class
//                        and the Reduct class, since they are both
//                        index sets.
// Revisions.....:
//===================================================================

class EquivalenceClass : public Structure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<EquivalenceClass>) Handles;

private:

  //- Object indices.................................................
	Vector(int) objects_;

protected:

	//- Constructors...................................................
  EquivalenceClass(const EquivalenceClass &in);

public:

  //- Constructor and destructor....................................
	EquivalenceClass();
	EquivalenceClass(int size);
  virtual ~EquivalenceClass();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool          Load(ifstream &stream);
  virtual bool          Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure    *Duplicate() const;
  virtual void          Clear();

  //- Operators.....................................................
	int                   operator[](int i) const;
	EquivalenceClass     &operator=(const EquivalenceClass &in);
	bool                  operator==(const EquivalenceClass &in) const;
	bool                  operator!=(const EquivalenceClass &in) const;

  //- Equivalence class management..................................
	virtual int           GetNoObjects() const;
	virtual int           GetObject(int position_no) const;

	virtual bool          InsertObject(int object_no);
	virtual bool          RemoveObject(int position_no);

	virtual int           FindObject(int object_no) const;
	virtual bool          IsMember(int object_no) const;

	virtual bool          Sort(bool ascending);

	virtual bool          Reserve(int size);

	//- Numerical methods.............................................
	virtual bool          GetGeneralizedDecision(GeneralizedDecision &decisions, const DecisionTable &table) const;
	virtual bool          GetGeneralizedDecision(GeneralizedDecision &decisions, const DecisionTable &table, int attribute_no, bool masked) const;

	virtual int           GetNoObjects(int decision, const DecisionTable &table) const;
	virtual int           GetNoObjects(int decision, const DecisionTable &table, int attribute_no, bool masked) const;

	virtual float         GetMembership(int decision, const DecisionTable &table) const;
	virtual float         GetMembership(int decision, const DecisionTable &table, int attribute_no, bool masked) const;

	//- Querying methods..............................................
	virtual bool          Contains(const EquivalenceClass &in, bool proper = false) const;

	virtual bool          IsConsistent(const DecisionTable &table) const;
	virtual bool          IsConsistent(const DecisionTable &table, int attribute_no, bool masked) const;

	//- Formatting....................................................
	virtual bool          Format(String &formatted, int offset = 0) const;

};

//-------------------------------------------------------------------
// Inlined operators/methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Operator []
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline int
EquivalenceClass::operator[](int i) const {
	return GetObject(i);
}

//-------------------------------------------------------------------
// Method........: Operator ==
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: Assumes that the eq. classes to compare are
//                 similarly sorted.
// Revisions.....:
//===================================================================

inline bool
EquivalenceClass::operator==(const EquivalenceClass &in) const {

	int i, no_objects = GetNoObjects();

	// Are the cardinalities equal?
  if (no_objects != in.GetNoObjects())
		return false;

	// Are all elements equal?
	for (i = 0; i < no_objects; i++) {
		if (GetObject(i) != in.GetObject(i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Operator !=
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline bool
EquivalenceClass::operator!=(const EquivalenceClass &in) const {
	return !(*this == in);
}

#endif 
