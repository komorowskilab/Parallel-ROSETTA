//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/attribute.h>

#include <kernel/basic/vector.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Dictionary
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...: Functions as a map between the coded integral values
//                 (as found in decision tables) and their actual
//                 (textual) values (as perceived by a user).
//
//                 Cf. the Attribute class.
// Comments......:
// Revisions.....:
//===================================================================

class Dictionary : public Structure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<Dictionary>) Handles;

private:

  //- Private data members...........................................
	Attribute::Handles attributes_;

protected:

  //- Constructors...................................................
  Dictionary(const Dictionary &in);

public:

  //- Constructors/destructor........................................
  Dictionary();
  virtual ~Dictionary();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool                 Load(ifstream &stream);
  virtual bool                 Save(ofstream &stream) const;

  //- Methods inherited from Structure...............................
  virtual Structure           *Duplicate() const;
  virtual void                 Clear();

	//- Attribute administration.......................................
	virtual int                  GetNoAttributes() const;
	virtual Attribute           *GetAttribute(int i) const;

	virtual bool                 InsertAttribute(const Attribute *attribute, int i);
	virtual bool                 AppendAttribute(const Attribute *attribute);
	virtual bool                 RemoveAttribute(int i);
	virtual bool                 RemoveAllAttributes();

	virtual bool                 SwapAttributes(int i, int j);

  //- Mapping methods.................................................
	virtual int                  GetEntry(int attribute_no, const String &text) const;
	virtual const String        &GetEntry(int attribute_no, int value) const;

	virtual int                  SuggestEntry(int attribute_no, const String &text) const;

	virtual bool                 SetEntry(int attribute_no, int value, const String &text);

  //- Miscellaneous methods..........................................
	bool                         IsCompatible(const Dictionary &in) const;

	//- Operators......................................................
  bool                         operator==(const Dictionary &in) const;
  bool                         operator!=(const Dictionary &in) const;
  bool                         operator<=(const Dictionary &in) const;

};

#endif
 
