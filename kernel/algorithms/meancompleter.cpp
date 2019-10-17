//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/meancompleter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class MeanCompleter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MeanCompleter::MeanCompleter() {
}

MeanCompleter::~MeanCompleter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MeanCompleter, MEANCOMPLETER, Completer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a new decision table which is a duplicate
//                 of the input table, except for that missing values
//                 have been substituted with the mean (or mode) of the
//                 attribute.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
MeanCompleter::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Create a new table. (Duplicate to "inherit" masking and dictionary.)
	Handle<DecisionTable> complete = dynamic_cast(DecisionTable *, table->Duplicate());

	// Operate on a masked table.
	bool masked = true;

	Vector(int) means;

	// Calculate mean/mode values.
	if (!CalculateMeans(means, *table, masked)) {
		Message::Error("Error computing mean/mode values.");
		return NULL;
	}

	int no_objects    = complete->GetNoObjects(masked);
	int no_attributes = complete->GetNoAttributes(masked);

	int i, j;

	// Substitute all missing entries.
	for (i = 0; i < no_objects; i++) {
		for (j = 0; j < no_attributes; j++) {

			// Entry is non-missing, skip it.
			if (!complete->IsMissing(i, j, masked))
				continue;

			// Do the substitution.
			if (!complete->SetEntry(i, j, means[j], masked)) {
				Message::Error("Error setting mean/mode value.");
				return NULL;
			}

		}
	}

	return complete.Release();

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: CalculateMeans
// Author........: Aleksander šhrn
// Date..........:
// Description...: Calculates means/modes for all attributes,
//                 ignoring missing entries.
// Comments......:
// Revisions.....:
//===================================================================

bool
MeanCompleter::CalculateMeans(Vector(int) &means, const DecisionTable &table, bool masked) const {

	int no_attributes = table.GetNoAttributes(masked);

	// Erase all entries, if any.
	means.erase(means.begin(), means.end());
	means.reserve(no_attributes);

	Vector(int) entries;

	int i;

	// Fill mean/mode lookup structure.
	for (i = 0; i < no_attributes; i++) {
		if (!table.GetEntries(entries, i, masked, false)) {
			Message::Error("Failed to extract table entries.");
			return false;
		}
		if (table.IsNumeric(i, masked))      // Numerical attribute, use mean.
			means.push_back(MathKit::Round(MathKit::Mean(entries)));
		else                                 // Symbolic attribute, use mode.
			means.push_back(MathKit::Mode(entries));
	}

	return true;

}
 
MeanCompleter *
MeanCompleter::Clone() {
	return new MeanCompleter;
}
