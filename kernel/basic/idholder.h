//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __IDHOLDER_H__
#define __IDHOLDER_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/macros.h>
#include <kernel/basic/undefined.h>
#include <kernel/basic/string.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: IdHolder
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...: Class for identifier administration.
// Revisions.....:
//===================================================================

class IdHolder {
private:

	//- Data structure.................................................
  struct Description {

		//- Various descriptive levels...................................
		String classname_;   // Classname.
		String description_; // Brief description (e.g. for menu entries).

		//- Constructor..................................................
    Description() {classname_ = description_ = Undefined::String();}

	};

  //- Static structures for identifier administration................
  static Vector(Description) *descriptions_;

public:

  //- Register a new identifier......................................
  static Id            Register(const String &classname, const String &description);

  //- Unregister all identifiers.....................................
  static bool          Unregister();

	//- Get descriptions existing identifiers..........................
  static const String &GetClassname(Id id);
  static const String &GetDescription(Id id);

	//- Get an identifier from a textual description (non-unique)......
	static Id            GetId(const String &text);

};

#endif 
