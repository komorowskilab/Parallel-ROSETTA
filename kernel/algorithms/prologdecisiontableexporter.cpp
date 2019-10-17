//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/prologdecisiontableexporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>

#include <kernel/utilities/systemkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class PrologDecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

PrologDecisionTableExporter::PrologDecisionTableExporter() {
}

PrologDecisionTableExporter::~PrologDecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(PrologDecisionTableExporter, PROLOGDECISIONTABLEEXPORTER, DecisionTableExporter)

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider using IOKit instead.
// Revisions.....:
//===================================================================

bool
PrologDecisionTableExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	// Cast to already verified type (in Apply method).
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));

	bool masked = false;

	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	// Write some general information.
	stream << "% Decision table exported from ROSETTA." << endl;
	stream << "% Exported " << SystemKit::GetTimestamp() << " by " << SystemKit::GetUser() << "." << endl;
	stream << "%" << endl;
	stream << "% " << table->GetName() << endl;
	stream << "%" << endl;
	stream << "% " << no_objects << " objects, " << no_attributes << " attributes." << endl << endl;

	return true;

}

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
PrologDecisionTableExporter::ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const {

  // Extract an inf. vector.
	Handle<InformationVector> inf = Creator::InformationVector();

	if (!inf->Create(table, object_no, masked))
		return false;

	String formatted;

	// Format the inf. vector into a string.
	if (!inf->FormatProlog(formatted, "o" + String::Format(object_no + 1), &table, masked)) {
	  Message::Error("Error formatting information vector.");
		return false;
	}

	// Dump string to file stream.
	stream << formatted << endl << endl;

	return true;

}

PrologDecisionTableExporter *
PrologDecisionTableExporter::Clone() {
   return new PrologDecisionTableExporter;
} 
