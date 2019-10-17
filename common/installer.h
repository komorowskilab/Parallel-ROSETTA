//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __INSTALLER_H__
#define __INSTALLER_H__

#include <copyright.h>

//-------------------------------------------------------------------
// Class.........: Installer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Determines which algorithms and structures that are
//                 available for the user, i.e., that are "installed".
// Revisions.....:
//===================================================================

class Installer {
public:

	//- Installation...................................................
	static bool Install();
	static bool InstallStructures();
	static bool InstallAlgorithms();

};

#endif 
