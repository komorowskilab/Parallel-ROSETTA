//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/mydecisiontableexporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class MyDecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyDecisionTableExporter::MyDecisionTableExporter() {
}

MyDecisionTableExporter::~MyDecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyDecisionTableExporter, MYDECISIONTABLEEXPORTER, DecisionTableExporter)

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
MyDecisionTableExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

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

	// Write attribute names, possibly padded.
	for (i = 0; i < no_attributes; i++) {
		String name = table->GetAttributeName(i, masked);
		if (name.Contains(' ') || name.Contains('\t') || name.Contains(','))
			name = '\"' + name + '\"';
		stream << name;
		if (i < no_attributes - 1)
			stream << '\t';
		else
			stream << endl;
	}

	// Write attribute types, possibly with exponent fields.
	for (i = 0; i < no_attributes; i++) {
		String type = Attribute::GetString(table->GetAttributeType(i, masked));
		if (table->IsFloat(i, masked))
			type += '(' + String::Format(table->GetAttributeScalingExponent(i, masked)) + ')';
		stream << type;
		if (i < no_attributes - 1)
			stream << '\t';
		else
			stream << endl;
	}

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
MyDecisionTableExporter::ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const {

	int i, no_attributes = table.GetNoAttributes(masked);

	// Save textual entries, possibly padded.
	for (i = 0; i < no_attributes; i++) {
		String entry = table.GetEntry(object_no, i, true, masked);
		if (entry.Contains(' ') || entry.Contains('\t') || entry.Contains(','))
			entry = '\"' + entry + '\"';
		stream << entry;
		if (i < no_attributes - 1)
			stream << '\t';
		else
			stream << endl;
	}

	return true;

}

MyDecisionTableExporter *
MyDecisionTableExporter::Clone() {
   return new MyDecisionTableExporter;
} 
