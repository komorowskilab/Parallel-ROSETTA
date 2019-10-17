//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/equivalenceclasses.h>
#include <kernel/structures/equivalenceclass.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Static stuff (file scope). Hack to use STL sorting.
//===================================================================

static bool static_compare_ascending_ = true;

//-------------------------------------------------------------------
// STL comparator methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Comparison operator
// Author........: Aleksander šhrn
// Date..........:
// Description...: Compares the two specified eq. classes in the
//                 eq. class set with respect to their cardinalities.
// Comments......: Assumes static stuff has been properly set.
// Revisions.....:
//===================================================================

bool
EquivalenceClasses::Compare::operator()(const Handle<Structure> &a, const Handle<Structure> &b) const {

	if (a == b)
		return false;

	Handle<EquivalenceClass> eqclass_a = dynamic_cast(EquivalenceClass *, const_cast(Structure *, a.GetPointer()));
	Handle<EquivalenceClass> eqclass_b = dynamic_cast(EquivalenceClass *, const_cast(Structure *, b.GetPointer()));

	int cardinality_a = eqclass_a->GetNoObjects();
	int cardinality_b = eqclass_b->GetNoObjects();

	if (static_compare_ascending_)
		return (cardinality_a < cardinality_b);
	else
		return (cardinality_a > cardinality_b);

}

//-------------------------------------------------------------------
// Methods for class EquivalenceClasses.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

EquivalenceClasses::EquivalenceClasses(const EquivalenceClasses &in) : Structures(in) {
}

EquivalenceClasses::EquivalenceClasses() {
}

EquivalenceClasses::~EquivalenceClasses() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(EquivalenceClasses, EQUIVALENCECLASSES, Structures)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
EquivalenceClasses::Duplicate() const {
  return new EquivalenceClasses(*this);
}

//-------------------------------------------------------------------
// Methods inherited from Structures.
//===================================================================

//-------------------------------------------------------------------
// Method........: InsertStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Overloaded to check for consistency/homogenity.
//                 Ensures that the inserted structure really is an
//                 equivalence class.
// Comments......: AppendStructure is not necessary to overload, since
//                 Structures::AppendStructure is implemented via the
//                 InsertStructure method.
// Revisions.....:
//===================================================================

bool
EquivalenceClasses::InsertStructure(Structure *structure, int i) {

	if (structure == NULL) {
		Message::Error("Cannot insert a NULL eq. class.");
		return false;
	}

	if (!structure->IsA(EQUIVALENCECLASS)) {
		Message::Error("Attempted to insert something other than an equivalence class into a equivalence class set.");
		return false;
	}

	return Structures::InsertStructure(structure, i);

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Sort
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sorts the equivalence classes according to their
//                 cardinalities.
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClasses::Sort(bool ascending) {

	// Set static stuff.
	static_compare_ascending_ = ascending;

	Compare comparator;

	// Sort.
	std::sort(structures_.begin(), structures_.end(), comparator);

	return true;

}

 
