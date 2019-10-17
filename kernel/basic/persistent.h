//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PERSISTENT_H__
#define __PERSISTENT_H__

#include <copyright.h>

#include <kernel/basic/identifier.h>
#include <kernel/basic/string.h>
#include <kernel/basic/undefined.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Persistent
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for persistent objects.
// Revisions.....:
//===================================================================

class Persistent : public Identifier {
public:

  //- Constructors/destructor........................................
  Persistent();
  virtual ~Persistent();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Load/save from/to given stream.................................
  virtual bool Load(ifstream &stream);
  virtual bool Save(ofstream &stream) const;

	//- Load/save from/to given filename...............................
  virtual bool Load(const String &filename);
  virtual bool Save(const String &filename) const;

	//- Load/save from/to default file.................................
  virtual bool Load();
  virtual bool Save() const;

};

#endif 
