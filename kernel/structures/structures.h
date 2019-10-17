//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#include <copyright.h>

#include <kernel/structures/parentstructure.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/handle.h>
#include <kernel/system/iostream.h>


//-------------------------------------------------------------------
// Class.........: Structures
// Author........: Aleksander
// Date..........:
// Description...: A structure which is a set of other structures.
// Comments......: This class is a subclassed to cater for more
//                 specialized structure collections, e.g. rule sets,
//                 reduct sets and decision table sets.  This class
//                 would therefore be a strong candidate for
//                 templatization, if it hadn't been for the fact that
//                 the RSES library has to encapsulated in a special
//                 way, thus making direct templatization difficult
//                 in practice.
// Revisions.....:
//===================================================================

class Structures : public ParentStructure {
protected:

	//- The inherent member structures......................................
	Structure::Handles structures_;

protected:

  //- Constructors........................................................
  Structures(const Structures &in);

public:

  //- Constructors/destructor.............................................
  Structures();
  virtual ~Structures();

  //- Methods inherited from Identifier...................................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent...................................
  virtual bool        Load(ifstream &stream);
  virtual bool        Save(ofstream &stream) const;

  //- Methods inherited from Structure....................................
  virtual Structure  *Duplicate() const;
	virtual void        Clear();

  //- Methods inherited from Structure, member structure administration...
  virtual int         GetNoStructures() const;
  virtual Structure  *GetStructure(int i) const;

  virtual bool        InsertStructure(Structure *member, int i);
  virtual bool        RemoveStructure(int i);

  //- New virtual methods.................................................
	virtual Structures *DuplicateStructure(int i) const;

};

#endif 
