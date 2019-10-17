//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __INFORMATIONVECTOR_H__
#define __INFORMATIONVECTOR_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: InformationVector
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class InformationVector : public Structure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<InformationVector>) Handles;

protected:

	//- Constructors...................................................
  InformationVector(const InformationVector &in);

public:

  //- Constructor and destructor....................................
	InformationVector();
  virtual ~InformationVector();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool       Load(ifstream &stream);
  virtual bool       Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual void       Clear();

  //- Access methods................................................
	virtual int        GetNoAttributes() const = 0;

	virtual int        GetEntry(int attribute_no) const = 0;
  virtual bool       SetEntry(int attribute_no, int value) = 0;

  //- Attribute administration......................................
  virtual bool       InsertAttribute(int attribute_no) = 0;
  virtual bool       AppendAttribute();
  virtual bool       RemoveAttribute(int attribute_no) = 0;

	virtual bool       SwapAttributes(int i, int j);

	virtual bool       Reserve(int no_attributes);
	virtual bool       Resize(int no_attributes);

  //- Operators.....................................................
	InformationVector &operator=(const InformationVector &in);

	bool               operator==(const InformationVector &in) const;
	bool               operator!=(const InformationVector &in) const;

	bool               operator<(const InformationVector &in) const;

  //- Formatting....................................................
	bool               Format(String &formatted, const String &name = "x", const DecisionTable *table = NULL, bool masked = true) const;
	String             Format(const String &name = "x", const DecisionTable *table = NULL, bool masked = true) const;
	bool               FormatProlog(String &formatted, const String &name = "x", const DecisionTable *table = NULL, bool masked = true) const;
	String             FormatProlog(const String &name = "x", const DecisionTable *table = NULL, bool masked = true) const;

  //- Creation......................................................
	virtual bool       Create(const DecisionTable &table, int object_no, bool masked);

};

#endif 
