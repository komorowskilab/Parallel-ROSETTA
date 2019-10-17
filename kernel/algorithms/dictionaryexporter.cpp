//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/dictionaryexporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class DictionaryExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DictionaryExporter::DictionaryExporter() {
}

DictionaryExporter::~DictionaryExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DictionaryExporter, DICTIONARYEXPORTER, Exporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....: Aš 000310: Added DECISIONTABLE support.
//===================================================================

bool
DictionaryExporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return (structure.IsA(DICTIONARY) || structure.IsA(DECISIONTABLE));
}

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
DictionaryExporter::ExportPrologue(ofstream &stream, const Structure &/*structure*/) const {

	// Write timestamp/user.
	stream << "% Dictionary exported from ROSETTA " << SystemKit::GetTimestamp() << " by " << SystemKit::GetUser() << "." << endl << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 000310: Added DECISIONTABLE support.
//===================================================================

bool
DictionaryExporter::ExportData(ofstream &stream, const Structure &structure) const {

	Handle<Dictionary> dictionary;

	// Get dictionary.
	if (structure.IsA(DICTIONARY))
		dictionary = dynamic_cast(Dictionary *, const_cast(Structure *, &structure));
	else if (structure.IsA(DECISIONTABLE))
		dictionary = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure))->GetDictionary();

	if (dictionary == NULL)
		return false;

	Message message;

	bool ok = true;

	int i, no_attributes = dictionary->GetNoAttributes();

	// Do the actual exporting.
	for (i = 0; i < no_attributes; i++) {

		if (!message.Progress("Exporting dictionary...", i, no_attributes))
			break;

		if (!ExportAttribute(stream, *dictionary, i)) {
			Message::Error("Failed to export attribute.");
			ok = false;
			break;
		}

	}

	return ok;

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
DictionaryExporter::ExportEpilogue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportEpilogue(stream, structure);
}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Default implementation, can be overloaded for
//                 alternative formats.
// Revisions.....:
//===================================================================

bool
DictionaryExporter::ExportAttribute(ofstream &stream, const Dictionary &dictionary, int attribute_no) const {

	// Get attribute.
	Handle<Attribute> attribute = dictionary.GetAttribute(attribute_no);

	if (attribute == NULL)
		return true;

	// Set minimum length of LHS.
	int length_lhs = 12;

	// Format LHS.
	String name_lhs = String::Format(attribute_no) + ".name";
	String unit_lhs = String::Format(attribute_no) + ".unit";
	String type_lhs = String::Format(attribute_no) + ".type";

	if (name_lhs.GetLength() < length_lhs)
		name_lhs.Pad(' ', length_lhs);
	if (unit_lhs.GetLength() < length_lhs)
		unit_lhs.Pad(' ', length_lhs);
	if (type_lhs.GetLength() < length_lhs)
		type_lhs.Pad(' ', length_lhs);

	// Export 'LHS = RHS'.
	stream << name_lhs << " = " << attribute->GetName() << endl;
	stream << unit_lhs << " = " << attribute->GetUnit() << endl;
	stream << type_lhs << " = " << Attribute::GetString(attribute->GetType()) << endl;

	// Ugly.
	if (attribute->IsFloat()) {

		// Format LHS.
		String decimals_lhs = String::Format(attribute_no) + ".decimals";
		if (decimals_lhs.GetLength() < length_lhs)
			decimals_lhs.Pad(' ', length_lhs);

		// Export 'LHS = RHS'.
		stream << decimals_lhs << " = " << attribute->GetScalingExponent() << endl;

	}

	int i;

	// Export map.
	for (i = 0; i < attribute->GetNoEntries(); i++) {

		// Format LHS.
		String s_entry = attribute->GetEnumeratedEntry(i);
		int    i_entry = attribute->GetEntry(s_entry);
		String map_lhs = String::Format(attribute_no) + ".map." + String::Format(i_entry);
		if (map_lhs.GetLength() < length_lhs)
			map_lhs.Pad(' ', length_lhs);

		// Export 'LHS = RHS'.
		stream << map_lhs << " = " << s_entry << endl;

	}

	stream << endl;

	return true;

}

DictionaryExporter *
DictionaryExporter::Clone() {
   return new DictionaryExporter;
} 
