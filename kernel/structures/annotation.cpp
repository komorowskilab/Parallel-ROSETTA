//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/annotation.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/delimiter.h>

#include <kernel/basic/undefined.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Constructors/destructors.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Copy constructor.
// Comments......:
// Revisions.....:
//===================================================================

Annotation::Annotation(const Annotation &in) {
  name_      = in.name_;
  filename_  = in.filename_;
	comments_  = in.comments_;
	if (in.history_.IsNULL())
		history_ = NULL;
	else
    history_ = dynamic_cast(History *, in.history_->Duplicate());
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Annotation::Annotation() {
  name_      = "No name";
	filename_  = Undefined::String();
	comments_  = "";
	history_   = new History();
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The handles should take care of everything.
// Revisions.....:
//===================================================================

Annotation::~Annotation() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Annotation, ANNOTATION, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::Load(ifstream &stream) {

	// Load the name.
	if (!IOKit::Load(stream, name_))
		return false;

	// Load any comments.
	if (!IOKit::Load(stream, comments_))
		return false;

	// Load the history log.
  if (!history_->Load(stream))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

bool
Annotation::Save(ofstream &stream) const {

	// Save the name.
	if (!IOKit::Save(stream, name_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save any comments.
	if (!IOKit::Save(stream, comments_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save the history log.
	if (!history_->Save(stream))
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
Annotation::Duplicate() const {
  return new Annotation(*this);
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
Annotation::Clear() {
	name_      = "";
	filename_  = Undefined::String();
	comments_  = "";
	history_->Clear();
}

//-------------------------------------------------------------------
// Method........: SetName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::SetName(const String &name) {

  name_ = name;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetName
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the name of the object the annotation is
//                 attached to.
// Comments......:
// Revisions.....:
//===================================================================

const String &
Annotation::GetName() const {
  return name_;
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Assignment operator
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Annotation &
Annotation::operator=(const Annotation &in) {

	if (this == &in)
		return *this;

  name_      = in.name_;
	filename_  = in.filename_;  // CHANGE set undefined?
	comments_  = in.comments_;
	history_   = dynamic_cast(History *, in.history_->Duplicate());

	return *this;

}

//-------------------------------------------------------------------
// Method........: SetFilename
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::SetFilename(const String &filename) {

  filename_ = filename;

	return true;

}


//-------------------------------------------------------------------
// Method........: GetFilename
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
Annotation::GetFilename() const {
  return filename_;
}

//-------------------------------------------------------------------
// Method........: AddComment
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::AddComment(const String &comment) {

  comments_ += comment;

	return true;

}

//-------------------------------------------------------------------
// Method........: SetComments
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::SetComments(const String &comments) {

  comments_ = comments;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetComments
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
Annotation::GetComments() const {
  return comments_;
}

//-------------------------------------------------------------------
// Method........: GetHistory
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the history log of the object the
//                 annotation is attached to.
// Comments......:
// Revisions.....:
//===================================================================

const History *
Annotation::GetHistory() const {
  return history_.GetPointer();
}

//-------------------------------------------------------------------
// Method........: ClearHistory
// Author........: Aleksander šhrn
// Date..........:
// Description...: Clears the history log.
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::ClearHistory() {

	if (history_ == NULL)
		return false;

	history_->Clear();

	return true;

}

//-------------------------------------------------------------------
// Method........: Touch
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Annotation::Touch(const String &action) {
	return history_->Touch(action);
}
 
