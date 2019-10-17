//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

#include <stdafx.h>
#include <afxdb.h>

#include <frontend/algorithms/odbcdecisiontableimporter.h>
#include <frontend/algorithms/odbckit.h>

#include <frontend/dialogs/dialoggenericlist.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/dictionary.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/bits.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Static helper methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticInitializeTable
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Initializes the given table to a state suitable for
//                 subsequent filling. Assigns a dictionary created
//                 from provided (name, type, scale) information.
// Comments......: 
// Revisions.....: 
//===================================================================

static bool
StaticInitializeTable(DecisionTable &table, const Bits &selected, const Vector(String) &names, const Vector(Attribute::Type) &types, const Vector(int) &scales) {

	// Verify input vector dimensions.
	if (names.size() != types.size())
		return false;

	if (names.size() != scales.size())
		return false;	

	// Remove all children.
	if (!table.RemoveAllChildren())
		return false;

	bool masked = false;

	// Empty the input table. Let the table have one initial object.
	if (!table.Resize(1, 0, masked))
		return false;

	// Create an empty dictionary.
	Handle<Dictionary> dictionary = Creator::Dictionary();

	// Assign the empty dictionary to the table.
	if (!table.SetDictionary(dictionary.GetPointer()))
		return false;

	int i;

	// Add attributes to the table (and dictionary).
	for (i = 0; i < selected.GetSize(); i++) {

		// Skip non-selected columns.
		if (!selected[i])
			continue;

		Handle<Attribute> attribute;

		switch (types[i]) {
			case Attribute::TYPE_STRING:  attribute = Creator::StringAttribute();                      break;
			case Attribute::TYPE_INTEGER: attribute = Creator::IntegerAttribute();                     break;
			case Attribute::TYPE_FLOAT:   attribute = Creator::FloatAttribute(scales[i], NULL, false); break;
			default:                      attribute = Creator::StringAttribute();                      break;
		}

		if (!attribute->SetName(names[i]))
			return false;

		if (!table.AppendAttribute(attribute.GetPointer(), masked))
			return false;

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticFillTable
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

static bool
StaticFillTable(CDatabase &database, const String &tablename, DecisionTable &table, const Bits &selected, const Vector(String) &names, const Vector(Attribute::Type) &types, const Vector(int) &scales) {

	// Initialize table before filling it.
	if (!StaticInitializeTable(table, selected, names, types, scales)) {
		Message::Error("Failed to initialize input decision table properly.");
		return false;
	}

	String tablename_copy = tablename;

	// Strip enclosing quotes that may have been added.
	tablename_copy.Trim('\"');

	// Set decision table name.
	table.SetName(tablename_copy);

	// Count the number of records in the table.
	int no_records = ODBCKit::GetNoRecords(database, tablename);

	if (no_records == Undefined::Integer()) {
		Message::Error("Failed to count the number of records in " + tablename + ".");
		return false;
	}

	// Prepare SQL statement.
	String sql = "SELECT * FROM " + tablename;

	CRecordset recordset(&database);

	// Open record set.
	if (!recordset.Open(CRecordset::forwardOnly, sql.GetBuffer(), CRecordset::readOnly)) {
		Message::Error("Failed to open record set.");
		return false;
	}

	int  object_no = 0;
	bool first     = true;
	bool masked    = false;

	CWaitCursor wait;
	Message     message;

	// Read all record sets.
	while (!recordset.IsEOF()) {

		// The user may interrupt the process.
		if (!message.Progress("Reading record " + String::Format(object_no + 1) + " of " + String::Format(no_records) + "...", object_no, no_records))
			break;

		// Append an object to table if needed.
		if (!first && !table.AppendObject(masked)) {
			Message::Error("Failed to append object to decision table.");
			return false;
		}

		first = false;

		// Count the number of fields.
		int no_fields = recordset.GetODBCFieldCount();

		if (no_fields != selected.GetSize()) {
			Message::Error("Record " + String::Format(object_no + 1) + " had an unexpected number of fields.");
			return false;
		}

		int attribute_no = 0;
		int i;

		// Process all fields in record.
		for (i = 0; i < no_fields; i++) {

			// Skip non-selected columns.
			if (!selected[i])
				continue;

			CString tmp;

			// Get field content.
			recordset.GetFieldValue(i, tmp);

			String content = tmp.IsEmpty() ? Undefined::String() : tmp.GetBuffer(0);

			// Let the dictionary suggest an integral value for the token.
			int suggested = table.SuggestDictionaryEntry(attribute_no, content, masked);

			// Update the table.
			if (!table.SetEntry(object_no, attribute_no, suggested, masked)) {
				Message::Error("Failed to set decision table entry.");
				return false;
			}

			// Update the dictionary.
			if (!table.SetDictionaryEntry(attribute_no, suggested, content, masked)) {
				Message::Error("Failed to update dictionary.");
				return false;
			}

			attribute_no++;

		}

		// Move to next record.
		recordset.MoveNext();

		object_no++;

	}

	// Close connection.
	recordset.Close();

	return true;

}

//-------------------------------------------------------------------
// Methods for class ODBCDecisionTableImporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ODBCDecisionTableImporter::ODBCDecisionTableImporter() {
	SetDatabase(NULL);
	SetTable(Undefined::String());
}

ODBCDecisionTableImporter::~ODBCDecisionTableImporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ODBCDecisionTableImporter, ODBCDECISIONTABLEIMPORTER, DecisionTableImporter)

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
ODBCDecisionTableImporter::GetParameters() const {

	String parameters;

	// Append current parameter settings.
	// ...

	return parameters + DecisionTableImporter::GetParameters();

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
ODBCDecisionTableImporter::SetParameter(const String &keyword, const String &value) {

	// Is the keyword handled higher up?
	if (DecisionTableImporter::SetParameter(keyword, value))
		return true;

	return false;
	
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

Structure *
ODBCDecisionTableImporter::Apply(Structure &structure) const {

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable>             table = dynamic_cast(DecisionTable *, &structure);
	Handle<ODBCDecisionTableImporter> self  = const_cast(ODBCDecisionTableImporter *, this);

	// Import table.
	if (!self->ImportTable(*table))
		return NULL;

	return table.Release();

}


//-------------------------------------------------------------------
// Methods inherited from Importer
//===================================================================

//-------------------------------------------------------------------
// Method........: SetFilename
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

bool
ODBCDecisionTableImporter::SetFilename(const String &filename) {
	return Importer::SetFilename(filename);
}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: VerifyParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

bool
ODBCDecisionTableImporter::VerifyParameters() const {

	if (GetDatabase() == NULL) {
		Message::Error("Database not set.");
		return false;
	}

	if (GetTable() == Undefined::String()) {
		Message::Error("No table name set.");
		return false;
	}

	if (GetNames().empty()) {
		Message::Error("No column/attribute names set.");
		return false;
	}

	if (GetTypes().empty()) {
		Message::Error("No column/attribute types set.");
		return false;
	}

	if (GetScales().empty()) {
		Message::Error("No column/attribute scales set.");
		return false;
	}

	if (GetSelected().GetSize() == 0) {
		Message::Error("No column/attribute selection mask set.");
		return false;
	}

	if (GetNames().size() != GetTypes().size() || 
		  GetNames().size() != GetScales().size() || 
			GetNames().size() != GetSelected().GetSize()) {
		Message::Error("Dimension mismatch.");
		return false;
	}

	return true;

}


//-------------------------------------------------------------------
// Method........: ImportTable
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

bool
ODBCDecisionTableImporter::ImportTable(DecisionTable &table) {

	if (!VerifyParameters())
		return false;

	try {

		if (!GetDatabase()->IsOpen()) {
			Message::Error("Database not open.");
			// Open it!
			return false;
		}

		// Fill table with DB contents.
		if (!StaticFillTable(*GetDatabase(), GetTable(), table, GetSelected(), GetNames(), GetTypes(), GetScales()))
			return NULL;

		// Close connection.
		GetDatabase()->Close();

	}

	// Catch exceptions thrown by MFC.
	catch (CException *exception) {
		exception->ReportError();
		return false;
	}

	return true;

}
 
