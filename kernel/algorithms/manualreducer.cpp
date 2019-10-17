//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/manualreducer.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class ManualReducer.
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
// Revisions.....:
//===================================================================

ManualReducer::ManualReducer() {
	SetAttributes("{}");
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ManualReducer::~ManualReducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ManualReducer, MANUALREDUCER, Reducer)

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
ManualReducer::GetParameters() const {

	String parameters;

	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += GetAttributes();

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
ManualReducer::SetParameter(const String &keyword, const String &value) {

	// Set attribute descriptions.
	if (keyword == Keyword::Attributes())
		return SetAttributes(value);

	// For backwards compatibility.
	if (keyword == Keyword::Reduct())
		return SetAttributes(value);

	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Functions as an algorithmic shell around the
//                 Creator::Reduct method.
// Revisions.....:
//===================================================================

Structure *
ManualReducer::Apply(Structure &structure) const {

	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to correct type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	bool masked = true;

	// Create a reduct from the given description.
	Handle<Reduct> reduct = Creator::Reduct(GetAttributes(), table.GetPointer(), masked);

	if (reduct == NULL) {
		Message::Error("Failed to create a reduct from the given textual description.");
		return NULL;
	}

	// Create a reduct set.
	Handle<Reducts> reducts = Creator::Reducts();

	// Add the reduct to the reduct set.
	if (!reducts->AppendStructure(reduct.GetPointer())) {
		Message::Error("Failed to append the reduct to the reduct set.");
		return NULL;
	}

	return reducts.Release();

}

ManualReducer *
ManualReducer::Clone() {
	return new ManualReducer;
}
