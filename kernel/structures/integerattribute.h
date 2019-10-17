//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#ifndef __INTEGERATTRIBUTE_H__
#define __INTEGERATTRIBUTE_H__

#include <copyright.h>

#include <kernel/structures/attribute.h>

//-------------------------------------------------------------------
// Class.........: IntegerAttribute
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...: For integers, n maps to "n" and vice versa.
// Comments......:
// Revisions.....:
//===================================================================

class IntegerAttribute : public Attribute {
protected:

  //- Constructor....................................................
  IntegerAttribute(const IntegerAttribute &in);

public:

  //- Constructors/destructor........................................
  IntegerAttribute();
  virtual ~IntegerAttribute();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual Structure                *Duplicate() const;

  //- Methods inherited from Attribute...............................
	virtual int                       GetEntry(const String &text) const;
	virtual const String             GetEntry(int value) const;
	virtual int                       SuggestEntry(const String &text) const;
	virtual bool                      SetEntry(int value, const String &text);

	virtual Attribute::Type           GetType() const;

};

#endif
 
