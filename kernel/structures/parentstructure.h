//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PARENTSTRUCTURE_H__
#define __PARENTSTRUCTURE_H__

#include <copyright.h>

#include <kernel/structures/annotatedstructure.h>

#include <kernel/basic/handle.h>
#include <kernel/basic/undefined.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structures;

//-------------------------------------------------------------------
// Class.........: ParentStructure
// Author........: Aleksander
// Date..........:
// Description...: A structure which is a parent, i.e., has pointers
//                 to other structures derived from it ("child
//                 structures").
//
// Comments......: Note the difference between this class and the
//                 Structures class.  This class functions as a base
//                 class for classes that have children pointers.
//                 The Structures class functions as a base class for
//                 classes that are conceived as sets of structures.
//
//                 Implementationwise, this class can be implemented
//                 in terms of an internal Structures object, however.
//                 A bit backwards considering the current class
//                 hierarchy, but...
// Revisions.....:
//===================================================================

class ParentStructure : public AnnotatedStructure {
private:

	//- Children (derived) structures..................................
	Handle<Structures> children_;

protected:

  //- Constructors...................................................
  ParentStructure(const ParentStructure &in);

public:

  //- Constructors/destructor........................................
  ParentStructure();
  virtual ~ParentStructure();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent..............................
	virtual bool        Load(ifstream &stream);
	virtual bool        Save(ofstream &stream) const;

  //- Methods inherited from Structure...............................
  virtual Structure  *Duplicate() const;
	virtual void        Clear();

  //- Methods inherited from Structure, child management.............
  virtual int         GetNoChildren() const;
  virtual Structure  *GetChild(int i) const;

  virtual bool        InsertChild(Structure *child, int i);
  virtual bool        AppendChild(Structure *child);
  virtual bool        RemoveChild(int i);

};

#endif 
