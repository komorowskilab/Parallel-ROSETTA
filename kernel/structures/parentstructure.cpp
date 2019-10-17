//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/parentstructure.h>
#include <kernel/structures/structures.h>
#include <kernel/structures/batchclassification.h> // Hack.

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/systemkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Methods for class ParentStructure.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The children are (presently) not duplicated.
// Revisions.....:
//===================================================================

ParentStructure::ParentStructure(const ParentStructure &in) : AnnotatedStructure(in) {

  // Remove any present children.
	RemoveAllChildren();

#if 0
	int i, no_children = in.GetNoChildren();

	// Duplicate the children.
  for (i = 0; i < no_children; i++) {
    AppendChild(in.GetChild(i)->Duplicate());
  }
#endif

}

//-------------------------------------------------------------------
// Method........: Empty constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ParentStructure::ParentStructure() {
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ParentStructure::~ParentStructure() {
  Clear();
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ParentStructure, PARENTSTRUCTURE, AnnotatedStructure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Too much code overlap with Creator::Load...
//                 Unify later.
//
// Revisions.....: Aš 980603: Added hack for backwards compatibility.
//===================================================================

bool
ParentStructure::Load(ifstream &stream) {

	Message message;

	// Load stuff higher up in the hierarchy.
	if (!AnnotatedStructure::Load(stream))
		return false;

	int i, no_children;

	// Load the number of children.  If the loading fails, assume no children.
	if (!IOKit::Load(stream, no_children))
		return true;

	// Load the children recursively.
  for (i = 0; i < no_children; i++) {

		String type;

		// Load the type description of the child.
		if (!IOKit::Load(stream, type))
			return false;

		// Reconstruct the type of the child.
		Id id = IdHolder::GetId(type);

		// Check that the structure type is installed.
		// If not, take appropriate measures.
		if (ObjectManager::Count(id) == 0) {

			// Ugly hack: To allow component interchangeability.
			if (type.Contains("DecisionTable"))
				type = "DecisionTable";

			id = IdHolder::GetId(type);

		}

		if (id == Undefined::Id()) {
			Message::Error("Unknown child type identifier (" + type + ").");
			return false;
		}

		// Hack: For backwards compatibility.
		if (id == CONFUSIONMATRIX)
			id = BATCHCLASSIFICATION;

		// Installed object type?
		if (ObjectManager::Count(id) == 0) {
			Message::Error("Type " + type + " not installed.");
			return false;
		}

		// Create a new child.
		Handle<Structure> child = Creator::Create(id);

		message.Notify("Loading " + type + "...");

		// Let the child load itself.
		if (!child->Load(stream))
			return false;

		// Append the child to the list of children.
		if (!AppendChild(child.GetPointer()))
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
ParentStructure::Save(ofstream &stream) const {

	Message message;

	// Save stuff higher up in the hierarchy.
	if (!AnnotatedStructure::Save(stream))
		return false;

	// Save the number of children.
	if (!IOKit::Save(stream, GetNoChildren()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	int i;

	// Save the children recursively.
  for (i = 0; i < GetNoChildren(); i++) {

		Handle<Structure> child = GetChild(i);

		String type = IdHolder::GetClassname(child->GetId());

		message.Notify("Saving " + type + "...");

		// Save the type description of the child (for later reconstruction).
		if (!IOKit::Save(stream, type))
			return false;

		if (!IOKit::Save(stream, '\n'))
			return false;

		// Save the child.
		if (!child->Save(stream))
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
ParentStructure::Duplicate() const {
	return new ParentStructure(*this);
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
ParentStructure::Clear() {
	RemoveAllChildren();
	AnnotatedStructure::Clear();
}

//-------------------------------------------------------------------
// Methods inherited from Structure, child management methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoChildren
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
ParentStructure::GetNoChildren() const {

	if (children_.IsNULL())
		return 0;

	return children_->GetNoStructures();

}

//-------------------------------------------------------------------
// Method........: GetChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ParentStructure::GetChild(int i) const {

	if (children_.IsNULL())
		return NULL;

	return children_->GetStructure(i);

}

//-------------------------------------------------------------------
// Method........: InsertChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ParentStructure::InsertChild(Structure *child, int i) {

	if (child == NULL)
		return false;

	// Create the children container if needed.
	if (children_.IsNULL())
		children_ = Creator::Structures();

	// If the structure's already a member of the child set, then do nothing.
	if (children_->IsMember(child))
		return true;

	// Do the insertion.
	if (!children_->InsertStructure(child, i))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: AppendChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ParentStructure::AppendChild(Structure *child) {
	return InsertChild(child, GetNoChildren());
}

//-------------------------------------------------------------------
// Method........: RemoveChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ParentStructure::RemoveChild(int i) {

	if (children_.IsNULL())
		return false;

	// Do the removal.
	return children_->RemoveStructure(i);

}

 
