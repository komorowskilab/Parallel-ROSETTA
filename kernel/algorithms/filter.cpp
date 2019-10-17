//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/filter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structures.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Filter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Filter::Filter() {
	InvertLogic(false);
}

Filter::~Filter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Filter, FILTER, Algorithm)

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
Filter::GetParameters() const {

	String parameters;

	parameters += Keyword::Invert();
	parameters += Keyword::Assignment();
	parameters += String::Format(InvertLogic());

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
Filter::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Invert() && value.IsBoolean())
		return InvertLogic(value.GetBoolean());

	return false;

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
Filter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(STRUCTURES);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes a structure set as input and filters it according
//                 to some (overloaded) filtering scheme.  By filtering is
//                 meant complete removal of individual member structures.
//
// Comments......: Assumes that library clients use handles.
//
// Revisions.....: Aš 19990511: Added progress message hack.
//===================================================================

Structure *
Filter::Apply(Structure &structure) const {

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	Handle<Structures> structures = dynamic_cast(Structures *, &structure);

	// Get size of original structure set.
	int no_structures = structures->GetNoStructures();

	int i, no_filtered = 0;

	// How often should we poll for cancellations? How many structures do, say, 5 percent correspond to?
	int pollsize = no_structures / 20, pollcount = 0;

	Message message;

	// Prune away all structures that match the overloaded filtering criterion.
	// Iterate backwards to avoid indexing trouble.
	for (i = no_structures - 1; i >= 0; i--) {

		// Cancel filtering?
		if (pollcount == pollsize || i == 0 || i == no_structures - 1) {
			if (!message.Progress("Filtering...", no_structures - 1 - i, no_structures))
				break;
			pollcount = 0;
		}
		else {
			pollcount++;
		}

		// Filter away current structure?
		bool filter = Remove(*structures, i);

		// Invert filtering logic?
		if (InvertLogic())
			filter = !filter;

		// Remove the current structure from the structure set.
		if (filter) {
			if (!structures->RemoveStructure(i)) {
				Message::Error("Error pruning structure from structure set.");
				return NULL;
			}
			no_filtered++;
		}

	}

	message.Notify("Filtered away " + String::Format(no_filtered) + " out of " + String::Format(no_structures) + " structures.");

	return structures.Release();

}
 
