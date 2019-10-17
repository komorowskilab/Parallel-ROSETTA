//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/matlabdecisiontableexporter.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class MatlabDecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MatlabDecisionTableExporter::MatlabDecisionTableExporter() {
}

MatlabDecisionTableExporter::~MatlabDecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MatlabDecisionTableExporter, MATLABDECISIONTABLEEXPORTER, DecisionTableExporter)

//-------------------------------------------------------------------
// Methods inherited from DecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportInformationVector
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
MatlabDecisionTableExporter::ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const {

	int i, no_attributes = table.GetNoAttributes(masked);

	// Save numerical entries.
	for (i = 0; i < no_attributes; i++) {

		String entry;

		// Get the entry.
		if (table.IsMissing(object_no, i, masked))
			entry = "NaN";
		else
			entry = table.GetEntry(object_no, i, table.IsNumeric(i, masked), masked);

		// Dump to file stream.
		stream << entry;

		if (i < no_attributes - 1)
			stream << ' ';
		else
			stream << endl;

	}

	return true;

}

MatlabDecisionTableExporter *
MatlabDecisionTableExporter::Clone() {
   return new MatlabDecisionTableExporter;
} 
