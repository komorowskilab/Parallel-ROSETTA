//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/combinatorialcompleter.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/bits.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class CombinatorialCompleter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

CombinatorialCompleter::CombinatorialCompleter() {
}

CombinatorialCompleter::~CombinatorialCompleter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(CombinatorialCompleter, COMBINATORIALCOMPLETER, Completer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a new decision table which is a duplicate
//                 of the input table, except for that missing values
//                 have been substituted with all possible value
//                 combinations.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
CombinatorialCompleter::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Operate on an unmasked table.
	bool masked = false;

	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	// Create a new table. Duplicate the old one to "inherit" masking and dictionary.
	Handle<DecisionTable> complete = dynamic_cast(DecisionTable *, table->Duplicate());

	// Resize the new table to initially contain one object.
	if (!complete->Resize(1, no_attributes, masked)) {
		Message::Error("Failed to resize the new table.");
		return NULL;
	}

	Vector(Vector(int)) values;

	// Calculate value sets (the sets of possible values the attributes can take on).
	if (!CalculateValueSets(values, *table, masked)) {
		Message::Error("Error computing value sets.");
		return NULL;
	}

	int i, j;

	bool first = true;

  // Iterate over all objects.
	for (i = 0; i < no_objects; i++) {

		Bits        is_missing(no_attributes, false);
		Vector(int) no_values;

		no_values.reserve(no_attributes);

		int no_expansions = 1;

		// Compute how many objects the current object expands to.
		for (j = 0; j < no_attributes; j++) {

			// Don't expand missing entries that are masked (or non-missing entries).
			if (!table->IsAttributeEnabled(j) || !table->IsObjectEnabled(i) || !table->IsMissing(i, j, masked)) {
				is_missing[j] = false;
				no_values.push_back(1);
			}
			else {
				is_missing[j] = true;
				no_values.push_back(values[j].size());
			}

			no_expansions *= no_values[j];

		}

		// Append new, empty objects to the complete table.
		for (j = 0; j < no_expansions; j++) {

			// The table had one object originally.
			if (first) {
				first = false;
				continue;
			}

			if (!complete->AppendObject(masked)) {
				Message::Error("Failed to append object to table.");
				return NULL;
			}

		}

		// Get the index of the first new object in the complete table.
		int start = complete->GetNoObjects(masked) - no_expansions;

		// Iterate over all attributes.
		for (j = 0; j < no_attributes; j++) {

			int l, m, k = start;

			// Iterate over all newly appended objects.
			while (k < start + no_expansions) {

				// Iterate over all possible values.
				for (l = 0; l < no_values[j]; l++) {

					int no_repetitions = no_expansions;

					// Compute how often to repeat the current value.
					if (is_missing[j]) {
						for (m = 0; m <= j; m++)
							no_repetitions /= no_values[m];
					}

					// Repeat the current value a certain number of times.
					for (m = 0; m < no_repetitions; m++) {

						int expanded;

						// Determine expansion.
						if (is_missing[j] && !values[j].empty())
							expanded = values[j][l];
						else
							expanded = table->GetEntry(i, j, masked);

						// Set expanded table entry.
						if (!complete->SetEntry(k++, j, expanded, masked)) {
							Message::Error("Failed to set table entry.");
							return NULL;
						}

					}

				}

			}

		}

	}

	return complete.Release();

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: CalculateValueSets
// Author........: Aleksander šhrn
// Date..........:
// Description...: Calculates value sets for all attributes,
//                 ignoring missing entries.
// Comments......:
// Revisions.....:
//===================================================================

bool
CombinatorialCompleter::CalculateValueSets(Vector(Vector(int)) &values, const DecisionTable &table, bool masked) const {

	int no_attributes = table.GetNoAttributes(masked);

	// Erase all entries, if any.
	values.erase(values.begin(), values.end());
	values.reserve(no_attributes);

	int i, j;

	// Calculate value sets.
	for (i = 0; i < no_attributes; i++) {

		// Calculate the distribution.
		Vector(int) entries;
		Vector(int) frequencies;

		if (!table.GetValueSet(entries, frequencies, i, masked))
			return false;

		// Remove missing entries from value set.
		for (j = 0; j < entries.size(); j++) {
			if (entries[j] == Undefined::Integer()) {
				entries.erase(entries.begin() + j);
				break;
			}
		}

		// Store value set for current attribute.
		values.push_back(entries);

	}

	return true;

}
 
CombinatorialCompleter *
CombinatorialCompleter::Clone() {
	return new CombinatorialCompleter;
}
