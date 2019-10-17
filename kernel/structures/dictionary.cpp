//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/dictionary.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/stringattribute.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Dictionary.
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

Dictionary::Dictionary(const Dictionary &in) : Structure(in) {

	int i;

	// Duplicate all attributes.
	for (i = 0; i < in.GetNoAttributes(); i++)
		AppendAttribute(dynamic_cast(Attribute *, in.GetAttribute(i)->Duplicate()));

}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Dictionary::Dictionary() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Dictionary::~Dictionary() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Dictionary, DICTIONARY, Structure)

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
Dictionary::Load(ifstream &stream) {

	Handle<Attribute> attribute;
	int               no_attributes;
	String            type;

	// Clear the present dictionary.
	if (!RemoveAllAttributes())
		return false;

	// Load the number of attributes.
	if (!IOKit::Load(stream, no_attributes))
		return false;

	int i;

	// Load each individual attribute.
  for (i = 0; i < no_attributes; i++) {
		if (!IOKit::Load(stream, type))
			return false;
		attribute = dynamic_cast(Attribute *, Creator::Create(IdHolder::GetId(type)));
		if (!attribute->Load(stream))
			return false;
		if (!AppendAttribute(attribute.GetPointer()))
			return false;
	}

  return true;


}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The type information is really superfluous (since
//                 attribute saves this, too).  However, saving it
//                 here also avoids having to rollback during loading.
// Revisions.....:
//===================================================================

bool
Dictionary::Save(ofstream &stream) const {

	// Save the number of attributes.
	if (!IOKit::Save(stream, GetNoAttributes()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	int i;

	// Save each individual attribute, preceeded by its type.
  for (i = 0; i < GetNoAttributes(); i++) {
		if (!IOKit::Save(stream, IdHolder::GetClassname(GetAttribute(i)->GetId())))
			return false;
		if (!IOKit::Save(stream, '\n'))
			return false;
		if (!GetAttribute(i)->Save(stream))
			return false;
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
Dictionary::Duplicate() const {
  return new Dictionary(*this);
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
Dictionary::Clear() {

	int i;

	// Clear all attributes.
	for (i = 0; i < GetNoAttributes(); i++)
		GetAttribute(i)->Clear();

}

//-------------------------------------------------------------------
// Attribute administration.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of attributes in the dictionary.
// Comments......:
// Revisions.....:
//===================================================================

int
Dictionary::GetNoAttributes() const {
	return attributes_.size();
}

//-------------------------------------------------------------------
// Method........: GetAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the specified attribute.
// Comments......:
// Revisions.....:
//===================================================================

Attribute *
Dictionary::GetAttribute(int i) const {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return NULL;
	}
#endif

	return const_cast(Attribute *, attributes_[i].GetPointer());

}


//-------------------------------------------------------------------
// Method........: InsertAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::InsertAttribute(const Attribute *attribute, int i) {

#ifdef _DEBUG
	// Index in range?  Note that an out-of-range by one at the end is allowed.
	if ((i < 0) || (i > GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return false;
	}

	// Valid attribute object?
	if (attribute == NULL) {
		Message::Error("Invalid attribute passed to dictionary.");
		return false;
	}
#endif

	// Safeguard due to possible compiler bug..?
	Handle<Attribute> handle(const_cast(Attribute *, attribute));

	// Do the insertion.
	attributes_.insert(attributes_.begin() + i, handle);

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
Dictionary::AppendAttribute(const Attribute *attribute) {
	return InsertAttribute(attribute, GetNoAttributes());
}

//-------------------------------------------------------------------
// Method........: RemoveAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::RemoveAttribute(int i) {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return false;
	}
#endif

	// Do the removal.
	attributes_.erase(attributes_.begin() + i);

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveAllAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::RemoveAllAttributes() {
	attributes_.erase(attributes_.begin(), attributes_.end());
	return true;
}

//-------------------------------------------------------------------
// Method........: SwapAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Swaps attributes i and j.
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::SwapAttributes(int i, int j) {

	if (i == j)
		return true;

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoAttributes()) || (j < 0) || (j >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return false;
	}
#endif

	Handle<Attribute> tmp;

	tmp            = attributes_[i];
	attributes_[i] = attributes_[j];
	attributes_[j] = tmp;

	return true;

}

//-------------------------------------------------------------------
// Mapping methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from real-world value to coded value.
// Comments......:
// Revisions.....:
//===================================================================

int
Dictionary::GetEntry(int attribute_no, const String &text) const {

#ifdef _DEBUG
	// Index in range?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return Undefined::Integer();
	}
#endif

	return GetAttribute(attribute_no)->GetEntry(text);

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from coded value to real-world value.
// Comments......:
// Revisions.....:
//===================================================================

const String &
Dictionary::GetEntry(int attribute_no, int value) const {

#ifdef _DEBUG
	// Index in range?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return Undefined::String();
	}
#endif

	return GetAttribute(attribute_no)->GetEntry(value);

}

//-------------------------------------------------------------------
// Method........: SuggestEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Use this method in preference to the corresponding
//                 method on the attribute level, as this method also
//                 makes some "global" considerations.
// Revisions.....:
//===================================================================

int
Dictionary::SuggestEntry(int attribute_no, const String &text) const {

#ifdef _DEBUG
	// Index in range?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return Undefined::Integer();
	}
#endif

	// Get the attribute.
	Handle<Attribute> attribute = GetAttribute(attribute_no);

	// Is the attribute of integer or float type?
	if (attribute->IsA(INTEGERATTRIBUTE) || attribute->IsA(FLOATATTRIBUTE))
		return attribute->SuggestEntry(text);

	// Is the real-world value already present?
	int coded = attribute->GetEntry(text);
	if (coded != Undefined::Integer())
		return coded;

	int i;

	// Is a similar real-world value present in any of the other string attribute maps?
	for (i = 0; i < GetNoAttributes(); i++) {
		if (i == attribute_no)
			continue;
		if (!GetAttribute(i)->IsA(STRINGATTRIBUTE))
			continue;
		coded = GetEntry(i, text);
		if (coded != Undefined::Integer())
			break;
	}

	// Is the coded entry available?
	String realworld;
	if (coded != Undefined::Integer()) {
		realworld = attribute->GetEntry(coded);
		if (realworld == Undefined::String())
			return coded;
	}

	// Come up with a default local suggestion.
	return attribute->SuggestEntry(text);

}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assigns a mapping between a real-world value and
//                 a coded value.
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::SetEntry(int attribute_no, int value, const String &text) {

#ifdef _DEBUG
	// Index in range?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes())) {
		Message::Error("Attribute index in dictionary out of range.");
		return false;
	}
#endif

	return GetAttribute(attribute_no)->SetEntry(value, text);

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
Dictionary::operator==(const Dictionary &in) const {

  // Are the two dictionaries physically equal?
  if (&in == this)
    return true;

	int i, no_attributes = GetNoAttributes();

	// Same number of attributes?
	if (in.GetNoAttributes() != no_attributes)
		return false;

	// Are all attributes equal?
	for (i = 0; i < no_attributes; i++) {
		if (*(in.GetAttribute(i)) != *(GetAttribute(i)))
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
Dictionary::operator!=(const Dictionary &in) const {
  return !(*this == in);
}

//-------------------------------------------------------------------
// Operator......: <=
// Author........: Aleksander šhrn
// Date..........:
// Description...: Determines if one dictionary "covers" or is more
//                 general than another dictionary.
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::operator<=(const Dictionary &in) const {

  // Are the two dictionaries physically equal?
  if (&in == this)
    return true;

	int i, no_attributes = GetNoAttributes();

	// Same number of attributes?
	if (in.GetNoAttributes() != no_attributes)
		return false;

	// Compare attributes.
	for (i = 0; i < no_attributes; i++) {
		if (!(*(in.GetAttribute(i)) <= *(GetAttribute(i)))) {
			String name = GetAttribute(i)->GetName();
			return false;
		}
	}

  return true;

}

//-------------------------------------------------------------------
// Method........: IsCompatible
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the two dictionaries are deemed
//                 compatible.
// Comments......:
// Revisions.....:
//===================================================================

bool
Dictionary::IsCompatible(const Dictionary &in) const {

  // Are the two dictionaries physically equal?
  if (&in == this)
    return true;

	int i, no_attributes = GetNoAttributes();

	// Same number of attributes?
	if (in.GetNoAttributes() != no_attributes)
		return false;

	// For compatibility, one attribute should "cover" the other.
	for (i = 0; i < no_attributes; i++) {
		if (!(*(in.GetAttribute(i)) <= *(GetAttribute(i))) && !(*(GetAttribute(i)) <= *(in.GetAttribute(i))))
			return false;
	}

	// The dictionaries are compatible.
  return true;

}
 
