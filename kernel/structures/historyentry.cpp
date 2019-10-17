//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/historyentry.h>

#include <kernel/system/time.h>
#include <kernel/system/stdio.h>
#include <kernel/system/fstream.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class HistoryEntry.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

HistoryEntry::HistoryEntry(const HistoryEntry &in) : Structure(in) {
	*this = in;
}

HistoryEntry::HistoryEntry() {
	Touch("");
}

HistoryEntry::~HistoryEntry() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(HistoryEntry, HISTORYENTRY, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent
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
HistoryEntry::Load(ifstream &stream) {

	if (!IOKit::Load(stream, timestamp_))
		return false;

	if (!IOKit::Load(stream, user_))
		return false;

	if (!IOKit::Load(stream, action_))
		return false;

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
HistoryEntry::Save(ofstream &stream) const {

	if (!IOKit::Save(stream, timestamp_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	if (!IOKit::Save(stream, user_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	if (!IOKit::Save(stream, action_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure
//===================================================================

Structure *
HistoryEntry::Duplicate() const {
  return new HistoryEntry(*this);
}

void
HistoryEntry::Clear() {
	Touch("");
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Assignment operator.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

HistoryEntry &
HistoryEntry::operator=(const HistoryEntry &in) {

	if (this == &in)
		return *this;

	timestamp_ = in.timestamp_;
	user_      = in.user_;
	action_    = in.action_;

	return *this;

}
 
