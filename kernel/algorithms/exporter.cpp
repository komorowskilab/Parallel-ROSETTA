//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/exporter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/basic/message.h>

#include <kernel/utilities/iokit.h>

#include <kernel/system/fstream.h>

#include "omp.h"

//-------------------------------------------------------------------
// Methods for class Exporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Exporter::Exporter() {
	filename_ = Undefined::String();
}

Exporter::~Exporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Exporter, EXPORTER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Exporter::GetParameters() const {

	String parameters;

	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();

	return parameters;

}

//-------------------------------------------------------------------
// Method........: GetOutputFilenames
// Author........: Aleksander šhrn
// Date..........:
// Description...: See Algorithm::GetOutputFilenames method.
// Comments......:
// Revisions.....:
//===================================================================

bool
Exporter::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	filenames.push_back(GetFilename());

	return true;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Exporter::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Filename())
		return SetFilename(value);

	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Consider using IOKit instead.
//
//                 Conceptually const only, some temporary
//                 behind-the-scenes internal assignments violate the
//                 method's strict constness.
//
// Revisions.....:
//===================================================================

Structure *
Exporter::Apply(Structure &structure) const {

	if (!IsApplicable(structure))
		return NULL;

	Exporter *self = const_cast(Exporter *, this);

	// Set dictionary to use (if any).
	self->SetDecisionTable(dynamic_cast(DecisionTable *, structure.FindParent(DECISIONTABLE)));

	ofstream stream;

	//int thread_num = omp_get_thread_num();

	String filename = GetFilename();
	//cout << "Filename equal to " << filename << " for thread " << thread_num << endl;
	//cout << "Parameters equal to " << GetParameters() << " for thread " << thread_num << endl;	

	if (!IOKit::Open(stream, filename)) {
		Message::Error("Unable to open output file.");
		return NULL;
	}

	Message message;

	message.Notify("Exporting \"" + structure.GetName() + "\" to \"" + GetFilename() + "\"...");

	// Export the prologue.
	if (!ExportPrologue(stream, structure)) {
		Message::Error("Error saving file prologue.");
		return NULL;
	}

	// Export the data.
	bool test;
	#pragma omp critical(ExportData)
	{
		test = ExportData(stream, structure);
	}
	if (!test) {
		Message::Error("Error saving data section.");
		return NULL;
	}

	// Export the epilogue.
	if (!ExportEpilogue(stream, structure)) {
		Message::Error("Error saving file epilogue.");
		return NULL;
	}

	// Reset dictionary to use (if any).
	self->SetDecisionTable(NULL);

	return &structure;

}

//-------------------------------------------------------------------
// New virtual methods.
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
Exporter::ExportPrologue(ofstream &/*stream*/, const Structure &/*structure*/) const {
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
Exporter::ExportData(ofstream &/*stream*/, const Structure &/*structure*/) const {
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
Exporter::ExportEpilogue(ofstream &/*stream*/, const Structure &/*structure*/) const {
	return true;
}

//-------------------------------------------------------------------
// Method........: SetFilename/GetFilename
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Exporter::SetFilename(const String &filename) {
	filename_ = filename;
	return true;
}

const String &
Exporter::GetFilename() const {
	return filename_;
}

Exporter *
Exporter::Clone() {
	return new Exporter;
}
