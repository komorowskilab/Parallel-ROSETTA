//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/decisiontableimporter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Methods for class DecisionTableImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DecisionTableImporter::DecisionTableImporter() {
}

DecisionTableImporter::~DecisionTableImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DecisionTableImporter, DECISIONTABLEIMPORTER, Importer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 given structure.
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTableImporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

 
