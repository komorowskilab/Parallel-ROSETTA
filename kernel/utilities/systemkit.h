//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SYSTEMKIT_H__
#define __SYSTEMKIT_H__

#include <copyright.h>

#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class.........: SystemKit
// Author........: Aleksander Ûhrn, Lukasz Ligowski
// Date..........:
// Description...:
// Revisions.....: Moved Encrypt and Decrypt to licensing class
//===================================================================

class SystemKit {
public:

	//- Miscellaneous system calls.....................................
	static String GetTimestamp();
	static String GetUser();

	//- File system stuff..............................................
	static bool   ChangeDirectory(const String &directory);
	static bool   CreateDirectory(const String &directory);
	static bool   GetTimestamp(const String &filename, String &timestamp, int type = 0);

};

#endif 
