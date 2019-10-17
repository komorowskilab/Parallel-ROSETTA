//-------------------------------------------------------------------
// Author........: Susanne Bornelöv
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/arffdecisiontableimporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/dictionary.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class ArffDecisionTableImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ArffDecisionTableImporter::ArffDecisionTableImporter() {
}

ArffDecisionTableImporter::~ArffDecisionTableImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ArffDecisionTableImporter, ARFFDECISIONTABLEIMPORTER, DecisionTableImporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Susanne Bornelöv, Aleksander šhrn
// Date..........:
// Description...: Given a decision table, empties it and fills it
//                 with data imported from a file on alien format.
//
// Comments......: 
// Revisions.....: 
//===================================================================

Structure *
ArffDecisionTableImporter::Apply(Structure &structure) const {

	Message message;

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

	Vector(String)          names;
	Vector(Attribute::Type) types;
	Vector(int)             exponents;
	Vector(DecisionTable::Mask) masks;

	int line_no = 0;


	bool dimensions_supplied    = false;
	int  no_objects_supplied    = 0;
	int  no_attributes_supplied = 0;

	message.Notify("Importing attribute names and types...");

	// Import names and types.
	if (!ImportNamesAndTypes(stream, names, types, exponents, line_no, masks))
		return NULL;

	bool masked = false;

	message.Notify("Initializing table and dictionary...");

	// Initialize table.
	if (!InitializeTable(*table, masked, names, types, exponents))
		return NULL;

	if (dimensions_supplied && !table->Reserve(no_objects_supplied, no_attributes_supplied))
		return NULL;

	message.Notify("Importing data...");

	// Import data.
	if (!ImportData(stream, *table, masked, line_no))
		return NULL;

	// Set attribute masks
	if (!table->SetAttributeMasks(masks))
		Message::Warning("Failed to apply attribute masks",false);

	return table.Release();

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetTokens
// Author........: Aleksander šhrn
// Date..........:
// Description...: Splits the line into several tokens. The tokens
//                 are returned in-place.
// Comments......:
// Revisions.....: Aš 000221: Call line.GetLength() just once...
//                            Huge savings for very wide tables.
//===================================================================

bool
ArffDecisionTableImporter::GetTokens(const String &line, Vector(String) &tokens) const {

	const char space   = ' ';
	const char tab     = '\t';
	const char newline = '\n';
	const char quote   = '\"';
	const char comma   = ',';

	String whitespace;

	whitespace += space;
	whitespace += tab;
	whitespace += newline;

	// How long is the line to scan for tokens?
	int length = line.GetLength();

	// Does the line contain only whitespace?
	if (line.Count(space) + line.Count(tab) + line.Count(newline) == length)
		return false;

	// Clear token vector.
	tokens.erase(tokens.begin(), tokens.end());

	int current = 0;

	// Scan line from left to right.
	while (current < length) {

		String token;

		// Build token.
		while (current < length) {

			// Get current symbol.
			char c = line[current];

			// Is there a quoted string in progress. e.g. "Hello world"?
			bool is_quoted_string_in_progress = (token.Count(quote) == 1);

			// Are we done building the current token?
			// Quoted strings may contain whitespace. Otherwise, whitespace is a delimiter.
			if (!is_quoted_string_in_progress && whitespace.Contains(c))
				break;

			current++;

			// Unless a quoted string is in progress, consider a comma a delimiter.
			if (!is_quoted_string_in_progress && c == comma)
				break;

			token += c;

			// If the quote found its match, the token is considered complete.
			if (is_quoted_string_in_progress && c == quote)
				break;

		}

		// Post-process token.
		token.Trim(quote);
		if (token.IsEmpty())
			token = Undefined::String();

		tokens.push_back(token);

		// Scan to next non-whitespace symbol.
		while (current < length && whitespace.Contains(line[current]))
			current++;

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ImportNamesAndTypes
// Author........: Susanne Bornelöv
// Date..........: 2010.10.04
// Description...: 
// Comments......: Needed for file types where name and type are on the
// 									same line, like arff, adx, ...
// Revisions.....:
//===================================================================

bool
ArffDecisionTableImporter::ImportNamesAndTypes(ifstream &stream, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &exponents, int &line_no, Vector(DecisionTable::Mask) &masks) const {

	String line;

	// Delete old stuff.
	names.erase(names.begin(), names.end());
	types.erase(types.begin(), types.end());
	exponents.erase(exponents.begin(), exponents.end());

	// Should memory reservation be done?
	// Hopefully not.

	// Count no. of attributes
	int attribute_no = 0;
	String type;

	// Read lines until @data
	while (IOKit::LoadLine(stream, line, &line_no)) {
		
		Vector(String) tokens;
	
		// Extract tokens.
		if (!GetTokens(line, tokens))
			continue; // Empty line? Try next line...

		tokens[0].ToLowercase();

		// Check if attribute is found
		if (tokens[0] == "@attribute") {
			tokens[1].Trim('\"');
			names.push_back(tokens[1]);

			if (tokens[2][0] == '{') { // stringname
				types.push_back(Attribute::TYPE_STRING);
				exponents.push_back(0);
				masks.push_back(DecisionTable::MASK_ENABLED);
			}
			else if (tokens[2] == "numeric") { // intname or floatname
				types.push_back(Attribute::TYPE_FLOAT);
				exponents.push_back(4); // Guess
				masks.push_back(DecisionTable::MASK_ENABLED);
			}
			else if (tokens[2] == "string") { // stringname, mask_disabled
				types.push_back(Attribute::TYPE_STRING);
				exponents.push_back(0);
				masks.push_back(DecisionTable::MASK_DISABLED);
			}
			else {
				Message::Warning("Unrecognized type token (" + tokens[2] + ") in line " + String::Format(line_no) + ". Assuming string type.", false);
				types.push_back(Attribute::TYPE_STRING);
				exponents.push_back(0);
				masks.push_back(DecisionTable::MASK_ENABLED);
			}	

			attribute_no++;
		}
		else if (tokens[0] == "@data")
			return (attribute_no>0);
	}

	Message::Warning("@data tag missing",false);
	return false;
}

//-------------------------------------------------------------------
// Method........: ImportNames
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ArffDecisionTableImporter::ImportNames(ifstream &stream, Vector(String) &names, int &line_no) const {

	String line;

	// Initialize vector.
	names.erase(names.begin(), names.end());

	// Extract name tokens from line.
	if (!IOKit::LoadLine(stream, line, &line_no))
		return false;

	Message::Debug(line);

	return GetTokens(line, names);

}

//-------------------------------------------------------------------
// Method........: ImportTypes
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ArffDecisionTableImporter::ImportTypes(ifstream &stream, Vector(Attribute::Type) &types, Vector(int) &exponents, int &line_no, int no_attributes) const {

	String stringname  = Attribute::GetString(Attribute::TYPE_STRING);
	String integername = Attribute::GetString(Attribute::TYPE_INTEGER);
	String floatname   = Attribute::GetString(Attribute::TYPE_FLOAT);

	// For case-insensitivity.
	stringname.ToLowercase();
	integername.ToLowercase();
	floatname.ToLowercase();

	String         line;
	Vector(String) tokens;

	// Extract name tokens from line.
	if (!IOKit::LoadLine(stream, line, &line_no))
		return false;

	Message::Debug(line);

	if (!GetTokens(line, tokens))
		return false;

	// Expected number of tokens?
	if (tokens.size() != no_attributes) {
#ifdef X86_64	
		Message::Error("Unexpected number of tokens in line " + String::Format((int)line_no) + ".  Expected " + String::Format((int)no_attributes) + ", found " + String::Format((int)tokens.size()) + ".");
#else
		Message::Error("Unexpected number of tokens in line " + String::Format(line_no) + ".  Expected " + String::Format(no_attributes) + ", found " + String::Format(tokens.size()) + ".");

#endif
		return false;
	}

	// Initialize vectors.
	types.erase(types.begin(), types.end());
	exponents.erase(exponents.begin(), exponents.end());

	types.reserve(tokens.size());
	exponents.reserve(tokens.size());

	int i;

	// Process type tokens, extract exponent fields.
	for (i = 0; i < tokens.size(); i++) {

		tokens[i].ToLowercase();

		// String type?
		if (tokens[i] == stringname) {
			types.push_back(Attribute::TYPE_STRING);
			exponents.push_back(0);
		}

		// Integer type?
		else if (tokens[i] == integername) {
			types.push_back(Attribute::TYPE_INTEGER);
			exponents.push_back(0);
		}

		// Float type?
		else if (tokens[i].GetSubstring(0, floatname.GetLength()) == floatname) {
			types.push_back(Attribute::TYPE_FLOAT);
			String exponent = tokens[i].GetSubstring(floatname.GetLength(), tokens[i].GetLength() - floatname.GetLength());
			exponent.Trim('(');
			exponent.Trim(')');
			if (!exponent.IsInteger()) {
				Message::Error("Unrecognized scaling exponent for attribute " + String::Format(i + 1) + ".");
				exponents.push_back(0);
			}
			else {
				exponents.push_back(exponent.GetInteger());
			}
		}

		// Unknown type?
		else {
			Message::Warning("Unrecognized type token (" + tokens[i] + ") in line " + String::Format(line_no) + ".  Assuming string type.", false);
			types.push_back(Attribute::TYPE_STRING);
			exponents.push_back(0);
		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ImportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes the decision table has been initialized.
// Revisions.....:
//===================================================================

bool
ArffDecisionTableImporter::ImportData(ifstream &stream, DecisionTable &table, bool masked, int &line_no) const {

	String line;

	int object_no = 0;

	// Get table dimension.
	int no_attributes = table.GetNoAttributes(masked);

	// Read all data fields.
	while (IOKit::LoadLine(stream, line, &line_no)) {

		// For debugging purposes.
		Message::Debug(line);

		Vector(String) tokens;

		// Extract tokens.
		if (!GetTokens(line, tokens))
			return false;

		if (tokens.size() != no_attributes) {
#ifdef X86_64	
			Message::Error("Unexpected number of tokens in line " + String::Format((int)line_no) + ".  Expected " + String::Format((int)no_attributes) + ", found " + String::Format((int)tokens.size()) + ".");
#else
			Message::Error("Unexpected number of tokens in line " + String::Format(line_no) + ".  Expected " + String::Format(no_attributes) + ", found " + String::Format(tokens.size()) + ".");

#endif
			return false;
		}

		// Append an object to the table, unless it is the first data line. (A single object was already
		// added in the InitializeTable method.)
		if ((object_no > 0) && !table.AppendObject(masked)) {
			Message::Error("Error appending object " + String::Format(object_no + 1) + " to decision table.");
			return false;
		}

		int i;

		// Update table and dictionary.
		for (i = 0; i < tokens.size(); i++) {
			tokens[i].Trim('\"');
			int suggested = table.SuggestDictionaryEntry(i, tokens[i], masked);
			if (!table.SetEntry(object_no, i, suggested, masked)) {
				Message::Error("Error setting table entry (" + String::Format(object_no) + ", " + String::Format(i) + ").");
				return false;
			}
			if (!table.SetDictionaryEntry(i, suggested, tokens[i], masked)) {
				Message::Error("Error setting dictionary entry (" + String::Format(i) + ", " + String::Format(suggested) + ", \"" + tokens[i] + "\").");
				return false;
			}
		}

		object_no++;

	}

	return (object_no > 0);

}

//-------------------------------------------------------------------
// Method........: InitializeTable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ArffDecisionTableImporter::InitializeTable(DecisionTable &table, bool masked, const Vector(String) &names, const Vector(Attribute::Type) &types, const Vector(int) &exponents) const {

	int i;

	// Verify dimensions.
	if (names.size() != types.size())
		return false;

	if (names.size() != exponents.size())
		return false;

	int no_attributes = names.size();

	// Remove all children of the input table.
	table.RemoveAllChildren();

	// Resize/empty the table. Let the table have one initial object.
	table.Resize(1, no_attributes, masked);

	// Assign the table a new, virgin dictionary.
	table.SetDictionary(Creator::Dictionary());

	// Create and insert attributes into the dictionary, making it the same size as the table.
	for (i = 0; i < no_attributes; i++) {

		Handle<Attribute> attribute;

		// String type?
		if (types[i] == Attribute::TYPE_STRING) {
			attribute = Creator::StringAttribute();
		}

		// Integer type?
		else if (types[i] == Attribute::TYPE_INTEGER) {
			attribute = Creator::IntegerAttribute();
		}

		// Float type?
		else if (types[i] == Attribute::TYPE_FLOAT) {
			attribute = Creator::FloatAttribute(exponents[i], NULL, false);
		}

		// Unknown type.
		else {
			Message::Error("Unrecognized attribute type.");
			return false;
		}

		attribute->SetName(names[i]);

		if (!table.GetDictionary()->AppendAttribute(attribute.GetPointer())) {
			Message::Error("Error appending attribute " + String::Format(i + 1) + " to dictionary.");
			return false;
		}

	}

	// Set default table status fields.
	for (i = 0; i < no_attributes; i++) {
		if (i == no_attributes - 1)
			table.SetStatus(i, DecisionTable::STATUS_DECISION, masked);
		else
			table.SetStatus(i, DecisionTable::STATUS_CONDITION, masked);
	}

	return true;

}

 
ArffDecisionTableImporter *
ArffDecisionTableImporter::Clone() {
	return new ArffDecisionTableImporter;
}
