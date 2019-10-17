//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/myreductimporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/reduct.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class MyReductImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyReductImporter::MyReductImporter() {
}

MyReductImporter::~MyReductImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyReductImporter, MYREDUCTIMPORTER, ReductImporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a decision table, creates, fills and returns
//                 a reduct set with data imported from a file on
//                 the format that MyReductExporter exports.
//
// Comments......: Assumes that library clients use handles.
// Revisions.....:
//===================================================================

Structure *
MyReductImporter::Apply(Structure &structure) const {

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	ifstream stream;

	if (!IOKit::Open(stream, GetFilename())) {
		Message::Error("Error opening file " + GetFilename() + " for input.");
		return NULL;
	}

	// Create an empty reduct set.
	Handle<Reducts> reducts = Creator::Reducts();

	String line, support;

	Handle<Reduct> previous;

	// Process all lines.
	while (IOKit::Load(stream, line, false)) {

		// Trim away whitespace.
		line.Trim(" \t");

		// Support count of previous reduct?
		if (line.StartsWith("Support")) {
			support = line.After('[');
			support.ToLowercase();
			support = support.Before("subtable");
			support.Trim(" \t");
			if (!support.IsInteger()) {
				Message::Error("Failed to parse " + line + ".");
				return NULL;
			}
			if (previous == NULL || !previous->SetSupport(support.GetInteger()))
				return NULL;
		}

		// Not a reduct specification?
		if (!line.StartsWith('{'))
			continue;

		bool masked = true;

		// Create reduct.
		Handle<Reduct> reduct = Creator::Reduct(line, table.GetPointer(), masked);

		if (reduct == NULL) {
			Message::Error("Failed to parse " + line + ".");
			return NULL;
		}

		if (!reducts->AppendStructure(reduct.GetPointer()))
			return NULL;

		previous = reduct;

	}

	return reducts.Release();

}


MyReductImporter *
MyReductImporter::Clone() {
	return new MyReductImporter;
}
