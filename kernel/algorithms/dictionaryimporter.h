//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DICTIONARYIMPORTER_H__
#define __DICTIONARYIMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/importer.h>

#include <kernel/structures/attribute.h>

#include <kernel/basic/map.h>
#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Dictionary;

//-------------------------------------------------------------------
// Class.........: DictionaryImporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Imports a dictionary.
// Revisions.....: AÛ 990218: Had to make AttributeInfo public due
//                            to an STL quirk.
//===================================================================

class DictionaryImporter : public Importer {
public:

	//- Type definitions...............................................
	typedef Map(int, String) ValueMap;

	//- Internal holder for attribute information......................
	struct AttributeInfo {

		//- Data.........................................................
		String                  name_;
		String                  unit_;
		Attribute::Type         type_;
		int                     no_decimals_;
		ValueMap                map_;

		//- Constructor/destructor.......................................
		AttributeInfo() {name_ = unit_ = Undefined::String(); type_ = Attribute::TYPE_INTEGER; no_decimals_ = 0;}
		~AttributeInfo() {}

	};

protected:

	//- Type definitions...............................................
	typedef Map(int, AttributeInfo) AttributeMap;

	//- Protected methods..............................................
	virtual bool              Parse(ifstream &stream, AttributeMap &map) const;
	virtual bool              FillDictionary(Dictionary &dictionary, AttributeMap &map) const;
	virtual Handle<Attribute> CreateAttribute(const AttributeInfo &info) const;

public:

  //- Constructors/destructor........................................
  DictionaryImporter();
  virtual ~DictionaryImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual bool              IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure        *Apply(Structure &structure) const;
	virtual DictionaryImporter 	*Clone();

};

#endif 
