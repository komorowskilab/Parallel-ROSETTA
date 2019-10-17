//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/reducts.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/rules.h>

#include <kernel/system/stdlib.h>
#include <kernel/system/fstream.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

//-------------------------------------------------------------------
// Static stuff (file scope). Hack to use STL sorting.
//===================================================================

static bool static_compare_ascending_ = true;
static int  static_compare_property_  = 0;

//-------------------------------------------------------------------
// STL comparator methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Comparison operator
// Author........: Aleksander šhrn
// Date..........:
// Description...: Compares two reducts according to some specified
//                 reduct property.
// Comments......: Assumes static stuff has been properly set.
// Revisions.....:
//===================================================================

bool
Reducts::Compare::operator()(const Handle<Structure> &a, const Handle<Structure> &b) const {

	if (a == b)
		return false;

	Handle<Reduct> reduct_a = dynamic_cast(Reduct *, const_cast(Structure *, a.GetPointer()));
	Handle<Reduct> reduct_b = dynamic_cast(Reduct *, const_cast(Structure *, b.GetPointer()));

	int property_a = 0;
	int property_b = 0;

	switch (static_compare_property_) {
		case 0:  // Support.
		         property_a = reduct_a->GetSupport();
			       property_b = reduct_b->GetSupport();
						 break;
		case 1:  // Length.
		         property_a = reduct_a->GetNoAttributes();
			       property_b = reduct_b->GetNoAttributes();
						 break;
		default: return false;
	}

	if (static_compare_ascending_)
		return (property_a < property_b);
	else
		return (property_a > property_b);

}

//-------------------------------------------------------------------
// Methods for class Reducts.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Reducts::Reducts(const Reducts &in) : Structures(in) {
}

Reducts::Reducts() {
}

Reducts::~Reducts() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Reducts, REDUCTS, Structures)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
Reducts::Duplicate() const {
  return new Reducts(*this);
}

//-------------------------------------------------------------------
// Method........: FindMember
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Eliminate this method with time -- should be
//                 implemented on a more general level.
// Revisions.....:
//===================================================================

int
Reducts::FindMember(const Structure *member, bool physical) const {

	// Physical membership is implemented elsewhere.
	if (physical)
		return Structure::FindMember(member, physical);

	if (member == NULL)
		return Undefined::Integer();

	if (!member->IsA(REDUCT))
		return Undefined::Integer();

	// Ugly, but safe.
	Handle<Reduct> reduct = dynamic_cast(Reduct *, const_cast(Structure *, member));

	int i, no_reducts = GetNoReducts();

	// Check logical membership.
	for (i = 0; i < no_reducts; i++) {
		if (*reduct == *GetReduct(i))
			return i;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Methods inherited from Structures.
//===================================================================

//-------------------------------------------------------------------
// Method........: InsertStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Overloaded to check for consistency/homogenity.
//                 Ensures that the inserted structure really is a
//                 reduct.
// Comments......: AppendStructure is not necessary to overload, since
//                 Structures::AppendStructure is implemented via the
//                 InsertStructure method.
// Revisions.....:
//===================================================================

bool
Reducts::InsertStructure(Structure *structure, int i) {

	if (structure == NULL) {
		Message::Error("Cannot insert a NULL reduct.");
		return false;
	}

	if (!structure->IsA(REDUCT)) {
		Message::Error("Attempted to insert a non-reduct to a reduct set.");
		return false;
	}

	return Structures::InsertStructure(structure, i);

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoReducts/GetReduct
// Author........: Aleksander šhrn
// Date..........:
// Description...: Convenience methods.
// Comments......: Made virtual due to the special nature of the
//                 wrapper for RSES reduct collections.
// Revisions.....:
//===================================================================

int
Reducts::GetNoReducts() const {
	return GetNoStructures();
}

Reduct *
Reducts::GetReduct(int i) const {
	return dynamic_cast(Reduct *, GetStructure(i));
}

//-------------------------------------------------------------------
// Method........: Reindex
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reindexes all member reducts.
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducts::Reindex(const Vector(int) &indices) {

	int i, no_reducts = GetNoReducts();

	for (i = 0; i < no_reducts; i++) {
		if (!GetReduct(i)->Reindex(indices))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Sort
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sorts the reducts according to some specified
//                 property.
//
//                    0 = Support
//                    1 = Length
//
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducts::Sort(int property, bool ascending) {

	// Check property parameter.
	if (property < 0 || property > 1)
		return false;

	// Set static stuff.
	static_compare_property_  = property;
	static_compare_ascending_ = ascending;

	Compare comparator;
	Message message;

	message.Notify("Sorting reducts...");

	// Sort.
	std::sort(structures_.begin(), structures_.end(), comparator);

	return true;

}

//-------------------------------------------------------------------
// Method........: PostCreation
// Author........: Aleksander šhrn
// Date..........:
// Description...: This method can be overloaded by derived reduct
//                 set representations that need to have some kind of
//                 post-processing performed after thay have been
//                 created.
//
//                 This method is called after the set of reducts
//                 has been computed/created.
//
// Comments......:
// Revisions.....: Aš 990412: Added discerner argument.
//		   -N 30/08/13: Added critical section to the append process
//			of the reduct set and changed some structuring to allow for it.
//		   -N 02/09/13: Added a lock and unlock version so that Johnson threading
//			does not negatively impact CV threading.
//===================================================================

bool
Reducts::PostCreation(const DecisionTable &table, const Discerner &discerner, bool modulo, bool lock) {

	if (GetNoReducts() == 0)
		return true;
	// Assume all reducts are of the same type.
	// As default, only generate rules from object-related reducts.
	if (GetReduct(0)->GetDiscernibilityType() == Reduct::DISCERNIBILITY_FULL)
		return true;

	Message message;

	if (modulo)
		message.Notify("Computing rules...");
	else
		message.Notify("Computing patterns...");

	Handle<Rules> rules = Creator::Rules();

/*
	if (!rules->Create(*this, table, discerner, false))
		return false;
*/

	bool ret;
//	#pragma omp critical(postcreation)
//	{
	ret = rules->Create(*this, table, discerner, false);
//	}
	if (!ret) return false;

	if (modulo)
		rules->SetName("Rules");
	else
		rules->SetName("Patterns");

	//Better alternative. This way appends won't overlap in multi-threaded mode and cause undefined behaviour. -N
	//Depending on the situation, we may need to lock the insertion.

	//No need to lock -N
	if (!lock) {
		return AppendChild(rules.GetPointer());
	}
	//Lock version -N
	else {
		bool success;
		#pragma omp critical(appendreduct)
		{
			success = AppendChild(rules.GetPointer());
		}
		return success;
	}
	return false;
}

//-------------------------------------------------------------------
// Method........: AppendReduct
// Author........: Aleksander šhrn
// Date..........:
// Description...: This method can be overloaded by derived reduct
//                 set representations that need to have some kind of
//                 processing performed when reducts are added, for
//                 instance overlaying the reduct over
//                 a table to produce a (reduct, pattern/rule) pair,
//                 in case one of these cannot exist without the other.
//
//                 The default ignores most arguments, and simply
//                 adds the reduct to the set.
// Comments......:
// Revisions.....: 02/09/13 -N Added lock and unlock variant, so that
//			threaded Johnson does not bog down threaded CV.
//===================================================================

bool
Reducts::AppendReduct(Reduct *reduct, const DecisionTable &/*table*/, int /*object_no*/, bool /*masked*/, bool /*modulo*/, bool verify, bool lock) {

	// Verify uniqueness?
	if (verify) {
		Message::Debug("Reducts::AppendReduct not fully implemented yet.");
	}
	//No lock version -N
	if (!lock)
		return AppendStructure(reduct);
	//Locked version. -N
	else {
		bool success;
		#pragma omp critical(appendreduct)
		{
			success = AppendStructure(reduct);
		}
		return success;
	}
	//This return should never execute.
	return false;
}

//-------------------------------------------------------------------
// Method........: GetAttributeSupports
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a map from the attribute indices
//                 to the individual supports from each reduct that
//                 contains that particular attribute.
// Comments......: Done in two passes to allow use of reserve method.
// Revisions.....:
//===================================================================

bool
Reducts::GetAttributeSupports(IVMap &attributes) const {

	// Clear input map.
	attributes.erase(attributes.begin(), attributes.end());

	int i, j, no_reducts = GetNoReducts();

	if (no_reducts == 0)
		return true;

	// Do a convenient typedef.
	typedef Map(int, int) IIMap;

	IIMap iimap;

	// Build index map.
	for (i = 0; i < no_reducts; i++) {

		Handle<Reduct> reduct = GetReduct(i);

		// Get length of reduct.
		int length = reduct->GetNoAttributes();

		// Update map.
		for (j = 0; j < length; j++) {
			int attribute = reduct->GetAttribute(j);
			iimap.insert(Pair(const int, int)(attribute, 0));
			iimap[attribute]++;
		}

	}

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	IIMap::iterator iterator = iimap.begin();

	// Initialize vector map.
	while (!(iterator == iimap.end())) {
		attributes.insert(Pair(const int, Vector(int))((*iterator).first, Vector(int)()));
		attributes[(*iterator).first].reserve((*iterator).second);
		iterator++;
	}

	// Fill vector map.
	for (i = 0; i < no_reducts; i++) {

		Handle<Reduct> reduct = GetReduct(i);

		// Get length of reduct.
		int length = reduct->GetNoAttributes();

		// Collect attribute supports.
		for (j = 0; j < length; j++)
			attributes[reduct->GetAttribute(j)].push_back(reduct->GetSupport());

	}

	return true;

}


 
