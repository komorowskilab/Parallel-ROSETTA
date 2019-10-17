//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/binarysplitter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>

#include <kernel/utilities/rng.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class BinarySplitter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

BinarySplitter::BinarySplitter() {
	SetSeed(1);
	SetSplittingFactor(0.5);
}

BinarySplitter::~BinarySplitter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BinarySplitter, BINARYSPLITTER, Splitter)

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
BinarySplitter::GetParameters() const {

	String parameters;

	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());

	parameters += Keyword::Separator();

	parameters += Keyword::Factor();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSplittingFactor());

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
BinarySplitter::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());

	if (keyword == Keyword::Factor() && value.IsFloat())
		return SetSplittingFactor(value.GetFloat());

	return Splitter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes a decision table as input and splits it
//                 in two according to some splitting scheme.
//
//                 If specified, the resulting subtables are appended
//                 to the input table as children, and the input table
//                 itself is returned.  Alternatively, the subtables are
//                 not appended as children, but are instead returned as
//                 a decision table set.
//
// Comments......: Assumes that library clients use handles.
// Revisions.....: Aš 000229: Simplified implementation a bit.
//===================================================================

Structure *
BinarySplitter::Apply(Structure &structure) const {

	Message message;

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	bool masked = false;

	// Get size of original table.
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	// Calculate ideal subtable sizes.
	int ideal_size1 = MathKit::Round(GetSplittingFactor() * no_objects);
	int ideal_size2 = no_objects - ideal_size1;

	// Create two subtables.
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	Handle<DecisionTable> subtable2 = Creator::DecisionTable();

	message.Notify("Initializing new tables...");

	// Resize them.
	if (!subtable1->Resize(ideal_size1, no_attributes, masked) || !subtable2->Resize(ideal_size2, no_attributes, masked))
		return NULL;

	message.Notify("Sampling indices...");

	// Set random number generator seed.
	RNG rng(GetSeed());

	Vector(int) indices;  indices.reserve(no_objects);
	Vector(int) indices1; indices1.reserve(ideal_size1);
	Vector(int) indices2; indices2.reserve(ideal_size2);

	int i, j;

	// Sample object indices,
	for (i = 0; i < no_objects; i++)
		indices.push_back(i);

	while (!indices.empty()) {

		// Sample an index into the vector of remaining indices.
		int index = rng.DrawInteger(0, indices.size() - 1);

		// Move index to appropriate index set.
		if (indices1.size() < ideal_size1)
			indices1.push_back(indices[index]);
		else
			indices2.push_back(indices[index]);

		// To remove the index, move the tail into its slot and remove the tail.
		indices[index] = indices[indices.size() - 1];
		indices.pop_back();

	}

	// This isn't necessary, but a user might appreciate it if the table has masked attributes...
	std::sort(indices1.begin(), indices1.end());
	std::sort(indices2.begin(), indices2.end());

	message.Notify("Copying entries...");

	// Copy sampled objects into the two already resized tables.
	for (i = 0; i < indices1.size(); i++) {
		int index = indices1[i];
		for (j = 0; j < no_attributes; j++) {
			if (!subtable1->SetEntry(i, j, table->GetEntry(index, j, masked), masked))
				return NULL;
		}
	}

	for (i = 0; i < indices2.size(); i++) {
		int index = indices2[i];
		for (j = 0; j < no_attributes; j++) {
			if (!subtable2->SetEntry(i, j, table->GetEntry(index, j, masked), masked))
				return NULL;
		}
	}

	// Set subtable names.
	subtable1->SetName(String::Format(subtable1->GetNoObjects(masked)) + " objects");
	subtable2->SetName(String::Format(subtable2->GetNoObjects(masked)) + " objects");

	message.Notify("Copying metadata...");

	// Duplicate the dictionaries.
	if (table->HasDictionary()) {
		subtable1->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
		subtable2->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
	}
	else {
		subtable1->SetDictionary(NULL);
		subtable2->SetDictionary(NULL);
	}

	// Set attribute status fields.
	for (i = 0; i < no_attributes; i++) {
		subtable1->SetStatus(i, table->GetStatus(i, masked), masked);
		subtable2->SetStatus(i, table->GetStatus(i, masked), masked);
	}

	// Set attribute masks.
	for (i = 0; i < no_attributes; i++) {
		subtable1->SetAttributeMask(i, table->GetAttributeMask(i));
		subtable2->SetAttributeMask(i, table->GetAttributeMask(i));
	}

	// Set object masks.
	for (i = 0; i < indices1.size(); i++)
		subtable1->SetObjectMask(i, table->GetObjectMask(indices1[i]));
	for (i = 0; i < indices2.size(); i++)
		subtable2->SetObjectMask(i, table->GetObjectMask(indices2[i]));

	// Determine fate of subtables and return.
	if (AppendAsChildren()) {
		// Append the subtables as children to the input table.
	  if (!table->AppendChild(subtable1.GetPointer()) || !table->AppendChild(subtable2.GetPointer())) {
		  Message::Error("Error appending the subtables to the parent table.");
		  return NULL;
		}
		// Return the input table itself.
		return table.Release();
	}
	else {
		// Create a decision table set.
    Handle<DecisionTables> subtables = Creator::DecisionTables();
		if (!subtables->AppendStructure(subtable1.GetPointer()) || !subtables->AppendStructure(subtable2.GetPointer())) {
		  Message::Error("Error appending the subtables to the subtable set.");
		  return NULL;
		}
		// Return the set of subtables.
		return subtables.Release();
	}

} 

BinarySplitter *
BinarySplitter::Clone() {
	return new BinarySplitter;
}
