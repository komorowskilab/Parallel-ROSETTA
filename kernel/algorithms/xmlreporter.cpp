//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/xmlreporter.h>

#include <kernel/structures/annotatedstructure.h>

#include <kernel/utilities/xmlkit.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class HTMLReporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

XMLReporter::XMLReporter() {
}

XMLReporter::~XMLReporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(XMLReporter, XMLREPORTER, Reporter)

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
XMLReporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	// Output some general stuff.
	stream << "<?xml version=\"1.0\"?>" << endl;
	stream << endl;
	stream << "<!-- Exported from ROSETTA at " << SystemKit::GetTimestamp() << " by " << XMLKit::Escape(SystemKit::GetUser()) << " -->" << endl;
	stream << endl;

	// Output opening tag.
	stream << "<report name=\"" << XMLKit::Escape(structure.GetName()) << "\">" << endl;

	return true;

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
XMLReporter::ExportData(ofstream &stream, const Structure &structure) const {
	return ExportData(stream, structure, "  ");
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
XMLReporter::ExportEpilogue(ofstream &stream, const Structure &/*structure*/) const {

	// Output closing tag.
	stream << "</report>" << endl;

	return true;

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: TODO: Some characters must be escaped.
// Revisions.....:
//===================================================================

bool
XMLReporter::ExportData(ofstream &stream, const Structure &structure, const String &indent) const {

	int i;

	// Get basic data.
	String name = XMLKit::Escape(structure.GetName());
	String type = IdHolder::GetClassname(structure.GetId());

	// Output opening tag.
	stream << indent << "<structure type=\"" << type << "\" name=\"" << name << "\"";

	if (structure.HasMembers()) {
		stream << " size=\"" << structure.GetNoStructures() << "\"";
	}

	if (structure.IsAnnotated() && structure.GetAnnotation()->GetFilename() != Undefined::String()) {
		stream << " filename=\"" << structure.GetAnnotation()->GetFilename() << "\"";
	}

	stream << ">" << endl;

	// Output annotations, if any.
	if (structure.IsAnnotated()) {

		stream << indent << "  <annotations>" << endl;

		Handle<Annotation> annotation = structure.GetAnnotation();
		Handle<History>    history    = const_cast(History *, annotation->GetHistory());

		// History items.
		if (!history.IsNULL()) {
			stream << indent << "    <actions>" << endl;
			for (i = 0; i < history->GetNoEntries(); i++) {
				stream << indent << "      <action user=\"" << XMLKit::Escape(history->GetEntry(i)->GetUser()) << "\" timestamp=\"" << history->GetEntry(i)->GetTimestamp() << "\">" << endl;
				stream << indent << "        " << XMLKit::Escape(history->GetEntry(i)->GetAction()) << endl;
				stream << indent << "      </action>" << endl;
			}
			stream << indent << "    </actions>" << endl;
		}

		String comments = XMLKit::Escape(annotation->GetComments());

		// Comments.
		if (!comments.IsEmpty()) {
			stream << indent << "    <comments>" << endl;
			stream << indent << "    " << comments << endl;
			stream << indent << "    </comments>" << endl;
		}
		
		stream << indent << "  </annotations>" << endl;

	}

	// Output children, recursively.
	for (i = 0; i < structure.GetNoChildren(); i++) {
		if (!ExportData(stream, *(structure.GetChild(i)), indent + "  "))
			return false;
	}

	// Output closing tag.
	stream << indent << "</structure>" << endl;

	return true;

}
 
XMLReporter *
XMLReporter::Clone() {
   return new XMLReporter;
}
