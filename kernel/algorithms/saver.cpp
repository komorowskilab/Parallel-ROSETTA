//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....: Aš 000211: Full rewrite.
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/saver.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>

//-------------------------------------------------------------------
// Methods for class Saver.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Saver::Saver() {
	filename_ = Undefined::String();
}

Saver::~Saver() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Saver, SAVER, ScriptAlgorithm)

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
Saver::GetParameters() const {

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
Saver::SetParameter(const String &keyword, const String &value) {

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
Saver::IsApplicable(const Structure &/*structure*/, bool /*warn*/) const {
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
Saver::Apply(Structure &structure) const {

	// Check applicability.
	if (!IsApplicable(structure))
		return NULL;

	// Save.
//	cout << "Filename: |" << GetFilename() << "|"<< endl;
	if (!structure.Save(GetFilename()))
		return NULL;
//	cout << "Saved: |" << GetFilename() << "|"<< endl;
	
	return &structure;

}

Saver *
Saver::Clone() {
	return new Saver;
}
