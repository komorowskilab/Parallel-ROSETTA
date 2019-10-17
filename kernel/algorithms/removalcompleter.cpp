//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/removalcompleter.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class RemovalCompleter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RemovalCompleter::RemovalCompleter() {
}

RemovalCompleter::~RemovalCompleter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RemovalCompleter, REMOVALCOMPLETER, Completer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a new decision table which is a duplicate
//                 of the input table, except for that all objects
//                 with missing values have been removed.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
RemovalCompleter::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Create a new table.
	Handle<DecisionTable> complete = dynamic_cast(DecisionTable *, table->Duplicate());

	// Operate on a masked table.
	bool masked = true;

	int no_objects    = complete->GetNoObjects(masked);
	int no_attributes = complete->GetNoAttributes(masked);

	int i, j, count = 0;

	// Remove all objects with missing entries.
	for (i = no_objects - 1; i >= 0; i--) {
		bool remove = false;
		for (j = 0; j < no_attributes; j++) {
			if (complete->IsMissing(i, j, masked)) {
				remove = true;
				break;
			}
		}
		if (!remove)
			continue;
		if (!complete->RemoveObject(i, masked)) {
			Message::Error("Error removing object.");
			return NULL;
		}
		count++;
	}

	Message message;

	message.Notify(String::Format(count) + " objects were removed, " + String::Format(no_objects - count) + " are left.");

	return complete.Release();

}

 
RemovalCompleter *
RemovalCompleter::Clone() {
	return new RemovalCompleter;
}
