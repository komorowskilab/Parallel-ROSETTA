//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/permloocvserialexecutor.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/structures/roccurve.h>

#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

#include <time.h>


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
// Revisions.....:
//===================================================================

static void
StaticCleanUp(SerialExecutorLoop &executor, Id id) {
	
	// Reset rules.
	executor.SetRules(Handle<Rules>(NULL));
	
	// Reset output type.
	executor.SetOutputType(id);
	
}

static void
StaticCleanUp(Project &project, const DecisionTable &table) {
	
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
// Revisions.....:
//===================================================================
/*
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
*/

static Rules *
StaticGetRules(const DecisionTable &table) {
	
	Handle<Rules>       rules;
	Identifier::Handles rulesets;
	
	bool recursive = true;
	
	// Get all children rulesets. If several, assume that the order in the list is an indication of being "last"...
	if (table.GetAllChildren(RULES, rulesets, recursive) && !rulesets.empty())
		rules = dynamic_cast(Rules *, rulesets[rulesets.size() - 1].GetPointer());
	
	if (rulesets.size() > 1)
		Message::Warning("More than one rule set detected, selecting one.", false);
	
	return rules.Release();
	
}


//-------------------------------------------------------------------
// Methods for class PermLeaveOneOutCVSerialExecutor.
//===================================================================


//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

PermLeaveOneOutCVSerialExecutor::PermLeaveOneOutCVSerialExecutor() {
	InvertSampling(false);
}

PermLeaveOneOutCVSerialExecutor::~PermLeaveOneOutCVSerialExecutor() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(PermLeaveOneOutCVSerialExecutor, PERMLOOCVSERIALEXECUTOR, SerialExecutorLoop)

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
PermLeaveOneOutCVSerialExecutor::GetParameters() const {
	
	String parameters;
	
	// Invert.
	parameters += Keyword::Invert();
	parameters += Keyword::Assignment();
	parameters += String::Format(InvertSampling());
	
	parameters += Keyword::Separator();
	
	return parameters + SerialExecutorLoop::GetParameters();
	
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
PermLeaveOneOutCVSerialExecutor::SetParameter(const String &keyword, const String &value) {
	
	// Invert.
	if (keyword == Keyword::Invert() && value.IsBoolean())
		return InvertSampling(value.GetBoolean());
	
	return SerialExecutorLoop::SetParameter(keyword, value);
	
}

//-------------------------------------------------------------------
// Methods inherited from SerialExecutorLoop.
//===================================================================

//-------------------------------------------------------------------
// Method........: InitializeSamplingScheme
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
PermLeaveOneOutCVSerialExecutor::InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng) {
	
	int n = GetN();
	
	if (n <= 0)
		return false;
	
	int no_objects = table.GetNoObjects(masked);
	
	// Clear and initialize vector of partition indices.
	indices_.erase(indices_.begin(), indices_.end());
	
	// Reserve space to avoid unneeded allocations.
	indices_.reserve(no_objects);
	
	int i, j, k;
	
	for (i = 0; i < no_objects; i++)
		indices_.push_back(Undefined::Integer());
	
	// Create vector for sampling indices.
	Vector(int) remaining;
	
	// Reserve space to avoid unneeded allocations.
	remaining.reserve(no_objects);
	
	for (i = 0; i < no_objects; i++)
		remaining.push_back(i);
	
	// Compute partitionsize, rounded to nearest integer.
	int partitionsize = MathKit::Round(static_cast(float, no_objects) / n);
	
	// Compute partitions. The last partition may be non-ideal in size.
	for (i = 0; i < n; i++) {
		for (j = 0; j < partitionsize; j++) {
			if (remaining.empty())
				break;
			int index = rng.DrawInteger(0, remaining.size() - 1);
			indices_[remaining[index]] = i;
			remaining[index] = remaining[remaining.size() - 1];
			remaining.pop_back();
		}
		if (i == n - 1 && !remaining.empty()) {
			for (k = 0; k < remaining.size(); k++)
				indices_[remaining[k]] = n - 1;
		}
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: SampleTables
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a set of partioning indices, returns (in-place)
//                 suitable training and testing subtables.
//
//                 If specified, the contents of the two returned
//                 tables are swapped.
//
//                 Assumes that the input training and testing tables
//                 are duplicates of the table the partitioning indices
//                 were generated from.
//
// Comments......: Can be optimized.
//
//                 Assumes that InitializeSampler has initialized
//                 the partition index mapping.
// Revisions.....:
//===================================================================

bool
PermLeaveOneOutCVSerialExecutor::SampleTables(int i, const RNG &/*rng*/, DecisionTable &training, DecisionTable &testing, bool masked) const {
	
	// Verify dimensions.
	if (indices_.size() != training.GetNoObjects(masked))
		return false;
	
	if (indices_.size() != testing.GetNoObjects(masked))
		return false;
	
	// Remove children, if any.
	training.RemoveAllChildren();
	testing.RemoveAllChildren();
	
	int j;
	
	// Sample tables (by removing objects)
	for (j = indices_.size() - 1; j >= 0; j--) {
		
		// Does the current object belong to the training table?
		bool is_training = (indices_[j] != i);
		
		// Invert sampling? (Swap training/testing table proportions?)
		if (invert_)
			is_training = !is_training;
		
		// Remove object.
		if (is_training) {
			if (!testing.RemoveObject(j, masked))
				return false;
		}
		else {
			if (!training.RemoveObject(j, masked))
				return false;
		}
		
	}
	
	return true;
	
}



Structure *
PermLeaveOneOutCVSerialExecutor::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const {
	clock_t start,stop;
	start=clock();
	Message message;
	
	
	// Cast, type verified by IsApplicable method.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	int permutation=GetPermutation();
	Algorithm::Handles algorithms_training; // Algorithms, training pipeline (must produce a rule set).
	Algorithm::Handles algorithms_testing;  // Algorithms, testing pipeline (should include a batch classifier).
	Vector(String)     parameters_training; // Parameters, training pipeline.
	Vector(String)     parameters_testing;  // Parameters, testing pipeline.
	
	// Split pipeline.
	if (!SplitCommands(GetLength(), algorithms, parameters, algorithms_training, parameters_training, algorithms_testing, parameters_testing)) {
		Message::Error("Failed to split command sequence.");
		return NULL;
	}
	
	RNG rng(GetSeed());
	
	// Operate on a masked table.
	bool masked = true;
	int roc_class = Undefined::Integer();
	// This method is conceptually const only.
	PermLeaveOneOutCVSerialExecutor *self = const_cast(PermLeaveOneOutCVSerialExecutor *, this);
	
	message.Notify("Initializing sampling scheme...");
	
	// Initialize sampler.
	if (!self->InitializeSamplingScheme(*table, masked, rng))
		return NULL;
	
	// Get desired output type.
	Id outputtype = GetOutputType();
	
	// The structure to output.
	Handle<Structure> output;
	typedef Pair(int, Handle<Classification>) ICPair;
	Vector(ICPair) myclass;
	Vector(float) accuracies;
	Vector(float) rocareas;
	Vector(float) rocstderrs;
	Vector(ICPair) votes;
	accuracies.reserve(GetN());
	rocareas.reserve(1);
	rocstderrs.reserve(1);
	votes.reserve(GetN());
	int i, j;
	//	Handle<Rules> rules;
	Handle<BatchClassification> batchclassification;
	Handle <DecisionTable> parenttable ;
	Handle<DecisionTable> training ;
	Handle<DecisionTable> testing ;
	
	Handle<Rules> rules;
	// Do training/testing loop.
	
	SerialExecutor pipeline_training;
	
	for (i = 0; i < GetN(); i++) {
		
		if (!message.Progress("Executing iteration " + String::Format(i + 1) + "...", i, GetN())) {
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		message.Notify("Sampling tables...");
		
		// Initialize training/testing tables before sampling. Since we don't know if
		// the input structures get physically modified by any of the algorithms in the
		// pipeline, we have to operate on duplicates for each iteration.
		// Handle<DecisionTable> training = dynamic_cast(DecisionTable *, table->Duplicate());
		// Handle<DecisionTable> testing  = dynamic_cast(DecisionTable *, table->Duplicate());
		
		training.Rebind(dynamic_cast(DecisionTable *, table->Duplicate()));
		testing.Rebind(dynamic_cast(DecisionTable *, table->Duplicate()));
		
		
		// Sample training/testing tables.
		if (!SampleTables(i, rng, *training, *testing, masked)) {
			Message::Error("Failed to sample tables.");
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		Vector(String) parameters_training_copy = parameters_training;
		Vector(String) parameters_testing_copy  = parameters_testing;
		
		// Replace all iteration no. "macros" with index of current iteration. Try to be case-tolerant.
		for (j = 0; j < parameters_training_copy.size(); j++) {
			parameters_training_copy[j].Replace("#iteration#", String::Format(i));
			parameters_training_copy[j].Replace("#ITERATION#", String::Format(i));
		}
		for (j = 0; j < parameters_testing_copy.size(); j++) {
			parameters_testing_copy[j].Replace("#iteration#", String::Format(i));
			parameters_testing_copy[j].Replace("#ITERATION#", String::Format(i));
		}
		
		// Set up training pipeline. (Executed by separate object.)
		// SerialExecutor pipeline_training;
		
		// We want the originating decision table so that we get the dictionary as well.
		pipeline_training.SetOutputType(DECISIONTABLE);
		
		if (!message.Progress("Permutation "+ String::Format(permutation) + ", executing iteration " + String::Format(i + 1) + ", training pipeline...", i, GetN())) {
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		// Execute training pipeline.
		
		
		//	Handle <DecisionTable> parenttable = dynamic_cast(DecisionTable *, pipeline_training.ExecuteCommands(*training, algorithms_training, parameters_training_copy, stream, false));
		
		
		parenttable.Rebind(dynamic_cast(DecisionTable *, pipeline_training.ExecuteCommands(*training, algorithms_training, parameters_training_copy, stream)));
		
		if (parenttable == NULL) {
			Message::Error("Failed to execute training pipeline.");
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		// Get the last rule set derived from the output decision table.
		//	delete rules.GetPointer();
		
		rules.Rebind(StaticGetRules(*parenttable));
		//	 message.Warning("sizeof rules..." + String::Format(sizeof(rules)));
		// rules.Rebind(StaticGetRules(*parenttable));
		if (rules == NULL) {
			Message::Error("Unable to locate rule set generated in training pipeline.");
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		// Temporarily set rules so that they can be employed in the testing pipeline.
		self->SetRules(rules);
		
		// Set up a temporary project so that FindParent method will function.
		Handle<Project> project = Creator::Project();
		
		if (!ProjectManager::InsertProject(project.GetPointer())) {
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		if (!project->AppendChild(parenttable.GetPointer())) {
			StaticCleanUp(*self, outputtype);
			StaticCleanUp(*project, *parenttable);
			return NULL;
		}
		
		// Set up training pipeline. (Executed by this object.)
		self->SetOutputType(BATCHCLASSIFICATION);
		
		if (!message.Progress("Permutation "+ String::Format(permutation) + ", executing iteration " + String::Format(i + 1) + ", testing pipeline...", i, GetN())) {
			StaticCleanUp(*self, outputtype);
			return NULL;
		}
		
		// Execute testing pipeline.
		//		Handle<BatchClassification> batchclassification = dynamic_cast(BatchClassification *, SerialExecutor::ExecuteCommands(*testing, algorithms_testing, parameters_testing_copy, stream));
		batchclassification.Rebind(dynamic_cast(BatchClassification *, SerialExecutor::ExecuteCommands(*testing, algorithms_testing, parameters_testing_copy, stream)));
		
		
		if (batchclassification == NULL) {
			Message::Error("Failed to execute testing pipeline.");
			StaticCleanUp(*self, outputtype);
			StaticCleanUp(*project, *parenttable);
			return NULL;
		}
		
		//jac
		
		myclass= batchclassification->GetClassificationDetails();
		if((myclass.size())!=1)
		{
			Message::Error("Failed to execute testing pipeline\nClassification error (#classifications!=0");
			StaticCleanUp(*self, outputtype);
			StaticCleanUp(*project, *parenttable);
			return NULL;
		}
		
		votes.push_back(myclass[0]);
		
		
		int temp_roc_class = batchclassification->GetROCClass();
		// message.Warning("temp_roc:" + String::Format(temp_roc_class));
		if  ((roc_class==(Undefined::Integer()))||temp_roc_class==roc_class) {
			roc_class= temp_roc_class;
		}
		else {
			
			Message::Error("Error: Different roc_class for different iterations.");
			StaticCleanUp(*self, outputtype);
			StaticCleanUp(*project, *parenttable);
			return NULL;
		}
		//	message.Warning("temp_roc:" + String::Format(temp_roc_class) + "\nUndef:" + String::Format(Undefined::Integer()));
		// roc_class=0;
		
		// (obselete) Display matrix contents in log file.
		
		/*
		if (!SaveLogEntry(stream, *batchclassification)) {
		StaticCleanUp(*self, outputtype);
		StaticCleanUp(*project, *parenttable);
		return NULL;
		}
		*/
		// Collect statistics for this loop.
		accuracies.push_back(batchclassification->GetConfusionMatrix().GetDiagonalRatio());
		/*
		if (batchclassification->GetROCArea() != Undefined::Float())
		rocareas.push_back(batchclassification->GetROCArea());
		
		  if (batchclassification->GetROCStandardError() != Undefined::Float())
		  rocstderrs.push_back(batchclassification->GetROCStandardError());
		*/
		// Try to adhere to specified output type.
		if (batchclassification->IsA(outputtype))
			output = batchclassification.GetPointer();
		else if (rules->IsA(outputtype))
			output = rules.GetPointer();
		else if (parenttable->IsA(outputtype))
			output = parenttable.GetPointer();
		else if (structure.IsA(outputtype))
			output = &structure;
		
		// Clean up temporary project stuff.
		StaticCleanUp(*project, *parenttable);
		
		
	}
	
	// Clean up.
	StaticCleanUp(*self, outputtype);
	
	// roc_class=1;
	
	//
	// Put new confusion matrix output here.
	//
	// Display statistics in log file.
	
	
	if (!SaveAUCLogStatistics(stream, accuracies, "Accuracy"))
		return NULL;
	
	
		/*
		// ROCCurve::Create(const Vector(ICPair) &pairs, int decision_class, int resolution, bool progress, Vector(int) *targets, Vector(float) *outputs, bool cindex) {
		//
		//roc_class hämtas fràn batchclassification
		// roc weight..
		
		  
			roc_class = StaticGetDecisionClass(GetROCClass(), *table, masked);
	*/
	
	ROCCurve roc;
	
	
	Vector(int)   roc_targets;
	Vector(float) roc_outputs;
	
	int  roc_resolution = 251;
	bool roc_progress   = false;
	if (!roc.Create(votes, roc_class, roc_resolution, roc_progress, &roc_targets, &roc_outputs,false)) {
		Message::Warning("Generation of ROC curve failed.");
	}
	float myroc=roc.GetArea();
	//		message.Warning("Area: " + String::Format(roc.GetArea()));
	rocareas.push_back(roc.GetArea());
	rocstderrs.push_back(roc.GetStandardError());
	float mean= MathKit::Mean(accuracies);
	//	 message.Warning("Inline Accuracy : " + String::Format(mean));
	self->SetAccuracy(mean);
	self->SetAUC(myroc);
	
	// message.Warning("Inline AUC :" + String::Format(roc.GetArea()));
	
	//message.Warning("Inline deep \nAUC :" + String::Format(GetAUC()) + "\nAccuracy :" + String::Format(GetAccuracy()));
	
	
	
	if (!rocareas.empty() && !SaveAUCLogStatistics(stream, rocareas, "ROC.AUC"))
		return NULL;
	
	if (!rocstderrs.empty() && !SaveAUCLogStatistics(stream, rocstderrs, "ROC.AUC.SE"))
		return NULL;
		/*
		if (output == NULL) {
		if (outputtype == Undefined::Id())
		output = &structure;
		else
		Message::Error("Unable to return a structure of type " + IdHolder::GetClassname(GetOutputType()) + ".");
		}
	*/
	stop=clock();
	
	message.Warning("Differ" + String::Format(stop-start));
	
	return output.Release();
	
}


//-------------------------------------------------------------------
// Method........: ExecuteCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Opens log file and calls ExecuteCommands.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
PermLeaveOneOutCVSerialExecutor::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters) const {
	
	ofstream stream;
	
	if (!IOKit::Open(stream, GetLogFilename(),true)) {
		Message::Error("Failed to open " + GetLogFilename() + " for output.");
		return NULL;
	}
	
	// Save log header.
	if (!SaveLogHeader(stream)) {
		Message::Error("Failed to save log file header.");
		return NULL;
	}
	
	// Execute commands.
	Handle<Structure> output = ExecuteCommands(structure, algorithms, parameters, stream);
	
	IOKit::Close(stream);
	
	return output.Release();
	
}

bool
PermLeaveOneOutCVSerialExecutor::SaveAUCLogStatistics(ofstream &stream, const Vector(float) &statistics, const String &name) const {
	
	String indent(' ', 1 + SystemKit::GetTimestamp().GetLength());
	
	// Compute statistics.
	float mean    = MathKit::Mean(statistics);
	
	// Format.
	String formatted_mean    = (mean    == Undefined::Float())? Undefined::String() : String::Format(mean);
	
	stream << indent << name << "    = " << formatted_mean    << endl;
	
	stream << endl;
	
	return true;
	
} 
