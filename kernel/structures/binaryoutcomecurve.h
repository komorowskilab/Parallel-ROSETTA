//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BINARYOUTCOMECURVE_H__
#define __BINARYOUTCOMECURVE_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: BinaryOutcomeCurve
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Abstract base class.
// Comments......:
// Revisions.....:
//===================================================================

class BinaryOutcomeCurve : public Structure {
protected:

  //- Constructors...................................................
  BinaryOutcomeCurve(const BinaryOutcomeCurve &in);

public:

  //- Constructors/destructor........................................
  BinaryOutcomeCurve();
  virtual ~BinaryOutcomeCurve();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent..............................
  virtual bool Load(ifstream &stream);

	//- New virtual methods............................................
  virtual bool Load(ifstream &stream, bool swap);
  virtual bool Load(const String &filename, bool swap);

	virtual bool Create(const Vector(int) &targets, const Vector(float) &outputs) = 0;

	//- New static methods.............................................
  static  bool Load(ifstream &stream, Vector(int) &targets, Vector(float) &outputs, bool swap, bool sort);

};

#endif 
