//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960419
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reductshortener.h>

//-------------------------------------------------------------------
// Methods for class ReductShortener.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ReductShortener::ReductShortener() {
}

ReductShortener::~ReductShortener() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReductShortener, REDUCTSHORTENER, ReductFilter)

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
ReductShortener::GetParameters() const {
	return ReductFilter::GetParameters();
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
ReductShortener::SetParameter(const String &keyword, const String &value) {
	return ReductFilter::SetParameter(keyword, value);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Blocks default implementation from Filter class.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ReductShortener::Apply(Structure &/*structure*/) const {
	return NULL;
}

//-------------------------------------------------------------------
// Methods inherited from Filter.
//===================================================================

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Aleksander šhrn
// Date..........:
// Description...: Dummy implementation, not in use.
// Comments......:
// Revisions.....:
//===================================================================

bool
ReductShortener::Remove(const Structures &/*structures*/, int /*i*/) const {
	return false;
}
 
