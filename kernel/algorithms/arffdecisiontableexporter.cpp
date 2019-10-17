//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/arffdecisiontableexporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>

#include <kernel/structures/dictionary.h>

#include <kernel/system/fstream.h>

#include <iostream>
#include <sstream>
//-------------------------------------------------------------------
// Methods for class ArffDecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ArffDecisionTableExporter::ArffDecisionTableExporter() {
}

ArffDecisionTableExporter::~ArffDecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ArffDecisionTableExporter, ARFFDECISIONTABLEEXPORTER, DecisionTableExporter)

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
ArffDecisionTableExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	// Cast to already verified type (in Apply method).
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));

	bool masked = false;

	int i;
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);

	// Write some general information.
	stream << "% Exported " << SystemKit::GetTimestamp() << " from ROSETTA by " << SystemKit::GetUser() << "." << endl;
	stream << "%" << endl;
	stream << "% " << table->GetName() << endl;
	stream << "%" << endl;
	stream << "% " << no_objects << " objects, " << no_attributes << " attributes." << endl;
	stream << endl;

	String relation = table->GetName();
	if (relation.Contains(' ') || relation.Contains('\t') || relation.Contains(','))
		relation = '\"' + relation + '\"';

	// Write header
	stream << "@relation " << relation << endl << endl;

	// Write attribute names and types
	for (i = 0; i < no_attributes; i++) {
		String name = table->GetAttributeName(i, masked);
		String type = "";
		
		if (name.Contains(' ') || name.Contains('\t') || name.Contains(','))
			name = '\"' + name + '\"';

		// Masked attributes correspong to string comments
		if (table->GetAttributeMask(i) == DecisionTable::MASK_DISABLED)
			type = "string";
		// Integers and floats are represented as numeric
		else if (table->IsFloat(i, masked) || table->IsInteger(i, masked))
			type = "numeric";
		// String is represented as a list of all possible values
		else {
			type += "{";

			String values = "";

			int no_entries = table->GetDictionary()->GetAttribute(i)->GetNoEntries();
			for (int j = 0; j < no_entries; j++) {
				String value = table->GetDictionary()->GetAttribute(i)->GetEnumeratedEntry(j);
				values += value;
				if (j < no_entries-1)
					values += ",";
			}
			// Should check for Undefined in addition?
			// It would require to go trough all elements and check for IsMissing()
		
			type += values;	
			type += "}";
		}

		stream << "@attribute " << name << "\t" << type << endl;
	}
	stream << endl;
	stream << "@data" << endl;

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
ArffDecisionTableExporter::ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const {

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
 
ArffDecisionTableExporter *
ArffDecisionTableExporter::Clone() {
	return new ArffDecisionTableExporter;
}
