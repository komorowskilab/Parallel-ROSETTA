//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/idholder.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Implementation of class IdHolder
//===================================================================

//-------------------------------------------------------------------
// Initialization of static class variables.
//===================================================================

Vector(IdHolder::Description) *IdHolder::descriptions_ = NULL;

//-------------------------------------------------------------------
// Method........: Register
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...: Registers (and returns) a new identifier along
//                 with descriptions of the identifier.
// Comments......: Should never be called directly, but via the
//                 ImplementId macro.
//
//                 The class name is self-explanatory.
//
//                 The textual description might be used for
//                 purposes such as e.g. creation of dynamic menus.
//
//                 Assumes that the Id type is really an integer.
//
//                 A warning is given if parts of the tuple has been
//                 assigned a previously registred identifier.  This since
//                 the textual description is typically used in UI menus, and
//                 the objects should thus be textually discernible.
// Revisions.....:
//===================================================================

Id
IdHolder::Register(const String &classname, const String &description) {

	Description newdescription;

	int i;

	// Does another identifier have the same entries?
	if (descriptions_ != NULL) {
		for (i = 0; i < descriptions_->size(); i++) {
			if ((*descriptions_)[i].classname_ == classname)
				Message::FatalError("The classname (" + classname + ") is not unique.");
			if ((*descriptions_)[i].description_ == description)
				Message::Debug("The description (" + description + ") is not unique.");
		}
	}

	// Create the static container if this is the first time around.
	if (descriptions_ == NULL) {
		descriptions_ = new Vector(Description);
		descriptions_->push_back(newdescription);
	}

	// Fill the description structure.
	newdescription.classname_   = classname;
	newdescription.description_ = description;

	// Insert the description structure into the static container.
	descriptions_->push_back(newdescription);

	int id = descriptions_->size() - 1;

  return Id(id);

}

//-------------------------------------------------------------------
// Method........: Unregister
// Author........: Aleksander šhrn
// Date..........:
// Description...: Unregisters all identifiers.
// Comments......:
// Revisions.....:
//===================================================================

bool
IdHolder::Unregister() {

	// No identifiers registered?
	if (descriptions_ == NULL)
		return true;

	// Reset everything.
	delete descriptions_;
	descriptions_ = NULL;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetClassname
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...: Returns the classname associated with the given
//                 identifier.
// Comments......: Assumes that an Id is really an integer.
// Revisions.....:
//===================================================================

const String &
IdHolder::GetClassname(Id id) {

	// Prepare a default response.
  static String undefined(Undefined::String());

	// Handle special cases.
	if (id == Undefined::Id())
		return undefined;

	if (descriptions_ == NULL)
		return undefined;

	int no_identifiers = descriptions_->size();

	// Check input identifier validity.
  if ((int(id) < 0) || (int(id) >= no_identifiers)) {
    Message::Debug("Invalid identifier passed to IdHolder::GetClassname.");
    return undefined;
  }

	// Return the requested description.
  return (*descriptions_)[int(id)].classname_;

}

//-------------------------------------------------------------------
// Method........: GetDescription
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...: Returns the brief textual description of the given
//                 identifier.
// Comments......: Assumes that an Id is really an integer.
// Revisions.....:
//===================================================================

const String &
IdHolder::GetDescription(Id id) {

	// Prepare a default response.
  static String undefined(Undefined::String());

	// Handle special cases.
	if (id == Undefined::Id())
		return undefined;

	if (descriptions_ == NULL)
		return undefined;

	int no_identifiers = descriptions_->size();

	// Check input identifier validity.
  if ((int(id) < 0) || (int(id) >= no_identifiers)) {
    Message::Debug("Invalid identifier passed to IdHolder::GetDescription.");
    return undefined;
  }

	// Return the requested description.
  return (*descriptions_)[int(id)].description_;

}

//-------------------------------------------------------------------
// Method........: GetId
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns an identifier that might match the given string.
//                 Matching takes place on (in the given order):
//
//                 1) Class names.
//                 2) Textual descriptions.
//
//                 If multiple matches exist, the first one found is
//                 returned.  Returns an undefined Id if no match is found.
// Comments......: Assumes that the Id type is really an integer.
// Revisions.....: Aš 971010 : All string matching in lowercase.
//===================================================================

Id
IdHolder::GetId(const String &text) {

	// Handle special cases.
	if (descriptions_ == NULL)
		return Undefined::Id();

	if (text == Undefined::String())
		return Undefined::Id();

	int no_identifiers = descriptions_->size();

	// Convert string to lowercase for case-insensitive matching.
	String string_lc(text);
	string_lc.ToLowercase();

	int i;

	// Classname match?
	for (i = 0; i < no_identifiers; i++) {
		String classname_lc = (*descriptions_)[i].classname_;
		classname_lc.ToLowercase();
		if (classname_lc == string_lc)
			return Id(i);
	}

	// Description match?
	for (i = 0; i < no_identifiers; i++) {
		String description_lc = (*descriptions_)[i].description_;
		description_lc.ToLowercase();
		if (description_lc == string_lc)
			return Id(i);
	}

	// No match found.
	return Undefined::Id();

} 
