//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....: Aš 000211: Full rewrite.
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/loader.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>

//-------------------------------------------------------------------
// Methods for class Loader.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Loader::Loader() {
	filename_ = Undefined::String();
}

Loader::~Loader() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Loader, LOADER, ScriptAlgorithm)

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
Loader::GetParameters() const {

	String parameters;

	// Filename.
	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();

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
Loader::SetParameter(const String &keyword, const String &value) {

	// Filename.
	if (keyword == Keyword::Filename())
		return SetFilename(value);

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
Loader::IsApplicable(const Structure &/*structure*/, bool /*warn*/) const {
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
Loader::Apply(Structure &structure) const {

	// Check applicability.
	if (!IsApplicable(structure))
		return NULL;

	// Load.
	if (!structure.Load(GetFilename()))
		return NULL;

	return &structure;

}

 
Loader *
Loader::Clone() {
	return new Loader;
}
