//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/dictionaryimporter.h>

#include <kernel/structures/dictionary.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/stringattribute.h>

#include <kernel/system/fstream.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class DictionaryImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DictionaryImporter::DictionaryImporter() {
}

DictionaryImporter::~DictionaryImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DictionaryImporter, DICTIONARYIMPORTER, Importer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 given structure.
// Comments......:
// Revisions.....: Aš 000310: Added DECISIONTABLE support.
//===================================================================

bool
DictionaryImporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return (structure.IsA(DICTIONARY) || structure.IsA(DECISIONTABLE));
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 000310: Added DECISIONTABLE support.
//===================================================================

Structure *
DictionaryImporter::Apply(Structure &structure) const {

	if (!IsApplicable(structure))
		return NULL;

	Handle<Dictionary> dictionary;

	// Get dictionary, or create one if necessary.
	if (structure.IsA(DICTIONARY))
		dictionary = dynamic_cast(Dictionary *, const_cast(Structure *, &structure));
	else
		dictionary = Creator::Dictionary();
			
	ifstream stream;

	if (!IOKit::Open(stream, GetFilename())) {
		Message::Error("Failed to open input stream.");
		return NULL;
	}

	AttributeMap map;

	// Parse stream.
	if (!Parse(stream, map))
		return NULL;

	// Build dictionary.
	if (!FillDictionary(*dictionary, map))
		return NULL;

	// Verify compatibility, if necessary.
	if (structure.IsA(DECISIONTABLE)) {

		Handle<DecisionTable> table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));

		bool masked = false;

		if (dictionary->GetNoAttributes() != table->GetNoAttributes(masked)) {
			Message::Error("Size mismatch between imported dictionary and decision table.");
			return NULL;
		}

		table->SetDictionary(dictionary.GetPointer());

	}

	return &structure;

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: FillDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DictionaryImporter::FillDictionary(Dictionary &dictionary, AttributeMap &attributemap) const {

	// Empty the dictionary.
	if (!dictionary.RemoveAllAttributes())
		return false;

	if (attributemap.empty())
		return false;

	// These should be const_iterators, but VC++ 6.0 won't let me...
	AttributeMap::iterator curr_iterator = attributemap.begin();
	AttributeMap::iterator prev_iterator = attributemap.begin();

	// Make sure all attributes have been defined.
	while (!(curr_iterator == attributemap.end())) {
		if ((*curr_iterator).first  - (*prev_iterator).first > 1) {
			Message::Error("Not all attributes have been defined.");
			return false;
		}
		prev_iterator = curr_iterator;
		curr_iterator++;
	}

	int i;

	// Create attributes.
	for (i = 0; i < attributemap.size(); i++) {
		Handle<Attribute> attribute = CreateAttribute(attributemap[i]);
		if (attribute == NULL) {
			Message::Error("Failed to create attribute " + attributemap[i].name_ + ".");
			return false;
		}
		if (!dictionary.AppendAttribute(attribute.GetPointer())) {
			Message::Error("Failed to insert attribute into dictionary.");
			return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: CreateAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Handle<Attribute>
DictionaryImporter::CreateAttribute(const AttributeInfo &info) const {

	Handle<Attribute> attribute;

	// Create attribute.
	switch (info.type_) {
		case Attribute::TYPE_STRING:  attribute = Creator::StringAttribute();                              break;
		case Attribute::TYPE_INTEGER: attribute = Creator::IntegerAttribute();                             break;
		case Attribute::TYPE_FLOAT:   attribute = Creator::FloatAttribute(info.no_decimals_, NULL, false); break;
		default:                      Message::Error("Unrecognized attribute type.");                      return NULL;
	}

	// Fill slots.
	attribute->SetName(info.name_);
	attribute->SetUnit(info.unit_);

	ValueMap::const_iterator iterator = info.map_.begin();

	while (!(iterator == info.map_.end())) {
		int    value = (*iterator).first;
		String text  = (*iterator).second;
		if (!attribute->SetEntry(value, text)) {
			Message::Error("Failed to fill attribute map.");
			return NULL;
		}
		iterator++;
	}

	return attribute;

}

//-------------------------------------------------------------------
// Method........: Parse
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DictionaryImporter::Parse(ifstream &stream, AttributeMap &attributemap) const {

	String stringname  = Attribute::GetString(Attribute::TYPE_STRING);   stringname.ToLowercase();
	String integername = Attribute::GetString(Attribute::TYPE_INTEGER);	 integername.ToLowercase();
	String floatname   = Attribute::GetString(Attribute::TYPE_FLOAT);    floatname.ToLowercase();

	// Clear map.
	attributemap.erase(attributemap.begin(), attributemap.end());

 	String line;

	int line_no = 0;

	// Process all lines.
	while (IOKit::LoadLine(stream, line, &line_no)) {

		// Split line in two.
		if (!line.Contains('=')) {
			Message::Error("Line " + String::Format(line_no) + " does not contain a '=' symbol.");
			return false;
		}

		String lhs = line.Before('=');
		String rhs = line.After('=');

		lhs.Trim(" \t");
		rhs.Trim(" \t");

		// Extract attribute number.
		if (!lhs.Contains('.')) {
			Message::Error("Line " + String::Format(line_no) + " does not contain a '.' symbol on the left.");
			return false;
		}

		String lhs_no = lhs.Before('.');

		lhs_no.Trim(" \t");

		if (lhs_no.IsEmpty()) {
			Message::Error("No '.' symbol found on line " + String::Format(line_no) + ".");
			return false;
		}

		if (!lhs_no.IsInteger()) {
			Message::Error("First token on line " + String::Format(line_no) + " is not an integer.");
			return false;
		}

		int attribute_no = lhs_no.GetInteger();

		if (attribute_no < 0) {
			Message::Error("Attribute index on line " + String::Format(line_no) + " is negative.");
			return false;
		}

		// Make sure map index is valid.
		attributemap.insert(Pair(const int, DictionaryImporter::AttributeInfo)(attribute_no, AttributeInfo()));

		// Extract field identifier.
		String lhs_fi = lhs.After('.');
		String lhs_va;

		lhs_fi.ToLowercase();

		lhs_fi.Trim(" \t");

		if (lhs_fi.Contains('.')) {
			lhs_va = lhs_fi.After('.');
			lhs_va.Trim(" \t");
			lhs_fi = lhs_fi.Before('.');
			lhs_fi.Trim(" \t");
		}

		// Name?
		if (lhs_fi == "name") {
			attributemap[attribute_no].name_ = rhs;
		}

		// Unit?
		else if (lhs_fi == "unit") {
			attributemap[attribute_no].unit_ = rhs;
		}

		// Type?
		else if (lhs_fi == "type") {
			rhs.ToLowercase();
			if (rhs == stringname)
				attributemap[attribute_no].type_ = Attribute::TYPE_STRING;
			else if (rhs == integername)
				attributemap[attribute_no].type_ = Attribute::TYPE_INTEGER;
			else if (rhs == floatname)
				attributemap[attribute_no].type_ = Attribute::TYPE_FLOAT;
			else {
				Message::Error("Attribute type " + rhs + " on line " + String::Format(line_no) + " not recognized.");
				return false;
			}
		}

		// Decimals?
		else if (lhs_fi == "decimals") {
			if (!rhs.IsInteger()) {
				Message::Error("Token " + rhs + " on line " + String::Format(line_no) + " is not an integer.");
				return false;
			}
			attributemap[attribute_no].no_decimals_ = rhs.GetInteger();
		}

		// Map?
		else if (lhs_fi == "map") {
			if (!lhs_va.IsInteger()) {
				Message::Error("Token " + lhs_va + " on line " + String::Format(line_no) + " is not an integer.");
				return false;
			}
			attributemap[attribute_no].map_.insert(Pair(const int, String)(lhs_va.GetInteger(), rhs));
		}

		// Unknown.
		else {
			Message::Error("Field identifier " + lhs_fi + " on line " + String::Format(line_no) + " not recognized.");
			return false;
		}

	}

	return true;

}
 
DictionaryImporter *
DictionaryImporter::Clone() {
	return new DictionaryImporter;
}
