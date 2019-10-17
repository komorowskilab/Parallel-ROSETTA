//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/serialexecutor.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/project.h>
#include <kernel/structures/projectmanager.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

#include <common/objectmanager.h>

// -N 30/05/13
#include <kernel/algorithms/batchclassifier.h>
#include <kernel/algorithms/serialexecutorloop.h>
#include <kernel/algorithms/serialexecutorloop.cpp>

#ifdef _OPENMP // -N
	#include "omp.h"
#endif
//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticCleanUp
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cleans up temporary project stuff set in the
//                 ExecuteCommands method.
// Comments......:
// Revisions.....: -N 14/06/13
//		   Does nothing anymore. Retained for future changes.
//===================================================================

static void
StaticCleanUp(Handle<Project> project, Handle<Structure> first, Structure &structure, bool remove) {

	
	return;
	// Remove project from project pool.
	ProjectManager::RemoveProject(project.GetPointer());

	// Don't remove the child?
	if (!remove)
		return;

	if (first == NULL)
		return;

	// Find child index.
	int index = structure.FindChild(first.GetPointer());

	// Remove child.
	if (index != Undefined::Integer())
		structure.RemoveChild(index);

}

#if 0
static void
StaticCleanUp(Handle<Project> project, Handle<Structure> first, Structure &structure, Handle<Structure> output, bool remove) {

	// Remove project from project pool.
	ProjectManager::RemoveProject(project.GetPointer());

	if (output == &structure && !remove)
		return;

	if (first == NULL)
		return;

	// Find child index.
	int index = structure.FindChild(first.GetPointer());

	// Remove child.
	if (index != Undefined::Integer())
		structure.RemoveChild(index);

}
#endif

//-------------------------------------------------------------------
// Methods for class SerialExecutor.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

SerialExecutor::SerialExecutor(const Executor *in) {
	if (in->GetSaveClassifier())
		SetSave(in->GetClassifierSource());
	if (in->GetLoadClassifier())
		SetLoad(in->GetClassifierSource());
	SErules_ = NULL;
}

SerialExecutor::SerialExecutor() {
	SErules_ = NULL;
	load_ = false;
	save_ = false;
}

SerialExecutor::~SerialExecutor() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(SerialExecutor, SERIALEXECUTOR, Executor)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
SerialExecutor::GetParameters() const {
	return Executor::GetParameters();
}

//-------------------------------------------------------------------
// Method........: etParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
SerialExecutor::SetParameter(const String &keyword, const String &value) {
	return Executor::SetParameter(keyword, value);
}

//-------------------------------------------------------------------
// Methods inherited from Executor.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExecuteCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Adds a fixed Boolean argument and passes the
//                 command on.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
SerialExecutor::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const {
	return ExecuteCommands(structure, algorithms, parameters, stream, true);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetSpecialParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...: Can be used if an algorithm needs to have some
//                 parameters set that cannot be represented as a
//                 text string.
//
//                 Default implementation does nothing. Can be
//                 overloaded by special-purpose subclasses.
// Comments......:
// Revisions.....:
//===================================================================

bool
SerialExecutor::SetSpecialParameters(Algorithm &/*algorithm*/, const String &/*parameters*/) const {
	return true;
}

//-------------------------------------------------------------------
// Method........: SetSERules
// Author........: Nicholas Baltzer
// Date..........: 30/05/13
// Description...: Sets the batchclassifier ruleset from the 
//                 SerialExecutor SErules_, and resets the 
//                 parameters.
//
//                 
//                 
// Comments......: Intended for use with the parallel application
//		   of crossvalidation. Using SerialExecutor objects
//		   as wrappers for each thread solves some threading
//		   issues, and creates fewer shared variables
//		   (such as the SerialExecutorLoop object calling
//		   the pipeline).
// Revisions.....: 14/06/13
//		   Added a second SetRules to reset the rules upon
//		   creating a new classifier embedded in the batchclassification
//		   process. Also moved the batchclassifier verification
//		   to the ExecuteCommands section to allow for validation
//		   of the rules and parameters. 
//===================================================================

bool 
SerialExecutor::SetSERules(Algorithm &algorithm, const String &parameters, Handle<Rules> ruleset) const {


	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *, &algorithm);

	//Set rules.
	//If the ruleset is empty, try the inherited ruleset.
	if (ruleset == NULL) {
		if (!batchclassifier->SetRules(GetRules().GetPointer()) || !batchclassifier->SetParameters(parameters) \
		|| !batchclassifier->SetRules(GetRules().GetPointer()))
		return false;
	}
	//Try the supplied ruleset.
	else if (!batchclassifier->SetRules(ruleset.GetPointer()) || !batchclassifier->SetParameters(parameters) \
		|| !batchclassifier->SetRules(ruleset.GetPointer()))
		return false;

	//Set parameters again with the rules specified. This also sets the classifier to be used. 
	//Set rules again for the new classifiers.
	return true;
}

//-------------------------------------------------------------------
// Method........: ExecuteCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Starting with the input structure, all algorithms in
//                 the pipeline are applied in sequence, with the output
//                 of one being the input to the next. The last produced
//                 structure of a specified type is returned. If the
//                 specified type is Undefined::Id(), the input structure
//                 itself is returned unless an error occurs.
//
//                 A Boolean flag is passed to indicate how the clean-up
//                 is performed. Ordinarily, children of the input structure
//                 are to be removed. However, if the input structure is
//                 equal to the output structure, this may not always be
//                 desirable.
//
// Comments......: A temporary project is set up so that the
//                 FindParent method will function properly in case
//                 any of the algorithms need this.
// Revisions.....: -N 14/06/13
//		   Major revision. Instantiations moved to top
//		   for debugging purposes. Multiple CleanUps removed.
//		   Added SetSERules to grab object specific rulesets
//		   from SerialExecutor objects instead of static
//		   grabs (to allow threadspecific rules and algorithms
//		   as well as classifiers and executors).Several of 
//		   the older functions are now deprecated.
//		   Writing is now suspended by the threads in favour
//		   of threadspecific logfiles that will be printed
//		   once the process is concluded. Print functions
//		   in Executor are thus deprecated. The function is
//		   now considered threadsafe, and each thread space
//		   has its own set of algorithms, rules, logs, and
//		   classifiers.
//===================================================================

Structure *
SerialExecutor::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream, bool remove, bool execute) const {

	// Set up a temporary project.
	Handle<Project> 	project = Creator::Project();
	Handle<Structure> 	current; // Current structure in pipeline.
	Handle<Structure> 	first;   // Structure generated from method input.
	Handle<Structure> 	output;  // Output to return.
	Handle<Algorithm> 	algorithm;
	Handle<Structure> 	result;
	
	Handle<Rules> 		ruleset = Creator::Rules();	
	Structure *release;
	int i;
	bool test;

	test = ProjectManager::InsertProject(project.GetPointer());
	
	if (!test) {
		Message::Error("Failed to insert project."); 	// sus - test
		return NULL;
	}
	if (!project->AppendChild(&structure)) {
		ProjectManager::RemoveProject(project.GetPointer());
		Message::Error("Failed to append structure to project."); 	// sus - test
		return NULL;
	}

		
		

	
	current = &structure;

	if (GetLoadClassifier() && GetRules() == NULL) {
		if (structure.IsA(DECISIONTABLE)) {
			Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
			ruleset = table->GetRuleset();
			
			if (!algorithms[0]->IsApplicable(structure)) {
			Message::Warning(IdHolder::GetClassname(algorithms[0]->GetId()) + " is not applicable to a " + IdHolder::GetClassname(current->GetId()) + ". Attempting ruleset conversion.");
				Structure *conversion;
				conversion = ruleset.GetPointer();
				current = conversion;
			}
			
		}
		if (ruleset->GetNoRules() == 0) {
			Message::Error("Failed to set rules.");
			return NULL;
		
		}
	}
	else {
		ruleset = GetRules();
	}

	
	// Execute pipeline.
	for (i = 0; i < algorithms.size(); i++) {
				
		// Get algorithm.
		algorithm = algorithms[i];	
	
		// Set parameters. This is necessary for all algorithms. -N
		algorithm->SetParameters(parameters[i]);

		// Needed for specific algorithms. -N
		test = SetSpecialParameters(*algorithm, parameters[i]);
		if (!test) {
			Message::Error("Failed to set special parameters for " + IdHolder::GetClassname(algorithm->GetId()) + ".");
			StaticCleanUp(project, first, structure, true);
			return NULL;
		}
		//Only batchclassifiers need this. Rearranged to save time (and prevent errors). -N

		if (algorithm->IsA(BATCHCLASSIFIER)) {
			//Set the rules and init classifiers.. -N
//			test = SetSERules(*algorithm, parameters[i], GetRules());				
			test = SetSERules(*algorithm, parameters[i], ruleset);			
			if (!test) {
				Message::Error("Failed to set SERules for " + IdHolder::GetClassname(algorithm->GetId()) + ".");
				StaticCleanUp(project, first, structure, true);
				return NULL;
			}
		}
		
		// Check applicability.
		if (!algorithm->IsApplicable(*current)) {
			Message::Error(IdHolder::GetClassname(algorithm->GetId()) + " is not applicable to a " + IdHolder::GetClassname(current->GetId()) + ".");
			StaticCleanUp(project, first, structure, true);
			return NULL;
		}

/*		// Save algorithm status to log.
		if (!SaveLogEntry(stream, *algorithm)) {
			Message::Error("Failed to save log entry."); 	// sus - test
			StaticCleanUp(project, first, structure, true);
			return NULL;
		}
*/
				
		Message message;
	

		/*************************************************************************************************
		*This is the execution step of the process. An algorithm is applied on the structure, 
		*and the resultant structure (if any), is returned. Note that the input structure may 
		*also be the output structure.
		*
		*Any optimizations should be performed in the relevant structure and/or algorithms, 
		*not here, as the granularity is too coarse. Enveloping this in a critical section 
		*is comparable to serializing the entire process. -N
		**************************************************************************************************/

			
		
		//Writes pre-entry status to the execution buffer. Replaces filewrite. -N
		WritePreStatusToLog(*algorithm);

		message.Notify("Executing " + IdHolder::GetClassname(algorithm->GetId()) + "...");
		
		result = current->Apply(*algorithm);
		
		if (result == NULL) {
			Message::Error("Application of " + IdHolder::GetClassname(algorithm->GetId()) + " failed.");
			StaticCleanUp(project, first, structure, true);
			return NULL;
		}
		
		// Save result structure status to log.
/*		if (!SaveLogEntry(stream, *result)) {
			Message::Error("Failed to save results log entry."); 	// sus - test
			StaticCleanUp(project, first, structure, true);
			return NULL;
		}
*/	
		//Writes log entry to execution buffer. Replaces the filewrite. -N	
		WritePostStatusToLog(*result);

		// Possibly append as child (for dictionary purposes, mostly).		
		if (result != current)
			current->AppendChild(result.GetPointer());

		if (first == NULL)
			first = result;
	
		// Remember the last produced structure of the desired type.
		if (result->IsA(GetOutputType()))
			output = result;

		current = result;
			
	
	}	
		
	// Was an output produced? If not, does the input match the desired type?
	if (output == NULL) {
		if (GetOutputType() == Undefined::Id() || structure.IsA(GetOutputType()))
			output = &structure;
	}

	if (output == NULL)
		Message::Error("Unable to return a structure of type " + IdHolder::GetClassname(GetOutputType()) + ".");

	// Clean up.
	if (remove)
		StaticCleanUp(project, first, structure, true);
	else
		StaticCleanUp(project, first, structure, output != &structure);

	// So that handles do not inadvertently delete output structure on method exit.
	current = NULL;
	first   = NULL;
		
	if (GetSaveClassifier()) {
		if (output->IsA(DECISIONTABLE)) {
			Handle<DecisionTable> table = dynamic_cast(DecisionTable *, output.GetPointer());
			if (table->GetRuleset()->GetNoRules())
				SaveClassifier(table, true);
			else
				Message::Error("No rules to save. SaveClassifier failed.");
		}
		else if (output->IsA(RULES) && structure.IsA(DECISIONTABLE)) {
			Message::Warning("Output is a RULES structure. Using input decisiontable for mapping.");
			Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
			table->AppendStructure(output.GetPointer());
			SaveClassifier(table, true);
		}
/*
		else {
			Message::Error("SerialExecutor -> Cannot save classifier, output structure does not match.");
		}
*/
	}	
	release = output.Release();	

	return release;

}
 
SerialExecutor *
SerialExecutor::Clone() {
	return new SerialExecutor;
}
