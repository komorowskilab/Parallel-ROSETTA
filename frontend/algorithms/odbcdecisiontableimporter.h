//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

#ifndef __ODBCDECISIONTABLEIMPORTER_H__
#define __ODBCDECISIONTABLEIMPORTER_H__

#include <kernel/algorithms/decisiontableimporter.h>

#include <kernel/structures/attribute.h>

#include <kernel/basic/bits.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class CDatabase;
class DecisionTable;

//-------------------------------------------------------------------
// Class.........: ODBCDecisionTableImporter
// Author........: Aleksander Ûhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

class ODBCDecisionTableImporter : public DecisionTableImporter {
protected:

	//- Parameters.....................................................
	CDatabase                     *database_; // The database itself.
	String                         table_;    // Name of table.
	Bits                           selected_; // Mask of attribute/columns to import.
	Vector(String)                 names_;    // Attribute names.
	Vector(Attribute::Type)        types_;    // Attribute types.
	Vector(int)                    scales_;   // Attribute scales (for float type).

protected:

	//- Parameters methods.............................................
	virtual bool                   VerifyParameters() const;

public:

  //- Constructors/destructor........................................
  ODBCDecisionTableImporter();
  virtual ~ODBCDecisionTableImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String                 GetParameters() const;
	virtual bool                   SetParameter(const String &keyword, const String &value);
	virtual Structure             *Apply(Structure &structure) const;

  //- Methods inherited from Importer................................
	virtual bool                   SetFilename(const String &filename);

  //- Database parameters............................................
	bool                           SetDatabase(CDatabase *database) {database_ = database; return true;}
	CDatabase                     *GetDatabase() const {return database_;}

	bool                           SetTable(const String &table) {table_ = table; return true;}
	const String                  &GetTable() const {return table_;}

	bool                           SetSelected(const Bits &selected) {selected_ = selected; return true;}
	const Bits                    &GetSelected() const {return selected_;}

	bool                           SetNames(const Vector(String) &names) {names_ = names; return true;}
	const Vector(String)          &GetNames() const {return names_;}

	bool                           SetTypes(const Vector(Attribute::Type) &types) {types_ = types; return true;}
	const Vector(Attribute::Type) &GetTypes() const {return types_;}

	bool                           SetScales(const Vector(int) &scales) {scales_ = scales; return true;}
	const Vector(int)             &GetScales() const {return scales_;}

  //- Database methods...............................................
	bool                           ImportTable(DecisionTable &table);

};

#endif 
