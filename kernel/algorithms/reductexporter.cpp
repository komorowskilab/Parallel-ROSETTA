//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reductexporter.h>

#include <kernel/structures/reducts.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class ReductExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ReductExporter::ReductExporter() {
}

ReductExporter::~ReductExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReductExporter, REDUCTEXPORTER, Exporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

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
ReductExporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(REDUCTS);
}

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ReductExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportPrologue(stream, structure);
}

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Merge code with RuleExporter.
// Revisions.....:
//===================================================================

bool
ReductExporter::ExportData(ofstream &stream, const Structure &structure) const {

	Message message;

	bool ok = true;

	// Cast to type verified in Apply method.
	Handle<Reducts> reducts = dynamic_cast(Reducts *, const_cast(Structure *, &structure));

	int i, no_reducts = reducts->GetNoReducts();

	// Do the actual exporting.
	for (i = 0; i < no_reducts; i++) {

		if (!message.Progress("Exporting reducts...", i, no_reducts))
			break;

		Handle<Reduct> reduct = reducts->GetReduct(i);

		// Export the reduct (format specifics are overloaded).
		if (!ExportReduct(stream, i, *reduct)) {
			Message::Error("Failed to export reduct.");
			ok = false;
			break;
		}

	}

	/*
	// Make sure all windows are popped down.
	if (!ok)
		Message::Reset();
	*/

	return ok;

}

//-------------------------------------------------------------------
// Method........: ExportEpilogue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ReductExporter::ExportEpilogue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportEpilogue(stream, structure);
}

