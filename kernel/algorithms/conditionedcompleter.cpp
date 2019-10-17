//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/conditionedcompleter.h>
#include <kernel/algorithms/valuesplitter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class ConditionedCompleter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ConditionedCompleter::ConditionedCompleter() {
}

ConditionedCompleter::~ConditionedCompleter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ConditionedCompleter, CONDITIONEDCOMPLETER, Completer)

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
ConditionedCompleter::GetParameters() const {

	String parameters;

	parameters += Keyword::Completer();
	parameters += Keyword::Assignment();

	if (GetCompleter() == NULL) {
		parameters += "NULL";
		return parameters;
	}
	else {
		parameters += IdHolder::GetClassname(GetCompleter()->GetId());
	  parameters += Keyword::Separator();
	  parameters += GetCompleter()->GetParameters();
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
ConditionedCompleter::SetParameter(const String &keyword, const String &value) {

	// The completer cannot (currently) be set this way.
	if (keyword == Keyword::Completer())
		return false;

	// Could the parameters concern the embedded completer?
	if (GetCompleter() == NULL)
		return false;

	return GetCompleter()->SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Conditions completion on the decision classes.
// Comments......: Simple (non-optimized) algorithm:
//
//                  1) Split the input table into several subtables,
//                     each containing separate decision classes.
//                  2) Do unconditoned completion for each
//                     subtable.
//                  3) Combine the completed subtables into a single
//                     table.
//
// Revisions.....: Aš 0797 - Use ValueSplitter.
//===================================================================

Structure *
ConditionedCompleter::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Is a completer set?
	if (GetCompleter() == NULL) {
		Message::Error("No completer algorithm set.");
		return NULL;
	}

	// Operate on an unmasked table.
	bool masked = false;

	// Get the decision attribute.
	int decision_attribute = table->GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer()) {
		Message::Error("No decision attribute to condition on.");
		return NULL;
	}

	ValueSplitter splitter;

	// Set splitter parameters.
	splitter.SetAttribute(decision_attribute);
	splitter.AppendAsChildren(false);

	// Split.
	Handle<DecisionTables> subtables = dynamic_cast(DecisionTables *, table->Apply(splitter));

	if (subtables == NULL) {
		Message::Error("Failed to split table by decision classes.");
		return NULL;
	}

	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	DecisionTable::Handles subcomplete;
	subcomplete.reserve(subtables->GetNoDecisionTables());

	int i, j, k;

	// Complete the now uniform (wrt. the decision classes) subtables.
	for (i = 0; i < subtables->GetNoDecisionTables(); i++) {

		subcomplete.push_back(dynamic_cast(DecisionTable *, subtables->GetDecisionTable(i)->Apply(*GetCompleter())));

		if (subcomplete[i] == NULL) {
			Message::Error("Failed to complete subtable.");
			return NULL;
		}

	}

	// Create a new table. (Duplicate the old one to "inherit" masking and dictionary.)
	Handle<DecisionTable> complete = dynamic_cast(DecisionTable *, table->Duplicate());

	no_objects = 0;

	// Resize the table to the right size.
	for (i = 0; i < subcomplete.size(); i++)
		no_objects += subcomplete[i]->GetNoObjects(masked);

	if (!complete->Resize(no_objects, no_attributes, masked))
		return NULL;

	int object_no = 0;

	// Combine the completed subtables into a complete table.
	for (i = 0; i < subcomplete.size(); i++) {

		// Get subtable.
		Handle<DecisionTable> subtable = subcomplete[i];

		int no_subtable_objects = subtable->GetNoObjects(masked);

		// Copy all objects from the subtable.
		for (j = 0; j < no_subtable_objects; j++) {
			for (k = 0; k < no_attributes; k++) {
				int entry = subtable->GetEntry(j, k, masked);
				if (!complete->SetEntry(object_no, k, entry, masked))
					return NULL;
			}
			object_no++;
		}

	}

	return complete.Release();

}

 

ConditionedCompleter *
ConditionedCompleter::Clone() {
	return new ConditionedCompleter;
}
