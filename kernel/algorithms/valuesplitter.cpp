//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/valuesplitter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class ValueSplitter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ValueSplitter::ValueSplitter() {
	SetAttribute(0);
}

ValueSplitter::~ValueSplitter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ValueSplitter, VALUESPLITTER, Splitter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
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
ValueSplitter::GetParameters() const {

	String parameters;

	parameters += Keyword::Attribute();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAttribute());

	parameters += Keyword::Separator();

	parameters += Splitter::GetParameters();

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
ValueSplitter::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Attribute() && value.IsInteger())
		return SetAttribute(value.GetInteger());

	return Splitter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes a decision table as input and splits it
//                 into several subtables.
//
//                 If specified, the resulting subtables are appended
//                 to the input table as children, and the input table
//                 itself is returned.  Alternatively, the subtables are
//                 not appended as children, but are instead returned as
//                 a decision table set.
//
// Comments......: Assumes that library clients use handles.
// Revisions.....: Aš 0797 - Use GetValueSet
//===================================================================

Structure *
ValueSplitter::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Operate on an unmasked table.
	bool masked = false;

	Vector(int) values;
	Vector(int) frequencies;

	// Get the value set set of unique decisions.
  if (!table->GetValueSet(values, frequencies, GetAttribute(), masked)) {
		Message::Error("Failed to get decision value set.");
		return NULL;
	}

	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	Map(int, Handle<DecisionTable>) subtables; // The subtables, indexed by decision values.
	Map(int, bool)                  is_first;  // Flags used in subsequent duplication check.

	int i, j;

	// Create one empty subtable for each unique decision. (Duplicate to "inherit" masking.)
	for (i = 0; i < values.size(); i++) {

		int decision = values[i];

		if (i == 0) {
			subtables.insert(Pair(const int, Handle<DecisionTable>)(decision, Handle<DecisionTable>(dynamic_cast(DecisionTable *, table->Duplicate()))));
			subtables[decision]->Resize(1, no_attributes, masked);
		}
		else {
			subtables.insert(Pair(const int, Handle<DecisionTable>)(decision, Handle<DecisionTable>(dynamic_cast(DecisionTable *, subtables[values[i - 1]]->Duplicate()))));
		}

		is_first.insert(Pair(const int, bool)(decision, true));

	}

	// Fill the subtables.
	for (i = 0; i < no_objects; i++) {

		int decision = table->GetEntry(i, GetAttribute(), masked);

		Handle<DecisionTable> subtable = subtables[decision];

		// Append a new object to the subtable, if needed.
		if (!is_first[decision]) {
			if (!subtable->AppendObject(masked)) {
				Message::Error("Failed to append object to subtable.");
				return NULL;
			}
		}

		is_first[decision] = false;

		int last = subtable->GetNoObjects(masked) - 1;

		// Copy the object into the subtable.
		for (j = 0; j < no_attributes; j++) {
			int entry = table->GetEntry(i, j, masked);
			if (!subtable->SetEntry(last, j, entry, masked))
				return NULL;
		}

	}

	// Set subtable names.
	for (i = 0; i < values.size(); i++) {
		int    value         = values[i];
		String attributename = table->GetAttributeName(GetAttribute(), masked);
		String valuename     = table->GetDictionaryEntry(GetAttribute(), value, masked);
		subtables[value]->SetName(attributename + "(" + valuename + ")");
	}

	// Determine fate of subtables and return.
	if (AppendAsChildren()) {                             // Append the subtables as children to the input table.
		for (i = 0; i < values.size(); i++) {
			int value = values[i];
			if (!table->AppendChild(subtables[value].GetPointer())) {
				Message::Error("Error appending the subtables to the parent table.");
				return NULL;
			}
		}
		return table.Release();                             // Return the input table itself.
	}
	else {                                                // Create a decision table set.
    Handle<DecisionTables> subtableset = Creator::DecisionTables();
		for (i = 0; i < values.size(); i++) {
			int value = values[i];
			if (!subtableset->AppendStructure(subtables[value].GetPointer())) {
				Message::Error("Error appending the subtables to the parent table.");
				return NULL;
			}
		}
		return subtableset.Release();                       // Return the set of subtables.
	}

} 

ValueSplitter *
ValueSplitter::Clone() {
	return new ValueSplitter;
}
