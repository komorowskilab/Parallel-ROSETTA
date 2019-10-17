//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PROJECTMANAGER_H__
#define __PROJECTMANAGER_H__

#include <copyright.h>

#include <kernel/structures/project.h>

//-------------------------------------------------------------------
// Class.........: ProjectManager
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A pool containing all top-level projects that
//                 exist in the system.
// Revisions.....:
//===================================================================

class ProjectManager {
private:

	//- Private variables..............................................
	static Project::Handles *projects_;

public:

	//- Member methods.................................................
	static Project         *GetProject(const Structure *structure);
	static bool             InsertProject(const Project *project);
	static bool             RemoveProject(const Structure *structure);

};

#endif 
