//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/structurecreator.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>

#include <kernel/basic/idholder.h>

#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class StructureCreator.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

StructureCreator::StructureCreator() {
	output_ = Undefined::Id();
}

StructureCreator::~StructureCreator() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(StructureCreator, STRUCTURECREATOR, ScriptAlgorithm)

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
StructureCreator::GetParameters() const {

	String parameters;

	// Output type.
	parameters += Keyword::Output();
	parameters += Keyword::Assignment();
	parameters += IdHolder::GetClassname(GetOutputType());

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
StructureCreator::SetParameter(const String &keyword, const String &value) {

	// Output type.
	if (keyword == Keyword::Output())
		return SetOutputType(IdHolder::GetId(value));

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
StructureCreator::IsApplicable(const Structure &/*structure*/, bool /*warn*/) const {
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
StructureCreator::Apply(Structure &structure) const {

	// Check applicability.
	if (!IsApplicable(structure))
		return NULL;

	// Sanity check.
	if (GetOutputType() == Undefined::Id())
		return NULL;

	// Create structure.
	Handle<Structure> output = Creator::Create(GetOutputType());

	return output.Release();

}

 
StructureCreator *
StructureCreator::Clone() {
	return new StructureCreator;
}
