//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/booleanposfunction.h>

//-------------------------------------------------------------------
// Methods for class BooleanPOSFunction.
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

BooleanPOSFunction::BooleanPOSFunction(const BooleanPOSFunction &in) : BooleanFunction(in) {
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

BooleanPOSFunction::BooleanPOSFunction() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BooleanPOSFunction::~BooleanPOSFunction() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BooleanPOSFunction, BOOLEANPOSFUNCTION, BooleanFunction)

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
BooleanPOSFunction::Duplicate() const {
  return new BooleanPOSFunction(*this);
}

//-------------------------------------------------------------------
// Methods inherited from BooleanFunction.
//===================================================================

bool
BooleanPOSFunction::Create(const String &function, const DecisionTable &table, bool masked) {
	return BooleanFunction::Create(function, '*', '+', table, masked);
}

bool
BooleanPOSFunction::Create(const String &function, Map(String, int) &names) {
	return BooleanFunction::Create(function, '*', '+', names);
}

bool
BooleanPOSFunction::Format(String &formatted, const DecisionTable *table, bool masked) const {
	return BooleanFunction::Format(formatted, '*', '+', table, masked);
}

String
BooleanPOSFunction::Format(const DecisionTable *table, bool masked) const {
	return BooleanFunction::Format('*', '+', table, masked);
}

bool
BooleanPOSFunction::Load(ifstream &stream, const DecisionTable *table, bool masked) {
	return BooleanFunction::Load(stream, '*', '+', table, masked);
}

bool
BooleanPOSFunction::Save(ofstream &stream, const DecisionTable *table, bool masked) const {
	return BooleanFunction::Save(stream, '*', '+', table, masked);
}
 
