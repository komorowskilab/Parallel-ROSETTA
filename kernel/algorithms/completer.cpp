//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/completer.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Methods for class Completer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Completer::Completer() {
}

Completer::~Completer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Completer, COMPLETER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Completer::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

 
