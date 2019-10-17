//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DELIMITER_H__
#define __DELIMITER_H__

#include <copyright.h>

#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class.........: Delimiter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Scope around "global" delimiter constants.
// Revisions.....:
//===================================================================

class Delimiter {
public:

  static char Line()  {return '\n';} // Separates two lines in a file.
	static char Text()  {return '"';}  // Encloses a string value in a file.
	static char Field() {return ' ';}  // Separates two data fields in a file.

};

#endif 
