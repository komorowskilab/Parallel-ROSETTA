//-------------------------------------------------------------------
// Author........: Robin Andersson
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MODEMANAGER_H__
#define __MODEMANAGER_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>
#include <kernel/basic/handle.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/macros.h>

#include <kernel/structures/structure.h>
#include <kernel/algorithms/algorithm.h>

#include <map>

//-------------------------------------------------------------------
// Class.........: ModeManager
// Author........: Robin Andersson
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class ModeManager {
private:

	//- Type definitions...............................................
	typedef std::multimap<char*,Id>    NodeAlgorithms;
public:
	typedef enum {ROSETTA,GENOMIC}	   AppMode;
	struct Mode { AppMode app_mode_; };

private:

	//- Type definitions...............................................
	typedef std::map<Id,Mode>          GenomicAlgorithms;
	typedef Vector(char*)              NodeVector;

	//- Private variables..............................................
	static Mode					       *mode_;					//Current application mode
	static NodeAlgorithms		       *node_algorithms_;		//Genomic algorithms associated with nodes
	static NodeVector			       *nodes_;				    //The genomic nodes
	static GenomicAlgorithms	       *genomic_algorithms_;    //Algorithms ids mapped to booleans

public:

	//- AppMode settings...............................................
	static void				           SetMode(AppMode mode);
	static void				           RemoveMode();
	static bool				           IsMode(AppMode mode);
	static void				           SetRosetta(Id id);
	static void				           SetGenomic(Id id);
	static bool				           IsRosettaAlgorithm(Id id);
	static bool				           IsGenomicAlgorithm(Id id);

	//- Genomic settings...............................................
	static void						   AddNode(char* nodename);
	static void						   ConnectToNode(Id id, char* nodename);
	static void						   RemoveNodes();
	static std::vector<char*>		   GetNodes();
	static std::vector<Id>			   GetIdsForNode(char* nodename);
	static std::vector<Id>			   GetGenomicIds();

};

#endif
 
