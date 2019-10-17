//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/structures.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/indiscernibilitygraph.h>
#include <kernel/structures/generalizeddecision.h>
#include <kernel/structures/rules.h>

#include <kernel/utilities/discerner.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/types.h>
#include <kernel/basic/message.h>

#include "omp.h"

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticRescaleEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticRescaleEntries(Vector(float) &entries, const DecisionTable &table, int attribute_no, bool masked) {

	if (!table.HasDictionary() || !table.IsFloat(attribute_no, masked))
		return false;

	// Get unmasked attribute index.
	int unmasked = masked ? table.GetUnmaskedAttribute(attribute_no) : attribute_no;

	// Get dictionary attribute.
	Handle<Attribute> attribute = table.GetDictionary()->GetAttribute(unmasked);

	if (attribute == NULL)
		return false;

	// Get scaling exponent.
	int exponent = attribute->GetScalingExponent();

	int i;

	// Do the conversion, if needed.
	if (exponent != 0) {
		float factor = MathKit::Power(10.0, -exponent);
		for (i = 0; i < entries.size(); i++)
			entries[i] *= factor;
	}

	return true;

}

//-------------------------------------------------------------------
// Methods for class DecisionTable.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable::DecisionTable(const DecisionTable &in) : ParentStructure(in) {

	// Duplicate dictionary.
	if (in.dictionary_ == NULL)
		dictionary_ = NULL;
	else
		dictionary_ = dynamic_cast(Dictionary *, in.dictionary_->Duplicate());

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: No dictionary as default.
// Revisions.....:
//===================================================================

DecisionTable::DecisionTable() {
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable::~DecisionTable() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DecisionTable, DECISIONTABLE, ParentStructure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Cf. Save method comments.
// Revisions.....:
//===================================================================

bool
DecisionTable::Load(ifstream &stream) {

	Message message;

	// Load header and type identifier only, load the rest of the stuff higher up later.
	if (!Persistent::Load(stream))
		return false;

	bool masked = false;

	int no_objects;
	int no_attributes;

	// Load table dimensions.
	if (!IOKit::Load(stream, no_objects) || !IOKit::Load(stream, no_attributes))
		return false;

	message.Notify("Table has " + String::Format(no_objects) + " objects and " + String::Format(no_attributes) + " attributes.");

	Handle<Dictionary> dictionary = GetDictionary();

	// The resizing method may indirectly alter the dictionary, which may already have been read OK.
	SetDictionary(NULL);

	// Adjust current dimensions.
	if (!Resize(no_objects, no_attributes, masked))
		return false;

	SetDictionary(dictionary.GetPointer());

	int i, j;

	// Load table entries.
	for (i = 0; i < no_objects; i++) {
		for (j = 0; j < no_attributes; j++) {
			int value;
			if (!IOKit::Load(stream, value) || !SetEntry(i, j, value, masked))
				return false;
		}
	}

	// Load all attribute status fields.
	for (i = 0; i < no_attributes; i++) {
		int status;
		if (!IOKit::Load(stream, status))
			return false;
		if (!SetStatus(i, static_cast(DecisionTable::Status, status), masked))
			return false;
	}

	// Load all attribute masks.
	for (i = 0; i < no_attributes; i++) {
		int mask;
		if (!IOKit::Load(stream, mask))
			return false;
		if (!SetAttributeMask(i, static_cast(DecisionTable::Mask, mask)))
			return false;
	}

	// Load all object masks.
	for (i = 0; i < no_objects; i++) {
		int mask;
		if (!IOKit::Load(stream, mask))
			return false;
		if (!SetObjectMask(i, static_cast(DecisionTable::Mask, mask)))
			return false;
	}

	int has_dictionary;

	// Is there a dictionary associated with the dec. table?
	if (!IOKit::Load(stream, has_dictionary))
		return false;

	// Create a dictionary if needed.  Not having a dictionary is OK.
	if (has_dictionary)
		SetDictionary(Creator::Dictionary());
	else
		SetDictionary(NULL);

	// Load dictionary.
	if (HasDictionary() && !GetDictionary()->Load(stream))
		return false;

	// Load stuff higher up in the hierarchy.
	if (!ParentStructure::Load(stream))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Table data has to be loaded before the children,
//                 since loading table data alters the table, and table
//                 altering is only allowed if there are no children.
// Revisions.....:
//===================================================================

bool
DecisionTable::Save(ofstream &stream) const {

	Message message;

	// Save header and type identifier only, save the rest of the stuff higher up later.
	if (!Persistent::Save(stream))
		return false;

	bool masked = false;

	// Get table dimensions.
	int no_objects    = GetNoObjects(masked);
	int no_attributes = GetNoAttributes(masked);

	message.Notify("Table has " + String::Format(no_objects) + " objects and " + String::Format(no_attributes) + " attributes.");

	// Save table dimensions.
	if (!IOKit::Save(stream, no_objects))
		return false;

	if (!IOKit::Save(stream, no_attributes))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	int i, j;

	// Save table entries.
	for (i = 0; i < no_objects; i++) {
		for (j = 0; j < no_attributes; j++) {
			if (!IOKit::Save(stream, GetEntry(i, j, masked)))
				return false;
		}
		if (!IOKit::Save(stream, '\n'))
			return false;
	}

	// Save all attribute status fields.
	for (i = 0; i < no_attributes; i++) {
		if (!IOKit::Save(stream, GetStatus(i, masked)))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save all attribute masks.
	for (i = 0; i < no_attributes; i++) {
		if (!IOKit::Save(stream, GetAttributeMask(i)))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save all object masks.
	for (i = 0; i < no_objects; i++) {
		if (!IOKit::Save(stream, GetObjectMask(i)))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Does the decision table have an associated dictionary?
	if (!IOKit::Save(stream, static_cast(int, HasDictionary())))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Not having a dictionary is OK.
	if (HasDictionary() && !GetDictionary()->Save(stream))
		return false;

	// Save stuff higher up in the hierarchy.  Children can be safely loaded now
	// that the decision table has been loaded.
	if (!ParentStructure::Save(stream))
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
DecisionTable::Clear() {

	// Do stuff higher up.
	ParentStructure::Clear();

	// Resize table.
	Resize(0, 0, false);

}

//-------------------------------------------------------------------
// Dimensionality methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reserves space, if relevant for the physical
//                 implementation, for a decision table of the
//                 specified size.
//
// Comments......: Since this is an abstract base class, must be
//                 overloaded. Default implementation does nothing.
// Revisions.....:
//===================================================================

bool
DecisionTable::Reserve(int /*no_objects*/, int /*no_attributes*/) {
	return true;
}

//-------------------------------------------------------------------
// Method........: Resize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Resizes the current decision table.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::Resize(int no_objects, int no_attributes, bool masked) {

	int i;

	// Negative dimensions are illegal.
	if ((no_objects < 0) || (no_attributes < 0))
		return false;

	if (!Reserve(no_objects, no_attributes))
		return false;

	// Get the current dimensions.
	int current_no_objects    = GetNoObjects(masked);
	int current_no_attributes = GetNoAttributes(masked);

	// Adjust the number of objects if needed.
  if (current_no_objects > no_objects) {
    for (i = 0; i < (current_no_objects - no_objects); i++) {
      if (!RemoveObject(current_no_objects - 1 - i, masked))
				return false;
		}
	}
  else if (current_no_objects < no_objects) {
    for (i = 0; i < (no_objects - current_no_objects); i++) {
      if (!AppendObject(masked))
				return false;
		}
	}

	// Adjust the number of attributes if needed.
  if (current_no_attributes > no_attributes) {
    for (i = 0; i < (current_no_attributes - no_attributes); i++) {
      if (!RemoveAttribute(current_no_attributes - 1 - i, masked))
				return false;
		}
	}
  else if (current_no_attributes < no_attributes) {
    for (i = 0; i < (no_attributes - current_no_attributes); i++) {
      if (!AppendAttribute(NULL, masked))
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Access methods, mapped though the dictionary.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a requested table entry, mapped (one way)
//                 through the associated dictionary if specified.
// Comments......:
// Revisions.....:
//===================================================================

const String &
DecisionTable::GetEntry(int object_no, int attribute_no, bool use_dictionary, bool masked) const {

	// Static so we can return a reference.
	static String dummy;

	// Get integral contents.
	int value = GetEntry(object_no, attribute_no, masked);

	// If we don't want to or cannot use the dictionary, return the integral contents themselves.
	if (!HasDictionary() || !use_dictionary) {
		if (value == Undefined::Integer())
			return Undefined::String();
		dummy = String::Format(value);
		return dummy;
	}

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetEntry(unmasked, value);

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a requested table entry, mapped (both ways)
//                 through the associated dictionary.
//
//                 Ineffiecient (but occasionally convenient) way of
//                 getting an entry, allowing e.g. constructs like:
//
//                   if (table->GetEntry(4, "color") == "blue") {
//                     ...
//                   }
//
// Comments......: Could be made faster by eliminating the attribute
//                 index search by introducing an index cache.
// Revisions.....:
//===================================================================

const String &
DecisionTable::GetEntry(int object_no, const String &attribute_name, bool masked) const {

	// Is a dictionary present?
	if (dictionary_ == NULL)
		return Undefined::String();

  bool case_sensitive = true;

	// Get the specified attribute.  If names are non-unique, return the first match found.
	int attribute_no = GetAttributeIndex(attribute_name, case_sensitive, masked);

	// Was a matching attribute not found?
	if (attribute_no == Undefined::Integer())
		return Undefined::String();

	// Get the integral table entry.
	int entry = GetEntry(object_no, attribute_no, masked);

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	// Return the entry, mapped through the dictionary.
	return dictionary_->GetEntry(unmasked, entry);

}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets a specified table entry, mapped through the
//                 associated dictionary.
//
//                 Inefficient (but occasionally convenient) way of
//                 setting an entry, allowing e.g. constructs like:
//
//                   table->SetEntry(4, "color", "blue");
//
// Comments......: Could be made faster by eliminating the attribute
//                 index search by introducing an index cache.
// Revisions.....:
//===================================================================

bool
DecisionTable::SetEntry(int object_no, const String &attribute_name, const String &value, bool masked) {

	// Is a dictionary present?
	if (dictionary_ == NULL)
		return false;

  bool case_sensitive = true;

	// Get the specified attribute.  If names are non-unique, return the first match found.
	int attribute_no = GetAttributeIndex(attribute_name, case_sensitive, masked);

	// Was a matching attribute not found?
	if (attribute_no == Undefined::Integer())
		return false;

	// Is the entry deliberately set as undefined/missing?
	if (value == Undefined::String())
		return SetEntry(object_no, attribute_no, Undefined::Integer(), masked);

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	// Suggest an integral entry for the given textual value.
	int entry = dictionary_->SuggestEntry(unmasked, value);

	// Could a suggestion be made?
	if (entry == Undefined::Integer())
		return false;

	// Set dictionary entry.
	if (!dictionary_->SetEntry(unmasked, entry, value))
		return false;

	// Set integral table entry.
	if (!SetEntry(object_no, attribute_no, entry, masked))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a vector representation of a
//                 table column. If specified, leaves out missing
//                 values.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetEntries(Vector(int) &entries, int attribute_no, bool masked, bool missing) const {

	// Is the index in range?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes(masked)))
		return false;

	int i, no_objects = GetNoObjects(masked);

	entries.erase(entries.begin(), entries.end());
	entries.reserve(no_objects);

	// Extract values.
	for (i = 0; i < no_objects; i++) {
		if (!missing && IsMissing(i, attribute_no, masked))
			continue;
		entries.push_back(GetEntry(i, attribute_no, masked));
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) vectors representation two table
//                 columns. If specified, leaves out values for objects
//                 that have a missing value for one of the columns
//                 specified.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetEntries(Vector(int) &entries1, Vector(int) &entries2, int attribute_no1, int attribute_no2, bool masked, bool missing) const {

	// Is the index in range?
	if ((attribute_no1 < 0) || (attribute_no1 >= GetNoAttributes(masked)))
		return false;

	if ((attribute_no2 < 0) || (attribute_no2 >= GetNoAttributes(masked)))
		return false;

	int i, no_objects = GetNoObjects(masked);

	entries1.erase(entries1.begin(), entries1.end());
	entries2.erase(entries2.begin(), entries2.end());

	entries1.reserve(no_objects);
	entries2.reserve(no_objects);

	// Extract values.
	for (i = 0; i < no_objects; i++) {
		if (!missing && (IsMissing(i, attribute_no1, masked) || IsMissing(i, attribute_no2, masked)))
			continue;
		entries1.push_back(GetEntry(i, attribute_no1, masked));
		entries2.push_back(GetEntry(i, attribute_no2, masked));
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a vector representation of a
//                 table column.  If specified, leaves out missing
//                 values and/or converts the value via a scaling
//                 exponent.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetEntries(Vector(float) &entries, int attribute_no, bool masked, bool missing, bool use_dictionary) const {

	// Is the index in range?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes(masked)))
		return false;

	int i, no_objects = GetNoObjects(masked);

	entries.erase(entries.begin(), entries.end());
	entries.reserve(no_objects);

	// Extract values.
	for (i = 0; i < no_objects; i++) {
		if (!missing && IsMissing(i, attribute_no, masked))
			continue;
		entries.push_back(GetEntry(i, attribute_no, masked));
	}

	// We're done if the specified attribute is not a float attribute, or conversion is not wanted.
	if (!IsFloat(attribute_no, masked) || !use_dictionary)
		return true;

	return StaticRescaleEntries(entries, *this, attribute_no, masked);

}

//-------------------------------------------------------------------
// Method........: GetEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) vectors representation two table
//                 columns. If specified, leaves out values for objects
//                 that have a missing value for one of the columns
//                 specified. Value can be converted via scaling
//                 exponents.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetEntries(Vector(float) &entries1, Vector(float) &entries2, int attribute_no1, int attribute_no2, bool masked, bool missing, bool use_dictionary) const {

	// Is the index in range?
	if ((attribute_no1 < 0) || (attribute_no1 >= GetNoAttributes(masked)))
		return false;

	if ((attribute_no2 < 0) || (attribute_no2 >= GetNoAttributes(masked)))
		return false;

	int i, no_objects = GetNoObjects(masked);

	entries1.erase(entries1.begin(), entries1.end());
	entries2.erase(entries2.begin(), entries2.end());

	entries1.reserve(no_objects);
	entries2.reserve(no_objects);

	// Extract values.
	for (i = 0; i < no_objects; i++) {
		if (!missing && (IsMissing(i, attribute_no1, masked) || IsMissing(i, attribute_no2, masked)))
			continue;
		entries1.push_back(GetEntry(i, attribute_no1, masked));
		entries2.push_back(GetEntry(i, attribute_no2, masked));
	}

	// Rescale entries via dictionary?
	if (IsFloat(attribute_no1, masked) && use_dictionary) {
		if (!StaticRescaleEntries(entries1, *this, attribute_no1, masked))
			return false;
	}

	if (IsFloat(attribute_no2, masked) && use_dictionary) {
		if (!StaticRescaleEntries(entries2, *this, attribute_no2, masked))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetRuleset
// Author........: Nicholas Baltzer
// Date..........: 20/08/13
// Description...: Returns the ruleset from the decision table. Moved here
//                 from SerialExecutorLoop. 
//
// Comments......: 
// Revisions.....: 20/08/13 -N Moved here from SerialExecutorLoop.
//===================================================================

Handle<Rules>
DecisionTable::GetRuleset() const {

	Handle<Rules>		rules;
	Identifier::Handles	rulesets;

	bool			recursive = true;


	if (GetAllChildren(RULES, rulesets, recursive) && !rulesets.empty())
		rules = dynamic_cast(Rules *, rulesets[rulesets.size() - 1].GetPointer());

	if (rulesets.size() > 1)
		Message::Warning("More than one rule set detected, selecting one.", false);

	return rules;
}
//-------------------------------------------------------------------
// Method........: GetGeneralizedDecisions
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the generalized decision value
//                 for each object.
//
// Comments......: Slightly suboptimal, but code reuse is given a
//                 higher priority than a marginal speed gain.
//
//                 For some unknown reason, the current compiler
//                 complains (deep down in STL) if I use a reference
//                 instead of a pointer for the vector of generalized
//                 decisions... Hmmf.
// Revisions.....:
//===================================================================

bool
DecisionTable::GetGeneralizedDecisions(GeneralizedDecision::Handles &decisions, const Discerner &discerner, bool masked) const {

	bool all         = false;
	int  cardinality = 0;
	int  names       = Undefined::Integer();

	IndiscernibilityGraph graph;

	return graph.Create(*this, masked, discerner, all, cardinality, names, &decisions);

}

//-------------------------------------------------------------------
// Row/column administration.
//===================================================================

//-------------------------------------------------------------------
// Method........: AppendObject
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Default implementation.
//
//                 Ignores the masking and appends to the very end
//                 of the table (the end is the end, regardless of
//                 the masking).  The masking argument is for
//                 interface completeness/uniformity reasons.
// Revisions.....:
//===================================================================

bool
DecisionTable::AppendObject(bool /*masked*/) {
	return InsertObject(GetNoObjects(false), false);
}

//-------------------------------------------------------------------
// Method........: SwapObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Swaps objects i and j in the table.  Swaps entries
//                 for all attributes (disabled or not) to avoid creating
//                 inconsistencies in the table.
// Comments......: Swapping of associated object masks not implemented.
// Revisions.....:
//===================================================================

bool
DecisionTable::SwapObjects(int i, int j, bool masked) {

	if (i == j)
		return true;

	int no_attributes = GetNoAttributes(false);

	int unmasked_i;
	int unmasked_j;

	if (masked) {
		unmasked_i = GetUnmaskedObject(i);
		unmasked_j = GetUnmaskedObject(j);
	}
	else {
		unmasked_i = i;
		unmasked_j = j;
	}

	int k;

	// Swap table entries.
	for (k = 0; k < no_attributes; k++) {
		int tmp = GetEntry(unmasked_i, k, false);
		if (!SetEntry(unmasked_i, k, GetEntry(unmasked_j, k, false), false))
			return false;
		if (!SetEntry(unmasked_j, k, tmp, false))
			return false;
	}

	// Swap object masks.
	// ...

	return true;

}

//-------------------------------------------------------------------
// Method........: SortObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sorts the objects according to the integral value of
//                 the specified attribute.
// Comments......: Quick and dirty solution, very ineffiecient.
// Revisions.....:
//===================================================================

bool
DecisionTable::SortObjects(int attribute_no, bool masked) {

	Message message;

	int no_attributes = GetNoAttributes(masked);
	int no_objects    = GetNoObjects(masked);

	// Check index validity.
	if ((attribute_no < 0) || (attribute_no >= no_attributes)) {
		Message::Error("Attribute index out of range.");
		return false;
	}

	message.Notify("Sorting objects...");

	int i, j;

	// Do a dumb bubble sort.
	for (i = 0; i < no_objects; i++) {
		for (j = i + 1; j < no_objects; j++) {
			if (GetEntry(i, attribute_no, masked) > GetEntry(j, attribute_no, masked)) {
				if (!SwapObjects(i, j, masked))
					return false;
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SwapAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Swaps attributes i and j in the table, and in the
//                 dictionary if this is present.  Swaps entries
//                 for all objects (disabled or not) to avoid creating
//                 inconsistencies in the table.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SwapAttributes(int i, int j, bool masked) {

	if (i == j)
		return true;

	// Does the decision table have any children?
	if (HasChildren()) {
		Message::Error("Cannot swap attributes, the decision table has children.");
		return false;
	}

	int no_objects = GetNoObjects(false);

	int unmasked_i;
	int unmasked_j;

	if (masked) {
		unmasked_i = GetUnmaskedAttribute(i);
		unmasked_j = GetUnmaskedAttribute(j);
	}
	else {
		unmasked_i = i;
		unmasked_j = j;
	}

	int k;

	for (k = 0; k < no_objects; k++) {
		int tmp = GetEntry(k, unmasked_i, false);
		if (!SetEntry(k, unmasked_i, GetEntry(k, unmasked_j, false), false))
			return false;
		if (!SetEntry(k, unmasked_j, tmp, false))
			return false;
	}

	// Swap attributes in dictionary, if present.
	if (HasDictionary() && !GetDictionary()->SwapAttributes(unmasked_i, unmasked_j))
		return false;

  bool swap_ok = true;

	// Swap attribute status fields.
	Status tmp_status = GetStatus(i, masked);
	if (!SetStatus(i, GetStatus(j, masked), masked))
		swap_ok = false;
	if (swap_ok && !SetStatus(j, tmp_status, masked))
		swap_ok = false;

	if (!swap_ok)
		Message::Error("Attribute status fields not swapped.");

  swap_ok = true;

	// Swap attribute masks.
	Mask tmp_mask = GetAttributeMask(unmasked_i);
	if (!SetAttributeMask(unmasked_i, GetAttributeMask(unmasked_j)))
		swap_ok = false;
	if (swap_ok && !SetAttributeMask(unmasked_j, tmp_mask))
		swap_ok = false;

	if (!swap_ok)
		Message::Error("Attribute masks not swapped.");

	return true;

}

//-------------------------------------------------------------------
// Method........: InsertAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Handles the dictionary part of attribute insertion.
//                 Can only be called from subclasses, since this
//                 method is abstract.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::InsertAttribute(int attribute_no, const Attribute *attribute, bool masked) {

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	// If there is no dictionary present and an attribute is supplied, something's wrong.
	if (!HasDictionary() && (attribute != NULL)) {
		Message::Error("There is no dictionary to insert the attribute in.");
		return false;
	}

	// If there is no dictionary and no attribute is supplied, we're done.
	if (!HasDictionary() && (attribute == NULL))
		return true;

	// If there is a dictionary present and no dictionary attribute is given, create a default one.
	if (HasDictionary() && (attribute == NULL))
		return GetDictionary()->InsertAttribute(Creator::IntegerAttribute(), unmasked);

	return GetDictionary()->InsertAttribute(attribute, unmasked);

}

//-------------------------------------------------------------------
// Method........: AppendAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Default implementation.
// Comments......: Ignores masking (irrelevant anyway for an append).
// Revisions.....:
//===================================================================

bool
DecisionTable::AppendAttribute(const Attribute *attribute, bool /*masked*/) {
	return InsertAttribute(GetNoAttributes(false), attribute, false);
}

//-------------------------------------------------------------------
// Method........: RemoveAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Handles the dictionary part of attribute removal.
//                 Can only be called from subclasses, since this
//                 method is abstract.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::RemoveAttribute(int attribute_no, bool masked) {

	if (!HasDictionary())
		return true;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;


	// Remove dictionary attribute.
	return GetDictionary()->RemoveAttribute(unmasked);

}

//-------------------------------------------------------------------
// Attribute status methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsCondition
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsCondition(int attribute_no, bool masked) const {
	return (GetStatus(attribute_no, masked) == STATUS_CONDITION);
}

//-------------------------------------------------------------------
// Method........: IsDecision
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsDecision(int attribute_no, bool masked) const {
	return (GetStatus(attribute_no, masked) == STATUS_DECISION);
}

//-------------------------------------------------------------------
// Method........: GetDecisionAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the index of the attribute that is marked
//                 as the decision attribute.  If more than one
//                 attribute has this marking, then the index of the
//                 first one encountered is returned.  If no attribute
//                 has this marking, Undefined::Integer() is returned.
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::GetDecisionAttribute(bool masked) const {

	int i, no_attributes = GetNoAttributes(masked);

	// Loop backwards, since it's likely that the decision is at the end.
	for (i = no_attributes - 1; i >= 0; i--) {
		if (IsDecision(i, masked))
			return i;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Attribute range methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetValueSet
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a vector of the values the
//                 given attribute takes on in the table, and their
//                 cardinalities.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetValueSet(Vector(int) &values, Vector(int) &cardinalities, int attribute_no, bool masked) const {

	// Clear current contents.
	values.erase(values.begin(), values.end());
	cardinalities.erase(cardinalities.begin(), cardinalities.end());

	// Do a sanity check.
	if (attribute_no < 0 || attribute_no >= GetNoAttributes(masked))
		return false;

	int i, no_objects = GetNoObjects(masked);

	// Make a convenient typedef.
	typedef Map(int, int) EntryMap;

	EntryMap entrymap;

	// Fill the map.
	for (i = 0; i < no_objects; i++) {

		// Get entry value.
		int entry = GetEntry(i, attribute_no, masked);

		EntryMap::iterator iterator = entrymap.find(entry);

		// Update map.
		if (iterator == entrymap.end())
			entrymap.insert(Pair(const int, int)(entry, 1));
		else
			(*iterator).second++;

	}

	// Reserve space to minimize allocations.
	values.reserve(entrymap.size());
	cardinalities.reserve(entrymap.size());

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	EntryMap::iterator iterator = entrymap.begin();

	// Fill the vectors.
	while (!(iterator == entrymap.end())) {

		// Get (entry, cardinality) pair.
		int entry       = (*iterator).first;
		int cardinality = (*iterator).second;

		// Update vectors.
		values.push_back(entry);
		cardinalities.push_back(cardinality);

		iterator++;

	}

	return true;

}

//-------------------------------------------------------------------
// Missing values querying methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsMissing
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified entry is marked as
//                 missing.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsMissing(int object_no, int attribute_no, bool masked) const {
	return (GetEntry(object_no, attribute_no, masked) == Undefined::Integer());
}

//-------------------------------------------------------------------
// Method........: HasMissingEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the decision table contains any
//                 missing values.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::HasMissingEntries(bool masked) const {

	int i, no_attributes = GetNoAttributes(masked);

	for (i = 0; i < no_attributes; i++) {
		if (HasMissingEntries(i, masked))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Method........: HasMissingEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified attribute (i.e.
//                 column in the decision table) contains any
//                 missing values.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::HasMissingEntries(int attribute_no, bool masked) const {

	int i, no_objects = GetNoObjects(masked);

	for (i = 0; i < no_objects; i++) {
		if (IsMissing(i, attribute_no, masked))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Dictionary (meta-data) methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assigns a new dictionary to the decision table.
//                 No dimensionality checks are performed.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetDictionary(Dictionary *dictionary) {
  dictionary_ = dictionary;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a pointer to the associated dictionary.
// Comments......: Use judiciously, access the dictionary through the
//                 table instead.
// Revisions.....:
//===================================================================

Dictionary *
DecisionTable::GetDictionary() const {
  return const_cast(Dictionary *, dictionary_.GetPointer());
}

//-------------------------------------------------------------------
// Method........: HasDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if there is a dictionary associated
//                 with the decision table.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::HasDictionary() const {
  return (GetDictionary() != NULL);
}

//-------------------------------------------------------------------
// Method........: GetAttributeName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
DecisionTable::GetAttributeName(int attribute_no, bool masked) const {

	// Static so we can return a reference.
	static String dummy;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	// Dictionary present?
	if (!HasDictionary()) {
		dummy = String::Format(unmasked);
		return dummy;
	}

	return GetDictionary()->GetAttribute(unmasked)->GetName();

}

//-------------------------------------------------------------------
// Method........: SetAttributeName
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetAttributeName(int attribute_no, const String &name, bool masked) {

	// No dictionary present.
	if (!HasDictionary())
		return false;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->SetName(name);

}

//-------------------------------------------------------------------
// Method........: GetAttributeUnit
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
DecisionTable::GetAttributeUnit(int attribute_no, bool masked) const {

	// Static so we can return a reference.
	static String dummy = "";

	// Dictionary present?
	if (!HasDictionary())
		return dummy;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetUnit();

}

//-------------------------------------------------------------------
// Method........: SetAttributeUnit
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetAttributeUnit(int attribute_no, const String &unit, bool masked) {

	// No dictionary present.
	if (!HasDictionary())
		return false;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->SetUnit(unit);

}

//-------------------------------------------------------------------
// Method........: GetAttributeScalingExponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::GetAttributeScalingExponent(int attribute_no, bool masked) const {

	// Dictionary present?
	if (!HasDictionary())
		return 0;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetScalingExponent();

}

//-------------------------------------------------------------------
// Method........: SetAttributeScalingExponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetAttributeScalingExponent(int attribute_no, int exponent, bool masked) {

	// No dictionary present.
	if (!HasDictionary())
		return false;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->SetScalingExponent(exponent);

}

//-------------------------------------------------------------------
// Method........: GetAttributeType
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Attribute::Type
DecisionTable::GetAttributeType(int attribute_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return Attribute::TYPE_INTEGER;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetType();

}

//-------------------------------------------------------------------
// Method........: GetAttributeIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the index of the named attribute.
//                 If names are non-unique, returns the first match
//                 found.
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::GetAttributeIndex(const String &attribute_name, bool case_sensitive, bool masked) const {

	int attribute_no, no_attributes = GetNoAttributes(masked);

	// Define a mutable copy.
	String tmp = attribute_name;

	if (!case_sensitive)
		tmp.ToLowercase();

	// Scan through all attributes.
	for (attribute_no = 0; attribute_no < no_attributes; attribute_no++) {
		String name = GetAttributeName(attribute_no, masked);
		if (!case_sensitive)
			name.ToLowercase();
		if (name == tmp)
			return attribute_no;
	}

	// No match.
	return Undefined::Integer();

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
DecisionTable::IsNumeric(int attribute_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return true;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->IsNumeric();

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
DecisionTable::IsSymbolic(int attribute_no, bool masked) const {
	return !IsNumeric(attribute_no, masked);
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
DecisionTable::IsInteger(int attribute_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return true;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->IsInteger();

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
DecisionTable::IsFloat(int attribute_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return false;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->IsFloat();

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
DecisionTable::IsString(int attribute_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return false;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->IsString();

}

//-------------------------------------------------------------------
// Method........: GetDictionaryEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::GetDictionaryEntry(int attribute_no, const String &text, bool masked) const {

	// No dictionary present.
	if (!HasDictionary()) {
		if (text.IsInteger())
			return text.GetInteger();
		else
			return Undefined::Integer();
	}

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetEntry(text);

}

//-------------------------------------------------------------------
// Method........: GetDictionaryEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

//const String &
const String
DecisionTable::GetDictionaryEntry(int attribute_no, int value, bool masked) const {

	// Static so we can return a reference.
	//static String dummy;
	String dummy;

	// No dictionary present.
	if (!HasDictionary()) {
		dummy = String::Format(value);
		return dummy;
	}

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetEntry(value);

}

//-------------------------------------------------------------------
// Method........: SuggestDictionaryEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::SuggestDictionaryEntry(int attribute_no, const String &text, bool masked) const {

	// No dictionary present.
	if (!HasDictionary()) {
		if (text.IsInteger())
			return text.GetInteger();
		else
			return Undefined::Integer();
	}

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->SuggestEntry(unmasked, text);

}

//-------------------------------------------------------------------
// Method........: SetDictionaryEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetDictionaryEntry(int attribute_no, int value, const String &text, bool masked) {

	// No dictionary present.
	if (!HasDictionary())
		return false;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->SetEntry(value, text);

}

//-------------------------------------------------------------------
// Method........: GetNoDictionaryEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Note that this method does not return the number of
//                 distinct entries that actually are in the table, but
//                 the number of possible values that the attribute
//                 might undertake, as governed by the dictionary.
// Revisions.....:
//===================================================================

int
DecisionTable::GetNoDictionaryEntries(int attribute_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return 0;

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetNoEntries();

}

//-------------------------------------------------------------------
// Method........: GetEnumeratedDictionaryEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
DecisionTable::GetEnumeratedDictionaryEntry(int attribute_no, int entry_no, bool masked) const {

	// No dictionary present.
	if (!HasDictionary())
		return Undefined::String();

	int unmasked;

	if (masked)
		unmasked = GetUnmaskedAttribute(attribute_no);
	else
		unmasked = attribute_no;

	return GetDictionary()->GetAttribute(unmasked)->GetEnumeratedEntry(entry_no);

}

//-------------------------------------------------------------------
// Masking methods.
//===================================================================

//-------------------------------------------------------------------
// Operator......: GetAttributeMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the attribute masks of the
//                 decision table.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetAttributeMasks(Vector(DecisionTable::Mask) &masks) const {

	// Clear current contents.
	masks.erase(masks.begin(), masks.end());

	// Operate on an unmasked table.
	bool masked = false;

	// Get table dimensions.
	int i, no_attributes = GetNoAttributes(masked);

	// Reserve space to avoid spurious allocations.
	masks.reserve(no_attributes);

	for (i = 0; i < no_attributes; i++)
		masks.push_back(GetAttributeMask(i));

	return true;

}

//-------------------------------------------------------------------
// Operator......: SetAttributeMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the attribute masks as given by the supplied
//                 mask vector.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetAttributeMasks(const Vector(DecisionTable::Mask) &masks, bool safe)  {

	// Operate on an unmasked table.
	bool masked = false;

	int i, no_attributes = GetNoAttributes(masked);

	// Verify dimensions.
	if (no_attributes != masks.size())
		return false;

	for (i = 0; i < no_attributes; i++) {
		if (!SetAttributeMask(i, masks[i], safe))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Operator......: EnableAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Enables/disables the given attribute.
//
//                 Note that the given attribute index is considered
//                 to be actual and not virtual.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::EnableAttribute(int attribute_no, bool enable, bool safe) {

	if (enable)
		return SetAttributeMask(attribute_no, MASK_ENABLED, safe);
	else
		return SetAttributeMask(attribute_no, MASK_DISABLED, safe);

}

//-------------------------------------------------------------------
// Operator......: EnableAllAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Enables/disables all attributes.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::EnableAllAttributes(bool enable, bool safe) {

	int i, no_attributes = GetNoAttributes(false);

	bool ok = true;

	for (i = 0; i < no_attributes; i++) {
		if (!EnableAttribute(i, enable, safe))
			ok = false;
	}

	return ok;

}

//-------------------------------------------------------------------
// Method........: IsAttributeEnabled
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsAttributeEnabled(int attribute_no) const {
	return (GetAttributeMask(attribute_no) == MASK_ENABLED);
}

//-------------------------------------------------------------------
// Method........: IsAttributeDisabled
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsAttributeDisabled(int attribute_no) const {
	return (GetAttributeMask(attribute_no) == MASK_DISABLED);
}

//-------------------------------------------------------------------
// Operator......: GetObjectMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the object masks of the
//                 decision table.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetObjectMasks(Vector(DecisionTable::Mask) &masks) const {

	// Clear current contents.
	masks.erase(masks.begin(), masks.end());

	// Operate on an unmasked table.
	bool masked = false;

	int i, no_objects = GetNoObjects(masked);

	// Reserve space to avoid spurious allocations.
	masks.reserve(no_objects);

	for (i = 0; i < no_objects; i++)
		masks.push_back(GetObjectMask(i));

	return true;

}

//-------------------------------------------------------------------
// Operator......: SetObjectMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the object masks as given by the supplied
//                 mask vector.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::SetObjectMasks(const Vector(DecisionTable::Mask) &masks, bool safe)  {

	// Operate on an unmasked table.
	bool masked = false;

	int i, no_objects = GetNoObjects(masked);

	// Verify dimensions.
	if (no_objects != masks.size())
		return false;

	for (i = 0; i < no_objects; i++) {
		if (!SetObjectMask(i, masks[i], safe))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Operator......: EnableObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Enables/disables the given object.
//
//                 Note that the given object index is considered
//                 to be actual and not virtual.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::EnableObject(int object_no, bool enable, bool safe) {

	if (enable)
		return SetObjectMask(object_no, MASK_ENABLED, safe);
	else
		return SetObjectMask(object_no, MASK_DISABLED, safe);

}

//-------------------------------------------------------------------
// Operator......: EnableAllObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Enables/disables all objects.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::EnableAllObjects(bool enable, bool safe) {

	int i, no_objects = GetNoObjects(false);

	bool ok = true;

	for (i = 0; i < no_objects; i++) {
		if (!EnableObject(i, enable, safe))
			ok = false;
	}

	return ok;

}

//-------------------------------------------------------------------
// Method........: IsObjectEnabled
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsObjectEnabled(int object_no) const {
	return (GetObjectMask(object_no) == MASK_ENABLED);
}

//-------------------------------------------------------------------
// Method........: IsObjectDisabled
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::IsObjectDisabled(int object_no) const {
	return (GetObjectMask(object_no) == MASK_DISABLED);
}

//-------------------------------------------------------------------
// Index conversion (masked/unmasked) methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetMaskedAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from an actual (unmasked) attribute index to
//                 a virtual (masked) attribute index.
//
//                 As a special querying case, the unmasked query index
//                 is allowed to be one larger than the number of
//                 unmasked attributes present.
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::GetMaskedAttribute(int unmasked_attribute_no) const {

#ifdef _DEBUG
	if ((unmasked_attribute_no < 0) || (unmasked_attribute_no > GetNoAttributes(false))) {
		Message::Error("Attribute index out of range.");
		return Undefined::Integer();
	}
#endif

	int i;

	// Handle special case.
	if (unmasked_attribute_no == GetNoAttributes(false)) {
		int no_enabled = 0;
		for (i = 0; i < GetNoAttributes(false); i++) {
			if (IsAttributeEnabled(i))
				no_enabled++;
		}
		return no_enabled;
	}

	// For the mapping to be valid, the attribute must be valid when masked, too.
	if (!IsAttributeEnabled(unmasked_attribute_no)) {
		Message::Error("Attribute is not valid when masked, cannot perform mapping.");
		return Undefined::Integer();
	}

	int no_disabled = 0;

	// Count how many preceeding attributes that are disabled.
	for (i = 0; i < unmasked_attribute_no; i++) {
		if (!IsAttributeEnabled(i))
			no_disabled++;
	}

	return (unmasked_attribute_no - no_disabled);

}

//-------------------------------------------------------------------
// Method........: GetUnmaskedAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from a virtual (masked) attribute index to
//                 an actual (unmasked) attribute index.
//
//                 As a special querying case, the masked query index
//                 is allowed to be one larger than the number of
//                 masked attributes present.
// Comments......:
// Revisions.....:
//===================================================================

int
DecisionTable::GetUnmaskedAttribute(int masked_attribute_no) const {

#ifdef _DEBUG
	if ((masked_attribute_no < 0) || (masked_attribute_no > GetNoAttributes(false))) {
		Message::Error("Attribute index out of range.");
		return Undefined::Integer();
	}
#endif

	int i, no_enabled = 0;

	// Count how many enabled attributes we have, return when we have counted high enough.
	for (i = 0; i < GetNoAttributes(false); i++) {
		if (!IsAttributeEnabled(i))
			continue;
		if (no_enabled == masked_attribute_no)
			return i;
		no_enabled++;
	}

	// Handle special case.
	if (masked_attribute_no == no_enabled)
		return GetNoAttributes(false);

	Message::Error("There does not exist as many enabled attributes as specified.");

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: GetMaskedObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from an actual (unmasked) object index to
//                 a virtual (masked) object index.
// Comments......: Object masking not implemented yet.
// Revisions.....:
//===================================================================

int
DecisionTable::GetMaskedObject(int unmasked_object_no) const {
	return unmasked_object_no;
}

//-------------------------------------------------------------------
// Method........: GetUnmaskedObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from a virtual (masked) object index to
//                 an actual (unmasked) object index.
// Comments......: Object masking not implemented yet.
// Revisions.....:
//===================================================================

int
DecisionTable::GetUnmaskedObject(int masked_object_no) const {
	return masked_object_no;
}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: =
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assignment operator.
// Comments......: Any children (derived structures) are not copied
//                 or duplicated.
// Revisions.....:
//===================================================================

DecisionTable &
DecisionTable::operator=(const DecisionTable &in) {

	// Protect against self-assignment.
	if (this == &in)
		return *this;

	// Throw away the current dictionary.
	SetDictionary(NULL);

	bool masked = false;

	// Resize the current decision table.
	Resize(in.GetNoObjects(masked), in.GetNoAttributes(masked), masked);

	int i, j;

	// Copy the status fields.
	for (i = 0; i < GetNoAttributes(masked); i++)
		SetStatus(i, in.GetStatus(i, masked), masked);

	// Copy the attribute masks.
	for (i = 0; i < GetNoAttributes(masked); i++)
		SetAttributeMask(i, in.GetAttributeMask(i));

	// Copy the object masks.
	for (i = 0; i < GetNoObjects(masked); i++)
		SetObjectMask(i, in.GetObjectMask(i));

	// Duplicate the associated dictionary.
	if (!in.HasDictionary())
		SetDictionary(NULL);
	else
		SetDictionary(dynamic_cast(Dictionary *, in.GetDictionary()->Duplicate()));

  // Set all entries.
  for (i = 0; i < in.GetNoObjects(false); i++) {
    for (j = 0; j < in.GetNoAttributes(false); j++) {
      SetEntry(i, j, in.GetEntry(i, j, false), false);
		}
	}

	// Duplicate stuff higher up in the hierarchy.
	if (in.GetAnnotation() == NULL)
		SetAnnotation(NULL);
	else
		SetAnnotation(dynamic_cast(Annotation *, in.GetAnnotation()->Duplicate()));

  return *this;

}

//-------------------------------------------------------------------
// Operator......: ==
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Ignores masking and dictionaries.
// Revisions.....:
//===================================================================

bool
DecisionTable::operator==(const DecisionTable &in) const {

  // Are the two tables physically equal?
  if (&in == this)
    return true;

	// Do the tables have the same dimensions?
  if (GetNoObjects(false) != in.GetNoObjects(false))
    return false;

  if (GetNoAttributes(false) != in.GetNoAttributes(false))
    return false;

	int i, j;

	// Are all table entries equal?
  for (i = 0; i < in.GetNoObjects(false); i++) {
    for (j = 0; j < in.GetNoAttributes(false); j++) {
      if (GetEntry(i, j, false) != in.GetEntry(i, j, false))
        return false;
    }
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
DecisionTable::operator!=(const DecisionTable &in) const {
  return !(*this == in);
}

//-------------------------------------------------------------------
// Miscellaneous methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) those objects that have a specified
//                 value for a specified attribute.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTable::GetObjects(Vector(int) &objects, int attribute_no, int value, bool masked) const {

	// Clear vector.
	objects.erase(objects.begin(), objects.end());

	// Get table dimensions.
	int no_objects    = GetNoObjects(masked);
	int no_attributes = GetNoAttributes(masked);

	// Verify input.
	if (attribute_no < 0 || attribute_no >= no_attributes)
		return false;

	int i;

	// Fill vector.
	for (i = 0; i < no_objects; i++) {
		if (GetEntry(i, attribute_no, masked) == value)
			objects.push_back(i);
	}

	return true;

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
DecisionTable::GetString(DecisionTable::Status status) {

	switch (status) {
		case STATUS_CONDITION: return "Condition";
		case STATUS_DECISION:  return "Decision";
		default:               return Undefined::String();
	}

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
DecisionTable::GetString(DecisionTable::Mask mask) {

	switch (mask) {
		case MASK_DISABLED: return "Disabled";
		case MASK_ENABLED:  return "Enabled";
		default:            return Undefined::String();
	}

}


 
