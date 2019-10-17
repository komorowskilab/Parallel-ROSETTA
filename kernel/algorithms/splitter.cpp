//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/splitter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Methods for class Splitter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Splitter::Splitter() {
	AppendAsChildren(true);
}

Splitter::~Splitter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Splitter, SPLITTER, Algorithm)

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
Splitter::GetParameters() const {

	String parameters;

	parameters += Keyword::Append();
	parameters += Keyword::Assignment();
	parameters += String::Format(AppendAsChildren());

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
Splitter::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Append() && value.IsBoolean())
		return AppendAsChildren(value.GetBoolean());

	return false;

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
Splitter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}
 
