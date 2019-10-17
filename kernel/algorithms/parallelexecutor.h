//-------------------------------------------------------------------
// Author........: Aleksander �hrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PARALLELEXECUTOR_H__
#define __PARALLELEXECUTOR_H__

#include <copyright.h>

#include <kernel/algorithms/executor.h>

//-------------------------------------------------------------------
// Class.........: ParallelExecutor
// Author........: Aleksander �hrn
// Date..........:
// Description...: Executes a set of commands to the input structure.
// Comments......:
// Revisions.....: A� 000320: Renamed from ParallellExecutor.
//===================================================================

class ParallelExecutor : public Executor {
protected:

	//- Methods inherited from Executor................................
	virtual Structure *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const;

public:

  //- Constructors/destructor........................................
  ParallelExecutor();
  virtual ~ParallelExecutor();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	// - Methods inherited from Algorithm.............................
	virtual ParallelExecutor	*Clone();
};

#endif
 
