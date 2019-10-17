//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/objectselector.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/iokit.h>

#include <kernel/basic/set.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class ObjectSelector.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ObjectSelector::ObjectSelector(const ObjectSelector &in) {
	selection_ = in.selection_;
	attribute_ = in.attribute_;
	value_     = in.value_;
	filename_  = in.filename_;
	index_     = in.index_;
}

ObjectSelector::ObjectSelector() {
	selection_ = SELECTION_ALL;
	attribute_ = Undefined::String();
	value_     = Undefined::String();
	filename_  = Undefined::String();
	index_     = 0;
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ObjectSelector::~ObjectSelector() {
}

//-------------------------------------------------------------------
// Parameter string methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
ObjectSelector::GetParameters() const {

	String parameters;

	parameters += Keyword::Selection();
	parameters += Keyword::Assignment();
	parameters += GetString(GetSelection());

	// Object selection by attribute value.
	if (GetSelection() == SELECTION_VALUE) {

		parameters += Keyword::Separator();

		parameters += Keyword::Selection() + Keyword::Dot() + Keyword::Attribute();
		parameters += Keyword::Assignment();
		parameters += GetAttributeName();

		parameters += Keyword::Separator();

		parameters += Keyword::Selection() + Keyword::Dot() + Keyword::Value();
		parameters += Keyword::Assignment();
		parameters += GetValueName();

	}

	// Object selection from file.
	if (GetSelection() == SELECTION_FILE) {

		parameters += Keyword::Separator();

		parameters += Keyword::Selection() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetFilename();

	}

	// Object selection by index.
	if (GetSelection() == SELECTION_INDEX) {

		parameters += Keyword::Separator();

		parameters += Keyword::Selection() + Keyword::Dot() + Keyword::Index();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetIndex());

	}

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectSelector::SetParameter(const String &keyword, const String &value) {

	// Object selection.
	if (keyword == Keyword::Selection()) {
		if (value == GetString(SELECTION_ALL))
			return SetSelection(SELECTION_ALL);
		if (value == GetString(SELECTION_VALUE))
			return SetSelection(SELECTION_VALUE);
		if (value == GetString(SELECTION_FILE))
			return SetSelection(SELECTION_FILE);
		if (value == GetString(SELECTION_INDEX))
			return SetSelection(SELECTION_INDEX);
		return false;
	}

	// Selection attribute.
	if (keyword == Keyword::Selection() + Keyword::Dot() + Keyword::Attribute())
		return SetAttributeName(value);

	// Selection value.
	if (keyword == Keyword::Selection() + Keyword::Dot() + Keyword::Value())
		return SetValueName(value);

	// Selection filename.
	if (keyword == Keyword::Selection() + Keyword::Dot() + Keyword::Filename())
		return SetFilename(value);

	// Selection index.
	if (keyword == Keyword::Selection() + Keyword::Dot() + Keyword::Index() && value.IsInteger())
		return SetIndex(value.GetInteger());

	// Unknown keyword.
	return false;

}

//-------------------------------------------------------------------
// Selection methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Select
// Author........: Aleksander šhrn
// Date..........:
// Description...: Dispatch method.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectSelector::Select(Vector(int) &indices, const DecisionTable &table, bool masked) const {

	// Dispatch.
	switch (GetSelection()) {
		case SELECTION_ALL:   return SelectAll(indices, table, masked);
		case SELECTION_VALUE: return SelectByValue(indices, table, masked);
		case SELECTION_FILE:  return SelectFromFile(indices, table, masked);
		case SELECTION_INDEX: return SelectByIndex(indices, table, masked);
		default:              return false;
	}

}

//-------------------------------------------------------------------
// Method........: SelectAll
// Author........: Aleksander šhrn
// Date..........:
// Description...: Fills the vector with all indices.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectSelector::SelectAll(Vector(int) &indices, const DecisionTable &table, bool masked) const {

	// Get table dimension.
	int no_objects = table.GetNoObjects(masked);

	// Initialize object list.
	indices.erase(indices.begin(), indices.end());
	indices.reserve(no_objects);

	int i;

	// Fill object list.
	for (i = 0; i < no_objects; i++)
		indices.push_back(i);

	return true;

}

//-------------------------------------------------------------------
// Method........: SelectByValue
// Author........: Aleksander šhrn
// Date..........:
// Description...: Fills the vector with indices of those objects
//                 that have a certain (attribute, value) combination.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectSelector::SelectByValue(Vector(int) &indices, const DecisionTable &table, bool masked) const {

	// Resolve attribute representation.
	int attribute = table.GetAttributeIndex(GetAttributeName(), true, masked);

	if (attribute == Undefined::Integer())
		attribute = table.GetAttributeIndex(GetAttributeName(), false, masked);

	if (attribute == Undefined::Integer()) {
		masked = !masked;
		attribute = table.GetAttributeIndex(GetAttributeName(), true, masked);
		if (attribute == Undefined::Integer())
			attribute = table.GetAttributeIndex(GetAttributeName(), false, masked);
	}

	if (attribute == Undefined::Integer() && GetAttributeName().IsInteger())
		attribute = GetAttributeName().GetInteger();

	if (attribute == Undefined::Integer()) {
		Message::Error("Could not resolve attribute name \"" + GetAttributeName() + "\".");
		return false;
	}

	// Resolve value representation.
	int value = table.GetDictionaryEntry(attribute, GetValueName(), masked);

	// Get object indices.
	if (!table.GetObjects(indices, attribute, value, masked))
		return false;

#ifdef X86_64
	Message().Notify(String::Format((int)indices.size()) + " objects selected.");
#else
	Message().Notify(String::Format(indices.size()) + " objects selected.");
#endif

	return true;

}

//-------------------------------------------------------------------
// Method........: SelectFromFile
// Author........: Aleksander šhrn
// Date..........:
// Description...: Fills the vector with indices of those objects
//                 listed in the file.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectSelector::SelectFromFile(Vector(int) &indices, const DecisionTable &table, bool masked) const {

	// Get table dimension.
	int no_objects = table.GetNoObjects(masked);

	ifstream stream;

	if (!IOKit::Open(stream, GetFilename()))
		return false;

	// Do a convenient typedef.
	typedef STLSet(int) ISet;

	ISet   set;
	String line;

	// Process all lines in the file.
	while (IOKit::LoadLine(stream, line)) {

		// Is the line a single integer?
		if (line.IsInteger()) {
			int index = line.GetInteger();
			if (index < 0 || index >= no_objects) {
				Message::Error("Object index " + String::Format(index) + " is not compatible with the decision table.");
				return false;
			}
			set.insert(index);
		}

		// Is the line a set specification?
		else if (line.First() == '{' && line.Last() == '}') {
			line.Trim('{');
			line.Trim('}');
			while (!line.IsEmpty()) {
				int position = line.GetIndex(',');
				if (position == -1)
					break;
				String object = line.Before(position);
				object.Trim(" \t");
				if (object.IsInteger()) {
					int index = object.GetInteger();
					if (index < 0 || index >= no_objects) {
						Message::Error("Object index " + String::Format(index) + " is not compatible with the decision table.");
						return false;
					}
					set.insert(index);
				}
				line = line.After(position);
			}
		}

		// Unable to parse.
		else {
			Message::Error("Unable to parse line \"" + line + "\".");
			return false;
		}

	}

	IOKit::Close(stream);

#ifdef X86_64
	Message().Notify(String::Format((int)set.size()) + " objects selected.");
#else
	Message().Notify(String::Format(set.size()) + " objects selected.");
#endif

	// Initialize vector.
	indices.erase(indices.begin(), indices.end());
	indices.reserve(set.size());

	ISet::const_iterator iterator = set.begin();

	// Fill vector.
	while (!(iterator == set.end()))
		indices.push_back(*(iterator++));

	return true;

}

//-------------------------------------------------------------------
// Method........: SelectByIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...: Fills the vector with the specified index.
// Comments......:
// Revisions.....:
//===================================================================

bool
ObjectSelector::SelectByIndex(Vector(int) &indices, const DecisionTable &table, bool masked) const {

	int index = masked ? GetIndex() : table.GetMaskedObject(GetIndex());

	// Get table dimension.
	int no_objects = table.GetNoObjects(masked);

	// Sanity check.
	if (index < 0 || index >= no_objects)
		return false;

	indices.erase(indices.begin(), indices.end());
	indices.reserve(1);
	indices.push_back(index);

	return true;

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Method........: Assignment operator
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ObjectSelector &
ObjectSelector::operator=(const ObjectSelector &in) {

	if (this != &in) {
		selection_ = in.selection_;
		attribute_ = in.attribute_;
		value_     = in.value_;
		filename_  = in.filename_;
		index_     = in.index_;
	}

	return *this;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
ObjectSelector::GetString(Selection selection) {

	switch (selection) {
		case SELECTION_ALL:     return "All";
		case SELECTION_VALUE:   return "Value";
		case SELECTION_FILE:    return "File";
		case SELECTION_INDEX:   return "Index";
		default:                return Undefined::String();
	}

}
 
