//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#ifdef PARALLEL
	#include <kernel/parallel/parallel.h>
#endif
#include <copyright.h>
#include <string> //-N

#include <kernel/algorithms/serialexecutorloop.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/batchclassifier.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/project.h>
#include <kernel/structures/projectmanager.h>
#include <kernel/structures/confusionmatrix.h> 	// sus

#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/rng.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>


#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>

#ifdef _OPENMP
	#include "omp.h"
	#include <kernel/utilities/threadcontrol.h>
#endif

using namespace std;

//-------------------------------------------------------------------
// Static methods (file scope)
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticCleanUp
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cleans up temp. stuff set in the ExecuteCommands
//                 method.
// Comments......: 
// Revisions.....: -N 30/05/13 Deprecated. Since testing is no
//			longer called with *this (replaced by
//			threadspecific SerialExecutor objects),
//			static cleanups aren't necessary.
//			The functions do remain both in definition
//			and calling throughout the code for legacy
//			and revision purposes.
//===================================================================

static void
StaticCleanUp(SerialExecutorLoop &executor, Id id) {

	//Do nothing at all. -N
	return;
	// Reset rules.
	executor.SetRules(Handle<Rules>(NULL));

	// Reset output type.
	executor.SetOutputType(id);

}

static void
StaticCleanUp(Project &project, const DecisionTable &table) {

	//Do nothing. -N
	return;
	// Remove project from project pool.
	ProjectManager::RemoveProject(&project);

	// Find child index.
	int index = project.FindChild(&table);

	// Remove child.
	if (index != Undefined::Integer())
		project.RemoveChild(index);
}

//-------------------------------------------------------------------
// Method........: StaticGetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the last rule set derived from the given
//                 decision table.
//
// Comments......: Current implementation does not test for "lastness"...
//			Deprecated and replaced with 
//			GetRulesFromTable.
// Revisions.....:
//===================================================================

static Handle<Rules>
StaticGetRules(const DecisionTable &table) {

	Handle<Rules>       rules;
	Identifier::Handles rulesets;

	bool recursive = true;

	// Get all children rulesets. If several, assume that the order in the list is an indication of being "last"...
	if (table.GetAllChildren(RULES, rulesets, recursive) && !rulesets.empty())
		rules = dynamic_cast(Rules *, rulesets[rulesets.size() - 1].GetPointer());

	if (rulesets.size() > 1)
		Message::Warning("More than one rule set detected, selecting one.", false);

	return rules;

}

//-------------------------------------------------------------------
// Methods for class SerialExecutorLoop
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

SerialExecutorLoop::SerialExecutorLoop() {
	SetN(5);
	SetLength(0);
}

SerialExecutorLoop::~SerialExecutorLoop() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(SerialExecutorLoop, SERIALEXECUTORLOOP, SerialExecutor)

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
SerialExecutorLoop::GetParameters() const {

	String parameters;

	// N.
	parameters += Keyword::Number();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetN());

	parameters += Keyword::Separator();

	// Seed.
	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());

	parameters += Keyword::Separator();

	// Length.
	parameters += Keyword::Length();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetLength());

	parameters += Keyword::Separator();

	return parameters + SerialExecutor::GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
SerialExecutorLoop::SetParameter(const String &keyword, const String &value) {

	// N.
	if (keyword == Keyword::Number() && value.IsInteger())
		return SetN(value.GetInteger());

	// Seed.
	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());

	// Length.
	if (keyword == Keyword::Length() && value.IsInteger())
		return SetLength(value.GetInteger());

	// MaxThreads.
	if (keyword == Keyword::MaxThreads() && value.IsInteger())
		return ThreadControl::SetMaxThreads(value.GetInteger());
	

	return SerialExecutor::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
SerialExecutorLoop::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Methods inherited from Executor.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExecuteCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Executes a pipeline with some kind of systematic
//                 sampling per iteration.
//
//                 The input pipeline is split into two pipelines,
//                 one for training and one for testing.
//
//                 Input is a decision table. For each of the n loop
//                 iterations, the table is split into a training and a
//                 testing table.
//
//                 Starting with the training table, all algorithms in
//                 the training pipeline are then applied in sequence,
//                 with the output of one being the input to the next.
//                 The training pipeline is assumed to produce a set
//                 of rules along the way.
//
//                 Then, starting with the testing table, all algorithms
//                 in the testing pipeline are applied in sequence,
//                 with the output of one being the input to the next.
//                 It is assumed that the testing pipeline produces a
//                 confusion matrix (via a batch classification structure)
//                 along the way, typically by a batch classifier.
//                 The rule set from the training pipeline is used by the
//                 batch classifier.
//
//                 Various statistics are collected from the resulting
//                 confusion matrix, and dumped to a log file.
//
//                 If the desired output type is Undefined::Id(), return
//                 the input structure unless an error occurs.
//
// Comments......: Currently, the training pipelibne is executed by a
//                 separate SerialExecutor object, while this object
//                 excutes the training pipeline.
//
//                 This method could probably be implemented in a much
//                 "cleaner" fashion by doing some restructuring
//                 and using separate SerialExecutor objects for
//                 both pipelines. That would also eliminate the
//                 messy temporary project stuff and the clean-up
//                 methods. However, the current implementation works,
//                 and "if it ain't broke, don't fix it".
//
// Revisions.....: -N 30/05/13
//		   Major revision. ExecuteCommands is now a threaded
//		   process. SerialExecutor objects are now used
//		   with both pipelines, and the temporary project
//		   stuff and static cleanups have been removed or
//		   deprecated. Rulesets are no longer passed via
//		   the SerialExecutorLoop object, and the handover
//		   to batchclassification algorithms is managed via
//		   the SerialExecutor pipelines locally. Static
//		   rule gathering from decision tables has been
//		   replaced with non-static versions. This also
//		   deprecates certain functions in the 
//		   SerialExecutorLoop class, such as SetRules
//		   and GetRules.
//		   -N 12/06/13
//		   Added A split to algorithms. These should now
//		   be properly threadspecific. There is still a
//		   certain amount of unnecessary code in here,
//		   and there is still sharing of object chains
//		   (though not on the algorithm level).
//===================================================================

Structure *
SerialExecutorLoop::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const {
	Message message;

	// Cast, type verified by IsApplicable method.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	Algorithm::Handles algorithms_training; // Algorithms, training pipeline (must produce a rule set).
	Algorithm::Handles algorithms_testing;  // Algorithms, testing pipeline (should include a batch classifier).
	Vector(String)     parameters_training; // Parameters, training pipeline.
	Vector(String)     parameters_testing;  // Parameters, testing pipeline.


	// Split pipeline.
	if (!SplitCommands(GetLength(), algorithms, parameters, algorithms_training, parameters_training, algorithms_testing, parameters_testing)) {
		Message::Error("Failed to split command sequence.");
		//return NULL;
	}

	// Operate on a masked table.
	bool masked = true;	
	// This method is conceptually const only.

	//Removes any classifier file with the name specified in the configuration if CLASSIFIER.SAVE mode has been set.
/*
	if (!RemoveExistingClassifierFile()) {
		Message::Error("Failed to remove pre-existing file: "+ GetClassifierSource());
		return NULL;
	}
*/
	SerialExecutorLoop *self = const_cast(SerialExecutorLoop *, this);

	RNG rng(GetSeed());

	message.Notify("Initializing sampling scheme...");

	// Initialize sampler.
	if (!self->InitializeSamplingScheme(*table, masked, rng))
		return NULL;

	// Get desired output type.
	Id outputtype = GetOutputType();

	// The structure to output.
	Handle<Structure> output;

	Vector(float) accuracies;
	Vector(float) rocareas;
	Vector(float) rocstderrs;


	ConfusionMatrix confusion_matrix;	// sus

	int i, j;

	// Do training/testing loop.
	
	// LL somewhat temporary structure, 
	// we would like to have CV as top layer 
	// like having A * B 

	accuracies.reserve(GetN());
	rocareas.reserve(GetN());
	rocstderrs.reserve(GetN());

	bool return_null = false;
	String error_str;
	Vector(string) logVector;
	//logVector.resize(GetN()); //190705 N: Changed reserve to resize.
	logVector.reserve(GetN());

	//ThreadControl control;	
//	int nThreads = 1;
	//No need for more threads than we can use. The rest can be saved for nesting.
	ThreadControl::SetTeamSize(GetN());
//	nThreads = control.SetThreadingEnvironment();
	//Initiated before parallel clause so it can be copied in via firstprivate (threadprivate). -N
	Handle<Rules> rules;
/*
	if (GetLoadClassifier())
	{
		message.Notify("Loading ruleset " + GetClassifierSource() + "...");
		rules = LoadClassifier(table.GetPointer(), masked);
		if (rules == NULL)
			return NULL;
	}	
*/
	/**************************************************************************************************************
	**
	** Parallel start. Variables before this point are here split into groupings that are private (threadspecific)
	** firstprivate(threadspecific and initialized with the value held before entering the parallel region, and
	** shared (not threadspecific and requiring either atomic alterations, in-critical section alteration,
	** or read-only states. All variables must be declared as belonging to one of these groups as there is no default
	** setting allowed.
	**
	***************************************************************************************************************/
	#pragma omp parallel \
	private( message, i, j, output) \
	firstprivate(rules, outputtype, rng, masked, algorithms_training, algorithms_testing, parameters_training, parameters_testing) \
	shared(structure, logVector, confusion_matrix, table, return_null, accuracies, rocareas, rocstderrs, self, error_str, stream) \
	default(none)
	{	
		ThreadControl::AddThreadToCount();
		//The upper limit of the for loop. -N
		const int table_size = GetN();

		#ifdef _OPENMP
			//Thread identifier. -N
			int this_thread = omp_get_thread_num();	
	
			Algorithm::Handles trainingSet;
			Algorithm::Handles testingSet;
			//Copy the algorithms. -N
			for ( i=0; i < algorithms_training.size(); i++) {
				Id id = IdHolder::GetId(IdHolder::GetClassname(algorithms_training[i]->GetId()));
				Handle<Algorithm> trainingAlgorithm = ObjectManager::GetIdentifiedAlgorithm(id);
				trainingSet.push_back(trainingAlgorithm);
			}
			for ( i=0; i < algorithms_testing.size(); i++) {
				Id id = IdHolder::GetId(IdHolder::GetClassname(algorithms_testing[i]->GetId()));
				Handle<Algorithm> testingAlgorithm = ObjectManager::GetIdentifiedAlgorithm(id);
				testingSet.push_back(testingAlgorithm);
			}


		#endif


	
		// Declare local variables for use in cv loop.
		ConfusionMatrix cv_confusion_matrix;
		Vector(float) cv_accuracies;
		Vector(float) cv_rocareas;
		Vector(float) cv_rocstderrs;

		
		/**************************************************************************************************************
		**
		** OPENMP will schedule the for loop using the available threads. -N
		** int i is still available to each loop, indicating which iteration is currently under way. -N
		** The schedule is non-static, and each thread will get a new assignment once the previous has been completed. -N
		** Threads will not synchronize at the end, but keep going.
		** In ordered sections, the threads will operate in the same order as a serial execution would. -N
		**
		**************************************************************************************************************/
		#pragma omp for schedule(static, 1) nowait ordered
		for (int i = 0; i < table_size; i++) {
			//parameters are firstprivate. -N
			// Replace all iteration no. "macros" with index of current iteration. Try to be case-tolerant.
			Vector(String) vTrainingParams(parameters_training);
			Vector(String) vTestingParams(parameters_testing);

			for (j = 0; j < vTrainingParams.size(); j++) {
				vTrainingParams[j].Replace("#iteration#", String::Format(i));
				vTrainingParams[j].Replace("#ITERATION#", String::Format(i));
			}
			for (j = 0; j < vTestingParams.size(); j++) {
				vTestingParams[j].Replace("#iteration#", String::Format(i));
				vTestingParams[j].Replace("#ITERATION#", String::Format(i));
			}
	
			std::string log;

			// Break all threads if a break is encountered in any of them.
			if (return_null) {
				message.Notify("Error found in one of the threads. Exiting...");
				//break;
			}
			
			if (!message.Progress("Executing iteration " + String::Format(i + 1) + "...", i, GetN())) {
				StaticCleanUp(*self, outputtype);
				return_null = true;
				error_str += "E1";
			}
	
			message.Notify("Sampling tables...");
	
			// Initialize training/testing tables before sampling. Since we don't know if
			// the input structures get physically modified by any of the algorithms in the
			// pipeline, we have to operate on duplicates for each iteration.
			Handle<DecisionTable> training = dynamic_cast(DecisionTable *, table->Duplicate());
			Handle<DecisionTable> testing  = dynamic_cast(DecisionTable *, table->Duplicate());
	
			// Sample training/testing tables.
			if (!SampleTables(i, rng, *training, *testing, masked)) {
				Message::Error("Failed to sample tables.");
				StaticCleanUp(*self, outputtype);
				return_null = true;
				error_str += "E2";
			}
	
			//Should a ruleset be generated from the decision table?
			// Set up training pipeline. (Executed by separate object.)
			SerialExecutor pipeline_training(this);
			// We want the originating decision table so that we get the dictionary as well.
			pipeline_training.SetOutputType(DECISIONTABLE);

			if (!message.Progress("Executing iteration " + String::Format(i + 1) + ", training pipeline...", i, GetN())) {
				StaticCleanUp(*self, outputtype);
				return_null = true;
				error_str += "E3 ";
				//break;
			}

				// Execute training pipeline.
			if (!GetLoadClassifier()) {
				Handle <DecisionTable> parenttable = dynamic_cast(DecisionTable *, pipeline_training.ExecuteCommands(*training, trainingSet, vTrainingParams, stream, false));
				rules = parenttable->GetRuleset();
			}
			else {
				rules = table->GetRuleset();
			}
//			if (!GetLoadClassifier()) {
/*
				if (parenttable == NULL) {
					Message::Error("Failed to execute training pipeline.");
					StaticCleanUp(*self, outputtype);
					return_null = true;
					error_str += "E4 ";
				//break;
				}
*/			

				// Get the last rule set derived from the output decision table.
				//Note that it's the ruleset that gets passed in here, not a pointer. We want to keep rules local,
				//and ensure the whole set is allocated in (threadspecific)  memory. -N
				//And now we point to the privately allocated rules. -N
//				rules = parenttable->GetRuleset();
/*
				if (!SaveClassifier(parenttable, masked)) {
					Message::Error("Failed to write classifier to file");
					
				}
*///			}	
			
			//Or should the rules be loaded from file instead?
			//Condition is included again for clarity.
			//else if (GetLoadClassifier()) {
			//	message.Notify("Loading ruleset");
			//	rules = LoadClassifier(GetClassifierSource(), training.GetPointer(), masked);
			//}
//			if (GetLoadClassifier())
//				rules = table->GetRuleset();
//			else
//				rules = parenttable->GetRuleset();


			if (rules == NULL) {		
				Message::Error("Unable to locate rule set generated in training pipeline.");
				StaticCleanUp(*self, outputtype);
				return_null = true;
				error_str += "E5 ";
				
			}
	
			// Execute testing pipeline.
			Handle<BatchClassification> batchclassification;

			SerialExecutor pipeline_testing(this);
			pipeline_testing.SetOutputType(BATCHCLASSIFICATION);
			pipeline_testing.SetRules(rules);
			//Clear the log entry for this thread.	
			//InitLog();

			//batchclassification = dynamic_cast(BatchClassification *, SerialExecutor::ExecuteCommands(*testing, testing_handles, parameters_testing_copy, threadstream));
			
		
			
			batchclassification = dynamic_cast(BatchClassification *, pipeline_testing.ExecuteCommands(*testing, testingSet, vTestingParams, stream, true));

/*
			if (batchclassification == NULL) {
				message.Notify("Failed to execute testing pipeline.");
				StaticCleanUp(*self, outputtype);
				StaticCleanUp(*project, *parenttable);
				#pragma omp critical(returnnull)
				{
					return_null = true;
					error_str += "E9 ";
				}
			}
*/
			// Display matrix contents in log file.
/*			if (!return_null)
				if (!SaveLogEntry(threadstream, *batchclassification)) {
					Message::Error("Could not save log entry.");	// sus
					//StaticCleanUp(*self, outputtype);
					//StaticCleanUp(*project, *parenttable);					
					return_null = true;
					error_str += "E10 ";
					
					//break;
				}	
*/
			
			if (!SaveThreadedEntry(log, *batchclassification, pipeline_training, pipeline_testing)) {
				Message::Error("Could not save log entry");
			}		
				
			// Collect statistics for this loop.
			cv_accuracies.push_back(batchclassification->GetConfusionMatrix().GetDiagonalRatio());

			if (cv_confusion_matrix.GetDimension() == 0)
				cv_confusion_matrix = batchclassification->GetConfusionMatrix();
			else
				cv_confusion_matrix += batchclassification->GetConfusionMatrix();
			
			if (batchclassification->GetROCArea() != Undefined::Float())
				cv_rocareas.push_back(batchclassification->GetROCArea());
	
			if (batchclassification->GetROCStandardError() != Undefined::Float())
				cv_rocstderrs.push_back(batchclassification->GetROCStandardError());
/*	
			#pragma omp critical(log)
			{
				SaveThreadedStatistics(log, cv_accuracies, "Accuracy");
				if (!rocareas.empty() && !SaveThreadedStatistics(log, cv_rocareas, "ROC.AUC"))
					return_null = true;
	
				if (!rocstderrs.empty() && !SaveThreadedStatistics(log, cv_rocstderrs, "ROC.AUC.SE"))
					return_null = true;
			}
*/

			// Try to adhere to specified output type.
			if (batchclassification->IsA(outputtype))
				output = batchclassification.GetPointer();
			else if (rules->IsA(outputtype))
				output = rules.GetPointer();
//			else if (parenttable->IsA(outputtype))
//				output = parenttable.GetPointer();
			else if (structure.IsA(outputtype))
				output = &structure;

			#pragma omp ordered
			{
				logVector.push_back(log);
				//#pragma omp critical(log)
				//logVector[i] = log; // 190702 - N: Removed need for ordered sections.
			}
			//#pragma omp barrier	
			// Clean up temporary project stuff.
//		:	StaticCleanUp(*project, *parenttable);
	
		} // end for loop
		// Fix local variables used in cv loop, this will slightly slow down the non-MP version
		#pragma omp critical(cvmatrix)
		{
			// Check if it's first update of confusion_matrix, othervise add
			if (confusion_matrix.GetDimension() == 0)
				confusion_matrix = cv_confusion_matrix;
			else
				confusion_matrix += cv_confusion_matrix;
		
			accuracies.insert(accuracies.begin(),cv_accuracies.begin(),cv_accuracies.end());
			rocareas.insert(rocareas.begin(),cv_rocareas.begin(),cv_rocareas.end());
			rocstderrs.insert(rocstderrs.begin(),cv_rocstderrs.begin(),cv_rocstderrs.end());
		}
		ThreadControl::RemoveThreadFromCount();
	} // Parallel stop
//	if (!SaveClassifier(table, masked)) {
//		append_ = true;
//		Message::Error("Failed to save classifier");
//	}
	if (return_null) {
		message.Notify("Error found");
		cout << error_str << endl;
		return NULL;
	}

	// Clean up.
	StaticCleanUp(*self, outputtype);

	std::string log;	
	
	// MPI collecting data from processors
	// Display statistics in log file.
	if (!SaveThreadedStatistics(log, accuracies, "Accuracy"))
		return NULL;

	if (!rocareas.empty() && !SaveThreadedStatistics(log, rocareas, "ROC.AUC"))
		return NULL;

	if (!rocstderrs.empty() && !SaveThreadedStatistics(log, rocstderrs, "ROC.AUC.SE"))
		return NULL;

	logVector.push_back(log);

	if (!PrintThreadLog(logVector, stream))
		Message::Error("Could not write log");

	// Output confusion matrix for whole cv - sus
	String formatted;
	String indent(' ', 1 + SystemKit::GetTimestamp().GetLength());
	confusion_matrix.Format(formatted, indent);
	stream << endl << formatted << endl;

	if (output == NULL) {
		if (outputtype == Undefined::Id())
			output = &structure;
		else
			Message::Error("Unable to return a structure of type " + IdHolder::GetClassname(GetOutputType()) + ".");
	}


	return output.Release();

}

//-------------------------------------------------------------------
// Methods inherited from SerialExecutor.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetSpecialParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...: Used in the testing pipeline.
//                 If the algorithm is a batch classifier, then set
//                 rules produced in training pipeline.
// Comments......:
// Revisions.....: 
//===================================================================

bool
SerialExecutorLoop::SetSpecialParameters(Algorithm &algorithm, const String &parameters) const {

	if (!algorithm.IsA(BATCHCLASSIFIER))
		return true;

	// Cast to verified type.
	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *, &algorithm);

	// Set rules.
//	#ifdef _OPENMP
//	int this_thread = omp_get_thread_num();
//	if (!batchclassifier->SetRules(GetRules(this_thread).GetPointer()))
//		return false;
//	#else
	if (!batchclassifier->SetRules(GetRules().GetPointer()))
		return false;
//	#endif

	// Set parameters again, if some of them are for the embedded classifier.
	algorithm.SetParameters(parameters);
	batchclassifier->SetRules(GetRules().GetPointer());

	return true;

}

//-------------------------------------------------------------------
// Method........: SetSpecialParameters
// Author........: Nicholas Baltzer
// Date..........: 29/05/13
// Description...: Used in the testing pipeline.
//                 If the algorithm is a batch classifier, then set
//                 rules produced in training pipeline and passed
//		   to the pipeline_testing object. 
// Comments......: Similar to original version, but instead of passing
//		   rules from *this, get them from the ruleset
//		   passed (which should be an SErule). This means
//		   that the testing pipeline is not dependent on
//		   *this, which should alleviate several threading
//		   issues.
//
//
//		   
// Revisions.....: -N 30/05/13 Moved assignment to SerialExecutor.
//		   Function remains should there be need for it
//		   later.
//===================================================================
/*
bool
SerialExecutorLoop::SetSpecialParameters(Algorithm &algorithm, const String &parameters, Handle<Rules> ruleset) const {

	if (!algorithm.IsA(BATCHCLASSIFIER))
		return true;

	// Cast to verified type.
	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *, &algorithm);

	// Set rules.
//	#ifdef _OPENMP
//	int this_thread = omp_get_thread_num();
//	if (!batchclassifier->SetRules(GetRules(this_thread).GetPointer()))
//		return false;
//	#else
//	if (!batchclassifier->SetRules(GetRules().GetPointer()))
	//Changed to set rules passed, as these are the SE rules. -N
	if (!batchclassifier->SetRules(ruleset.GetPointer()))	
		return false;
//	#endif

	// Set parameters again, if some of them are for the embedded classifier.
	algorithm.SetParameters(parameters);

	return true;

}
*/
//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SplitCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Splits the input commands/parameters and returns
//                 (in-place) two command/parameter sets.
// Comments......:
// Revisions.....:
//===================================================================

bool
SerialExecutorLoop::SplitCommands(int index, const Algorithm::Handles &algorithms,  const Vector(String) &parameters, Algorithm::Handles &algorithms1, Vector(String) &parameters1, Algorithm::Handles &algorithms2, Vector(String) &parameters2) const {

	// Clear vectors.
	algorithms1.erase(algorithms1.begin(), algorithms1.end());
	algorithms2.erase(algorithms2.begin(), algorithms2.end());
	parameters1.erase(parameters1.begin(), parameters1.end());
	parameters2.erase(parameters2.begin(), parameters2.end());

	// Check input.
	if (algorithms.size() != parameters.size())
		return false;

	// Check index validity.
	if (index < 0 || index > algorithms.size())
		return false;

	int i;

	// Do split.
	for (i = 0; i < algorithms.size(); i++) {
		if (i < index) {
			algorithms1.push_back(algorithms[i]);
			parameters1.push_back(parameters[i]);
		}
		else {
			algorithms2.push_back(algorithms[i]);
			parameters2.push_back(parameters[i]);
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: InitializeSamplingScheme
// Author........: Aleksander šhrn
// Date..........:
// Description...: Initializes internals used for partitioning
//                 data into training and testing sets.
//
//                 Default implementation does nothing.
// Comments......:
// Revisions.....:
//===================================================================

bool
SerialExecutorLoop::InitializeSamplingScheme(const DecisionTable &/*table*/, bool /*masked*/, const RNG &/*rng*/) {
	return true;
}


//-------------------------------------------------------------------
// Method........: PrintThreadLog
// Author........: Nicholas Baltzer
// Date..........: 03/06/13
// Description...: Prints a string vector to file. This should only
//		   be executed in serial (keep outside of parallel
//		   sections).
// Comments......:
// Revisions.....:
//===================================================================
bool
SerialExecutorLoop::PrintThreadLog(Vector(string) &logVector, ofstream &stream) const {

	for (int i = 0; i < logVector.size(); i++) {
		stream << logVector[i];
	}

	return true;

}
/*
//-------------------------------------------------------------------
// Method........: WritePreStatusToLog
// Author........: Nicholas Baltzer
// Date..........: 05/06/13
// Description...: Appends a string to the SEL log_. 
//		   
//		   
// Comments......: Intended for use in SerialExecutor::ExecuteCommands
// Revisions.....: 
//	
//			
//===================================================================
bool
SerialExecutorLoop::WritePreStatusToLog(const Algorithm &algorithm) {

	std::string timestamp = SystemKit::GetTimestamp().GetString();
	std::string indent(timestamp.length(), ' ');

	AppendLog(timestamp + "Executing " + IdHolder::GetClassname(algorithm.GetId()).GetString() + "...\n");
	AppendLog(indent + " Parameters = {" + algorithm.GetParameters().GetString() + "}\n");
	
	return true;

}
*/
//-------------------------------------------------------------------
// Method........: SaveThreadedEntry
// Author........: Nicholas Baltzer
// Date..........: 03/06/13
// Description...: Same as SaveLogEntry, but instead of writing to a
//		   file output, appends the output to a shared
//		   vector element.
// Comments......:
// Revisions.....: -N 04/06/13 Changed vector insertions to string
//			appends. Works better with atomic insertions
//			and collaborates with SE log_.
//===================================================================

bool
SerialExecutorLoop::SaveThreadedEntry(std::string &log, const BatchClassification &batchclassification, SerialExecutor &pipeline_training, SerialExecutor &pipeline_testing) const {

	
	String indent(' ', 1 + SystemKit::GetTimestamp().GetString().length());
	String formatted;

	// Output confusion matrix.
	if (!batchclassification.GetConfusionMatrix().Format(formatted, indent))
		return false;
	

	// Output ROC stuff, if present.
	if (batchclassification.GetROCArea() != Undefined::Float()) {

		//int precision = 8;
		//char* buffer;
		//int decimal, sign;

			
		string entry = "-------------Thread " + String::Format(omp_get_thread_num()).GetString() + "\n" + pipeline_training.GetLog() + pipeline_testing.GetLog(); 
		entry += "\n" + indent.GetString() + "\n" + formatted.GetString() + "\n";

//		buffer = fcvt(batchclassification.GetROCArea(), precision, &decimal, &sign);
		entry += indent.GetString() +  "ROC.AUC    = " + String::Format(batchclassification.GetROCArea()).GetString()+  "\n";

//		buffer = fcvt(batchclassification.GetROCStandardError(), precision, &decimal, &sign);		
		entry += indent.GetString() + "ROC.AUC.SE = "  + String::Format(batchclassification.GetROCStandardError()).GetString() +  "\n\n";
		log += entry;
	}
	
	return true;

}
//-------------------------------------------------------------------
// Method........: SaveLogEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 981202: Added ROC information.
//===================================================================

bool
SerialExecutorLoop::SaveLogEntry(ofstream &stream, const BatchClassification &batchclassification) const {

	String indent(' ', 1 + SystemKit::GetTimestamp().GetLength());

	String formatted;

	// Output confusion matrix.
	if (!batchclassification.GetConfusionMatrix().Format(formatted, indent))
		return false;

	stream << endl << formatted << endl;

	// Output ROC stuff, if present.
	if (batchclassification.GetROCArea() != Undefined::Float()) {
		stream << indent << "ROC.AUC    = " << batchclassification.GetROCArea() << endl;
		stream << indent << "ROC.AUC.SE = " << batchclassification.GetROCStandardError() << endl << endl;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SaveThreadedStatistics
// Author........: Nicholas Baltzer
// Date..........: 03/06/13
// Description...: Store the formatted statistics in a string vector
//		   for printing. Threadsafe. Writing is handled
//		   later.
// Comments......:
// Revisions.....: -N 04/06/13 Changed the vector to a string.
//			Now works better with atomic insertions
//			to the output file. 
//===================================================================

bool
SerialExecutorLoop::SaveThreadedStatistics(std::string &log, const Vector(float) &statistics, const String &name) const {

	String indent2(' ', 1 + SystemKit::GetTimestamp().GetLength());
	string indent = indent2.GetString();

	string title = name.GetString();

	// Compute statistics.
	float mean    = MathKit::Mean(statistics);
	float median  = MathKit::Median(statistics);
	float stddev  = MathKit::StandardDeviation(statistics);
	float minimum = MathKit::Minimum(statistics);
	float maximum = MathKit::Maximum(statistics);

	// Format.
	String formatted_mean    = (mean    == Undefined::Float())? Undefined::String() : String::Format(mean);
	String formatted_median  = (median  == Undefined::Float())? Undefined::String() : String::Format(median);
	String formatted_stddev  = (stddev  == Undefined::Float())? Undefined::String() : String::Format(stddev);
	String formatted_minimum = (minimum == Undefined::Float())? Undefined::String() : String::Format(minimum);
	String formatted_maximum = (maximum == Undefined::Float())? Undefined::String() : String::Format(maximum);

	log += \
			indent + title + ".Mean    = " + formatted_mean.GetString()    + "\n" \
			+ indent + title + ".Median  = " + formatted_median.GetString()  + "\n" \
			+ indent + title + ".StdDev  = " + formatted_stddev.GetString()  + "\n" \
			+ indent + title + ".Minimum = " + formatted_minimum.GetString() + "\n" \
			+ indent + title + ".Maximum = " + formatted_maximum.GetString() + "\n\n";

	return true;

}
//-------------------------------------------------------------------
// Method........: SaveLogStatistics
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Not for use inside parallel regions. -N
// Revisions.....:
//===================================================================

bool
SerialExecutorLoop::SaveLogStatistics(ofstream &stream, const Vector(float) &statistics, const String &name) const {

	String indent(' ', 1 + SystemKit::GetTimestamp().GetLength());


	// Compute statistics.
	float mean    = MathKit::Mean(statistics);
	float median  = MathKit::Median(statistics);
	float stddev  = MathKit::StandardDeviation(statistics);
	float minimum = MathKit::Minimum(statistics);
	float maximum = MathKit::Maximum(statistics);

	// Format.
	String formatted_mean    = (mean    == Undefined::Float())? Undefined::String() : String::Format(mean);
	String formatted_median  = (median  == Undefined::Float())? Undefined::String() : String::Format(median);
	String formatted_stddev  = (stddev  == Undefined::Float())? Undefined::String() : String::Format(stddev);
	String formatted_minimum = (minimum == Undefined::Float())? Undefined::String() : String::Format(minimum);
	String formatted_maximum = (maximum == Undefined::Float())? Undefined::String() : String::Format(maximum);

	//stream << indent << "---Results" << endl;

	stream << indent << name << ".Mean    = " << formatted_mean    << endl;
	stream << indent << name << ".Median  = " << formatted_median  << endl;
	stream << indent << name << ".StdDev  = " << formatted_stddev  << endl;
	stream << indent << name << ".Minimum = " << formatted_minimum << endl;
	stream << indent << name << ".Maximum = " << formatted_maximum << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: SaveClassifier
// Author........: Nicholas Baltzer
// Date..........: 06/08/13
// Description...: Checks that the save_ parameter has been set, then saves the ruleset
//			obtained from the dataset to the specified file. The rules are mapped
//			through the dictionary such that the names are not specific to the supplied
//			dataset.
// Comments......: 
// Revisions.....:
//===================================================================
/*
bool 
SerialExecutorLoop::SaveClassifier(const Handle<DecisionTable> table, bool masked) const {
	
	//Should the rules be saved?
	if (!save_)
		return true;

	Message message;

	
	#pragma omp critical(ruleset)
	{	
	message.Notify("Saving ruleset...");	
	//Grab the rules from the decision table.
	ofstream stream;
	//Use the supplied filename for storage.
	
	IOKit::Open(stream ,classifierfile_, true);	

//	for (int j=0 ; j < rulesets_.size();j++) {
		Handle<Rules> ruleset = table->GetRuleset();

		for (int i = 0; i < ruleset->GetNoRules(); i++) {
			//Save the rule to the file. 
			ruleset->GetRule(i)->SaveMappedRule(stream, table, masked);
		}
//	}
//	bool append = true;
//	SetAppend(appendClassifierFile, true);
	//append_ = true;
	//And done.
	IOKit::Close(stream);
	}
	return true;
}


//-------------------------------------------------------------------
// Method........: LoadClassifier
// Author........: Nicholas Baltzer
// Date..........: 06/08/13
// Description...: Loads a ruleset from the supplied classifier file. The rules
//			are mapped through the decisiontable dictionary to properly
//			set their index values.
// Comments......: 
// Revisions.....:
//===================================================================

Handle<Rules>
SerialExecutorLoop::LoadClassifier(const String &sourcefile, Handle<DecisionTable> table, bool masked) const {

	//Should rules be loaded?
	if (!load_)
		return NULL;	
	//new ruleset.
	Handle<Rules> classifierRules = Creator::Rules();

	//Load them.
	if (!classifierRules->LoadRulesetFromFile(sourcefile, table, masked))
		Message::Error("SerialExecutorLoop::LoadClassifier -> Failed to build ruleset from file");
	//If we have atleast one rule in the set, it's a success.
	return classifierRules->GetNoRules() > 0 ? classifierRules : NULL;		
		
	
}


//-------------------------------------------------------------------
// Method........: FileExists
// Author........: Nicholas Baltzer
// Date..........: 12/08/13
// Description...: Static check. Looks for an existing file with the filename
//			specified in GetClassifierSource() (classifierfile_).
// Comments......: 
// Revisions.....:
//===================================================================
bool
SerialExecutorLoop::FileExists(String file) const {
	ifstream stream(file.GetString().c_str());
	return stream;
}

//-------------------------------------------------------------------
// Method........: RemoveExistingClassifierFile
// Author........: Nicholas Baltzer
// Date..........: 12/08/13
// Description...: Helper function to determine if there is an existing rcl
//			with the specified filename already. If that is the case,
//			delete that file.
// Comments......: 
// Revisions.....:
//===================================================================

bool 
SerialExecutorLoop::RemoveExistingClassifierFile() const {
	if (save_ == true) {
		//Does the file exist already?
		if (SerialExecutorLoop::FileExists(GetClassifierSource()))
			return !remove(GetClassifierSource().GetString().c_str());
		else
			//File does not exist. We're good.
			return true;
	}
	//Not in save mode, so we're good.
	return true;
}
*/
