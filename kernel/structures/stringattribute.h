//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#ifndef __STRINGATTRIBUTE_H__
#define __STRINGATTRIBUTE_H__

#include <copyright.h>

#include <kernel/structures/attribute.h>

#include <kernel/basic/map.h>
#include <kernel/basic/string.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: StringAttribute
// Author........: Aleksander Ûhrn
// Date..........: 960630
// Description...: Maps between real-world textual values and coded
//                 integer values.
//
//                 Examples:
//
//                   ("Boat",  4)     (0, "No")
//                   ("No",    0)     (1, "Yes")
//                   ("Plane", 2)     (2, "Plane")
//                   ("Train", 7)     (4, "Boat")
//                   ("Yes",   1)     (7, "Train")
//
// Comments......:
// Revisions.....: AÛ 160797 - Introduced inverse map for speed.
//===================================================================

class StringAttribute : public Attribute {
private:

	//- Type definitions...............................................
	typedef Map(String, int) FMap; // E.g., ("Boat", 314).
	typedef Map(int, String) BMap; // E.g., (314, "Boat").

private:

	//- Private member variables.......................................
	FMap  *f_map_; // Entries, indexed one way (by "Boat").
	BMap  *b_map_; // Entries, indexed other way (by 314).

protected:

  //- Constructors...................................................
  StringAttribute(const StringAttribute &in);

public:

  //- Constructors/destructor........................................
  StringAttribute();
  virtual ~StringAttribute();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool                      Load(ifstream &stream);
  virtual bool                      Save(ofstream &stream) const;

  //- Methods inherited from Structure...............................
  virtual Structure                *Duplicate() const;
  virtual void                      Clear();

  //- Methods inherited from Attribute...............................
	virtual int                       GetEntry(const String &text) const;
	virtual const String		  GetEntry(int value) const;
	virtual int                       SuggestEntry(const String &text) const;
	virtual bool                      SetEntry(int value, const String &text);
	virtual Attribute::Type           GetType() const;
	virtual int                       GetNoEntries() const;
	virtual const String             &GetEnumeratedEntry(int entry_no) const;

};

#endif
 
