//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/xmlreductexporter.h>

#include <kernel/structures/reducts.h>
#include <kernel/structures/reduct.h>

#include <kernel/utilities/xmlkit.h>
#include <kernel/utilities/systemkit.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class XMLReductExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

XMLReductExporter::XMLReductExporter() {
}

XMLReductExporter::~XMLReductExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(XMLReductExporter, XMLREDUCTEXPORTER, ReductExporter)

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
XMLReductExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	// Output some general stuff.
	stream << "<?xml version=\"1.0\"?>" << endl;
	stream << endl;
	stream << "<!-- Exported from ROSETTA at " << SystemKit::GetTimestamp() << " by " << XMLKit::Escape(SystemKit::GetUser()) << " -->" << endl;
	stream << endl;

	// Output opening tag.
	stream << "<reducts name=\"" << XMLKit::Escape(structure.GetName()) << "\">" << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportEpilogue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider using IOKit instead.
// Revisions.....:
//===================================================================

bool
XMLReductExporter::ExportEpilogue(ofstream &stream, const Structure &/*structure*/) const {

	// Output closing tag.
	stream << "</reducts>" << endl;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from ReductExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportReduct
// Author........: Aleksander šhrn
// Date..........:
// Description...: Overloaded to deal with this syntax.
// Comments......:
// Revisions.....:
//===================================================================

bool
XMLReductExporter::ExportReduct(ofstream &stream, int /*reduct_no*/, const Reduct &reduct) const {

	int i;

	// Define indentation.
	String indent(' ', 2);
	String start(indent);

	// Get decision table.
	Handle<DecisionTable> table = const_cast(DecisionTable *, GetDecisionTable());

	bool masked = true;

	// Get reduct dimensions.
	int no_attributes = reduct.GetNoAttributes();

	// Output opening tag.
	stream << start << "<reduct support=\"" << reduct.GetSupport();
	stream << "\" type=\"" << reduct.GetString(reduct.GetDiscernibilityType()) << "\"";
	stream << " modulo=\"";	
	if (reduct.IsModuloDecision())
		stream << "true";
	else
		stream << "false";	
	stream << "\"";

	if (reduct.GetDiscernibilityType() == Reduct::DISCERNIBILITY_OBJECT && reduct.GetObject() != Undefined::Integer())
		stream << " object=\"" << reduct.GetObject() << "\"";
	
	stream << ">" << endl;

	// Output attributes.
	for (i = 0; i < no_attributes; i++) {
	
		// Lookup name.
		String aname = (table == NULL) ? "a" + String::Format(i) : XMLKit::Escape(table->GetAttributeName(i, masked));

		stream << start << indent << "<attribute name=\"" << aname << "\"/>" << endl;

	}

	// Output closing tag.
	stream << start << "</reduct>" << endl;

	return true;

}

XMLReductExporter *
XMLReductExporter::Clone() {
   return new XMLReductExporter;
} 
