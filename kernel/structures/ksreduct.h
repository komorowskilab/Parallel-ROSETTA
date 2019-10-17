//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KSREDUCT_H__
#define __KSREDUCT_H__

#include <copyright.h>

#include <kernel/structures/reduct.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: KSReduct
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A simple realization of a reduct.
// Revisions.....:
//===================================================================

class KSReduct : public Reduct {
protected:

	//- Member variables...............................................
	Vector(int)                    attributes_;     // Attribute indices.
	Reduct::Discernibility         discernibility_; // Reduct type.
	int                            object_no_;      // Object index, if relevant.
	bool                           modulo_;         // Computed modulo the decision?
	int                            support_;        // Reduct "goodness", if relevant.

protected:

	//- Constructors...................................................
  KSReduct(const KSReduct &in);

public:

	//- Constructors/destructor........................................
  KSReduct();
  virtual ~KSReduct();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure..............................
  virtual Structure             *Duplicate() const;

  //- Methods inherited from Reduct.................................
  virtual int                    GetNoAttributes() const;
  virtual int                    GetAttribute(int position_no) const;

  virtual bool                   InsertAttribute(int attribute_no);
  virtual bool                   RemoveAttribute(int position_no);

	virtual bool                   Reserve(int no_attributes);

	virtual bool                   Reindex(const Vector(int) &indices);

	virtual Reduct::Discernibility GetDiscernibilityType() const;
	virtual bool                   SetDiscernibilityType(Reduct::Discernibility discernibility);

	virtual int                    GetObject() const;
	virtual bool                   SetObject(int object_no);

	virtual int                    GetSupport() const;
	virtual bool                   SetSupport(int support);

	virtual bool                   IsModuloDecision() const;
	virtual bool                   IsModuloDecision(bool modulo);

};

#endif 
