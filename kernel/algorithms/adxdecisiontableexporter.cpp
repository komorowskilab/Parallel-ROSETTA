//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/adxdecisiontableexporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>

#include <kernel/structures/dictionary.h>

#include <kernel/system/fstream.h>

#include <iostream>
#include <sstream>
//-------------------------------------------------------------------
// Methods for class AdxDecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

AdxDecisionTableExporter::AdxDecisionTableExporter() {
}

AdxDecisionTableExporter::~AdxDecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(AdxDecisionTableExporter, ADXDECISIONTABLEEXPORTER, DecisionTableExporter)

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Susanne Bornelöv, Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AdxDecisionTableExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	// Cast to already verified type (in Apply method).
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));

	bool masked = false;

	int i;
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	// Write some general information.
	stream << "# Exported " << SystemKit::GetTimestamp() << " from ROSETTA by " << SystemKit::GetUser() << "." << endl;
	stream << "#" << endl;
	stream << "# " << table->GetName() << endl;
	stream << "#" << endl;
	stream << "# " << no_objects << " objects, " << no_attributes << " attributes." << endl;
	stream << endl;

	stream << "attributes" << endl << "{" << endl;

	// Write attribute names and types
	for (i = 0; i < no_attributes; i++) {
		String name = table->GetAttributeName(i, masked);
		String type = "";
		
		if (name.Contains(' ') || name.Contains('\t') || name.Contains(','))
			name = '\"' + name + '\"';

		// Integers and floats are represented as numeric
		if (table->IsFloat(i, masked) || table->IsInteger(i, masked))
			type = "numeric";
		// String is represented as a list of all possible values
		else
			type = "nominal";

		// Decision attributes should be treated specially
		if (table->IsDecision(i, masked)) {
			type += " decision(";
			
			String values = "";
			int no_entries = table->GetDictionary()->GetAttribute(i)->GetNoEntries();
			for (int j = 0; j < no_entries; j++) {
				String value = table->GetDictionary()->GetAttribute(i)->GetEnumeratedEntry(j);
				values += value;
				if (j < no_entries-1)
					values += ",";
			}

			type += values;
			type += ")";
		}

		// Masked attributes correspong to ignored attributes
		if (table->GetAttributeMask(i) == DecisionTable::MASK_DISABLED)
		type += " ignore";

		stream << " " << name << "\t" << type << endl;
	}
	stream << "}" << endl << "events" << endl << "{" << endl;

	return true;
}
//-------------------------------------------------------------------
// Method........: ExportEpilogue
// Author........: Susanne Bornelöv
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AdxDecisionTableExporter::ExportEpilogue(ofstream &stream, const Structure &structure) const {

	stream << "}";

	return true;
}

//-------------------------------------------------------------------
// Methods inherited from DecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportInformationVector
// Author........: Susanne Bornelöv, Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
AdxDecisionTableExporter::ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const {

	int i, no_attributes = table.GetNoAttributes(masked);

	// Save textual entries, possibly padded.
	for (i = 0; i < no_attributes; i++) {
		String entry = table.GetEntry(object_no, i, true, masked);
		if (entry.Contains(' ') || entry.Contains('\t') || entry.Contains(','))
			entry = '\"' + entry + '\"';
		stream << entry;
		if (i < no_attributes - 1)
			stream << ',';
		else
			stream << endl;
	}

	return true;

}
 
AdxDecisionTableExporter *
AdxDecisionTableExporter::Clone() {
	return new AdxDecisionTableExporter;
}
