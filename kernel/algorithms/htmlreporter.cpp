//-------------------------------------------------------------------
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/htmlreporter.h>

#include <kernel/structures/annotatedstructure.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class HTMLReporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

HTMLReporter::HTMLReporter() {
}

HTMLReporter::~HTMLReporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(HTMLReporter, HTMLREPORTER, Reporter)

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
HTMLReporter::ExportPrologue(ofstream &stream, const Structure &/*structure*/) const {

	stream << "<HTML>" << endl;
	stream << "<HEAD>" << endl;
	stream << "<TITLE>Report</TITLE>" << endl;
	stream << "</HEAD>" << endl;
	stream << "<BODY TEXT=\"#000000\" BGCOLOR=\"#C0C0C0\" LINK=\"#0000EE\" VLINK=\"#551A8B\" ALINK=\"#FF0000\">" << endl;
	stream << endl;
	stream << "This R<SMALL>OSETTA</SMALL> report was generated by " << SystemKit::GetUser() << " " << SystemKit::GetTimestamp() << endl;
	stream << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
HTMLReporter::ExportData(ofstream &stream, const Structure &structure) const {

	// Tree header.
	stream << "<H2>Tree<HR WIDTH=\"100%\"></H2>" << endl << endl;

	// List, begin.
	stream << "<OL>" << endl;

	if (!ExportTree(stream, structure, "1"))
		return false;

	// List, end.
	stream << "</OL>" << endl;

	if (!ExportData(stream, structure, "1"))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportEpilogue
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
HTMLReporter::ExportEpilogue(ofstream &stream, const Structure &/*structure*/) const {

	stream << endl;
	stream << "</BODY>" << endl;
	stream << "</HTML>" << endl;

	return true;

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportTree
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
HTMLReporter::ExportTree(ofstream &stream, const Structure &structure, const String &label) const {

	// Get name and type of structure.
	String name = structure.GetName();
	String type = IdHolder::GetClassname(structure.GetId());

	// List item.
	stream << "<LI><A HREF=\"#" << label << "\">" << name << "</A> (" << type << ")" << "</LI>" << endl;

	if (!structure.HasChildren())
		return true;

	// List, begin.
	stream << "<OL>" << endl;

	int i;

	// Children list items.
	for (i = 0; i < structure.GetNoChildren(); i++) {
		if (!ExportTree(stream, *(structure.GetChild(i)), label + "." + String::Format(i + 1)))
			return false;
	}

	// List, end.
	stream << "</OL>" << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
HTMLReporter::ExportData(ofstream &stream, const Structure &structure, const String &label) const {

	// Header.
	stream << endl << "<H2><A NAME=\"" << label << "\">" << label << " " << structure.GetName() << "</A><HR WIDTH=\"100%\"></H2>" << endl << endl;

	// Type.
	stream << "<B>Type: </B>" << IdHolder::GetClassname(structure.GetId()) << "<BR>" << endl;

	// Size, if relevant.
	if (structure.HasMembers())
		stream << "<B>Size: </B>" << structure.GetNoStructures() << "<BR>" << endl;

	// Children, if relevant.
	if (structure.HasChildren())
		stream << "<B>No. children: </B>" << structure.GetNoChildren() << "<BR>" << endl;

	// Parent, if any.
	if (structure.FindParent() != NULL) {
		String parentlabel = label;
		parentlabel.Reverse();
		parentlabel = parentlabel.After('.');
		parentlabel.Reverse();
		if (parentlabel.IsEmpty())
			stream << "<B>Parent: </B>" << structure.FindParent()->GetName() << "<BR>" << endl;
		else
			stream << "<B>Parent: </B><A HREF=\"#" << parentlabel << "\">" << structure.FindParent()->GetName() << "</A><BR>" << endl;
	}

	stream << endl << endl;

	int i, j;

	// Annotation, if relevant.
	if (structure.IsAnnotated()) {

		Handle<Annotation> annotation = structure.GetAnnotation();

		// Filename, if any.
		if (annotation->GetFilename() != Undefined::String())
			stream << "<B>Filename: </B>" << annotation->GetFilename() << "<BR>" << endl;

		Handle<History> history = const_cast(History *, annotation->GetHistory());

		// History items, if any.
		if (!history.IsNULL()) {
			if (history->GetNoEntries() > 0) {
				stream << "<B>History:</B><BR>" << endl;
				stream << "<BR>" << endl;
				for (i = 0; i < annotation->GetHistory()->GetNoEntries(); i++) {
					stream << "<I>"
						     << history->GetEntry(i)->GetTimestamp() << " "
								 << history->GetEntry(i)->GetUser() << "</I><BR> "
						     << history->GetEntry(i)->GetAction() << "<BR><BR>" << endl;
				}
			}
		}

		String comments = annotation->GetComments();

		// Comments, if any.
		if (!comments.IsEmpty()) {
			stream << "<B>Comments:</B><BR>" << endl;
			stream << "<BR>" << endl;
			int length = comments.GetLength();
			for (i = 0; i < length; i++) {
				if (comments[i] == '\n')
					stream << "<BR>" << endl;
				else
					stream << comments[i];
			}
			stream << endl;
		}

	}

	stream << endl;

	// Children, recursively.
	for (j = 0; j < structure.GetNoChildren(); j++) {
		if (!ExportData(stream, *(structure.GetChild(j)), label + "." + String::Format(j + 1)))
			return false;
	}

	return true;

}

HTMLReporter *
HTMLReporter::Clone() {
   return new HTMLReporter;
} 