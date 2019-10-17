//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/equivalenceclass.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/algorithm.h>

//-------------------------------------------------------------------
// Methods for class EquivalenceClass.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

EquivalenceClass::EquivalenceClass(const EquivalenceClass &in) : Structure(in) {
	objects_ = in.objects_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

EquivalenceClass::EquivalenceClass() {
}

EquivalenceClass::EquivalenceClass(int size) {
	objects_.reserve(size);
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

EquivalenceClass::~EquivalenceClass() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(EquivalenceClass, EQUIVALENCECLASS, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::Load(ifstream &stream) {

	// Clear present representation.
	Clear();

	int i, no_objects, object_no;

	// Load cardinality.
	if (!IOKit::Load(stream, no_objects))
		return false;

	// Load and add set members.
	for (i = 0; i < no_objects; i++) {
		if (!IOKit::Load(stream, object_no) || !InsertObject(object_no))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::Save(ofstream &stream) const {

	int i, no_objects = GetNoObjects();

	// Save cardinality.
	if (!IOKit::Save(stream, no_objects))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save set members.
	for (i = 0; i < no_objects; i++) {
		if (!IOKit::Save(stream, GetObject(i)))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
EquivalenceClass::Duplicate() const {
  return new EquivalenceClass(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
EquivalenceClass::Clear() {
	objects_.erase(objects_.begin(), objects_.end());
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of objects in the eq. class.
// Comments......:
// Revisions.....:
//===================================================================

int
EquivalenceClass::GetNoObjects() const {
	return objects_.size();
}

//-------------------------------------------------------------------
// Method........: GetObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the object in the eq. class in the given
//                 position.
// Comments......:
// Revisions.....:
//===================================================================

int
EquivalenceClass::GetObject(int position_no) const {
	return objects_[position_no];
}

//-------------------------------------------------------------------
// Method........: InsertObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Adds a new member (i.e. object index) to the
//                 eq. class.
//
//                 Keeps the vector sorted and unique.
//
// Comments......: Multiple inserts are not accepted.
// Revisions.....: Aš 971303: Fixed bug in determining position.
//                            Added handling of special cases.
//===================================================================

bool
EquivalenceClass::InsertObject(int object_no) {

	int i, no_objects = GetNoObjects(), position;

	// Special case: Empty eq. class.
	if (no_objects == 0)
		position = 0;

	// Special case: Insert last.
	else if (GetObject(no_objects - 1) < object_no)
		position = no_objects;

	// Special case: Insert first.
	else if (GetObject(0) > object_no)
		position = 0;

	// General case: Determine position (to optimize, use binary search).
	else {
		for (i = 0; i < no_objects; i++) {
			if (GetObject(i) < object_no)  // Keep sorted.
				break;
			if (GetObject(i) == object_no) // Ensure uniqueness.
				return false;
		}
		position = i;
	}

	objects_.insert(objects_.begin() + position, object_no);

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Removes the member in the given position from the
//                 eq. class.
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::RemoveObject(int position_no) {

	objects_.erase(objects_.begin() + position_no);

	return true;

}

//-------------------------------------------------------------------
// Method........: FindObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an object index, returns the object's
//                 position index in the eq. class.
//
//                 Returns Undefined::Integer() if the specified object
//                 is not a member of the eq. class.
//
// Comments......: Rewrite to O(logn).
// Revisions.....:
//===================================================================

int
EquivalenceClass::FindObject(int object_no) const {

	int i, no_objects = GetNoObjects();

	for (i = 0; i < no_objects; i++) {
		if (GetObject(i) == object_no)
			return i;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: IsMember
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the given object index is a member
//                 of the eq. class.
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::IsMember(int object_no) const {
	return (FindObject(object_no) != Undefined::Integer());
}

//-------------------------------------------------------------------
// Method........: GetGeneralizedDecision
// Author........: Aleksander šhrn
// Date..........:
// Description...: Looks up the decision values of the objects in the
//                 eq. class, and returns (in-place) the
//                 "generalized decision".
//
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::GetGeneralizedDecision(GeneralizedDecision &decisions, const DecisionTable &table, int attribute_no, bool masked) const {

	// Clear current contents.
	decisions.Clear();

	int i, no_objects = GetNoObjects();

	Map(int, int) map;

	// Fill map.
	for (i = 0; i < no_objects; i++) {
		int decision = table.GetEntry(GetObject(i), attribute_no, masked);
		map.insert(Pair(const int, int)(decision, 0));
		map[decision]++;
	}

	decisions.Reserve(map.size());

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	Map(int, int)::iterator it;

	// Fill generalized decision.
	for (it = map.begin(); it != map.end(); it++)
		decisions.AppendDecision((*it).first, (*it).second);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetGeneralizedDecision
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Automatically detects the dec. attribute.
// Revisions.....:
//===================================================================

bool
EquivalenceClass::GetGeneralizedDecision(GeneralizedDecision &decisions, const DecisionTable &table) const {

	// Operate on a masked table.
	bool masked = true;

	// Get decision attribute.
	int attribute_no = table.GetDecisionAttribute(masked);

	if (attribute_no == Undefined::Integer()) {
		Message::Error("Could not acquire decision attribute index.");
		return false;
	}

	return GetGeneralizedDecision(decisions, table, attribute_no, masked);

}

//-------------------------------------------------------------------
// Method........: GetNoObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a decision value, looks up the decision values
//                 of the objects in the eq. class, and returns the
//                 number of object that have the specified decision value.
// Comments......:
// Revisions.....:
//===================================================================

int
EquivalenceClass::GetNoObjects(int decision, const DecisionTable &table, int attribute_no, bool masked) const {

	int i, no_objects = GetNoObjects(), count = 0;

	// Is the eq. class empty?
	if (no_objects == 0)
		return 0;

	// Count how many members of the eq. class that have the given decision value.
	for (i = 0; i < no_objects; i++) {
		if (table.GetEntry(GetObject(i), attribute_no, masked) == decision)
			count++;
	}

	return count;

}

//-------------------------------------------------------------------
// Method........: GetNoObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of objects that have the
//                 specified decision.  Automatically
//                 determines the index of the decision attribute.
// Comments......:
// Revisions.....:
//===================================================================

int
EquivalenceClass::GetNoObjects(int decision, const DecisionTable &table) const {

	// Operate on a masked table.
	bool masked = true;

	// Acquire the index of the decision attribute.
	int attribute_no = table.GetDecisionAttribute(masked);

	if (attribute_no == Undefined::Integer()) {
		Message::Error("Could not acquire decision attribute index.");
		return 0;
	}

	return GetNoObjects(decision, table, attribute_no, masked);

}

//-------------------------------------------------------------------
// Method........: GetMembership
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a decision value, looks up the decision values
//                 of the objects in the eq. class, and returns the rough
//                 membership function.
// Comments......:
// Revisions.....:
//===================================================================

float
EquivalenceClass::GetMembership(int decision, const DecisionTable &table, int attribute_no, bool masked) const {

	// Count how many objects in the eq. class that have the specified decision.
  int count = GetNoObjects(decision, table, attribute_no, masked);

	// None?
	if (count == 0)
		return 0.0;

	// How many objects are there in the equivalence class?
	int no_objects = GetNoObjects();

	// Add a safe-guard to avoid any numerical quirks.
	if (count == no_objects)
		return 1.0;

	return static_cast(float, count) / no_objects;

}

//-------------------------------------------------------------------
// Method........: GetMembership
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the rough membership function.  Automatically
//                 determines the index of the decision attribute.
// Comments......:
// Revisions.....:
//===================================================================

float
EquivalenceClass::GetMembership(int decision, const DecisionTable &table) const {

	// Operate on a masked table.
	bool masked = true;

	// Acquire the index of the decision attribute.
	int attribute_no = table.GetDecisionAttribute(masked);

	if (attribute_no == Undefined::Integer()) {
		Message::Error("Could not acquire decision attribute index.");
		return 0.0;
	}

	return GetMembership(decision, table, attribute_no, masked);

}

//-------------------------------------------------------------------
// Method........: IsConsistent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if all objects in the equivalence
//                 class have the same value for the specified
//                 attribute.
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::IsConsistent(const DecisionTable &table, int attribute_no, bool masked) const {

	int i, no_objects = GetNoObjects();

	// Is the eq. class empty or a singleton?
	if (no_objects <= 1)
		return true;

	// Get the attibute value of the first object.
	int value = table.GetEntry(GetObject(0), attribute_no, masked);

	// Check if all other objects have the same attribute value.
	for (i = 1; i < no_objects; i++) {
		if (value != table.GetEntry(GetObject(i), attribute_no, masked))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsConsistent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Automatically detects the dec. attribute.
// Revisions.....:
//===================================================================

bool
EquivalenceClass::IsConsistent(const DecisionTable &table) const {

	// Operate on a masked table.
	bool masked = true;

	// Acquire the index of the decision attribute.
	int attribute_no = table.GetDecisionAttribute(masked);

	if (attribute_no == Undefined::Integer()) {
		Message::Error("Could not acquire decision attribute index.");
		return false;
	}

	return IsConsistent(table, attribute_no, masked);

}

//-------------------------------------------------------------------
// Method........: Contains
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified eq. class is a subset
//                 of this eq. class.  The additional boolean argument
//                 specifies whether or not the subset must be a proper
//                 subset.
//
// Comments......: Rewrite to O(n).
// Revisions.....:
//===================================================================

bool
EquivalenceClass::Contains(const EquivalenceClass &in, bool proper) const {

	int this_size = GetNoObjects();
	int that_size = in.GetNoObjects();

	// Do a quick initial cardinality check.
	if ((that_size > this_size) || (proper && (that_size == this_size)))
		return false;

	int i;

  // Are all members of the input eq. class also members of this eq. class?
	for (i = 0; i < that_size; i++) {
		if (!IsMember(in.GetObject(i)))
			return false;
	}

	// Must the subset thingy be proper?
	if (proper && (that_size == this_size))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::Reserve(int size) {

	// Reserve space, if needed.
	if (objects_.size() < size)
		objects_.reserve(size);

	return true;

}

//-------------------------------------------------------------------
// Method........: Sort
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sorts the object indices, in either ascending or
//                 descending order.
// Comments......:
// Revisions.....:
//===================================================================

bool
EquivalenceClass::Sort(bool ascending) {

	// Sort.
	if (objects_.size() > 1) {
		if (ascending)
			std::sort(objects_.begin(), objects_.end());
		else
			std::sort(objects_.begin(), objects_.end(), std::greater<int>());
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The value of offset will usually be 0 (default)
//                 or 1.
// Revisions.....:
//===================================================================

bool
EquivalenceClass::Format(String &formatted, int offset) const {

	// Clear formatted string to return.
	formatted = "";

	int i, no_objects = GetNoObjects();

	formatted += '{';

	// List all members.
	for (i = 0; i < no_objects; i++) {
		formatted += String::Format(offset + GetObject(i));
		if (i < no_objects - 1)
			formatted += ", ";
	}

	formatted += '}';

	return true;

}
 
