//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/cvserialexecutor.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class CVSerialExecutor.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

CVSerialExecutor::CVSerialExecutor() {
	InvertSampling(false);
}

CVSerialExecutor::~CVSerialExecutor() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(CVSerialExecutor, CVSERIALEXECUTOR, SerialExecutorLoop)

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
CVSerialExecutor::GetParameters() const {

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
CVSerialExecutor::SetParameter(const String &keyword, const String &value) {

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
CVSerialExecutor::InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng) {

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
CVSerialExecutor::SampleTables(int i, const RNG &/*rng*/, DecisionTable &training, DecisionTable &testing, bool masked) const {

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

 
CVSerialExecutor *
CVSerialExecutor::Clone() {
	return new CVSerialExecutor;
}
