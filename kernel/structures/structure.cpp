//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/projectmanager.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method  ......: StaticRecursiveGetAllChildren
// Author........: Aleksander šhrn/Daniel Remmem
// Date..........:
// Description...: Made static so as to hide the last recursion parameter
//                 for clients of GetAllChildren method.
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticRecursiveGetAllChildren(Id id, Identifier::Handles &children, const Structure *parent) {

	// Valid parent?
	if (parent == NULL)
		return false;

	int i;

	// Loop recursively over all children.
	for (i = 0; i < parent->GetNoChildren(); i++) {
		Handle<Structure> child = parent->GetChild(i);
		if (child->IsA(id))
			children.push_back(Handle<Identifier>(child.GetPointer()));
		if (!child->HasChildren())
			continue;
		if (!StaticRecursiveGetAllChildren(id, children, child.GetPointer()))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticRecursiveFindParent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Called from FindParent, used to hide recursion.
//
//                 Given two structures, whereof one is a descendant
//                 of the other, returns the descendant's immediate
//                 parent. If the descendant appears as an internal
//                 member structure of a structure set, the structure
//                 set is returned. If the descendant appears as a
//                 direct child of a parent structure, the parent
//                 structure is returned. Otherwise, NULL is returned.
//
// Comments......: Note that traversal of internal member structures is
//                 not implemented, although it really should be.
//                 This may not be fatal however, since usually internal
//                 member structures are "simple" in the sense that they
//                 aren't parent structures themselves.
// Revisions.....:
//===================================================================

//static Structure *
Structure *
StaticRecursiveFindParent(const Structure *descendant, const Structure *ancestor) {

	// Check aregument validity.
	if (descendant == NULL || ancestor == NULL)
		return NULL;

	// Check if the descendant is an internal member of the ancestor.
	if (ancestor->IsMember(descendant))
	  return const_cast(Structure *, ancestor);

	int i, no_children = ancestor->GetNoChildren();

	// Iterate over all the ancestor's children.
	for (i = 0; i < no_children; i++) {

		// Get current child.
		Structure *child = ancestor->GetChild(i);

		// Is the current child and the descendant the same object?
		if (child == descendant)
			return const_cast(Structure *, ancestor);

		// Is the descendant an internal member of the current child?
		if (child->IsMember(descendant))
			return child;

		// Recurse.
		child = StaticRecursiveFindParent(descendant, child);

		// If the recursion was successful, return.
		if (child != NULL)
			return child;

	}

	// The structure is not a registered descendant of the given ancestor.
	return NULL;

}

//-------------------------------------------------------------------
// Methods for class Structure.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Structure::Structure(const Structure &/*in*/) {
}

Structure::Structure() {
}

Structure::~Structure() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Structure, STRUCTURE, Persistent)

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
Structure::Load(ifstream &stream) {
  return Persistent::Load(stream);
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
Structure::Save(ofstream &stream) const {
  return Persistent::Save(stream);
}


//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::Load(const String &filename) {
  return Persistent::Load(filename);
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
Structure::Save(const String &filename) const {
  return Persistent::Save(filename);
}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Structure::Apply(const Algorithm &algorithm) {
	
  return algorithm.Apply(*this);
}


/*
//-------------------------------------------------------------------
// Method........: Apply
// Author........: Nicholas Baltzer
// Date..........: 06/09/13
// Description...: Same as Apply, but with the option of disabling threads.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Structure::Apply(const Algorithm &algorithm, bool usethreads) {
	
  return algorithm.Apply(*this, usethreads);
}
*/

//-------------------------------------------------------------------
// Name management methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetName/SetName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures
//                 with or without annotations.
//
//                 Should be overloaded by annotated structures.
// Revisions.....:
//===================================================================

const String &
Structure::GetName() const {
  return IdHolder::GetClassname(GetId());
}

bool
Structure::SetName(const String &/*name*/) {
  return false;
}

//-------------------------------------------------------------------
// Child management methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoChildren
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures
//                 with or without children.
//
//                 Should be overloaded by structures with children
//                 (parent structures).
// Revisions.....:
//===================================================================

int
Structure::GetNoChildren() const {
	return 0;
}

//-------------------------------------------------------------------
// Method........: GetChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without children.
//
//                 Should be overloaded by structures with children
//                 (parent structures).
// Revisions.....:
//===================================================================

Structure *
Structure::GetChild(int /*i*/) const {
	Message::Error("This method should never have been invoked.");
	return NULL;
}

//-------------------------------------------------------------------
// Method........: InsertChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without children.
//
//                 Should be overloaded by structures with children
//                 (parent structures).
// Revisions.....:
//===================================================================

bool
Structure::InsertChild(Structure * /*child*/, int /*i*/) {
	return false;
}

//-------------------------------------------------------------------
// Method........: AppendChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without children.
// Revisions.....:
//===================================================================

bool
Structure::AppendChild(Structure *child) {
	return InsertChild(child, GetNoChildren());
}

//-------------------------------------------------------------------
// Method........: RemoveChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without children.
//
//                 Should be overloaded by structures with children
//                 (parent structures).
// Revisions.....:
//===================================================================

bool
Structure::RemoveChild(int /*i*/) {
  return false;
}

//-------------------------------------------------------------------
// Method........: RemoveAllChildren
// Author........: Aleksander šhrn
// Date..........:
// Description...: Removes all children.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::RemoveAllChildren() {

  int i, no_children = GetNoChildren();

	for (i = no_children - 1; i >= 0; i--) {
		if (!RemoveChild(i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetAllChildren
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in place) an array of pointers (handles)
//                 to all children of a specified type.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::GetAllChildren(Id id, Identifier::Handles &children, bool recursive) const {

	if (recursive)
		return StaticRecursiveGetAllChildren(id, children, this);

	int i;

	// Loop over all immediate children.
	for (i = 0; i < GetNoChildren(); i++) {
		Handle<Structure> child = GetChild(i);
		if (child->IsA(id))
			children.push_back(Handle<Identifier>(child.GetPointer()));
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: FindChild
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the child index of the argument structure,
//                 Undefined::Integer() if not found.
//
// Comments......: Physical (not logical) check.
// Revisions.....:
//===================================================================

int
Structure::FindChild(const Structure *child) const {

	int i, no_children = GetNoChildren();

  for (i = 0; i < no_children; i++) {
		if (child == GetChild(i))
			return i;
  }

  return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: IsParent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this structure is a parent of the
//                 argument structure. If the query is recursive,
//                 parenthood is generalized to being an ancestor.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::IsParent(const Structure *child, bool recursive) const {

	// Check immediate children.
	if (FindChild(child) != Undefined::Integer())
		return true;

	if (!recursive)
		return false;

	int i;

	// Propagate query.
	for (i = 0; i < GetNoChildren(); i++) {
		if (GetChild(i)->IsParent(child, recursive))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Method........: HasChildren
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the structure has any children.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::HasChildren() const {
	return (GetNoChildren() > 0);
}

//-------------------------------------------------------------------
// Method........: HasChild
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Returns true if the structure has a child of the
//                 specified type.
// Revisions.....:
//===================================================================

bool
Structure::HasChild(Id id, bool recursive) const {

	int i;

	// Query immediate children.
	for (i = 0; i < GetNoChildren(); i++) {
		if (GetChild(i)->IsA(id))
			return true;
	}

	if (!recursive)
		return false;

	// Propagate query.
	for (i = 0; i < GetNoChildren(); i++) {
		if (GetChild(i)->HasChild(id, recursive))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Member management methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoStructures
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures
//                 with or without internal member structures.
//
//                 Should be overloaded by structures with internal
//                 members (structure sets).
// Revisions.....:
//===================================================================

int
Structure::GetNoStructures() const {
	return 0;
}

//-------------------------------------------------------------------
// Method........: GetStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without internal member structures.
//
//                 Should be overloaded by structures with internal
//                 members (structure sets).
// Revisions.....:
//===================================================================

Structure *
Structure::GetStructure(int /*i*/) const {
	Message::Error("This method should never have been invoked.");
	return NULL;
}

//-------------------------------------------------------------------
// Method........: FindMember
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a structure, returns the position in
//                 the set of that structure.
//
//                 Example:  structures = {StructureA, StructureB}
//                           structures.FindMember(&StructureA) returns 0;
//                           structures.FindMember(&StructureB) returns 1;
//                           structures.FindMember(&StructureC) returns Undefined::Integer();
//
//                 Membership can be both physical and logical.
//
// Comments......: Logical membership currently not handled -- a
//                 small redesign may be called for.
// Revisions.....:
//===================================================================

int
Structure::FindMember(const Structure *member, bool physical) const {

	int i, no_structures = GetNoStructures();

  for (i = 0; i < no_structures; i++) {
		if (physical) {
			if (member == GetStructure(i))
				return i;
		}
		else {
			Message::Error("Check for logical membership in FindMember not implemented yet.");
			break;
		}
  }

  return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: IsMember
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Returns true if the given structure is an internal
//                 member structure of this structure.
//
//                 Membership can be both physical and logical.
// Revisions.....:
//===================================================================

bool
Structure::IsMember(const Structure *member, bool physical) const {
  return (FindMember(member, physical) != Undefined::Integer());
}

//-------------------------------------------------------------------
// Method........: HasMembers
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Returns true if this structure has any internal
//                 member structures.
// Revisions.....:
//===================================================================

bool
Structure::HasMembers() const {
	return (GetNoStructures() > 0);
}

//-------------------------------------------------------------------
// Method........: InsertStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without internal member structures.
//
//                 Should be overloaded by structures with internal
//                 members (structure sets).
// Revisions.....:
//===================================================================

bool
Structure::InsertStructure(Structure * /*member*/, int /*i*/) {
	return false;
}


//-------------------------------------------------------------------
// Method........: AppendStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without internal member structures.
// Revisions.....:
//===================================================================

bool
Structure::AppendStructure(Structure *member) {
	return InsertStructure(member, GetNoStructures());
}

//-------------------------------------------------------------------
// Method........: RemoveStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Makes the interface more uniform for structures with
//                 or without internal member structures.
//
//                 Should be overloaded by structures with internal
//                 members (structure sets).
// Revisions.....:
//===================================================================

bool
Structure::RemoveStructure(int /*i*/) {
  return false;
}

//-------------------------------------------------------------------
// Method........: RemoveAllStructures
// Author........: Aleksander šhrn
// Date..........:
// Description...: Removes all internal member structures.
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::RemoveAllStructures() {

  int i, no_members = GetNoStructures();

	for (i = no_members - 1; i >= 0; i--) {
		if (!RemoveStructure(i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Merge
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Does not eliminate duplicates, yet.
// Revisions.....:
//===================================================================

bool
Structure::Merge(const Structure &structure, bool verify) {

	// Verify types.
	if (verify && !structure.IsA(GetId()))
		return false;

	int i, no_structures = structure.GetNoStructures();

	// Append duplicates.
	for (i = 0; i < no_structures; i++) {
		if (!AppendStructure(structure.GetStructure(i)->Duplicate()))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Parent search methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsAnnotated
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Structure::IsAnnotated() const {
	return (GetAnnotation() != NULL);
}

//-------------------------------------------------------------------
// Method........: GetAnnotation
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Should be overloaded by annotatable structures.
// Revisions.....:
//===================================================================

Annotation *
Structure::GetAnnotation() const {
	return NULL;
}

//-------------------------------------------------------------------
// Method........: SetAnnotation
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Should be overloaded by annotatable structures.
// Revisions.....:
//===================================================================

bool
Structure::SetAnnotation(Annotation * /*annotation*/)  {
	return false;
}

//-------------------------------------------------------------------
// Method........: Touch
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Should be overloaded by annotatable structures.
// Revisions.....:
//===================================================================

bool
Structure::Touch(const String &/*action*/) {
	return false;
}

//-------------------------------------------------------------------
// Parent search methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: FindParent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Structure::FindParent() const {

	Handle<Project> project = ProjectManager::GetProject(this);

	if (project == NULL)
		return NULL;

	return FindParent(*project);

}

//-------------------------------------------------------------------
// Method........: FindParent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an ancestor structure that this structure is
//                 directly or indirectly derived from, returns this
//                 structure's immediate parent.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Structure::FindParent(const Structure &ancestor) const {

	// No object is its own parent.
	if (&ancestor == this)
		return NULL;

	Structure *structure;
	#pragma omp critical(SRFP)
	{
		structure = StaticRecursiveFindParent(this, &ancestor);
	}
	return structure;

}

//-------------------------------------------------------------------
// Method........: FindParent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an ancestor structure that this structure is
//                 directly or indirectly derived from, returns the
//                 closest ancestor to this structure of a specified
//                 type.
// Comments......: Can be implemented more efficiently, but that's
//                 probably not necessary.
// Revisions.....:
//===================================================================

Structure *
Structure::FindParent(Id id, const Structure &ancestor) const {

	// Find the immediate parent.
	Structure *parent = FindParent(ancestor);

	// Until a parent of the given type is found, get the parent's parent.
	while (parent != NULL) {
		if (parent->IsA(id))
			return parent;
		parent = parent->FindParent(ancestor);
	}

	// No parent of the specified type found.
	return NULL;

}

//-------------------------------------------------------------------
// Method........: FindParent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the most immediate ancestor that is of the
//                 specified type.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Structure::FindParent(Id id) const {

	Handle<Project> project = ProjectManager::GetProject(this);

	if (project == NULL)
		return NULL;

	return FindParent(id, *project);

}

 
