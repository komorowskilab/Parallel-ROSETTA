//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/projectmanager.h>
#include <kernel/structures/project.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Initialization of static variables.
//===================================================================

Project::Handles *ProjectManager::projects_ = NULL;

//-------------------------------------------------------------------
// Methods for class ProjectManager.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetProject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the project that the given structure is a
//                 member of.
// Comments......:
// Revisions.....:
//===================================================================

Project *
ProjectManager::GetProject(const Structure *structure) {

	// Any projects present?
	if (projects_ == NULL) {
		return NULL;
	}

	// Valid input?
	if (structure == NULL) {
		return NULL;
	}

	int i;

	Project * p = 0;

	// Check for direct match with a project.
	for (i = 0; i < projects_->size(); i++) {
		if ((*projects_)[i].GetPointer() == structure) {
			p = (*projects_)[i].GetPointer();
			break;
		}
	}
	

	if (p != NULL)
		return p;

	// Check project membership.
	for (i = 0; i < projects_->size(); i++) {
		if (structure->FindParent(*(*projects_)[i]) != NULL) {
			p = (*projects_)[i].GetPointer();
			break;
		}
	}

	if (p != NULL)
		return p;

	return NULL;

}

//-------------------------------------------------------------------
// Method........: InsertProject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Insert a new project into the pool.
// Comments......:
// Revisions.....:
//===================================================================

bool
ProjectManager::InsertProject(const Project *project) {

	// Any projects present?
	if (projects_ == NULL)
		projects_ = new Project::Handles;


	// Valid input?
	if (project == NULL)
		return false;

	int i;

	// Check if project is already present.
	for (i = 0; i < projects_->size(); i++) {
		if ((*projects_)[i] == project)
			return false;
	}

	Handle<Project> handle(const_cast(Project *, project));

	// Add project to pool
	#pragma omp critical(project_push_back)
	{ 
  	projects_->push_back(handle);
  	}

  return true;

}

//-------------------------------------------------------------------
// Method........: RemoveProject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Removes from the pool the project that the given
//                 structure is a member of.
// Comments......:
// Revisions.....:
//===================================================================

bool
ProjectManager::RemoveProject(const Structure *structure) {

	// Any projects present?
	if (projects_ == NULL)
		return false;

	// Valid input?
	if (structure == NULL)
		return false;

	int i;

	// Check for direct match with a project.
	for (i = 0; i < projects_->size(); i++) {
		if ((*projects_)[i].GetPointer() == structure) {
			Message::Debug("Removing project " + (*projects_)[i]->GetName() + " from project pool.");


			projects_->erase(projects_->begin() + i);

			return true;
		}
	}

	// Check project membership.
	for (i = 0; i < projects_->size(); i++) {
		if (structure->FindParent(*(*projects_)[i]) != NULL) {
			Message::Debug("Removing project " + (*projects_)[i]->GetName() + " from project pool.");


			projects_->erase(projects_->begin() + i);

			return true;
		}
	}

	return false;

}
 
