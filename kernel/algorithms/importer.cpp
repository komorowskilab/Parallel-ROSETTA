//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/importer.h>
#include <kernel/algorithms/keyword.h>

//-------------------------------------------------------------------
// Methods for class Importer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Importer::Importer() {
	filename_ = Undefined::String();
}

Importer::~Importer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Importer, IMPORTER, Algorithm)

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
Importer::GetParameters() const {

	String parameters;

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
Importer::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Filename())
		return SetFilename(value);

	return false;

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetFilename/GetFilename
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Importer::SetFilename(const String &filename) {
	filename_ = filename;
	return true;
}

const String &
Importer::GetFilename() const {
	return filename_;
}
 
