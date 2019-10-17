//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/scaler.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/stringattribute.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/system/fstream.h>
#include <kernel/system/math.h>

//-------------------------------------------------------------------
// Methods for class Scaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Scaler::Scaler() {
	SetMode(MODE_SAVE);
	SetFilename(Undefined::String());
	MaskSymbolic(true);
}

Scaler::~Scaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Scaler, SCALER, Algorithm)

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
Scaler::GetParameters() const {

	String parameters;

	parameters += Keyword::Mode();
	parameters += Keyword::Assignment();
	parameters += GetString(GetMode());

	parameters += Keyword::Separator();

	parameters += Keyword::Mask();
	parameters += Keyword::Assignment();
	parameters += String::Format(MaskSymbolic());

	if (GetMode() == MODE_DISCARD)
		return parameters;

	parameters += Keyword::Separator();

	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();

	return parameters;

}

//-------------------------------------------------------------------
// Method........: GetOutputFilenames
// Author........: Aleksander šhrn
// Date..........:
// Description...: See Algorithm::GetOutputFilenames method.
// Comments......:
// Revisions.....:
//===================================================================

bool
Scaler::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	if (GetMode() == MODE_SAVE)
		filenames.push_back(GetFilename());

	return true;

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
Scaler::SetParameter(const String &keyword, const String &value) {

	// Set mode.
	if (keyword == Keyword::Mode()) {
		if (value == GetString(MODE_DISCARD))
			return SetMode(MODE_DISCARD);
		if (value == GetString(MODE_SAVE))
			return SetMode(MODE_SAVE);
		if (value == GetString(MODE_LOAD))
			return SetMode(MODE_LOAD);
		return false;
	}

	// Set filename.
	if (keyword == Keyword::Filename())
		return SetFilename(value);

	// Set masking flag.
	if (keyword == Keyword::Mask() && value.IsBoolean())
		return MaskSymbolic(value.GetBoolean());

	// Unknown keyword.
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
Scaler::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Masking methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetTemporaryAttributeMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) how the maskvector would be if all
//                 symbolic condition attributes were disabled.
// Comments......:
// Revisions.....:
//===================================================================

bool
Scaler::GetTemporaryAttributeMasks(const DecisionTable &table, Vector(DecisionTable::Mask) &masks) const {

	// Clear current contents.
	masks.erase(masks.begin(), masks.end());

	// Operate on an unmasked table.
	bool masked = false;

	// Get table dimensions.
	int no_attributes = table.GetNoAttributes(masked);

	// Reserve space to avoid spurious allocations.
	masks.reserve(no_attributes);

	int i;

	for (i = 0; i < no_attributes; i++) {
		if (table.IsSymbolic(i, masked) && table.IsCondition(i, masked))
			masks.push_back(DecisionTable::MASK_DISABLED);
		else
			masks.push_back(table.GetAttributeMask(i));
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetAttributeMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the attribute masks of the
//                 given decision table.
// Comments......:
// Revisions.....: Aš 971024: Moved code to DecisionTable method.
//===================================================================

bool
Scaler::GetAttributeMasks(const DecisionTable &table, Vector(DecisionTable::Mask) &masks) const {
	return table.GetAttributeMasks(masks);
}

//-------------------------------------------------------------------
// Method........: SetAttributeMasks
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the attribute masks as given by the supplied
//                 mask vector.
//
// Comments......: Use with care since the masks are set in an "unsafe"
//                 manner.
// Revisions.....: Aš 971024: Moved code to DecisionTable method.
//===================================================================

bool
Scaler::SetAttributeMasks(DecisionTable &table, const Vector(DecisionTable::Mask) &masks) const {
	return table.SetAttributeMasks(masks, false);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

bool
Scaler::SetMode(Scaler::Mode mode) {
	mode_ = mode;
	return true;
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Scaler::GetString(Scaler::Mode mode) {

	switch (mode) {
		case MODE_DISCARD:  return "Discard";
		case MODE_SAVE:     return "Save";
		case MODE_LOAD:     return "Load";
		default:            return Undefined::String();
	}

}
