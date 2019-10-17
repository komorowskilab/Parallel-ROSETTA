//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCT_H__
#define __REDUCT_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: Reduct
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...: Base class for objects representing a reduct.
//
//                 A reduct can be seen as a list of attribute
//                 numbers indexing the originating decision table.
//
// Comments......: To do: Introduce a common base class for this class
//                        and the EquivalenceClass class, since they are
//                        both index sets.
//
// Revisions.....:
//===================================================================

class Reduct : public Structure {
public:

	//- Enumerated types...............................................
	enum Discernibility {DISCERNIBILITY_FULL, DISCERNIBILITY_OBJECT};

	//- Textual representations........................................
	static String                  GetString(Reduct::Discernibility discernibility);

public:

  //- Type definitions...............................................
	typedef Vector(Handle<Reduct>) Handles;

protected:

	//- Constructors...................................................
  Reduct(const Reduct &in);

public:

	//- Constructors/destructor........................................
  Reduct();
  virtual ~Reduct();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent.............................
  virtual bool                   Load(ifstream &stream);
  virtual bool                   Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual void                   Clear();

  //- Attribute management methods..................................
  virtual int                    GetNoAttributes() const = 0;
  virtual int                    GetAttribute(int position_no) const = 0;

  virtual bool                   InsertAttribute(int attribute_no) = 0;
  virtual bool                   RemoveAttribute(int position_no) = 0;

  virtual int                    FindAttribute(int attribute_no) const;

  virtual bool                   IsMember(int attribute_no) const;

	virtual bool                   Reserve(int no_attributes);

	virtual bool                   Reindex(const Vector(int) &indices) = 0;

	//- Miscellaneous methods..........................................
	virtual Reduct::Discernibility GetDiscernibilityType() const = 0;
	virtual bool                   SetDiscernibilityType(Reduct::Discernibility discernibility) = 0;

	virtual int                    GetObject() const = 0;
	virtual bool                   SetObject(int object_no) = 0;

	virtual int                    GetSupport() const = 0;
	virtual bool                   SetSupport(int support) = 0;

	virtual bool                   IsModuloDecision() const = 0;
	virtual bool                   IsModuloDecision(bool modulo) = 0;

  //- Operators......................................................
  Reduct                        &operator=(const Reduct &in);

  bool                           operator==(const Reduct &in) const;
  bool                           operator!=(const Reduct &in) const;

  //- Formatting.....................................................
	bool                           Format(String &formatted, const DecisionTable *table = NULL, bool masked = true) const;
	String                         Format(const DecisionTable *table = NULL, bool masked = true) const;
	bool                           FormatProlog(String &formatted, const String &name, const DecisionTable *table = NULL, bool masked = true) const;
	String                         FormatProlog(const String &name, const DecisionTable *table = NULL, bool masked = true) const;

};

#endif 
