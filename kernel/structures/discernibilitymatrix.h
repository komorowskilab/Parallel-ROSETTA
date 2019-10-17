//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DISCERNIBILITYMATRIX_H__
#define __DISCERNIBILITYMATRIX_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/generalizeddecision.h>

#include <kernel/basic/bits.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Discerner;

//-------------------------------------------------------------------
// Class.........: DiscernibilityMatrix
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents a discernibility matrix, optimized
//                 somewhat wrt space.
//
//                 Each matrix entry is a bitset. Slightly less than
//                 half the matrix is actually stored, since the matrix
//                 is symmetric with empty diagonal entries.
//
//                 NOTE: The assumption above implies that the
//                       Discerner object must define a reflexive and
//                       symmetric relation. It is the caller's
//                       responsibility to check this!
//
//                 Moreover, only one bitset per equivalence class
//                 is stored, with the mapping from object index to
//                 equivalence class index taking O(1) time.
//
//                 Empty sets are returned as NULL.
//
// Comments......: The friend declaration is an ugly hack. Resolve
//                 later,
//
// Revisions.....: AÛ 990223: Modularized creation code.
//                 AÛ 990301: Introduced lazy matrix allocation.
//                 AÛ 990629: Added BRT stuff.
//===================================================================

class DiscernibilityMatrix : public Structure {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<DiscernibilityMatrix>) Handles;

	//- Friends.......................................................
	friend class DiscernibilityFunction;

protected:

  //- Implementation structures......................................
	Vector(Bits *)              matrix_;               // Matrix representation (triangular, reduced/mapped).
	Vector(int)                 map_;                  // Maps from object index to eq. class index. Size as non-reduced/non-mapped dimension.
	Vector(int)                 cardinalities_;        // Cardinalities of each eq. class. Size as reduced/mapped dimension.
	Vector(int)                 representatives_;      // Indices (non-mapped) of one object from each eq. class. Size as reduced/mapped dimension.

protected:

	//- Constructors...................................................
  DiscernibilityMatrix(const DiscernibilityMatrix &in);

	//- Local methods, inlined.........................................
	bool                        HasAllocatedMatrix() const;
	int                         GetMap(int i) const;
	int                         GetIndex(int i, int j) const;

	//- Helper methods.................................................
	virtual bool                AllocateMatrix();
	virtual bool                CreateEntries(const DecisionTable &table, bool masked, int i, int j, int k, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision);
	virtual bool                SkipEntry(const GeneralizedDecision::Handles &decisions, int i, int j, float precision) const;
	virtual bool                ComputeEntry(const DecisionTable &table, bool masked, bool modulo, int i, int j, const Discerner &discerner, Bits &entry, int &count) const;
	virtual bool                GetRepresentatives(const Vector(int) &objects, Vector(int) &representatives) const;

public:

  //- Constructors and destructor....................................
	DiscernibilityMatrix();
  virtual ~DiscernibilityMatrix();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool                Load(ifstream &stream);
  virtual bool                Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure          *Duplicate() const;
  virtual void                Clear();

	//- Entry methods.................................................
	virtual int                 GetDimension(bool reduced = false) const;

	virtual const Bits         *GetEntry(int i, int j, bool reduced = false) const;
	virtual bool                RemoveEntry(int i, int j, bool destroy, bool reduced = false);
	virtual bool                RemoveEntries(int i, bool destroy, bool reduced = false);
	virtual bool                IsEmpty(int i, int j, bool reduced = false) const;

	//- Entry methods, reduced........................................
	virtual int                 GetCardinality(int i) const;
	virtual int                 GetCardinality(int i, int j) const;

	virtual int                 GetEquivalenceClass(int i) const;
	virtual int                 GetRepresentative(int i) const;

  //- Matrix creation methods.......................................
	virtual bool                Initialize(const DecisionTable &table, bool masked, bool modulo);

	virtual bool                Create(const DecisionTable &table, bool masked, bool modulo, const Discerner &discerner, float precision);
	virtual bool                Create(const DecisionTable &table, const Vector(int) &objects, bool masked, bool modulo, const Discerner &discerner, float precision);
	virtual bool                Create(const DecisionTable &table, int object_no, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision);

  //- I/O methods...................................................
	virtual bool                Save(ofstream &stream, char emptychar) const;

};

//-------------------------------------------------------------------
// Inlined operators/methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: HasAllocatedMatrix
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if the matrix seems to have been
//                 allocated. Assumes that it is then of correct
//                 size.
// Comments......:
// Revisions.....:
//===================================================================

inline bool
DiscernibilityMatrix::HasAllocatedMatrix() const {
	return !matrix_.empty();
}

//-------------------------------------------------------------------
// Method........: GetMap
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Maps the index of an object to the index of the
//                 object's equivalence class.
// Comments......:
// Revisions.....:
//===================================================================

inline int
DiscernibilityMatrix::GetMap(int i) const {
	return map_[i];
}

//-------------------------------------------------------------------
// Method........: GetIndex
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Maps from a (i, j) index of a symmetric matrix with
//                 empty diagonal to a single index of a vector
//                 holding the lower half of the matrix.
//
//                 Since a discernibility matrix is symmetric and
//                 has empty diagonal entries, we only need to store
//                 a little under half the matrix.
//
//                 Since Boolean algebras are multiplicative idempotent,
//                 we only need one representative from each equivalence
//                 class in the matrix.
//
// Comments......: Assumes that i and j are different.
// Revisions.....:
//===================================================================

inline int
DiscernibilityMatrix::GetIndex(int i, int j) const {

	if (i > j)
		return (((i * i) + i) >> 1) - i + j;
	else
		return (((j * j) + j) >> 1) - j + i;

}

#endif 
