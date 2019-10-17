//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

#ifndef __ODBCKIT_H__
#define __ODBCKIT_H__

#include <kernel/structures/attribute.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class CDatabase;

//-------------------------------------------------------------------
// Class.........: ODBCKit
// Author........: Aleksander Ûhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

class ODBCKit {
private:

	static bool RefineScales(CDatabase &database, const String &table, Vector(Attribute::Type) &types, Vector(int) &scales);

public:

	static bool GetTables(const CDatabase &database, Vector(String) &tables);
	static bool GetColumns(CDatabase &database, const String &table, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &scales);
	static int  GetNoRecords(CDatabase &database, const String &table);

};

#endif 
