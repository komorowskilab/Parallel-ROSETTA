//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reporter.h>

//-------------------------------------------------------------------
// Methods for class Reporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Reporter::Reporter() {
}

Reporter::~Reporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Reporter, REPORTER, Exporter)

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
Reporter::IsApplicable(const Structure &/*structure*/, bool /*warn*/) const {
	return true;
}

Reporter *
Reporter::Clone() {
   return new Reporter;
} 
