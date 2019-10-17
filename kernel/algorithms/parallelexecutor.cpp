//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....: Aš 000320: Renamed from ParallellExecutor.
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/parallelexecutor.h>

#include <kernel/structures/structure.h>

//-------------------------------------------------------------------
// Methods for class ParallelExecutor.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ParallelExecutor::ParallelExecutor() {
}

ParallelExecutor::~ParallelExecutor() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ParallelExecutor, PARALLELEXECUTOR, Executor)

//-------------------------------------------------------------------
// Methods inherited from Executor.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExecuteCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ParallelExecutor::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const {

	int i;

	// Check that all algorithms are applicable.
	for (i = 0; i < algorithms.size(); i++) {
		if (!algorithms[i]->IsApplicable(structure)) {
			Message::Error(IdHolder::GetClassname(algorithms[i]->GetId()) + " is not applicable to a " + IdHolder::GetClassname(structure.GetId()) + ".");
			return NULL;
		}
	}

	Handle<Structure> output;

	// Execute commands.
	for (i = 0; i < algorithms.size(); i++) {

		// Since we don't know if any of the algorithms alter the structure, operate on a duplicate.
		Handle<Structure> duplicate = structure.Duplicate();

		if (duplicate == NULL)
			return NULL;

		// Get algorithm.
		Handle<Algorithm> algorithm = algorithms[i];

		// Set parameters.
		algorithm->SetParameters(parameters[i]);

		// Save log entry.
		if (!SaveLogEntry(stream, *algorithm))
			return NULL;

		// Apply.
		Handle<Structure> result = duplicate->Apply(*algorithm);

		if (result == NULL) {
			Message::Error("Application of " + IdHolder::GetClassname(algorithm->GetId()) + " failed.");
			return NULL;
		}

		// Save log entry.
		if (!SaveLogEntry(stream, *result))
			return NULL;

		// Remember the last produced structure of the desired type.
		if (result->IsA(GetOutputType()))
			output = result;

	}

	// Can we use the input structure as a last resort output?
	if (output == NULL) {
		if (GetOutputType() == Undefined::Id() || structure.IsA(GetOutputType()))
			output = &structure;
		else
			Message::Error("Unable to return a " + IdHolder::GetClassname(GetOutputType()) + ".");
	}

	return output.Release();

}

 
ParallelExecutor *
ParallelExecutor::Clone() {
	return new ParallelExecutor;
}
