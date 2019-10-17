//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/myposdecisiontableimporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/discernibilityfunction.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/dictionary.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/map.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class MyDecisionTableImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyPOSDecisionTableImporter::MyPOSDecisionTableImporter() {
}

MyPOSDecisionTableImporter::~MyPOSDecisionTableImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyPOSDecisionTableImporter, MYPOSDECISIONTABLEIMPORTER, DecisionTableImporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
MyPOSDecisionTableImporter::Apply(Structure &structure) const {

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	ifstream stream;

	if (!IOKit::Open(stream, GetFilename())) {
		Message::Error("Error opening file \"" + GetFilename() + "\" for input.");
		return NULL;
	}

	// Parse function and build table.
	if (!Parse(stream, *table))
		return NULL;

	return table.Release();

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Parse
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: It is really very inefficient to read the full
//                 function def. before starting to parse it. The
//                 reason being potentially incredibly many
//                 reallocations in the String object holding the
//                 def., as individual lines get appended.
//
//                 However, the current implementation allows reuse
//                 of existing code.
//
//                 Currently, this method should not be used together
//                 with large function definitions.
//
// Revisions.....:
//===================================================================

bool
MyPOSDecisionTableImporter::Parse(ifstream &stream, DecisionTable &table) const {

	String function, line;

	// Reserve a chunk of 1M. Hopefully the function def. doesn't exceed that.
	function.Reserve(1024 * 1024);

	Message().Notify("Loading function definition...");

	// Load function definition.
	while (IOKit::LoadLine(stream, line)) {
		if (line == "end")
			break;
		function += line;
	}

	// Parse function and build table.
	return Parse(function, table);

}

//-------------------------------------------------------------------
// Method........: Parse
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The compiler requires that the call to Create is
//                 explicitly qualified... I don't know why, the
//                 call signatures seem unique..
// Revisions.....:
//===================================================================

bool
MyPOSDecisionTableImporter::Parse(const String &function, DecisionTable &table) const {

	Message message;

	// Define a map from attribute names to attribute indices.
	Map(String, int) names;

	DiscernibilityFunction f;

	message.Notify("Creating function...");

	// Create function.
	if (!f.BooleanPOSFunction::Create(function, names))
		return false;

	message.Notify("Creating table...");

	// Build table and dictionary.
	return BuildDecisionTable(table, f) && BuildDictionary(table, names);

}

//-------------------------------------------------------------------
// Method........: BuildDecisionTable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
MyPOSDecisionTableImporter::BuildDecisionTable(DecisionTable &table, const DiscernibilityFunction &function) const {

	table.SetName(function.GetName());

	// Remove existing dictionary.
	table.SetDictionary(NULL);

	// Add an extra object at the top and an extra index attribute.
	int no_objects    = function.GetNoSums() + 1;
	int no_attributes = function.GetArity(false) + 1;

	bool masked = false;

	if (!table.Resize(no_objects, no_attributes, masked))
		return false;

	int i, j;

	// Add first row, all zeros.
	for (i = 0; i < no_attributes; i++) {
		if (!table.SetEntry(0, i, 0, masked))
			return false;
	}

	// Add subsequent rows.
	for (j = 1; j < no_objects; j++) {

		// Set all 0s as default.
		for (i = 0; i < no_attributes - 1; i++) {
			if (!table.SetEntry(j, i, 0, masked))
				return false;
		}

		const Bits *sum = function.GetSum(j - 1);

		// Set 1 if the current sum contains the attribute.
		for (i = 0; i < sum->GetSize(); i++) {
			if ((*sum)[i]) {
				if (!table.SetEntry(j, i, 1, masked))
					return false;
			}
		}

		// Set index as decision.
		if (!table.SetEntry(j, no_attributes - 1, j, masked))
			return false;

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: BuildDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
MyPOSDecisionTableImporter::BuildDictionary(DecisionTable &table, const Map(String, int) &names) const {

	Handle<Dictionary> dictionary = Creator::Dictionary();

	int i, no_attributes = names.size() + 1;

	// Create dummy dictionary.
	for (i = 0; i < no_attributes; i++)
		dictionary->AppendAttribute(Creator::IntegerAttribute());

	table.SetDictionary(dictionary.GetPointer());

	Map(String, int)::const_iterator iterator = names.begin();

	// Set attribute names.
	while (!(iterator == names.end())) {
		table.SetAttributeName((*iterator).second, (*iterator).first, false);
		iterator++;
	}

	table.SetAttributeName(no_attributes - 1, "Index", false);

	return true;

}
 
MyPOSDecisionTableImporter *
MyPOSDecisionTableImporter::Clone() {
	return new MyPOSDecisionTableImporter;
}
