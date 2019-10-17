//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/xmldecisiontableexporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>

#include <kernel/utilities/xmlkit.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class XMLDecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

XMLDecisionTableExporter::XMLDecisionTableExporter() {
}

XMLDecisionTableExporter::~XMLDecisionTableExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(XMLDecisionTableExporter, XMLDECISIONTABLEEXPORTER, DecisionTableExporter)

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
XMLDecisionTableExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	int i;

	// Output some general stuff.
	stream << "<?xml version=\"1.0\"?>" << endl;
	stream << endl;
	stream << "<!-- Exported from ROSETTA at " << SystemKit::GetTimestamp() << " by " << XMLKit::Escape(SystemKit::GetUser()) << " -->" << endl;
	stream << endl;

	// Output opening tag.
	stream << "<decisiontable name=\"" << XMLKit::Escape(structure.GetName()) << "\">" << endl;

	// Cast to already verified type (in Apply method).
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));

	bool   masked(false);
	String indent(' ', 2);

	// Get table dimensions.
	int no_attributes = table->GetNoAttributes(masked);

	// Output attribute information.
	stream << indent << "<attributes>" << endl;

	for (i = 0; i < no_attributes; i++) {

		// Get attribute data.
		String type   = Attribute::GetString(table->GetAttributeType(i, masked));
		String name   = XMLKit::Escape(table->GetAttributeName(i, masked));
		String unit   = XMLKit::Escape(table->GetAttributeUnit(i, masked));
		String status = DecisionTable::GetString(table->GetStatus(i, masked));

		stream << indent << indent << "<attribute id=\"" << i << "\" name=\"" << name << "\" type=\"" << type << "\"";

		if (table->IsFloat(i, masked))
			stream << " exponent=\"" << table->GetAttributeScalingExponent(i, masked) << "\"";

		if (unit != Undefined::String())
			stream << " unit=\"" << unit << "\"";

		if (table->IsAttributeDisabled(i))
			stream << " masked=\"true\"";

		if (table->IsDecision(i, masked))
			stream << " status=\"" << status << "\"";

		stream << "/>" << endl;

	}

	stream << indent << "</attributes>" << endl;

	stream << indent << "<objects>" << endl;

	return true;

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
XMLDecisionTableExporter::ExportEpilogue(ofstream &stream, const Structure &/*structure*/) const {
	
	String indent(' ', 2);

	// Output closing tags.
	stream << indent << "</objects>" << endl;
	stream << "</decisiontable>" << endl;

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
XMLDecisionTableExporter::ExportInformationVector(ofstream &stream, const DecisionTable &table, int object_no, bool masked) const {

	String indent(' ', 2);
	String start(indent);

	// Get table dimensions.
	int no_attributes = table.GetNoAttributes(masked);

	// Output opening tag.
	stream << start << indent << "<object id=\"" << object_no << "\"";

	if (table.IsObjectDisabled(object_no))
		stream << " masked=\"true\"";

	stream << ">" << endl;

	int i;

	// Output descriptors.
	for (i = 0; i < no_attributes; i++) {

		// Skip undefined entries.
		if (table.GetEntry(object_no, i, masked) == Undefined::Integer())
			continue;

		String aname = XMLKit::Escape(table.GetAttributeName(i, masked));
		String vname = XMLKit::Escape(table.GetEntry(object_no, i, true, masked));

		stream << start << indent << indent <<"<descriptor attribute=\"" << aname << "\" value=\"" << vname << "\"/>" << endl;

	}

	// Output closing tag.
	stream << start << indent << "</object>" << endl;

	return true;

}

XMLDecisionTableExporter *
XMLDecisionTableExporter::Clone() {
   return new XMLDecisionTableExporter;
}
 
