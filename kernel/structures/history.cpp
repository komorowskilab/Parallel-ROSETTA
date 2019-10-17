//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/history.h>
#include <kernel/structures/historyentry.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/delimiter.h>

#include <kernel/basic/macros.h>
#include <kernel/basic/handle.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class History.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

History::History(const History &in) : Structure(in) {
	*this = in;
}

History::History() {
}

History::~History() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(History, HISTORY, Structure)

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
History::Load(ifstream &stream) {

	int i, no_entries;

  HistoryEntry entry;

	// Load the number of history items.
  if (!IOKit::Load(stream, no_entries))
		return false;

	// Initialize entry vector.
	entries_.erase(entries_.begin(), entries_.end());
	entries_.reserve(no_entries);

	// Load each individual history item.
  for (i = 0; i < no_entries; i++) {
		if (!entry.Load(stream))
			return false;
    entries_.push_back(dynamic_cast(HistoryEntry *, entry.Duplicate()));
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
History::Save(ofstream &stream) const {

	// Save the number of history items.
	if (!IOKit::Save(stream, GetNoEntries()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	int i;

	// Save each individual history item.
  for (i = 0; i < GetNoEntries(); i++) {
		if (!GetEntry(i)->Save(stream))
			return false;
	}

  return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
History::Duplicate() const {
  return new History(*this);
}

void
History::Clear() {
  entries_.erase(entries_.begin(), entries_.end());
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

History &
History::operator=(const History &in) {

	if (this == &in)
		return *this;

	// Remove all present entries.
	Clear();

	entries_.reserve(in.GetNoEntries());

	int i;

  // Copy all entries.
  for (i = 0; i < in.GetNoEntries(); i++)
    entries_.push_back(dynamic_cast(HistoryEntry *, in.GetEntry(i)->Duplicate()));

	return *this;

}

//-------------------------------------------------------------------
// Method........: GetNoEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of entries in the history log.
// Comments......:
// Revisions.....:
//===================================================================

int
History::GetNoEntries() const {
  return entries_.size();
}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the given entry in the history log.
// Comments......:
// Revisions.....:
//===================================================================

const HistoryEntry *
History::GetEntry(int i) const {

  if ((i < 0) || (i >= GetNoEntries())) {
		static HistoryEntry undefined;
		Message::Error("History entry index out of bounds.");
		return &undefined;
	}

  return entries_[i].GetPointer();

}

//-------------------------------------------------------------------
// Method........: Touch
// Author........: Aleksander šhrn
// Date..........:
// Description...: Appends a new entry to the history log.
// Comments......:
// Revisions.....:
//===================================================================

bool
History::Touch(const String &action) {

  HistoryEntry entry;

	if (!entry.Touch(action))
		return false;

  entries_.push_back(dynamic_cast(HistoryEntry *, entry.Duplicate()));

	return true;

}
 
