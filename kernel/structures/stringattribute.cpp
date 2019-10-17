//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/stringattribute.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/map.h>
#include <kernel/basic/pair.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class StringAttribute.
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
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//===================================================================

StringAttribute::StringAttribute(const StringAttribute &in) : Attribute(in) {

	// Duplicate maps.
	if (in.f_map_ == NULL)
		f_map_ = NULL;
	else
		f_map_ = new FMap(*(in.f_map_));

	if (in.b_map_ == NULL)
		b_map_ = NULL;
	else
		b_map_ = new BMap(*(in.b_map_));

}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//===================================================================

StringAttribute::StringAttribute() {
	f_map_ = NULL;
	b_map_ = NULL;
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//===================================================================

StringAttribute::~StringAttribute() {
	delete f_map_;
	delete b_map_;
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(StringAttribute, STRINGATTRIBUTE, Attribute)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider loading map other way.
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//===================================================================

bool
StringAttribute::Load(ifstream &stream) {

	if (!Attribute::Load(stream))
		return false;

	// Clear the current maps.
	if (f_map_ != NULL) {
		f_map_->erase(f_map_->begin(), f_map_->end());
		delete f_map_;
		f_map_ = NULL;
	}

	if (b_map_ != NULL) {
		b_map_->erase(b_map_->begin(), b_map_->end());
		delete b_map_;
		b_map_ = NULL;
	}

	int    i, size;

	String first;  // E.g., "Boat" in the dictionary entry ("Boat", 314).
	int    second; // E.g., 314 in the dictionary entry ("Boat", 314).

	// Load the map size.
	if (!IOKit::Load(stream, size))
		return false;

	// Create new maps.
	f_map_ = new FMap;
	b_map_ = new BMap;

	// Load one map and construct the inverse map automagically.
	for (i = 0; i < size; i++) {

		if (!IOKit::Load(stream, first))
			return false;

		if (!IOKit::Load(stream, second))
			return false;

		if (!(f_map_->insert(Pair(const String, int)(first, second))).second) {
			Message::Error("Failed to add pair (\"" + first + "\", " + String::Format(second) + ") to f-map.");
			return false;
		}

		if (!(b_map_->insert(Pair(const int, String)(second, first))).second) {
			Message::Error("Failed to add pair (\"" + String::Format(second) + "\", " + first + ") to b-map.");
			return false;
		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider saving map other way.
// Revisions.....:
//===================================================================

bool
StringAttribute::Save(ofstream &stream) const {

	if (!Attribute::Save(stream))
		return false;

	// Don't define this as an error.  Save an empty-sized map.
	if (f_map_ == NULL) {
		if (!IOKit::Save(stream, 0))
			return false;
		if (!IOKit::Save(stream, '\n'))
			return false;
		return true;
	}

	// Save the map's size.
#ifdef X86_64
	if (!IOKit::Save(stream,(int)f_map_->size()))
#else
	if (!IOKit::Save(stream, f_map_->size()))
#endif
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Acquire a map iterator.
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	FMap::iterator it = f_map_->begin();

	// Save all map entries.
	while (!(it == f_map_->end())) {
		if (!IOKit::Save(stream, (*it).first))
			return false;
		if (!IOKit::Save(stream, (*it).second))
			return false;
		if (!IOKit::Save(stream, '\n'))
			return false;
		it++;
	}

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
StringAttribute::Duplicate() const {
  return new StringAttribute(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//===================================================================

void
StringAttribute::Clear() {

	Attribute::Clear();

	if (f_map_ != NULL)
		f_map_->erase(f_map_->begin(), f_map_->end());

	if (b_map_ != NULL)
		b_map_->erase(b_map_->begin(), b_map_->end());

}

//-------------------------------------------------------------------
// Methods inherited from Attribute.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetType
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Attribute::Type
StringAttribute::GetType() const {
	return TYPE_STRING;
}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a real-world value, returns the internal
//                 coded value.
//
//                 Example:
//
//                   GetEntry("Boat") returns 314
//
//                 Undefined::Integer() is returned if the entry is
//                 not present in the map.
// Comments......:
// Revisions.....:
//===================================================================

int
StringAttribute::GetEntry(const String &text) const {

	if (text == Undefined::String())
		return Undefined::Integer();

	// Is the map instantiated?
	if (f_map_ == NULL)
		return Undefined::Integer();

	// Check if the real-world value exists in the map.
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	FMap::iterator it = f_map_->find(text);

	if (!(it == f_map_->end()))
		return (*it).second;

	// Unrecognized real-world value.
	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an internal coded value, returns the
//                 corresponding real-world value.
//
//                 Examples:
//
//                   GetEntry(314) returns "Boat"
//
//                 Undefined::String() is returned if the entry is
//                 not present in the inverse map.
// Comments......:
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//===================================================================

//const String &
const String 
StringAttribute::GetEntry(int value) const {

	if (value == Undefined::Integer())
		return Undefined::String();

	// Is the map instantiated?
	if (b_map_ == NULL)
		return Undefined::String();

	// Check if the real-world value exists in the inverse map.
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	BMap::iterator it = b_map_->find(value);

	if (!(it == b_map_->end()))
		return (*it).second;

	// Unrecognized coded value.
	return Undefined::String();

}

//-------------------------------------------------------------------
// Method........: SuggestEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an real-world value, returns a suggestion
//                 for an internal coded value.
//
//                 Undefined::Integer() is returned if the input is
//                 Undefined::String(), or if no suggestion can be
//                 made.
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
StringAttribute::SuggestEntry(const String &text) const {

	if (text == Undefined::String() || text.IsEmpty())
		return Undefined::Integer();

	// Is the real-world value already present?
	int coded = GetEntry(text);

	if (coded != Undefined::Integer())
		return coded;

	// Come up with a default suggestion.
	int suggestion = text.IsInteger() ? text.GetInteger() : GetNoEntries();

	// Make sure the default suggestion is valid.
	String realworld = GetEntry(suggestion);

	int i = 0;

	while ((i < 100) && (realworld != Undefined::String())) {
		i++;
		suggestion++;
		realworld = GetEntry(suggestion);
	}

	if (realworld == Undefined::String())
		return suggestion;

	// Unable to make suggestion.
	Message::Error("Unable to suggest a coded value for real-world string \"" + text + "\".");

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Modifies an existing (or adds a new) entry.  The
//                 new entry is a pair (c, v), where c is
//                 the internal coded value and v is the corresponding
//                 (textual) real-world value.  The pair (c, v) is to
//                 be read as "the internal code c actually means v in
//                 the real world".
//
//                 If the coded value is Undefined::Integer(), then this
//                 is not added to the map, as this has a special meaning.
//                 Cf. the GetEntry/SuggestEntry methods.
//
// Comments......:
// Revisions.....: Aš 0797 - Introduced inverse map for speed.
//                 Aš 0797 - Added verification so that no two
//                           real-world values map to the same
//                           coded value, and vice versa.
//===================================================================

bool
StringAttribute::SetEntry(int value, const String &text) {

	// Reserve this for having a special meaning.
	if (value == Undefined::Integer())
		return true;

	// Instantiate the maps if this is the first time around.
	if (f_map_ == NULL)
		f_map_ = new FMap;

	if (b_map_ == NULL)
		b_map_ = new BMap;

	// Check if the pair already exists in the maps. If so, remove them before proceeding.
	FMap::iterator f_iterator = f_map_->find(text);

	if (!(f_iterator == f_map_->end()))
		f_map_->erase(f_iterator);

	BMap::iterator b_iterator = b_map_->find(value);

	if (!(b_iterator == b_map_->end()))
		b_map_->erase(b_iterator);

	bool did_remove;

	// Remove old instances with same coded value, too.
	do {
		did_remove = false;
		f_iterator = f_map_->begin();
		while (!(f_iterator == f_map_->end())) {
			if ((*f_iterator).second == value) {
				f_map_->erase(f_iterator);
				did_remove = true;
				break;
			}
			f_iterator++;
		}
	} while (did_remove);

	// Remove old instances with same textual value, too.
	do {
		did_remove = false;
		b_iterator = b_map_->begin();
		while (!(b_iterator == b_map_->end())) {
			if ((*b_iterator).second == text) {
				b_map_->erase(b_iterator);
				did_remove = true;
				break;
			}
			b_iterator++;
		}
	} while (did_remove);

	// Add new entries to the maps.
	if (!(f_map_->insert(Pair(const String, int)(text, value))).second) {
		Message::Error("Failed to add pair (\"" + text + "\", " + String::Format(value) + ") to f-map.");
		return false;
	}

	if (!(b_map_->insert(Pair(const int, String)(value, text))).second) {
		Message::Error("Failed to add pair (" + String::Format(value) + ", \"" + text + "\") to b-map.");
		return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNoEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cf. comments for Attribute::GetNoEntries method.
//
//                 Example:
//
//                 Map contents:  ("Boat",   5)
//                                ("Car",    0)
//                                ("Train", 19)
//
//                 GetNoEntries() returns 3
// Comments......:
// Revisions.....:
//===================================================================

int
StringAttribute::GetNoEntries() const {

	// Is the map NULL?
	if (f_map_ == NULL)
		return 0;

	return (f_map_->size());

}

//-------------------------------------------------------------------
// Method........: GetEnumeratedEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cf. comments for Attribute::GetEnumeratedEntry method.
//
//                 Example:
//
//                 Map contents:  ("Boat",   5)
//                                ("Car",    0)
//                                ("Train", 19)
//
//                 GetEnumeratedEntry(0) returns "Boat"
//                 GetEnumeratedEntry(1) returns "Car"
//                 GetEnumeratedEntry(2) returns "Train"
//                 GetEnumeratedEntry(3) returns Undefined::String()
// Comments......:
// Revisions.....:
//===================================================================

const String &
StringAttribute::GetEnumeratedEntry(int entry_no) const {

#ifdef _DEBUG
	// Entry index in range?
	if ((entry_no < 0) || (entry_no >= GetNoEntries())) {
		Message::Error("Entry index in StringAttribute out of range.");
		return Undefined::String();
	}
#endif

	// Is the map NULL?
	if (f_map_ == NULL)
		return Undefined::String();

	// Get the entry.
	FMap::const_iterator it = f_map_->begin();

	int i;

	for (i = 0; i < entry_no; i++)
		it++;

	return (*it).first;

}

 
