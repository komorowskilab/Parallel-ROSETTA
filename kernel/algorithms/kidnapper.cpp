//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/kidnapper.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>

//-------------------------------------------------------------------
// Methods for class Kidnapper.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Kidnapper::Kidnapper() {
	index_ = 0;
}

Kidnapper::~Kidnapper() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Kidnapper, KIDNAPPER, ScriptAlgorithm)

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
Kidnapper::GetParameters() const {

	String parameters;

	// Index.
	parameters += Keyword::Index();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetChildIndex());

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
Kidnapper::SetParameter(const String &keyword, const String &value) {

	// Index.
	if (keyword == Keyword::Index() && value.IsInteger())
		return SetChildIndex(value.GetInteger());

	return false;

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Kidnapper::IsApplicable(const Structure &/*structure*/, bool /*warn*/) const {
	return true;
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Kidnapper::Apply(Structure &structure) const {

	// Check applicability.
	if (!IsApplicable(structure))
		return NULL;

	// Return child.
	return structure.GetChild(GetChildIndex());

}

 
Kidnapper *
Kidnapper::Clone() {
	return new Kidnapper;
}
