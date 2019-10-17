//-------------------------------------------------------------------
// Author........: Daniel Remmem
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/project.h>
#include <kernel/structures/projectmanager.h>

#include <kernel/basic/undefined.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/iokit.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Methods for class Project.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method  ......: Empty constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider adding ProjectManager::InsertProject(this).
// Revisions.....:
//===================================================================

Project::Project() {
}

//-------------------------------------------------------------------
// Method  ......: Copy constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider adding ProjectManager::InsertProject(this).
// Revisions.....:
//===================================================================

Project::Project(const Project &in) : ParentStructure(in) {
}

//-------------------------------------------------------------------
// Method  ......: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider adding ProjectManager::RemoveProject(this).
// Revisions.....:
//===================================================================

Project::~Project() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Project, PROJECT, ParentStructure)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method  ......: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider deep copy instead of shallow copy.
// Revisions.....:
//===================================================================

Structure *
Project::Duplicate() const {
	return new Project(*this);
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method  ......: GetAllIdentifiers
// Author........: Aleksander šhrn/Daniel Remmem
// Date..........:
// Description...: Returns (in-place) handles to all objects contained
//                 in the project that are of the specified type.
// Comments......:
// Revisions.....:
//===================================================================

bool
Project::GetAllIdentifiers(Id id, Identifier::Handles &identifiers) const {

	// Clear the input vector.
	identifiers.erase(identifiers.begin(), identifiers.end());

	int i;

	// Search through the installed algorithms first.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (algorithm->IsA(id))
			identifiers.push_back(Handle<Identifier>(algorithm.GetPointer()));
	}

	bool recursive = true;

	// Get all structural objects recursively.
	if (!GetAllChildren(id, identifiers, recursive))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method  ......: GetAllIdentifiers
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) handles to all identifiers
//                 contained in the project that are of the specified
//                 type and have the specified name.
// Comments......:
// Revisions.....:
//===================================================================

bool
Project::GetAllIdentifiers(Id id, const String &name, Identifier::Handles &identifiers) const {

	int i;

  // Get all identifiers of the specified type in the project.
	if (!GetAllIdentifiers(id, identifiers))
		return false;

	String current;

	// Remove those whose name does not match.
	for (i = identifiers.size() - 1; i >= 0; i--) {

		// Get name.
		if (identifiers[i]->IsA(STRUCTURE))
			current = dynamic_cast(Structure *, identifiers[i].GetPointer())->GetName();
		else
			current = IdHolder::GetClassname(identifiers[i]->GetId());

		// Erase?
		if (current != name)
			identifiers.erase(identifiers.begin() + i);

	}

	return true;

}

//-------------------------------------------------------------------
// Method  ......: Count
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Project::Count(Id id) const {

	Identifier::Handles identifiers;

	if (!GetAllIdentifiers(id, identifiers))
		return 0;

	return identifiers.size();

}

//-------------------------------------------------------------------
// Method  ......: Count
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Project::Count(Id id, const String &name) const {

	Identifier::Handles identifiers;

	if (!GetAllIdentifiers(id, name, identifiers))
		return 0;

	return identifiers.size();

}
 
