//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __OBJECTMANAGER_H__
#define __OBJECTMANAGER_H__

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
// Class prototypes.
//===================================================================

#if defined(_ROSGUI)
class CDialogAlgorithm;
#endif

//-------------------------------------------------------------------
// Class.........: ObjectManager
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Manages the algorithms and structures that are
//                 available for the user, i.e., that are "installed".
// Revisions.....:
//===================================================================

class ObjectManager {
private:

	//- Type definitions...............................................
	typedef Structure::Handles         StructureVector;
	typedef Algorithm::Handles         AlgorithmVector;
#if defined(_ROSGUI)
	typedef Vector(CDialogAlgorithm *) DialogVector;
#endif

private:

	//- Private variables..............................................
	static StructureVector		*structures_; // Installed structures.
	static AlgorithmVector		*algorithms_; // Installed algorithms.
#if defined(_ROSGUI)
	static DialogVector			*dialogs_;    // Associated algorithm dialogs.
#endif

public:

	//- Installation...................................................
	static bool              Install(Structure *structure, bool verify = true);
#if defined(_ROSGUI)
	static bool              Install(Algorithm *algorithm, CDialogAlgorithm *dialog, bool verify = true);
#else
	static bool              Install(Algorithm *algorithm, bool verify = true);
#endif

	//- Uninstallation.................................................
#if defined(_ROSGUI)
	static bool              Uninstall(Id id, bool dialog = true);
#else
	static bool              Uninstall(Id id);
#endif

	//- Installation querying..........................................
	static bool              IsInstalled(Id id);
	static bool              IsInstalled(Identifier *identifier);

	//- Access methods.................................................
	static int               GetNoStructures();

	static Structure        *GetEnumeratedStructure(int i);
	static Structure        *GetIdentifiedStructure(Id id);

	static int               GetNoAlgorithms();

	static Algorithm        *GetEnumeratedAlgorithm(int i);
	static Algorithm        *GetIdentifiedAlgorithm(Id id);

#if defined(_ROSGUI)
	static bool              HasEnumeratedDialog(int i);
	static bool              HasIdentifiedDialog(Id id);

	static CDialogAlgorithm *GetEnumeratedDialog(int i);
	static CDialogAlgorithm *GetIdentifiedDialog(Id id);
#endif

	//- Installation counting..........................................
	static int               Count(Id id);

};

#endif
 
