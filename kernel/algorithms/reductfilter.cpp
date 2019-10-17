//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reductfilter.h>

#include <kernel/structures/reducts.h>

//-------------------------------------------------------------------
// Methods for class ReductFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ReductFilter::ReductFilter() {
}

ReductFilter::~ReductFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReductFilter, REDUCTFILTER, Filter)

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
ReductFilter::GetParameters() const {
	return Filter::GetParameters();
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
ReductFilter::SetParameter(const String &keyword, const String &value) {
	return Filter::SetParameter(keyword, value);
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
ReductFilter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(REDUCTS);
}

 
