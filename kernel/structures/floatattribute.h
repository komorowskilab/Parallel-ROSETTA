//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#ifndef __FLOATATTRIBUTE_H__
#define __FLOATATTRIBUTE_H__

#include <copyright.h>

#include <kernel/structures/attribute.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: FloatAttribute
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...: For floating point values, n maps to "n" / m and
//                 "n" maps to n * m, m being a suitable scaling factor.
//
//                 Examples (scaling factor 100):
//
//                   "0.34"    <-> 34
//                   "3.45"    <-> 345
//                   "123.459" <-> 12346
//
// Comments......:
// Revisions.....:
//===================================================================

class FloatAttribute : public Attribute {
private:

	//- Private member variables.......................................
	int exponent_;  // Exponent of 10^n for scaling (determines resolution).

protected:

  //- Constructors...................................................
  FloatAttribute(const FloatAttribute &in);

public:

  //- Constructors/destructor........................................
  FloatAttribute();
  virtual ~FloatAttribute();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool                      Load(ifstream &in);
  virtual bool                      Save(ofstream &out) const;

  //- Methods inherited from Structure...............................
  virtual Structure                *Duplicate() const;
  virtual void                      Clear();

  //- Methods inherited from Attribute...............................
	virtual int                       GetEntry(const String &text) const;
	virtual const String             GetEntry(int value) const;
	virtual int                       SuggestEntry(const String &text) const;
	virtual bool                      SetEntry(int value, const String &text);

	virtual Attribute::Type           GetType() const;
	virtual int                       GetScalingExponent() const;
	virtual bool                      SetScalingExponent(int exponent);

};

#endif
 
