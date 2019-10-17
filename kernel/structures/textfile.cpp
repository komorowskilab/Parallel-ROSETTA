//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/textfile.h>

#include <kernel/utilities/iokit.h>

//-------------------------------------------------------------------
// Methods for class TextFile.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

TextFile::TextFile(const TextFile &in) : AnnotatedStructure(in) {
	filename_ = in.filename_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

TextFile::TextFile() {
  filename_ = Undefined::String();
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

TextFile::~TextFile() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(TextFile, TEXTFILE, AnnotatedStructure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
TextFile::Load(ifstream &stream) {

	if (!AnnotatedStructure::Load(stream))
		return false;

	// Load filename.
	if (!IOKit::Load(stream, filename_))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
TextFile::Save(ofstream &stream) const {

	if (!AnnotatedStructure::Save(stream))
		return false;

	// Save filename.
	if (!IOKit::Save(stream, filename_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
TextFile::Duplicate() const {
  return new TextFile(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
TextFile::Clear() {
	SetFilename(Undefined::String());
	AnnotatedStructure::Clear();
}

 
