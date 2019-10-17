//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KSINFORMATIONVECTOR_H__
#define __KSINFORMATIONVECTOR_H__

#include <copyright.h>

#include <kernel/structures/informationvector.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: KSInformationVector
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A simple realization of an inf. vector.
// Comments......: A perhaps better implementation than a vector would
//                 be to keep a pointer to a decision table and an
//                 object index. Then, one get the masking stuff for
//                 free, in addition to not having to copy data back and
//                 forth. On the downside, there'd be data consistency
//                 issues and I/O..
// Revisions.....:
//===================================================================

class KSInformationVector : public InformationVector {
protected:

	//- Member variables...............................................
	Vector(int)        inf_;

protected:

  //- Constructors...................................................
	KSInformationVector(const KSInformationVector &in);

public:

  //- Constructor and destructor....................................
	KSInformationVector();
	KSInformationVector(int size);
  virtual ~KSInformationVector();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure..............................
  virtual Structure *Duplicate() const;

  //- Access methods................................................
	virtual int        GetNoAttributes() const;

	virtual int        GetEntry(int attribute_no) const;
  virtual bool       SetEntry(int attribute_no, int value);

  //- Attribute administration......................................
  virtual bool       InsertAttribute(int attribute_no);
  virtual bool       RemoveAttribute(int attribute_no);

	virtual bool       Reserve(int no_attributes);

};

#endif 
