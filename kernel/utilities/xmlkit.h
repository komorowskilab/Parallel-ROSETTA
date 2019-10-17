//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __XMLKIT_H__
#define __XMLKIT_H__

#include <copyright.h>

#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class.........: XMLKit
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class XMLKit {
public:

  //- Miscellaneous XML-related stuff................................
  static String Escape(const String &unescaped, const String &escapees = "<>&\"");
  static String Escape(char character);

};

#endif 
