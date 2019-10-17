//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/structures.h>

#include <kernel/basic/undefined.h>
#include <kernel/basic/handle.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class Structures.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structures::Structures(const Structures &in) : ParentStructure(in) {

	int i, no_structures = in.GetNoStructures();

	// Duplicate the member structures.
  for (i = 0; i < no_structures; i++) {
    AppendStructure(in.GetStructure(i)->Duplicate());
  }

}

//-------------------------------------------------------------------
// Method........: Empty constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structures::Structures() {
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structures::~Structures() {
	Clear();
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Structures, STRUCTURES, ParentStructure)

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
Structures::Load(ifstream &stream) {

	Message message;

	// Load stuff higher up in the hierarchy.
	if (!ParentStructure::Load(stream))
		return false;

	int i, no_structures;

	// Load the number of member structures.
	if (!IOKit::Load(stream, no_structures))
		return false;

	message.Notify("Loading " + String::Format(no_structures) + " members...");

	// Load all member structures.
	for (i = 0; i < no_structures; i++) {

		String type;

		// Load the type description.
		if (!IOKit::Load(stream, type))
			return false;

		// Reconstruct the type of the child.
		Id id = IdHolder::GetId(type);

		if (id == Undefined::Id()) {
			Message::Error("Unknown member type identifier (" + type + ").");
			return false;
		}

		// Create a new member.
		Handle<Structure> member = Creator::Create(id);

		// Let the member load itself.
		if (!member->Load(stream))
			return false;

		// Append the member to the list of members.
		if (!AppendStructure(member.GetPointer()))
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
Structures::Save(ofstream &stream) const {

	Message message;

	// Save stuff higher up in the hierarchy.
	if (!ParentStructure::Save(stream))
		return false;

	// Save the number of member structures.
	if (!IOKit::Save(stream, GetNoStructures()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	message.Notify("Saving " + String::Format(GetNoStructures()) + " members...");

	int i;

	// Save all member structures to the same stream.
	for (i = 0; i < GetNoStructures(); i++) {

		Handle<Structure> member = GetStructure(i);

		String type = IdHolder::GetClassname(member->GetId());

		// Save the structure type (for reconstruction purposes).
		if (!IOKit::Save(stream, type))
			return false;

		if (!IOKit::Save(stream, '\n'))
			return false;

		// Save the structure.
		if (!member->Save(stream))
			return false;

	}

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure
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
Structures::Duplicate() const {
	return new Structures(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...: Total reset of object.
// Comments......:
// Revisions.....:
//===================================================================

void
Structures::Clear() {
	RemoveAllStructures();
	ParentStructure::Clear();
}

//-------------------------------------------------------------------
// Member structure administration methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoStructures
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of inherent member structures.
// Comments......:
// Revisions.....:
//===================================================================

int
Structures::GetNoStructures() const {
  return structures_.size();
}

//-------------------------------------------------------------------
// Method........: GetStructure
// Author........:
// Date..........:
// Description...: Returns a pointer to the specified member structure.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Structures::GetStructure(int i) const {

	// Check bounds.
  if ((i < 0) || (i >= GetNoStructures())) {
		Message::Error("Structure index out of bounds.");
		return NULL;
	}

//	Structure *pointer;
//	#pragma omp critical(pointer)
//	{
//		pointer = const_cast(Structure *, structures_[i].GetPointer());
//	}

	return const_cast(Structure *, structures_[i].GetPointer());
//	return pointer;

}

//-------------------------------------------------------------------
// Method........: InsertStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Inserts a new member structure at the given index.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structures::InsertStructure(Structure *member, int i) {

	if (member == NULL)
		return false;

	// Check bounds.
  if ((i < 0) || (i > GetNoStructures()) )
    return false;

	// Perform the insertion.
	
	structures_.insert(structures_.begin() + i, member);


  return true;

}

//-------------------------------------------------------------------
// Method........: RemoveStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Removes the specified member structure from the set.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structures::RemoveStructure(int i) {

	// Check bounds.
  if ((i < 0) || (i >= GetNoStructures()))
    return false;

	structures_[i] = NULL;

  structures_.erase(structures_.begin() + i);

  return true;

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: DuplicateStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structures *
Structures::DuplicateStructure(int i) const {

	// Check argument.
	if (i < 0 || i >= GetNoStructures())
		return NULL;

	// Create correct Structures subtype.
	Handle<Structures> structures = dynamic_cast(Structures *, Creator::Create(GetId()));

	// Duplicate indicated structure and append it to the new container.
	if (!structures->AppendStructure(GetStructure(i)->Duplicate()))
		return NULL;

	return structures.Release();

}
 
