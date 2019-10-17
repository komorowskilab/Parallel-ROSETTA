//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 9903
// Description...: Simple utility that enables ROSETTA script files
//                 to be run from the DOS command line.
// Revisions.....:
//===================================================================

#ifdef WIN32
	#include <stdafx.h> // Precompiled headers.
#endif

#include <common/installer.h>
#include <common/objectmanager.h>
#include <common/configuration.h>

#include <kernel/basic/idholder.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/creator.h>
#include <kernel/system/iostream.h>
//#include <kernel/utilities/license.h>

//No longer supporting single-cores. -N
#include "omp.h"
#include <kernel/utilities/threadcontrol.h>
//-------------------------------------------------------------------
// Method........: main
// Author........: Aleksander šhrn, Lukasz Ligowski
// Date..........: 9903
// Description...:
// Comments......: The current handling of command-line options is
//                 very ugly and ad hoc. Use getopt instead.
// Revisions.....: Added license checking.
//===================================================================

int
main(int argc, char *argv[]) {

	//if (!License::CheckLicense()){
	//	return 100;
	//}

  	// Expected number of arguments?
  	if (argc != 3 && argc != 4) {

    		cout << "Usage: " << argv[0] << " <algorithm> <parameter list> [<input filename>]" << endl;
    		cout << "Did you quote the parameter list, if needed?" << endl;
    		return 0;
  	}

	// Load settings.
	Configuration::Kernel::Load("rosetta.cfg");

  	// Name the parameters.
  	String algorithmname = argv[1];
  	String parameterlist = argv[2];

  	// Write some yada yada.
  	cout << "ROSETTA kernel version " << Configuration::Kernel::GetVersion() << endl;
	#if defined(_DEBUG)
		cout << "Debug build ";  
	#else
		cout << "Release build ";
	#endif	
  	cout << Configuration::GetCompilationTime() << " " << Configuration::GetCompilationDate() << endl;

  	cout << parameterlist << endl;
  	String inputfilename = (argc == 4) ? argv[3] : Undefined::String();

  	// Valid algorithm name?
  	Id id = IdHolder::GetId(algorithmname);

  	if (id == Undefined::Id()) {
    		Message::Error(algorithmname + " is not a recognized algorithm.");
    		return 1;
  	}

  	// Install prototypes.
  	Installer::Install();

  	// Specified algorithm installed?
  	if (!ObjectManager::IsInstalled(id)) {
    		Message::Error(algorithmname + " is not an installed algorithm.");
    		return 1;
  	}

  	// Acquire algorithm.
  	Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(id);


	#ifdef _OPENMP
		omp_set_dynamic(true);
		omp_set_nested(false);
		//ThreadControl::SetMaxThreads( omp_get_num_procs() * 2);
		//ThreadControl::SetMaxThreads(10);
		//omp_set_num_threads(5);
	#endif

	cout << "Dynamic: " << omp_get_dynamic() << endl;
	cout << "Nested: " << omp_get_nested() << endl;
	cout << "Max Threads: " << omp_get_max_threads() << endl;
  	// Set parameters.
  	if (!algorithm->SetParameters(parameterlist))
    		return 1;

	cout << "Dynamic: " << omp_get_dynamic() << endl;
	cout << "Nested: " << omp_get_nested() << endl;
	cout << "Max Threads: " << omp_get_max_threads() << endl;

  	Handle<Structure> structure;

  	// Create the starting point. Don't bother with setting up the ProjectManager,
  	// assume for now that the algorithm is an Executor (where this is done).
  	if (inputfilename == Undefined::String())
    		structure = Creator::Create(PROJECT);
  	else
//		cout << "Loadfile: |" << inputfilename << "|" << endl;
    		structure = Creator::Load(inputfilename);

  	if (structure == NULL) {
    		Message::Error("Failed to load " + inputfilename + ".");
    		return 1;
  	}

  	// Apply.
  	if (structure->Apply(*algorithm) == NULL) {
    		Message::Error("Execution of " + algorithmname + " failed.");
    		return 1;
  	}

  	cout << algorithmname << " executed successfully." << endl;

  	return 0;

}
 
