//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/informationvector.h>
#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Methods for class InformationVector.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

InformationVector::InformationVector(const InformationVector &in) : Structure(in) {
}

InformationVector::InformationVector() {
}

InformationVector::~InformationVector() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(InformationVector, INFORMATIONVECTOR, Structure)

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
InformationVector::Load(ifstream &/*stream*/) {

	Message::Error("Loading of a single inf. vector not implemented yet.");

	return false;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

bool
InformationVector::Save(ofstream &/*stream*/) const {

	Message::Error("Saving of a single inf. vector not implemented yet.");

	return false;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

void
InformationVector::Clear() {

	int i;

	for (i = 0; i < GetNoAttributes(); i++)
		SetEntry(i, Undefined::Integer());

}

//-------------------------------------------------------------------
// Attribute administration.
//===================================================================

//-------------------------------------------------------------------
// Method........: SwapAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
InformationVector::SwapAttributes(int i, int j) {

	if (i == j)
		return true;

	int tmp = GetEntry(i);
	if (!SetEntry(i, GetEntry(j)))
		return false;
	if (!SetEntry(j, tmp))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reserves space, if relevant for the physical
//                 implementation, for an inf. vector of the
//                 specified size.
//
// Comments......: Since this is an abstract base class, must be
//                 overloaded. Default implementation does nothing.
// Revisions.....:
//===================================================================

bool
InformationVector::Reserve(int /*no_attributes*/) {
	return true;
}

//-------------------------------------------------------------------
// Method........: Resize
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
InformationVector::Resize(int no_attributes) {

	if (!Reserve(no_attributes))
		return false;

	if (GetNoAttributes() > no_attributes) {
		while (GetNoAttributes() > no_attributes) {
			if (!RemoveAttribute(GetNoAttributes() - 1))
				return false;
		}
	}
	else {
		while (GetNoAttributes() < no_attributes) {
			if (!AppendAttribute())
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AppendAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
InformationVector::AppendAttribute() {
	return InsertAttribute(GetNoAttributes());
}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: =
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assignment operator.
// Comments......:
// Revisions.....:
//===================================================================

InformationVector &
InformationVector::operator=(const InformationVector &in) {

	// Protect against self-assignment.
	if (this == &in)
		return *this;

	// Does this inf. vector have too many or too few attributes?
  Resize(in.GetNoAttributes());

	int j;

  // Set all entries.
  for (j = 0; j < in.GetNoAttributes(); j++) {
		SetEntry(j, in.GetEntry(j));
	}

  return *this;

}

//-------------------------------------------------------------------
// Operator......: ==
// Author........: Aleksander šhrn
// Date..........:
// Description...: Equality operator.
// Comments......:
// Revisions.....:
//===================================================================

bool
InformationVector::operator==(const InformationVector &in) const {

  // Are the two objects physically equal?
  if (&in == this)
    return true;

	int j, no_attributes = in.GetNoAttributes();

	// Do the objects have the same dimensions?
  if (GetNoAttributes() != no_attributes)
    return false;

	// Are all entries equal?
  for (j = 0; j < no_attributes; j++) {
		if (GetEntry(j) != in.GetEntry(j))
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
InformationVector::operator!=(const InformationVector &in) const {
  return !(*this == in);
}

//-------------------------------------------------------------------
// Operator......: <
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this inf. vector is "less than"
//                 the input inf. vector.  "Less than" in this respect
//                 means "more general", i.e. a subset wrt both
//                 attributes *and* values.
//
//                 Examples:
//
//                   Inf1(x) = {(a(x), 0), (b(x), 5), (c(x), 2)}
//                   Inf2(x) = {(a(x), 0), (c(x), 2)}
//                   Inf3(x) = {(a(x), 3), (c(x), 2)}
//                   Inf4(x) = {(b(x), 5), (c(x), 2)}
//                   Inf5(x) = {}
//
//                   Inf2(x) < Inf1(x)    ): Inf2(x) is more general than Inf1(x).
//                   Inf3(x) !< Inf1(x)   ): Inf3(x) is not more general than Inf1(x) since they differ wrt a(x).
//                   Inf4(x) < Inf1(x)    ): Inf4(x) is more general than Inf1(x).
//
//                   Inf5(x) is the most general of all inf. vectors.
//
//                   Inf2(x) !< Inf4(x)   ): Inf2(x) is not more general than Inf4(x) since the attribute sets only partially overlap.
//                   Inf4(x) !> Inf2(x)   ): Inf4(x) is not more general than Inf2(x) since the attribute sets only partially overlap.
//                   Inf2(x) != Inf4(x)   ): Inf2(x) is not equal to Inf4(x).
//
// Comments......: Assumes that the vectors are of equal (full) length,
//                 but that for "missing" attributes, the attribute value
//                 is set to an undefined value.
//
//                 Note that with the above definition, !(a < b) and !(a == b) does not necessarily imply
//                 that (a > b).  Therefore, using the < operator for e.g. sorting is a bit dubious.
// Revisions.....:
//===================================================================

bool
InformationVector::operator<(const InformationVector &in) const {

  // Are the two objects physically equal?
  if (&in == this)
    return false;

	int j, no_attributes = in.GetNoAttributes();

	// Do the objects have the same dimensions?  If not, we cannot know whether or not
	// we are comparing the same attributes.
  if (GetNoAttributes() != no_attributes)
    return false;

	bool equal = true;

	// Examine and compare entries for each attribute.
  for (j = 0; j < no_attributes; j++) {
		int entry1 = GetEntry(j);
		int entry2 = in.GetEntry(j);
		if (entry1 != entry2)
			equal = false;
		if (entry1 == Undefined::Integer()) // Case 1: Entry 1 (this) missing, entry 2 (other) can be anything.
			continue;
		if (entry2 == Undefined::Integer()) // Case 2: Entry 1 (this) is present, entry 2 (other) is missing.
			return false;
		if (entry1 != entry2)               // Case 3: Entry 1 (this) and entry 2 (other) are both present, their values must then be equal.
			return false;
	}

	// No information vector is more defined to be more general than itself.
	if (equal)
		return false;

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
InformationVector::Format(String &formatted, const String &name, const DecisionTable *table, bool masked) const {

	// Clear formatted string to return.
	formatted = "";

#ifdef _DEBUG
	// Check dimensions for safety.
	if (table != NULL) {
		if (GetNoAttributes() != table->GetNoAttributes(masked)) {
			Message::Error("Inconsistent dimensions detected.");
			return false;
		}
	}
#endif

	int i, no_attributes = GetNoAttributes();

	formatted += "Inf(" + name + ") = {";

	// List as attribute-value pairs.
	for (i = 0; i < no_attributes; i++) {

		// Get inf. vector entry.
		int entry = GetEntry(i);

		// Only consider non-missing entries.
		if (entry == Undefined::Integer())
			continue;

		// Get attribute name.
		String aname = (table == NULL) ? "a" + String::Format(i) : table->GetAttributeName(i, masked);

		// Get entry name.
		String ename = (table == NULL) ? String::Format(entry) : table->GetDictionaryEntry(i, entry, masked);

		// Append to formatted string.
		formatted += "(" + aname + ", " + ename + ")";

		if (i < no_attributes - 1)
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
InformationVector::Format(const String &name, const DecisionTable *table, bool masked) const {

	String formatted;

	if (!Format(formatted, name, table, masked))
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
InformationVector::FormatProlog(String &formatted, const String &name, const DecisionTable *table, bool masked) const {

	// Clear formatted string to return.
	formatted = "";

#ifdef _DEBUG
	// Check dimensions for safety.
	if (table != NULL) {
		if (GetNoAttributes() != table->GetNoAttributes(masked)) {
			Message::Error("Inconsistent dimensions detected.");
			return false;
		}
	}
#endif

	int i, no_attributes = GetNoAttributes();

	// Each entry constitutes a Prolog fact.
	for (i = 0; i < no_attributes; i++) {

		// Get inf. vector entry.
		int entry = GetEntry(i);

		// Only consider non-missing entries.
		if (entry == Undefined::Integer())
			continue;

		// Get attribute name.
		String aname = (table == NULL) ? "a" + String::Format(i) : table->GetAttributeName(i, masked);

		// Get object name.
		String oname = name;

		// Get entry name.
		String ename = (table == NULL) ? String::Format(entry) : table->GetDictionaryEntry(i, entry, masked);

		// Uppercase is reserved for variables in Prolog.
		aname.ToLowercase();
		oname.ToLowercase();
		ename.ToLowercase();

		// Do we have to add quotes?
		if (aname.Contains(' ') || aname.Contains(','))
			aname = "\'" + aname + "\'";

		if (oname.Contains(' ') || oname.Contains(','))
			oname = "\'" + oname + "\'";

		if (ename.Contains(' ') || ename.Contains(','))
			ename = "\'" + ename + "\'";

		// Append to formatted string.
		formatted += aname + "(" + oname + ", " + ename + ").";

		if (i < no_attributes - 1)
			formatted += '\n';

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: FormatProlog
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
InformationVector::FormatProlog(const String &name, const DecisionTable *table, bool masked) const {

	String formatted;

	if (!FormatProlog(formatted, name, table, masked))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Creation methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
InformationVector::Create(const DecisionTable &table, int object_no, bool masked) {

#ifdef _DEBUG
	// Valid index argument?
	if ((object_no < 0) || (object_no >= table.GetNoObjects(masked))) {
		Message::Error("Object index out of range.");
		return false;
	}
#endif

	int i, no_attributes = table.GetNoAttributes(masked);

	// Resize it.
	if (!Resize(no_attributes))
		return false;

	// Fill it.
	for (i = 0; i < no_attributes; i++) {
		if (!SetEntry(i, table.GetEntry(object_no, i, masked)))
			return false;
	}

	return true;

}
 
