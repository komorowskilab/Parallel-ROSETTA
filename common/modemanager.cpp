//-------------------------------------------------------------------
// Author........: Robin Andersson
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers
#include <copyright.h>

#include <common/modemanager.h>
#include <common/objectmanager.h>

#include <kernel/basic/message.h>
#include <kernel/basic/idholder.h>

#include <kernel/structures/structure.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Initialization of static variables.
//===================================================================

ModeManager::Mode				*ModeManager::mode_				    = NULL;
ModeManager::NodeAlgorithms		*ModeManager::node_algorithms_		= NULL;
ModeManager::NodeVector			*ModeManager::nodes_				= NULL;
ModeManager::GenomicAlgorithms	*ModeManager::genomic_algorithms_	= NULL;

//-------------------------------------------------------------------
// Methods for class ModeManager.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetMode
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Sets the curent mode to either ROSETTA or GENOMIC
// Comments......:
// Revisions.....:
//===================================================================
void 
ModeManager::SetMode(AppMode mode) {
	if (mode_ != NULL && mode_->app_mode_ != mode)
		mode_->app_mode_ = mode;
	else {
		mode_ = new Mode;
		mode_->app_mode_ = mode;
	}
}

//-------------------------------------------------------------------
// Method........: IsMode
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Checks if the current mode is 'mode' (i.e. ROSETTA
//                 or GENOMIC)
// Comments......:
// Revisions.....:
//===================================================================
bool
ModeManager::IsMode(AppMode mode) {
	return mode_->app_mode_ == mode;
}

//-------------------------------------------------------------------
// Method........: RemoveMode
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Removes the current mode settings
// Comments......:
// Revisions.....:
//===================================================================
void
ModeManager::RemoveMode() {
	delete mode_;
	mode_ = NULL;

	if(genomic_algorithms_ != NULL) {
		genomic_algorithms_->clear();
		delete genomic_algorithms_;
		genomic_algorithms_ = NULL;
	}
}

//-------------------------------------------------------------------
// Method........: SetGenomic
// Author........: Robin Andersson
// Date..........: 040617
// Description...: Remember the algorithm to be a genomic algorithm
// Comments......:
// Revisions.....:
//===================================================================
void
ModeManager::SetGenomic(Id id) {
	if (genomic_algorithms_ == NULL)
		genomic_algorithms_ = new GenomicAlgorithms;

	ModeManager::Mode genmode;
	genmode.app_mode_ = GENOMIC;
	std::pair<Id const,ModeManager::Mode> entry(id,genmode);

	if (genomic_algorithms_->count(id) == 0) {
		genomic_algorithms_->insert(entry);
	}
	else if ((*genomic_algorithms_)[id].app_mode_ != GENOMIC) {
		genomic_algorithms_->erase(id);
		genomic_algorithms_->insert(entry);
	}
}

//-------------------------------------------------------------------
// Method........: SetRosetta
// Author........: Robin Andersson
// Date..........: 040617
// Description...: Remember the algorithm to be a standard Rosetta algorithm
// Comments......:
// Revisions.....:
//===================================================================
void
ModeManager::SetRosetta(Id id) {
	if(genomic_algorithms_ == NULL)
		genomic_algorithms_ = new GenomicAlgorithms;

	ModeManager::Mode rosmode;
	rosmode.app_mode_ = ROSETTA;
	std::pair<Id const,ModeManager::Mode> entry(id,rosmode);

	if (genomic_algorithms_->count(id) == 0) {
		genomic_algorithms_->insert(entry);
	}
	else if ((*genomic_algorithms_)[id].app_mode_ != ROSETTA) {
		genomic_algorithms_->erase(id);
		genomic_algorithms_->insert(entry);
	}
}

//-------------------------------------------------------------------
// Method........: IsRosettaAlgorithm
// Author........: Robin Andersson
// Date..........: 040617
// Description...: Is the algorithm identified by id an algorithm
//                 associated with the standard Rosetta mode
// Comments......:
// Revisions.....:
//===================================================================
bool
ModeManager::IsRosettaAlgorithm(Id id) {

	if (genomic_algorithms_->count(id) == 0) {
		String error = "Algorithm with id: ";
		error += id;
		error += " not declared as rosetta or genomic algorithm.";
		Message::Error(error, true);
		return false;
	}

	return (*genomic_algorithms_)[id].app_mode_ == ROSETTA;
}

//-------------------------------------------------------------------
// Method........: IsGenomicAlgorithm
// Author........: Robin Andersson
// Date..........: 040617
// Description...: Is the algorithm identified by id an algorithm
//                 associated with the Genomic Rosetta mode
// Comments......:
// Revisions.....:
//===================================================================
bool
ModeManager::IsGenomicAlgorithm(Id id) {
	
	if (genomic_algorithms_->count(id) == 0) {
		String error = "Algorithm with id: ";
		error += id;
		error += " not declared as rosetta or genomic algorithm.";
		Message::Error(error, true);
		return false;
	}

	return (*genomic_algorithms_)[id].app_mode_ == GENOMIC;
}

//-------------------------------------------------------------------
// Method........: AddNode
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Adds a node for genomic Rosetta
// Comments......:
// Revisions.....:
//===================================================================
void
ModeManager::AddNode(char* nodename) {
	if (nodes_ == NULL)
		nodes_ = new NodeVector;
	
	if (nodename == 0)
		return;

	for (int i = 0; i < nodes_->size(); i++){
	
		if (strcmp((*nodes_)[i],nodename) == 0)
			return;
	}

	nodes_->push_back(nodename);
}

//-------------------------------------------------------------------
// Method........: ConnectToNode
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Connects an identifier (for an algorithm)
//                 to a certain node
// Comments......:
// Revisions.....:
//===================================================================
void
ModeManager::ConnectToNode(Id id, char* nodename) {

	if (node_algorithms_ == NULL)
		node_algorithms_ = new NodeAlgorithms; 

	std::pair<char* const, Id> e(nodename,id);
	node_algorithms_->insert(e);
}

//-------------------------------------------------------------------
// Method........: RemoveNodes
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Removes the nodes and associated identifiers
// Comments......:
// Revisions.....:
//===================================================================
void
ModeManager::RemoveNodes() {
	if (nodes_ != NULL){
		nodes_->clear();
		delete nodes_;
		nodes_ = NULL;
	}

	if (node_algorithms_ != NULL){
		node_algorithms_->clear();
		delete node_algorithms_;
		node_algorithms_ = NULL;
	}
}

//-------------------------------------------------------------------
// Method........: GetNodes
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Returns a std::vector<char*> of nodenames to be
//                 used when building the project tree
// Comments......:
// Revisions.....:
//===================================================================
std::vector<char*>
ModeManager::GetNodes() {
	NodeVector nv(*nodes_);
	return nv;
}

//-------------------------------------------------------------------
// Method........: GetIdsForNode
// Author........: Robin Andersson
// Date..........: 040616
// Description...: Returns a std::vector<Id> with the Ids associated
//                 with the certain nodename
// Comments......:
// Revisions.....:
//===================================================================
std::vector<Id>
ModeManager::GetIdsForNode(char* nodename) {
	std::vector<Id> ids;

	NodeAlgorithms::iterator iter;
    NodeAlgorithms::iterator lower;
    NodeAlgorithms::iterator upper;

	lower = node_algorithms_->lower_bound(nodename);
	upper = node_algorithms_->upper_bound(nodename);

	for (iter = lower; iter != upper; iter++) {
        ids.push_back(iter->second);
    }

	return ids;
}

//-------------------------------------------------------------------
// Method........: GetGenomicIds
// Author........: Robin Andersson
// Date..........: 040921
// Description...: Returns a std::vector<Id> with the Ids associated
//                 with the Genomic application mode
// Comments......:
// Revisions.....:
//===================================================================
std::vector<Id>
ModeManager::GetGenomicIds() {
	std::vector<Id> ids;
	
	// Iterate over the algorithm pool
	for (int i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		Id id = algorithm->GetId();
        if ((*genomic_algorithms_)[id].app_mode_ == GENOMIC)
			ids.push_back(id);
    }

	return ids;
} 
