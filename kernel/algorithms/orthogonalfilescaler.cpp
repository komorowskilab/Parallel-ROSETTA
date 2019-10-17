//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/orthogonalfilescaler.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/iokit.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class OrthogonalFileScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

OrthogonalFileScaler::OrthogonalFileScaler() {
	SetMode(Scaler::MODE_LOAD);
}

OrthogonalFileScaler::~OrthogonalFileScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(OrthogonalFileScaler, ORTHOGONALFILESCALER, OrthogonalScaler)

//-------------------------------------------------------------------
// Methods inherited from Scaler.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetMode
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
OrthogonalFileScaler::SetMode(Scaler::Mode mode) {

	if (mode == Scaler::MODE_LOAD)
		return Scaler::SetMode(mode);

	return false;

}

//-------------------------------------------------------------------
// Methods inherited from OrthogonalScaler.
//===================================================================

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes that indices in cutfile are relative to a
//                 masked table.
//
//                 Partial code overlap between this method and a
//                 small section in CreateDictionary method.
//                 Generalize later on.
//
// Revisions.....:
//===================================================================

bool
OrthogonalFileScaler::FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const {

	// Clear cut vector.
	cuts.erase(cuts.begin(), cuts.end());

	// Convert attribute index if needed.
	if (!masked)
		attribute_no = table.GetMaskedAttribute(attribute_no);

	ifstream stream;

	if (!IOKit::Open(stream, GetFilename()))
		return false;

	// Read cuts for specified attribute.
	while (!IOKit::IsEOF(stream)) {

		String line;

		// Get index (virtual) and cut of attribute.
		if (!IOKit::Load(stream, line, false))
			return false;

    // Allow for blank lines.
		if (line.IsEmpty())
			continue;

    // Split string into (index, cut) pair.
		String index_s = line.Before(String("\t"));
		String cut_s   = line.After(String("\t"));

		Message message;


    // Verify types.
		if (!index_s.IsInteger() || !cut_s.IsFloat()) {
			Message::Error("Error reading (" + line.GetString() + ") pair from " + GetFilename() +".");
			return false;
		}

    // Extract values.
		int   index = index_s.GetInteger();
		float cut   = cut_s.GetFloat();

		// Is the read cut not for the specified attribute?
		if (index != attribute_no)
			continue;
			cuts.push_back(cut);
	
	}
	IOKit::Close(stream);

	return true;

}

OrthogonalFileScaler *
OrthogonalFileScaler::Clone() {
	return new OrthogonalFileScaler;
}
