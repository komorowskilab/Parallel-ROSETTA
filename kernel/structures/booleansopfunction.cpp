//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/booleansopfunction.h>

//-------------------------------------------------------------------
// Methods for class BooleanSOPFunction.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Copy constructor
// Comments......:
// Revisions.....:
//===================================================================

BooleanSOPFunction::BooleanSOPFunction(const BooleanSOPFunction &in) : BooleanFunction(in) {
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

BooleanSOPFunction::BooleanSOPFunction() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BooleanSOPFunction::~BooleanSOPFunction() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BooleanSOPFunction, BOOLEANSOPFUNCTION, BooleanFunction)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
BooleanSOPFunction::Duplicate() const {
  return new BooleanSOPFunction(*this);
}

//-------------------------------------------------------------------
// Methods inherited from BooleanFunction.
//===================================================================

bool
BooleanSOPFunction::Create(const String &function, const DecisionTable &table, bool masked) {
	return BooleanFunction::Create(function, '+', '*', table, masked);
}

bool
BooleanSOPFunction::Create(const String &function, Map(String, int) &names) {
	return BooleanFunction::Create(function, '+', '*', names);
}

bool
BooleanSOPFunction::Format(String &formatted, const DecisionTable *table, bool masked) const {
	return BooleanFunction::Format(formatted, '+', '*', table, masked);
}

String
BooleanSOPFunction::Format(const DecisionTable *table, bool masked) const {
	return BooleanFunction::Format('+', '*', table, masked);
}

bool
BooleanSOPFunction::Load(ifstream &stream, const DecisionTable *table, bool masked) {
	return BooleanFunction::Load(stream, '+', '*', table, masked);
}

bool
BooleanSOPFunction::Save(ofstream &stream, const DecisionTable *table, bool masked) const {
	return BooleanFunction::Save(stream, '+', '*', table, masked);
}
 
