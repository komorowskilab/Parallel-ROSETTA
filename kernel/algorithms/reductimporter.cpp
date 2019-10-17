//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reductimporter.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Methods for class ReductImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ReductImporter::ReductImporter() {
}

ReductImporter::~ReductImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReductImporter, REDUCTIMPORTER, Importer)

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
ReductImporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

 
