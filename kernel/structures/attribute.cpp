//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/attribute.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/interval.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Attribute.
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
// Revisions.....:
//===================================================================

Attribute::Attribute(const Attribute &in) : Structure(in) {
	name_ = in.name_;
	unit_ = in.unit_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Attribute::Attribute() {
	name_ = Undefined::String();
	unit_ = Undefined::String();
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Attribute::~Attribute() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Attribute, ATTRIBUTE, Structure)

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
Attribute::Load(ifstream &stream) {

	if (!IOKit::Load(stream, name_))
		return false;

	if (!IOKit::Load(stream, unit_))
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
Attribute::Save(ofstream &stream) const {

	if (!IOKit::Save(stream, GetName()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	if (!IOKit::Save(stream, GetUnit()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
Attribute::Clear() {
	SetName(Undefined::String());
	SetUnit(Undefined::String());
}

//-------------------------------------------------------------------
// Attribute field management methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
Attribute::GetName() const {
	return name_;
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
Attribute::SetName(const String &name) {
	name_ = name;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetUnit
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
Attribute::GetUnit() const {
	return unit_;
}

//-------------------------------------------------------------------
// Method........: SetUnit
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::SetUnit(const String &unit) {
	unit_ = unit;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetScalingExponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overloaded by attributes for which this is
//                 relevant.
// Revisions.....:
//===================================================================

int
Attribute::GetScalingExponent() const {
	return 0;
}

//-------------------------------------------------------------------
// Method........: SetScalingExponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overloaded by attributes for which this is
//                 relevant.
// Revisions.....:
//===================================================================

bool
Attribute::SetScalingExponent(int /*exponent*/) {
	return false;
}

//-------------------------------------------------------------------
// Method........: IsNumeric
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::IsNumeric() const {
	return !IsString();
}

//-------------------------------------------------------------------
// Method........: IsSymbolic
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::IsSymbolic() const {
	return !IsNumeric();
}

//-------------------------------------------------------------------
// Method........: IsInteger
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::IsInteger() const {
	return (GetType() == TYPE_INTEGER);
}

//-------------------------------------------------------------------
// Method........: IsFloat
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::IsFloat() const {
	return (GetType() == TYPE_FLOAT);
}

//-------------------------------------------------------------------
// Method........: IsString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::IsString() const {
	return (GetType() == TYPE_STRING);
}

//-------------------------------------------------------------------
// Method........: IsInterval
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::IsInterval() const {

	int i, no_entries = GetNoEntries();

	// Check if all entries are valid intervals.
	for (i = 0; i < no_entries; i++) {
		if (!Interval(GetEnumeratedEntry(i)).IsValid())
			return false;
	}

	return (no_entries > 0);

}


//-------------------------------------------------------------------
// Method........: GetNoEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of different entries, i.e. the
//                 cardinality of the set of possible values for the
//                 attribute.
//
//                 Default implementation.  Typically overloaded for
//                 non-numeric attributes.  For numeric types, this
//                 is really infinity, but 0 is returned as default
//                 for safety reasons. (This method is typically used
//                 as a loop counter.)
// Comments......:
// Revisions.....:
//===================================================================

int
Attribute::GetNoEntries() const {
	return 0;
}

//-------------------------------------------------------------------
// Method........: GetEnumeratedEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the specified distinct entry, i.e. the
//                 specified member (index) of the set of possible
//                 values for the attribute.
//
//                 Default implementation.  Typically overloaded for
//                 non-numeric attributes.
// Comments......:
// Revisions.....:
//===================================================================

const String &
Attribute::GetEnumeratedEntry(int /*entry_no*/) const {
	return Undefined::String();
}

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Attribute::GetString(Attribute::Type type) {

	switch (type) {
		case TYPE_INTEGER: return "Integer";
		case TYPE_FLOAT:   return "Float";
		case TYPE_STRING:  return "String";
		default:           return Undefined::String();
	}

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: ==
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::operator==(const Attribute &in) const {

  // Are the two attributes physically equal?
  if (&in == this)
    return true;

	// Same domain type?
	if (!in.IsA(GetId()))
		return false;

	int no_entries = GetNoEntries();

	// Same domain size?
	if (in.GetNoEntries() != no_entries)
		return false;

	int i;

	// Same domain?
	for (i = 0; i < no_entries; i++) {
		if (in.GetEnumeratedEntry(i) != GetEnumeratedEntry(i))
			return false;
	}

  return true;

}

//-------------------------------------------------------------------
// Operator......: !=
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::operator!=(const Attribute &in) const {
  return !(*this == in);
}

//-------------------------------------------------------------------
// Operator......: <=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Determines if one attribute "covers" or is more
//                 general than another attribute.
// Comments......:
// Revisions.....:
//===================================================================

bool
Attribute::operator<=(const Attribute &in) const {

  // Are the two attributes physically equal?
  if (&in == this)
    return true;

	// Same domain type?
	if (!in.IsA(GetId()))
		return false;

	int i, j, no_entries = GetNoEntries();

	// Every element of this domain...
	for (i = 0; i < no_entries; i++) {

		String entry = GetEnumeratedEntry(i);

		bool present = false;

    // ...must be present in the other domain.
		for (j = 0; j < in.GetNoEntries(); j++) {
			present = (in.GetEnumeratedEntry(j) == entry);
			if (present)
				break;
		}

		if (!present)
			return false;

	}

  return true;

}
 
