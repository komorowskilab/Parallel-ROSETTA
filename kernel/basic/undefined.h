//-------------------------------------------------------------------
//- Author........: Aleksander Ûhrn
//- Date..........:
//- Description...:
//- Revisions.....:
//===================================================================

#ifndef __UNDEFINED_H__
#define __UNDEFINED_H__

#include <copyright.h>

#include <kernel/system/limits.h>
#include <kernel/system/float.h>

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class.........: Undefined
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Scope around some "magic" constants indicating
//                 undefined values.
// Comments......: A static String object is present so that we can
//                 return a valid reference to a String and not a
//                 full String object.
// Revisions.....:
//===================================================================

class Undefined {
public:

	// Basic types.....................................................
	static int             Integer() {return INT_MAX;}
	static float           Float()   {return FLT_MAX;}
	static const ::String &String()  {static ::String undefined = "Undefined"; return undefined;}
	static ::Id            Id()      {return Integer();}

};

#endif 
