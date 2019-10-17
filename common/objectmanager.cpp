//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....: 
//===================================================================

#include <stdafx.h> // Precompiled headers
#include <copyright.h>

#include <common/objectmanager.h>
// RA 0406
#include <common/modemanager.h>

#include <kernel/basic/message.h>
#include <kernel/basic/idholder.h>

#include <kernel/structures/structure.h>

#include <kernel/algorithms/algorithm.h>

#if defined(_ROSGUI)
#include <frontend/dialogs/algorithmdialogs/dialogalgorithm.h>
#endif

//-------------------------------------------------------------------
// Initialization of static variables.
//===================================================================

ObjectManager::StructureVector		*ObjectManager::structures_			= NULL;
ObjectManager::AlgorithmVector		*ObjectManager::algorithms_			= NULL;
#if defined(_ROSGUI)
ObjectManager::DialogVector			*ObjectManager::dialogs_			= NULL;
#endif

//-------------------------------------------------------------------
// Methods for class ObjectManager.
//===================================================================

//-------------------------------------------------------------------
// Method........: Install
// Author........: Aleksander šhrn
// Date..........:
// Description...: Installs a new algorithm object along with its
//                 associated parameter input dialog box.
// Comments......:
// Revisions.....: Robin Andersson 0406: if ROSGUI -> define the algorithm
//                 to be a Rosetta mode algorithm (default)
//===================================================================

#if defined(_ROSGUI)
bool
ObjectManager::Install(Algorithm *algorithm, CDialogAlgorithm *dialog, bool verify) {
#else
bool
ObjectManager::Install(Algorithm *algorithm, bool verify) {
#endif

	// First time around?
	if (algorithms_ == NULL)
		algorithms_ = new AlgorithmVector;

#if defined(_ROSGUI)
	if (dialogs_ == NULL)
		dialogs_ = new DialogVector;
#endif

	// Valid input?
	if (algorithm == NULL) {
		Message::Error("Cannot install a NULL algorithm.");
		return false;
	}

	// Has the algorithm already been installed?
	if (verify && IsInstalled(algorithm->GetId())) {
		Message::Warning(IdHolder::GetClassname(algorithm->GetId()) + " has already been installed.");
		return false;
	}

	// Add to prototype pool.
	algorithms_->push_back(algorithm);
#if defined(_ROSGUI)
	dialogs_->push_back(dialog);
#endif

#if defined(_ROSGUI)
	//RA 0406: Remember algorithm as standard Rosetta algorithm (default)
	ModeManager::SetRosetta(algorithm->GetId());
#endif

	return true;

}

//-------------------------------------------------------------------
// Method........: Install
// Author........: Aleksander šhrn
// Date..........:
// Description...: Installs a new structure object.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectManager::Install(Structure *structure, bool verify) {

	// First time around?
	if (structures_ == NULL)
		structures_ = new StructureVector;

	// Valid input?
	if (structure == NULL) {
		Message::Error("Cannot install a NULL structure.");
		return false;
	}

	// Has the algorithm already been installed?
	if (verify && IsInstalled(structure->GetId())) {
		Message::Warning(IdHolder::GetClassname(structure->GetId()) + " has already been installed.");
		return false;
	}

	// Add to prototype pool.
	structures_->push_back(structure);

  return true;

}

//-------------------------------------------------------------------
// Method........: Uninstall
// Author........: Aleksander šhrn
// Date..........:
// Description...: Uninstalls all algorithms and structures of the
//                 specified type.  Uninstall(IDENTIFIER) hence
//                 corresponds to a complete uninstallation,
//
//                 The handles take care of the actual deletion of
//                 the structures/algorithms.
//
//                 Explicitly deletes any associated parameter dialog
//                 boxes, if specified (true by default).
//                 This may cause multiple deletes and dangling
//                 pointers if algorithms share the same physical
//                 dialog. Redesign later to allow this.
//
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI)
bool
ObjectManager::Uninstall(Id id, bool dialog) {
#else
bool
ObjectManager::Uninstall(Id id) {
#endif

	int i;

	// Uninstall structures.
	if (structures_ != NULL) {
		for (i = GetNoStructures() - 1; i >= 0; i--) {
			if (GetEnumeratedStructure(i)->IsA(id))
				structures_->erase(structures_->begin() + i);
		}
	}

	// Uninstall algorithms.
	if (algorithms_ != NULL) {
		for (i = GetNoAlgorithms() - 1; i >= 0; i--) {
			if (GetEnumeratedAlgorithm(i)->IsA(id)) {
#if defined(_ROSGUI)
				if (dialog && HasEnumeratedDialog(i)) {
					GetEnumeratedDialog(i)->DestroyWindow();
					// delete GetEnumeratedDialog(i); // Causes an assertion in MFC...
				}
#endif
				algorithms_->erase(algorithms_->begin() + i);
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsInstalled
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified object is installed.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectManager::IsInstalled(Id id) {

	if (GetIdentifiedAlgorithm(id) != NULL)
		return true;

	if (GetIdentifiedStructure(id) != NULL)
		return true;

	return false;

}

bool
ObjectManager::IsInstalled(Identifier *identifier) {

	if (identifier == NULL)
		return false;

	return IsInstalled(identifier->GetId());

}

//-------------------------------------------------------------------
// Method........: GetNoAlgorithms
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of installed algorithm objects.
// Comments......:
// Revisions.....:
//===================================================================

int
ObjectManager::GetNoAlgorithms() {

	// Any algorithms installed at all?
	if (algorithms_ == NULL)
		return 0;

  return algorithms_->size();

}

//-------------------------------------------------------------------
// Method........: GetEnumeratedAlgorithm
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the specified installed algorithm object.
// Comments......:
// Revisions.....:
//===================================================================

Algorithm *
ObjectManager::GetEnumeratedAlgorithm(int i) {

	if (algorithms_ == NULL)
		return NULL;

#ifdef _DEBUG
  // Check range.
  if ((i < 0) || (i >= algorithms_->size())) {
		Message::Error("Algorithm index out of range");
		return NULL;
	}
#endif

	return (*algorithms_)[i].GetPointer();
}

//-------------------------------------------------------------------
// Method........: HasEnumeratedDialog
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified algorithm has an
//                 associated dialog.
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI)
bool
ObjectManager::HasEnumeratedDialog(int i) {
	return (GetEnumeratedDialog(i) != NULL);
}
#endif

//-------------------------------------------------------------------
// Method........: HasIdentifiedDialog
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified algorithm has an
//                 associated dialog.
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI)
bool
ObjectManager::HasIdentifiedDialog(int i) {
	return (GetIdentifiedDialog(i) != NULL);
}
#endif

//-------------------------------------------------------------------
// Method........: GetEnumeratedDialog
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the dialog associated with the specified
//                 algorithm object.
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI)
CDialogAlgorithm *
ObjectManager::GetEnumeratedDialog(int i) {

	if (dialogs_ == NULL)
		return NULL;

#ifdef _DEBUG
  // Check range.
  if ((i < 0) || (i >= dialogs_->size())) {
		Message::Error("Dialog index out of range");
		return NULL;
	}
#endif

  return (*dialogs_)[i];

}
#endif

//-------------------------------------------------------------------
// Method........: GetNoStructures
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of installed structure objects.
// Comments......:
// Revisions.....:
//===================================================================

int
ObjectManager::GetNoStructures() {

	if (structures_ == NULL)
		return 0;

  return structures_->size();

}

//-------------------------------------------------------------------
// Method  ......: GetEnumeratedStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the specified installed structure object.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ObjectManager::GetEnumeratedStructure(int i) {

	if (structures_ == NULL)
		return NULL;

#ifdef _DEBUG
  // Check range.
  if ((i < 0) || (i >= structures_->size())) {
		Message::Error("Structure index out of range");
		return NULL;
	}
#endif

  return (*structures_)[i].GetPointer();

}

//-------------------------------------------------------------------
// Method........: GetIdentifiedAlgorithm
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the appropriate installed algorithm that
//                 matches the given Id.  In the case of an inexact
//                 (transitive) match, the first match is returned.
// Comments......:
// Revisions.....:
//===================================================================

Algorithm *
ObjectManager::GetIdentifiedAlgorithm(Id id) {

	int i;

	// Try an exact match.
	for (i = GetNoAlgorithms() - 1; i >= 0; i--) {
		if (GetEnumeratedAlgorithm(i)->GetId() == id)
//			if (GetEnumeratedAlgorithm(i)->Clone())
				return GetEnumeratedAlgorithm(i)->Clone();
//			else
//				return GetEnumeratedAlgorithm(i);
	}

	// Try a transitive match.
	for (i = GetNoAlgorithms() - 1; i >= 0; i--) {
		if (GetEnumeratedAlgorithm(i)->IsA(id))
//			if (GetEnumeratedAlgorithm(i)->Clone())
				return GetEnumeratedAlgorithm(i)->Clone();
//			else
//				return GetEnumeratedAlgorithm(i);
	}

	return NULL;

}

//-------------------------------------------------------------------
// Method........: GetIdentifiedDialog
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the appropriate installed dialog associated
//                 with the algorithm that matches the given Id.
//                 In the case of an inexact (transitive) match, the
//                 first match is returned.
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI)
CDialogAlgorithm *
ObjectManager::GetIdentifiedDialog(Id id) {

	int i;

	// Try an exact match.
	for (i = GetNoAlgorithms() - 1; i >= 0; i--) {
		if (GetEnumeratedAlgorithm(i)->GetId() == id)
			return GetEnumeratedDialog(i);
	}

	// Try a transitive match.
	for (i = GetNoAlgorithms() - 1; i >= 0; i--) {
		if (GetEnumeratedAlgorithm(i)->IsA(id))
			return GetEnumeratedDialog(i);
	}

	return NULL;

}
#endif

//-------------------------------------------------------------------
// Method........: GetIdentifiedStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the appropriate installed structure that
//                 matches the given Id.  In the case of an inexact
//                 (transitive) match, the first match is returned.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ObjectManager::GetIdentifiedStructure(Id id) {

	int i;
	Handle<Structure> str;
	int str_id;
	bool str_check;

	// Try an exact match.
	for (i = GetNoStructures() - 1; i >= 0; i--) {
//#pragma omp critical(getenumeratedstructure)
{
		str = GetEnumeratedStructure(i);
		str_id = str->GetId();
}
		if (str_id == id)
			return GetEnumeratedStructure(i);
	}

	// Try a transitive match.
	for (i = GetNoStructures() - 1; i >= 0; i--) {
//#pragma omp critical(getenumeratedstructure)
{
		str = GetEnumeratedStructure(i);
		str_check = str->IsA(id);
}
		if (str_check)
			return GetEnumeratedStructure(i);
	}

	return NULL;

}

//-------------------------------------------------------------------
// Method........: Count
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of installed objects that are of
//                 the specified type.
// Comments......:
// Revisions.....:
//===================================================================

int
ObjectManager::Count(Id id) {

	int i, count = 0;

	for (i = GetNoStructures() - 1; i >= 0; i--) {
		if (GetEnumeratedStructure(i)->IsA(id))
			count++;
	}

	for (i = GetNoAlgorithms() - 1; i >= 0; i--) {
		if (GetEnumeratedAlgorithm(i)->IsA(id))
			count++;
	}

	return count;

}
 
