//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <copyright.h>

#include <kernel/structures/structure.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Attribute
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...: Used by dictionaries.  Functions as a map between
//                 the coded integral values (as found in decision
//                 tables) and their actual (textual) values (as perceived
//                 by a user).
// Comments......:
// Revisions.....:
//===================================================================

class Attribute : public Structure {
public:

	//- Enumerated types...............................................
	enum Type {TYPE_INTEGER, TYPE_FLOAT, TYPE_STRING};

	//- Textual representations........................................
	static String                     GetString(Attribute::Type type);

public:

  //- Type definitions..............................................
	typedef Vector(Handle<Attribute>) Handles;

private:

	//- Private member variables.......................................
	String                            name_;   // Name of attribute, e.g. "Frequency".
	String                            unit_;   // Unit of attribute, e.g. "Hz".

protected:

  //- Constructors...................................................
  Attribute(const Attribute &in);

public:

  //- Constructors/destructor........................................
  Attribute();
  virtual ~Attribute();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool                      Load(ifstream &in);
  virtual bool                      Save(ofstream &out) const;

  //- Methods inherited from Structure...............................
  virtual void                      Clear();

  //- Map from real-world value to internal coded value...............
	virtual int                       GetEntry(const String &text) const = 0;

	//- Map from internal coded value to real-world value...............
	virtual const String             GetEntry(int value) const = 0;

	//- Mapping manipulation............................................
	virtual int                       SuggestEntry(const String &text) const = 0;
	virtual bool                      SetEntry(int value, const String &text) = 0;

	//- Attribute fields................................................
	virtual Attribute::Type           GetType() const = 0;

	virtual const String             &GetName() const;
	virtual bool                      SetName(const String &name);

	virtual const String             &GetUnit() const;
	virtual bool                      SetUnit(const String &unit);

	virtual int                       GetScalingExponent() const;
	virtual bool                      SetScalingExponent(int exponent);

	//- Querying methods................................................
	virtual bool                      IsNumeric() const;
	virtual bool                      IsSymbolic() const;

	virtual bool                      IsInteger() const;
	virtual bool                      IsFloat() const;
	virtual bool                      IsString() const;

	virtual bool                      IsInterval() const;

	//- Domain iteration methods.......................................
	virtual int                       GetNoEntries() const;
	virtual const String             &GetEnumeratedEntry(int entry_no) const;

	//- Operators......................................................
  bool                              operator==(const Attribute &in) const;
  bool                              operator!=(const Attribute &in) const;
  bool                              operator<=(const Attribute &in) const;

};

#endif
 
