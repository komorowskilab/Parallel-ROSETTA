//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/decisiontableexporter.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class DecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DecisionTableExporter::DecisionTableExporter() {
}

DecisionTableExporter::~DecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DecisionTableExporter, DECISIONTABLEEXPORTER, Exporter)

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
DecisionTableExporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
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
DecisionTableExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportPrologue(stream, structure);
}

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DecisionTableExporter::ExportData(ofstream &stream, const Structure &structure) const {

	Message message;

	// Cast to type verified in Apply method.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));

	bool ok     = true;
  bool masked = false;

	int i, no_objects = table->GetNoObjects(masked);

	// Do the actual exporting.
	for (i = 0; i < no_objects; i++) {

		if (!message.Progress("Exporting decision table...", i, no_objects))
			break;

		if (!ExportInformationVector(stream, *table, i, masked)) {
			Message::Error("Failed to export information vector.");
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
DecisionTableExporter::ExportEpilogue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportEpilogue(stream, structure);
}
 
