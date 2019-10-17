//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...: An annotated structure is a structure with an
//                 annotation object attached to it.
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/annotatedstructure.h>
#include <kernel/structures/annotation.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class AnnotatedStructure.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

AnnotatedStructure::AnnotatedStructure(const AnnotatedStructure &in) : Structure(in) {

	if (in.IsAnnotated())
		annotation_ = dynamic_cast(Annotation *, in.GetAnnotation()->Duplicate());
	else
		annotation_ = NULL;

}

AnnotatedStructure::AnnotatedStructure() {
	annotation_ = Creator::Annotation();
}

AnnotatedStructure::~AnnotatedStructure() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(AnnotatedStructure, ANNOTATEDSTRUCTURE, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Load(ifstream &stream) {

	if (!Structure::Load(stream))
		return false;

	bool is_annotated;

	// Load annotation presence flag.
	if (!IOKit::Load(stream, is_annotated))
		return false;

	if (is_annotated)
		SetAnnotation(Creator::Annotation());
	else
		SetAnnotation(NULL);

	// Load annotation, if present.
	if (is_annotated && !GetAnnotation()->Load(stream))
		return false;

  return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Save(ofstream &stream) const {

	if (!Structure::Save(stream))
		return false;

	// Save annotation presence flag.
	if (!IOKit::Save(stream, IsAnnotated()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	// Save annotation, if present.
	if (IsAnnotated() && !GetAnnotation()->Save(stream))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Load(const String &filename) {

	if (!Persistent::Load(filename))
		return false;

	// Set default filename/location.
	if (IsAnnotated())
		GetAnnotation()->SetFilename(filename);

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Save(const String &filename) const {

	if (!Persistent::Save(filename))
		return false;

	// Set default filename/location.
	if (IsAnnotated())
		GetAnnotation()->SetFilename(filename);

	return true;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...: Loads from default filename/location.
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Load() {

	if (IsAnnotated())
		return Load(GetAnnotation()->GetFilename());

	return false;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...: Saves to default filename/location.
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Save() const {

	if (IsAnnotated())
		return Save(GetAnnotation()->GetFilename());

	return false;

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
AnnotatedStructure::Duplicate() const {
  return new AnnotatedStructure(*this);
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
AnnotatedStructure::Clear() {

	if (IsAnnotated())
		GetAnnotation()->Clear();

}

//-------------------------------------------------------------------
// Method........: GetName
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the name of the structure.
// Comments......: Overloads default behaviour.
// Revisions.....:
//===================================================================

const String &
AnnotatedStructure::GetName() const {

	if (IsAnnotated())
		return GetAnnotation()->GetName();

	return Structure::GetName();

}

//-------------------------------------------------------------------
// Method........: SetName
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the name of the structure.
// Comments......: Overloads default behaviour.
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::SetName(const String &name) {

	if (!IsAnnotated())
		return false;

	return GetAnnotation()->SetName(name);

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetAnnotation
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Annotation *
AnnotatedStructure::GetAnnotation() const {
	return const_cast(Annotation *, annotation_.GetPointer());
}

//-------------------------------------------------------------------
// Method........: SetAnnotation
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::SetAnnotation(Annotation *annotation)  {

	annotation_ = annotation;

	return true;

}

//-------------------------------------------------------------------
// Method........: Touch
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AnnotatedStructure::Touch(const String &action) {

	if (IsAnnotated())
		return GetAnnotation()->Touch(action);

	return false;

}
 
