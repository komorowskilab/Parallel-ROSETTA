//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/reduct.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/basic/undefined.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Reduct.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Reduct::Reduct(const Reduct &in) : Structure(in) {
}

Reduct::Reduct() {
}

Reduct::~Reduct() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Reduct, REDUCT, Structure)

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
Reduct::Load(ifstream &stream) {

  int i, no_attributes;

	// Size.
  if (!IOKit::Load(stream, no_attributes))
		return false;

	// Indices.
  for (i = 0; i < no_attributes; i++) {
		int attribute_no;
		if (!IOKit::Load(stream, attribute_no))
			return false;
    if (!InsertAttribute(attribute_no))
			return false;
	}

	// Discernibility type.
	if (!IOKit::Load(stream, i) || !SetDiscernibilityType(static_cast(Discernibility, i)))
		return false;

	// Object.
	if (GetDiscernibilityType() == DISCERNIBILITY_OBJECT) {
		if (!IOKit::Load(stream, i) || !SetObject(i))
			return false;
	}

	// Support.
	if (!IOKit::Load(stream, i) || !SetSupport(i))
		return false;

	// Modulo.
	if (!IOKit::Load(stream, i) || !IsModuloDecision(i))
		return false;

  return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Reduct::Save(ofstream &stream) const {

	int i, no_attributes = GetNoAttributes();

	// Size.
  if (!IOKit::Save(stream, no_attributes))
		return false;

	// Indices.
  for (i = 0; i < no_attributes; i++) {
    if (!IOKit::Save(stream, GetAttribute(i)))
			return false;
	}

	// Discernibility type.
	if (!IOKit::Save(stream, GetDiscernibilityType()))
		return false;

	// Object.
	if (GetDiscernibilityType() == DISCERNIBILITY_OBJECT) {
		if (!IOKit::Save(stream, GetObject()))
			return false;
	}

	// Support.
	if (!IOKit::Save(stream, GetSupport()))
		return false;

	// Modulo.
	if (!IOKit::Save(stream, IsModuloDecision()))
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
// Author........:
// Date..........:
// Description...: Removes all attribute indices from the reduct.
//
//                 Example:  R = {3, 6, 7, 9}
//                           R.Clear();
//                           R = {}
//
// Comments......:
// Revisions.....:
//===================================================================

void
Reduct::Clear() {

	int i, no_attributes = GetNoAttributes();

	// Delete all attributes.
	for (i = 0; i < no_attributes; i++)
		RemoveAttribute(no_attributes - 1 - i);

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: FindAttribute
// Author........:
// Date..........:
// Description...: Given an attribute number, returns the position in
//                 the reduct of that attribute.  Returns an undefined
//                 integer if the specified attribute number is not
//                 present in the reduct.
//
//                 Example:  R = {3, 6, 7, 9}
//                           R.FindAttribute(8) will return Undefined::Integer()
//                           R.FindAttribute(6) will return 1.
//                           R.FindAttribute(3) will return 0.
//                           R.FindAttribute(9) will return 3.
//                           R.FindAttribute(2) will return Undefined::Integer();
//
// Comments......:
// Revisions.....:
//===================================================================

int
Reduct::FindAttribute(int attribute_no) const {

	int i, no_attributes = GetNoAttributes();

	// Scan through all attributes.
	for (i = 0; i < no_attributes; i++) {
		if (GetAttribute(i) == attribute_no)
			return i;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: IsMember
// Author........:
// Date..........:
// Description...: Returns true if the specified attribute (index) is
//                 a member of the reduct, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
Reduct::IsMember(int attribute_no) const {
  return (FindAttribute(attribute_no) != Undefined::Integer());
}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reserves space, if relevant for the physical
//                 implementation, for a reduct of the
//                 specified size.
//
// Comments......: Since this is an abstract base class, must be
//                 overloaded. Default implementation does nothing.
// Revisions.....:
//===================================================================

bool
Reduct::Reserve(int /*no_attributes*/) {
  return true;
}

//-------------------------------------------------------------------
// Formatting methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: If a decision table with an associated dictionary
//                 is supplied, this is used.
// Revisions.....:
//===================================================================

bool
Reduct::Format(String &formatted, const DecisionTable *table, bool masked) const {

	// Clear formatted string to return.
	formatted = "";

	int i, no_attributes = GetNoAttributes();

	formatted += '{';

	// Construct formatted string.
	for (i = 0; i < no_attributes; i++) {

		// Get attribute index.
		int attribute_no = GetAttribute(i);

		// Get attribute name and append it to the formatted string.
		if (table == NULL)
			formatted += String::Format(attribute_no);
		else
			formatted += table->GetAttributeName(attribute_no, masked);

		if (i < (no_attributes - 1))
			formatted += ", ";

	}

	formatted += '}';

	return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Reduct::Format(const DecisionTable *table, bool masked) const {

	String formatted;

	if (!Format(formatted, table, masked))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Method........: FormatProlog
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: If a decision table with an associated dictionary
//                 is supplied, this is used.
// Revisions.....:
//===================================================================

bool
Reduct::FormatProlog(String &formatted, const String &name, const DecisionTable *table, bool masked) const {

	// Clear formatted string to return.
	formatted = "";

	int i, no_attributes = GetNoAttributes();

	// Format as a set of Prolog facts.
	for (i = 0; i < no_attributes; i++) {

		// Get attribute index.
		int attribute_no = GetAttribute(i);

		// Get reduct name.
		String rname = name;

		// Get attribute name.
		String aname = (table == NULL) ? "a" + String::Format(attribute_no) : table->GetAttributeName(attribute_no, masked);

		// Uppercase is reserved for variables in Prolog.
		rname.ToLowercase();
		aname.ToLowercase();

		// Do we have to add quotes?
		if (rname.Contains(' ') || rname.Contains(','))
			rname = "\'" + rname + "\'";

		if (aname.Contains(' ') || aname.Contains(','))
			aname = "\'" + aname + "\'";

		// Append to formatted string.
		formatted += "reduct(" + rname + ", " + aname + ").";

		if (i < (no_attributes - 1))
			formatted += '\n';

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Reduct::FormatProlog(const String &name, const DecisionTable *table, bool masked) const {

	String formatted;

	if (!FormatProlog(formatted, name, table, masked))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: =
// Author........:
// Date..........: 960329
// Description...: Assignment operator.
// Comments......:
// Revisions.....:
//===================================================================

Reduct &
Reduct::operator=(const Reduct &in) {

  // Clear this reduct.
  Clear();

  int i, no_attributes = in.GetNoAttributes();

  // Copy all attributes that define the reduct.
  for (i = 0; i < no_attributes; i++)
    InsertAttribute(in.GetAttribute(i));

  return *this;

}

//-------------------------------------------------------------------
// Operator......: ==
// Author........:
// Date..........: 960329
// Description...: Equality operator.
// Comments......: Very inefficient implementation.
// Revisions.....:
//===================================================================

bool
Reduct::operator==(const Reduct &in) const {

  // Are the two object physically equal?
  if (&in == this)
    return true;

  int i, no_attributes = GetNoAttributes();

  // Are the number of attributes equal?
  if (no_attributes != in.GetNoAttributes())
    return false;

  // Are all attributes equal?
  for (i = 0; i < no_attributes; i++) {
    if (!IsMember(in.GetAttribute(i)))
      return false;
    if (!in.IsMember(GetAttribute(i)))
      return false;
  }

  return true;

}

//-------------------------------------------------------------------
// Operator......: !=
// Author........: Merete Hvalshagen
// Date..........: 960329
// Description...: Inequality operator.
// Comments......:
// Revisions.....:
//===================================================================

bool
Reduct::operator!=(const Reduct &in) const {
  return !(*this == in);
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
Reduct::GetString(Reduct::Discernibility discernibility) {

	switch (discernibility) {
		case DISCERNIBILITY_FULL:   return "Full";
		case DISCERNIBILITY_OBJECT: return "Object";
		default:                    return Undefined::String();
	}

}
 
