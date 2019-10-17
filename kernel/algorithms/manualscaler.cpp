//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/manualscaler.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/interval.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class ManualScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Sets default parameters.
// Revisions.....: Aš 990312: Added SetMode(MODE_DISCARD).
//===================================================================

ManualScaler::ManualScaler() {
	SetAttribute(0);
	UseDictionary(true);
	SetMode(MODE_DISCARD);
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ManualScaler::~ManualScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ManualScaler, MANUALSCALER, Scaler)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Append stuff returned by Scaler::GetParameters()
//                 as well..? Not really relevant, but perhaps for
//                 the sake of consistency and completeness...?
// Revisions.....:
//===================================================================

String
ManualScaler::GetParameters() const {

	String parameters;

	parameters += Keyword::Attribute();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAttribute());

	if (GetNoIntervals() == 0)
		return parameters;

	parameters += Keyword::Separator();

	parameters += Keyword::Intervals();
	parameters += Keyword::Assignment();

	int i;

	for (i = 0; i < GetNoIntervals(); i++) {
		parameters += GetInterval(i).Format();
		parameters += ' ';
		parameters += Keyword::MapsTo();
		parameters += ' ';
		parameters += String::Format(GetValue(i));
		if (i < GetNoIntervals() - 1) {
			parameters += ' ';
			parameters += Keyword::And();
			parameters += ' ';
		}
	}

	parameters += Keyword::Separator();

	parameters += Keyword::Dictionary();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseDictionary());

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990323: Fixed a minor bug. Forgot to return
//                            true after successfully processing the
//                            Keyword::Intervals() keyword.
//===================================================================

bool
ManualScaler::SetParameter(const String &keyword, const String &value) {

	// Set attribute.
	if (keyword == Keyword::Attribute() && value.IsInteger())
		return SetAttribute(value.GetInteger());

	// Set intervals.
	if (keyword == Keyword::Intervals())
		return SetIntervals(value);

	// Use dictionary (for backwards compatibility).
	if (keyword == Keyword::Dictionary() && value.IsBoolean())
		return UseDictionary(value.GetBoolean());

	// Use dictionary (for backwards compatibility).
	if (keyword == Keyword::UseDictionary() && value.IsBoolean())
		return UseDictionary(value.GetBoolean());

	// Unhandled keyword/value pair.
	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Runs through the set attribute of the decision table
//                 and performs the mapping as defined by the intervals.
//
//                 Note that if appropriate and the use dictionary flag is
//                 not set, the intervals are not defined through the
//                 dictionary, i.e. the intervals operate om the "raw"
//                 decision table entries.
//
//                 The attribute index is considered to be virtual, i.e.
//                 relative to the masked table.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ManualScaler::Apply(Structure &structure) const {

	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to correct type.
	DecisionTable *table = dynamic_cast(DecisionTable *, &structure);

	bool masked = true;

	// Is there anything to scale?
	if ((table->GetNoObjects(masked) == 0) || (table->GetNoAttributes(masked) == 0)) {
		Message::Error("The decision table is empty and cannot be scaled.");
		return NULL;
	}

	// Is the attribute index valid?
	if ((GetAttribute() < 0) || (GetAttribute() >= table->GetNoAttributes(masked))) {
		Message::Error("Attribute index parameter out of range.");
		return NULL;
	}

	int no_objects   = table->GetNoObjects(masked);
	int attribute    = GetAttribute();
	int no_intervals = GetNoIntervals();

	Vector(int) uncovered;

	bool is_float = table->IsFloat(attribute, masked);

	int i, j;

	// Perform discretization.
	for (i = 0; i < no_objects; i++) {

		// Get raw table entry.
		int entry = table->GetEntry(i, attribute, masked);

		// If it is undefined, leave it be.
		if (entry == Undefined::Integer())
			continue;

		// The floating point version of the entry.
		float f_entry = entry;

		// Interpret entry via dictionary?
		if (is_float && UseDictionary()) {
			String tmp = table->GetDictionaryEntry(attribute, entry, masked);
			f_entry = tmp.GetFloat();
		}

		for (j = 0; j < no_intervals; j++) {

			// Does the current interval cover the current entry?
			if (GetInterval(j).Contains(f_entry)) {

				// Get mapping value.
				int value = GetValue(j);

				if (!table->SetEntry(i, attribute, value, masked)) {
					Message::Error("Error setting table entry.");
					return NULL;
				}

				// Proceed with next object in table.
				break;

			}

		}

		// No intervals covered the current entry?
		if (j == no_intervals) {
			uncovered.push_back(entry);
		}

	}

	// Update the dictionary, if present.
	if (table->HasDictionary()) {

		// Create a new attribute for the dictionary.
		Handle<StringAttribute> new_attribute = Creator::StringAttribute();

		// Copy name/unit.
		new_attribute->SetName(table->GetAttributeName(attribute, masked));
		new_attribute->SetUnit(table->GetAttributeUnit(attribute, masked));

		// Set intervals as dictionary entries.
		for (j = 0; j < no_intervals; j++) {
			if (!new_attribute->SetEntry(GetValue(j), GetInterval(j).Format())) {
				Message::Error("Error setting dictionary entry.");
				return NULL;
			}
		}

		// Keep dictionary entries for the uncovered entries.
		for (j = 0; j < uncovered.size(); j++) {
			if (!new_attribute->SetEntry(uncovered[j], table->GetDictionaryEntry(attribute, uncovered[j], masked))) {
				Message::Error("Error setting dictionary entry.");
				return NULL;
			}
		}

		int unmasked = masked ? table->GetUnmaskedAttribute(attribute) : attribute;

		// Remove old attribute from dictionary (note indexing).
		if (!table->GetDictionary()->RemoveAttribute(unmasked)) {
			Message::Error("Error removing old attribute from dictionary.");
			return NULL;
		}

		// Insert new attribute into dictionary (note indexing).
		if (!table->GetDictionary()->InsertAttribute(new_attribute.GetPointer(), unmasked)) {
			Message::Error("Error inserting new attribute into dictionary.");
			return NULL;
		}

	}

	return table;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: InsertInterval
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ManualScaler::InsertInterval(const Interval &interval, int value, int i) {

	// Check bounds.
  if ((i < 0) || (i > GetNoIntervals()) )
    return false;

	// Check interval validity.
	if (!interval.IsValid()) {
		Message::Error("Invalid interval.");
		return false;
	}

	int j;

	// Check for possible overlaps.
	for (j = 0; j < GetNoIntervals(); j++) {
		if (GetInterval(j).Overlaps(interval))
			Message::Warning("Intervals " + interval.Format() + " and " + GetInterval(j).Format() + " overlap.");
	}

	// Perform the insertion.
  intervals_.insert(intervals_.begin() + i, interval);
  values_.insert(values_.begin() + i, value);

  return true;

}

//-------------------------------------------------------------------
// Method........: AppendInterval
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ManualScaler::AppendInterval(const Interval &interval, int value) {
	return InsertInterval(interval, value, GetNoIntervals());
}

//-------------------------------------------------------------------
// Method........: RemoveInterval
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ManualScaler::RemoveInterval(int i) {

	// Check bounds.
  if ((i < 0) || (i >= GetNoIntervals()))
    return false;

  intervals_.erase(intervals_.begin() + i);
  values_.erase(values_.begin() + i);

  return true;

}

//-------------------------------------------------------------------
// Method........: RemoveAllIntervals
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ManualScaler::RemoveAllIntervals() {

  intervals_.erase(intervals_.begin(), intervals_.end());
  values_.erase(values_.begin(), values_.end());

  return true;

}

//-------------------------------------------------------------------
// Method........: GetInterval
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const Interval &
ManualScaler::GetInterval(int i) const {

	static Interval errorinterval;

	// Valid argument?
	if ((i < 0) || (i >= GetNoIntervals())) {
		Message::Error("Interval index out of range.");
		return errorinterval;
	}

	return intervals_[i];

}

//-------------------------------------------------------------------
// Method........: GetValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
ManualScaler::GetValue(int i) const {

	// Valid argument?
	if ((i < 0) || (i >= GetNoIntervals())) {
		Message::Error("Value index out of range.");
		return Undefined::Integer();
	}

	return values_[i];

}

//-------------------------------------------------------------------
// Method........: SetIntervals
// Author........: Aleksander šhrn
// Date..........:
// Description...: Parses a string description of a set of intervals.
// Comments......:
// Revisions.....:
//===================================================================

bool
ManualScaler::SetIntervals(const String &intervals) {

	// Clear current interval mapping,
	if (!RemoveAllIntervals())
		return false;

	String mapsto = Keyword::MapsTo();
	String word_and    = Keyword::And();

	mapsto.Trim(" \t");
	word_and.Trim(" \t");

	String remaining = intervals;
	bool   done      = false;

	// Parse string.
	while (!done) {

		String mapping;

		// Extract one interval/value mapping pair.
		if (!remaining.Contains(word_and)) {
		  mapping = remaining;
			done = true;
		}
		else {
			mapping = remaining.Before(word_and);
		}

		remaining = remaining.After(word_and);

		// Sanity check.
		if (!mapping.Contains(mapsto))
			return false;

		// Extract interval and value,
		String interval = mapping.Before(mapsto);
		String mapvalue = mapping.After(mapsto);
		
		interval.Trim(" \t");
		mapvalue.Trim(" \t");

		// Sanity check,
		if (!mapvalue.IsInteger())
			return false;
	
		if (!AppendInterval(Interval(interval), mapvalue.GetInteger()))
			return false;

	}

	return true;

}

ManualScaler *
ManualScaler::Clone() {
	return new ManualScaler;
}
