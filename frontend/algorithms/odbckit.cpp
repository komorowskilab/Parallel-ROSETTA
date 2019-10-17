//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <afxdb.h>  // MFC ODBC.

#include <frontend/algorithms/odbckit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Static helpers (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticReportError
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Use ROSETTA notification instead of calling 
//                 CException::ReportError().
// Revisions.....: 
//===================================================================

static void
StaticReportError(CException *exception) {

	if (exception == NULL)
		return;

	char cause[255];

	if (!exception->GetErrorMessage(cause, 255))
		return;

	Message::Error(cause);

}

//-------------------------------------------------------------------
// Methods for class ODBCKit.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetTables
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Created by copying and pasting from MFC samples
//                 and help files.
//
//                 Should check return values more vigorously.
//
//                 Complex, should be able to use CRecordset methods
//                 instead.
//
// Revisions.....: 
//===================================================================

bool
ODBCKit::GetTables(const CDatabase &database, Vector(String) &tables) {

	// Can't get table names from a DB that's not open.
	if (!database.IsOpen())
		return false;

	// Erase name vector.
	tables.erase(tables.begin(), tables.end());

	try {

		HSTMT   hstmt;
		RETCODE	nRetCode;
		UWORD	  bFunctionExists;

		hstmt = SQL_NULL_HSTMT;

		// Make sure SQLTables exists.
		SQLGetFunctions(database.m_hdbc, SQL_API_SQLTABLES, &bFunctionExists);

		if (!bFunctionExists) {
			Message::Error("SQLTables not supported.");
			return false;
		}

		// Make sure hstmt is allocated
		if (hstmt == SQL_NULL_HSTMT)
			SQLAllocStmt(database.m_hdbc, &hstmt);

		// Must use char array for ODBC interface (can simply hard code max size).
		char lpszType[64] = "'TABLE','VIEW','SYSTEM TABLE','ALIAS','SYNONYM'";

		// Call the ODBC function.
		SQLTables(hstmt, NULL, SQL_NTS, NULL, SQL_NTS, NULL, SQL_NTS, (UCHAR FAR *) lpszType, SQL_NTS);

		const int STR_LEN = 128 + 1;
		const int REM_LEN = 254 + 1;

		// Declare storage locations for result set data.
		UCHAR  szQualifier[STR_LEN], szOwner[STR_LEN];
		UCHAR  szTableName[STR_LEN], szColName[STR_LEN];
		UCHAR  szTypeName[STR_LEN], szRemarks[REM_LEN];
		SDWORD Precision, Length;
		SWORD  DataType, Scale, Radix, Nullable;

		// Declare storage locations for bytes available to return.
		SDWORD cbQualifier, cbOwner, cbTableName, cbColName;
		SDWORD cbTypeName, cbRemarks, cbDataType, cbPrecision;
		SDWORD cbLength, cbScale, cbRadix, cbNullable;

		// Bind columns in result set to storage locations.
		SQLBindCol(hstmt, 1,  SQL_C_CHAR,   szQualifier, STR_LEN, &cbQualifier);
		SQLBindCol(hstmt, 2,  SQL_C_CHAR,   szOwner,     STR_LEN, &cbOwner);
		SQLBindCol(hstmt, 3,  SQL_C_CHAR,   szTableName, STR_LEN, &cbTableName);
		SQLBindCol(hstmt, 4,  SQL_C_CHAR,   szColName,   STR_LEN, &cbColName);
		SQLBindCol(hstmt, 5,  SQL_C_SSHORT, &DataType,         0, &cbDataType);
		SQLBindCol(hstmt, 6,  SQL_C_CHAR,   szTypeName,  STR_LEN, &cbTypeName);
		SQLBindCol(hstmt, 7,  SQL_C_SLONG,  &Precision,        0, &cbPrecision);
		SQLBindCol(hstmt, 8,  SQL_C_SLONG,  &Length,           0, &cbLength);
		SQLBindCol(hstmt, 9,  SQL_C_SSHORT, &Scale,            0, &cbScale);
		SQLBindCol(hstmt, 10, SQL_C_SSHORT, &Radix,            0, &cbRadix);
		SQLBindCol(hstmt, 11, SQL_C_SSHORT, &Nullable,         0, &cbNullable);
		SQLBindCol(hstmt, 12, SQL_C_CHAR,   szRemarks,   REM_LEN, &cbRemarks);

		// Process fetched data.
		while (true) {
			RETCODE nRetCode = SQLFetch(hstmt);
			if (nRetCode == SQL_ERROR) {
				return false;
			}
			else if (nRetCode == SQL_SUCCESS || nRetCode == SQL_SUCCESS_WITH_INFO) {
				tables.push_back(CString(szTableName).GetBuffer(0));
#ifdef _DEBUG
				Message::Debug("--- BEGIN ---");
				Message::Debug(CString("Qualifier = " + CString(szQualifier)).GetBuffer(0));
				Message::Debug(CString("Owner = " + CString(szOwner)).GetBuffer(0));
				Message::Debug(CString("TableName = " + CString(szTableName)).GetBuffer(0));
				Message::Debug(CString("ColName = " + CString(szColName)).GetBuffer(0));
				Message::Debug("DataType = " + String::Format(DataType));
				Message::Debug(CString("TypeName = " + CString(szTypeName)).GetBuffer(0));
				Message::Debug("Precision = " + String::Format(Precision));
				Message::Debug("Length = " + String::Format(Length));
				Message::Debug("Scale = " + String::Format(Scale));
				Message::Debug("Radix = " + String::Format(Radix));
				Message::Debug("Nullable = " + String::Format(Nullable));
				Message::Debug(CString("Remarks = " + CString(szRemarks)).GetBuffer(0));
				Message::Debug("--- END ---");
#endif
			} 
			else {
				break;
			}
		}

	}

	// Catch MFC exceptions.
	catch (CException *exception) {
		StaticReportError(exception);
		return false;
	}

	int i;

	// Enclose table names in quotes if needed.
	for (i = 0; i < tables.size(); i++) {
		if (tables[i].Contains(' ') || tables[i].Contains('$'))
			tables[i] = '\"' + tables[i] + '\"';
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetColumns
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Gets information about the fields in the record
//                 set defined by the SQL statement.
//
//                 For floating point fields, a default scale is set.
//                 Then, this scale is sought refined by the method
//                 StaticFindScales.
//
// Comments......: 
// Revisions.....: 
//===================================================================

bool
ODBCKit::GetColumns(CDatabase &database, const String &table, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &scales) {

	// Can't get table names from a DB that's not open.
	if (!database.IsOpen())
		return false;

	// Define SQL statment.
	String sql = "SELECT * FROM " + table;

	// Erase vectors.
	names.erase(names.begin(), names.end());
	types.erase(types.begin(), types.end());
	scales.erase(scales.begin(), scales.end());

	// Define containers for default values (used if an exception is thrown).
	Vector(String)          default_names;
	Vector(Attribute::Type) default_types;
	Vector(int)             default_scales;

	bool defaults_set = false;

	try {

		CRecordset recordset(&database);

		// Open record set.
		if (!recordset.Open(CRecordset::forwardOnly, sql.GetBuffer(), CRecordset::readOnly))
			return false;

		int no_fields = recordset.GetODBCFieldCount();
		int i;

		// Set default values.
		for (i = 0; i < no_fields; i++) {
			default_names.push_back("A" + String::Format(i + 1));
			default_types.push_back(Attribute::TYPE_STRING);
			default_scales.push_back(0);
		}

		// Flag that the defaults have been set. If an exception is thrown below, we can use these.
		defaults_set = true;

		// Get field information.
		for (i = 0; i < no_fields; i++) {

			CODBCFieldInfo info;

			// Fill informationstructure.
			recordset.GetODBCFieldInfo(i, info);

			// Get field name.
			names.push_back(info.m_strName.GetBuffer(0));

			// Get field type and scale.
			switch (info.m_nSQLType) {
				case SQL_INTEGER:  // Fall through.
				case SQL_SMALLINT: // Fall through.
				case SQL_BIGINT:   // Fall through.
				case SQL_TINYINT:  {
															types.push_back(Attribute::TYPE_INTEGER);
															scales.push_back(0);
															break;
													 }
				case SQL_NUMERIC:  // Fall through.
				case SQL_DECIMAL:  {
															if (info.m_nScale == 0) {
																types.push_back(Attribute::TYPE_INTEGER);
																scales.push_back(0);
															}
															else {
																types.push_back(Attribute::TYPE_FLOAT);
																scales.push_back(info.m_nScale);
															}
															break;
													 }
				case SQL_REAL:     // Fall through.
				case SQL_FLOAT:    // Fall through.
				case SQL_DOUBLE:   {
															types.push_back(Attribute::TYPE_FLOAT);
															scales.push_back(2);
															break;
													 }
				default:           {
															types.push_back(Attribute::TYPE_STRING);
															scales.push_back(0);
															break;
													 }
			}

#ifdef _DEBUG
			Message::Debug("--- DEBUG BEGIN ---");
			Message::Debug(info.m_strName.GetBuffer(0));
			Message::Debug(Attribute::GetString(types[types.size() - 1]));
			Message::Debug(String::Format(info.m_nSQLType));
			Message::Debug(String::Format(scales[scales.size() - 1]));
			Message::Debug("--- DEBUG END ---");
#endif

		}

	}

	// Catch any DB exceptions thrown.
	catch (CException *exception) {

		StaticReportError(exception);

		// Use defaults instead of aborting, if possible.
		if (defaults_set) {
			names  = default_names;
			types  = default_types;
			scales = default_scales;
			Message::Warning("Using default values for column information.");
		}

		return defaults_set;

	}

	// Try to make better guesses for scales (exponents).
	RefineScales(database, table, types, scales);

	return true;

}

//-------------------------------------------------------------------
// Method........: RefineScales
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Tries to deduce the scaling factors for floating
//                 point fields. The scaling factor translates to the
//                 number of decimal points to use.
//
//                 Takes as input types and scales. May modify both
//                 (in-place) as follows: If a type is TYPE_FLOAT and
//                 the deduced scaling factor is 0, convert the type to
//                 TYPE_INTEGER.
//
// Comments......: Called from StaticGetColumnInfo.
// Revisions.....: 
//===================================================================

bool
ODBCKit::RefineScales(CDatabase &database, const String &table, Vector(Attribute::Type) &types, Vector(int) &scales) {

	// Can't get table names from a DB that's not open.
	if (!database.IsOpen())
		return false;

	// Prepare SQL statement.
	String sql = "SELECT * FROM " + table;

	Vector(int) largest_scales;

	try {

		CRecordset recordset(&database);

		// Open record set.
		if (!recordset.Open(CRecordset::forwardOnly, sql.GetBuffer(), CRecordset::readOnly))
			return false;

		int no_fields = recordset.GetODBCFieldCount();

		// Check field size.
		if (no_fields != types.size() || no_fields != scales.size())
			return false;

		// Define how many records to examine.
		int no_records = 10;
		int i;

		// Set initial values.
		for (i = 0; i < no_fields; i++)
			largest_scales.push_back(0);

		while (!recordset.IsEOF() && no_records--) {

			// Process all fields in record.
			for (i = 0; i < no_fields; i++) {

				// Only examine float fields. (Assumed given by input).
				if (types[i] != Attribute::TYPE_FLOAT)
					continue;

				CString tmp;

				// Get field value.
				recordset.GetFieldValue(i, tmp);

				String content = tmp.GetBuffer(0);

				// Skip NULL fields.
				if (content.IsEmpty())
					continue;

				if (!content.IsFloat())
					continue;

				// Get index of decimal point. (Assumes format is not in scientific format).
				int index = content.GetIndex('.');

				if (index == -1)
					continue;

				// Get fractional part.
				String fraction = content.After(index);

				if (!fraction.IsInteger())
					continue;

				int nnz = fraction.GetLength();

				// Don't count trailing zeros.
				while (nnz > 0 && fraction[nnz - 1] == '0')
					nnz--;

				// Bookkeep the largest found number of decimals in use.
				if (largest_scales[i] < nnz)
					largest_scales[i] = nnz;

			}

			// Move to next record.
			recordset.MoveNext();

		}

	}

	// Catch any exceptions thrown.
	catch (CException *exception) {
		StaticReportError(exception);
		return false;
	}

	// Output (in-place) suitable scales.
	scales = largest_scales;

	int i;

	// Since all decision table entries are stored internally as integers, try to avoid overflows.
	for (i = 0; i < scales.size(); i++) {
		if (scales[i] > 5)
			scales[i] = 5;
	}

	// Output (in-place) suitable types.
	for (i = 0; i < types.size(); i++) {
		if (types[i] == Attribute::TYPE_FLOAT && scales[i] == 0)
			types[i] = Attribute::TYPE_INTEGER;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNoRecords
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Returns the number of records in the named table
//                 in the given database.
// Comments......: 
// Revisions.....: 
//===================================================================

int
ODBCKit::GetNoRecords(CDatabase &database, const String &table) {

	// Can't get table names from a DB that's not open.
	if (!database.IsOpen())
		return false;

	int count = Undefined::Integer();

	// Prepare SQL statement.
	String sql = "SELECT COUNT(*) FROM " + table;

	try {

		CRecordset recordset(&database);

		// Open record set.
		if (!recordset.Open(CRecordset::forwardOnly, sql.GetBuffer(), CRecordset::readOnly))
			return count;

		if (recordset.GetODBCFieldCount() != 1)
			return count;

		CString tmp;
		short   nIndex = 0;

		// Get field value.
		recordset.GetFieldValue(nIndex, tmp);

		String content = tmp.GetBuffer(0);

		// Convert to integer.
		if (content.IsInteger())
			count = content.GetInteger();

		// Clean up.
		recordset.Close();

	}

	catch (CException *exception) {
		StaticReportError(exception);
	}

	return count;

}


 
