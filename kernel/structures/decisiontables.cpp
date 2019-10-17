//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/decisiontables.h>

//-------------------------------------------------------------------
// Methods for class DecisionTables.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DecisionTables::DecisionTables(const DecisionTables &in) : Structures(in) {
}

DecisionTables::DecisionTables() {
}

DecisionTables::~DecisionTables() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DecisionTables, DECISIONTABLES, Structures)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
DecisionTables::Duplicate() const {
	return new DecisionTables(*this);
}

//-------------------------------------------------------------------
// Methods inherited from Structures.
//===================================================================

//------------------------------------------------------------------
// Method........: InsertStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Overloaded to check for consistency/homogenity.
//                 Ensures that only decision tables are inserted.
// Comments......: AppendStructure is not necessary to overload, since
//                 Structures::AppendStructure is implemented via the
//                 InsertStructure method.
// Revisions.....:
//===================================================================

bool
DecisionTables::InsertStructure(Structure *structure, int i) {

	if (structure == NULL) {
		Message::Error("Cannot insert a NULL decision table.");
		return false;
	}

	if (!structure->IsA(DECISIONTABLE)) {
		Message::Error("Cannot insert a non-decision table into a decision table set.");
		return false;
	}

	return Structures::InsertStructure(structure, i);

}

 
