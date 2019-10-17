//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/calibrationcurve.h>
#include <kernel/structures/classification.h>

#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/systemkit.h>
#include <kernel/utilities/permuter.h>
#include <kernel/utilities/iokit.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

#include <common/configuration.h>

//-------------------------------------------------------------------
// Methods for class CalibrationCurve.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

CalibrationCurve::CalibrationCurve() {
	decision_attribute_ = Undefined::Integer();
	decision_class_     = Undefined::Integer();
}

CalibrationCurve::CalibrationCurve(const CalibrationCurve &in) {
	targets_            = in.targets_;
	outputs_            = in.outputs_;
	indices_            = in.indices_;
	targets_mean_       = in.targets_mean_;
	outputs_mean_       = in.outputs_mean_;
	indices_summed_     = in.indices_summed_;
	group_sizes_        = in.group_sizes_;
	decision_attribute_ = in.decision_attribute_;
	decision_class_     = in.decision_class_;
}

CalibrationCurve::~CalibrationCurve() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(CalibrationCurve, CALIBRATIONCURVE, BinaryOutcomeCurve)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: BinaryOutcomeCurve::Load method not compatible with
//                 CalibrationCurve::Save method.
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Load(ifstream &stream) {
	return BinaryOutcomeCurve::Load(stream);
}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Save(ofstream &stream) const {
	return Save(stream, Undefined::Id(), Undefined::String());
}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
CalibrationCurve::Duplicate() const {
	return new CalibrationCurve(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
CalibrationCurve::Clear() {

	// Erase data.
	targets_.erase(targets_.begin(), targets_.end());
	outputs_.erase(outputs_.begin(), outputs_.end());
	indices_.erase(indices_.begin(), indices_.end());
	targets_mean_.erase(targets_mean_.begin(), targets_mean_.end());
	outputs_mean_.erase(outputs_mean_.begin(), outputs_mean_.end());
	indices_summed_.erase(indices_summed_.begin(), indices_summed_.end());
	group_sizes_.erase(group_sizes_.begin(), group_sizes_.end());

	// Invalidate member variables.
	decision_attribute_ = Undefined::Integer();
	decision_class_     = Undefined::Integer();

}

//-------------------------------------------------------------------
// Methods inherited from BinaryOutcomeCurve
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Create(const Vector(int) &targets, const Vector(float) &outputs) {
	return Create(targets, outputs, 1);
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Index of object assumed equal to index of
//                 classification.
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Create(const Vector(ICPair) &pairs, int decision_class, int no_groups, bool progress) {

	Message message;

	int no_pairs = pairs.size();

	// Is vector empty?
	if (no_pairs == 0) {
		Message::Error("No classifications to process.");
		return false;
	}

	Vector(int)   targets; targets.reserve(no_pairs);
	Vector(float) outputs; outputs.reserve(no_pairs);
	Vector(int)   indices; indices.reserve(no_pairs);

	int i;

	// Create (actual class, model output) vectors.
	for (i = 0; i < no_pairs; i++) {

		// Notify user of progress?
		if (progress) {
			if (!message.Progress("Generating calibration data...", i, no_pairs))
				return false;
		}

		// Skip invalid predictions.
		if (pairs[i].second == NULL) {
			Message::Warning("Invalid or no prediction value for object " + String::Format(i + 1) + ", skipped pair.", false);
			continue;
		}

		// Binarize.
		targets.push_back((pairs[i].first == decision_class) ? 1 : 0);
		outputs.push_back(pairs[i].second->GetBinaryOutcomeCoefficient(decision_class));
		indices.push_back(i);

	}

	return Create(targets, outputs, indices, decision_class, no_groups, progress);

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Index of object assumed equal to index of pair.
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Create(const Vector(int) &targets, const Vector(float) &outputs, int decision_class, int no_groups, bool progress) {

	Vector(int) indices;
	indices.reserve(targets.size());

	int i;

	// Create index vector.
	for (i = 0; i < targets.size(); i++)
		indices.push_back(i);

	return Create(targets, outputs, indices, decision_class, no_groups, progress);

}


//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates points on a calibration curve
//                 from a list of pairs (a, b), where:
//
//                    a = actual/target class, binarized wrt the
//                        specified decision class.
//                    b = model's output (predicted value) for the
//                        specified decision class.
//
// Comments......: a in {0, 1}, 0 <= b <= 1, b = Pr(a = 1).
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Create(const Vector(int) &targets, const Vector(float) &outputs, const Vector(int) &indices, int decision_class, int no_groups, bool progress) {

	Message message;

	// Verify dimensions.
	if (targets.size() != outputs.size())
		return false;

	if (targets.size() != indices.size())
		return false;

	int no_pairs = targets.size();

	// Is vector empty?
	if (no_pairs == 0) {
		Message::Error("No pairs to process.");
		return false;
	}

	// Valid number of groups?
	if (no_groups <= 0) {
		Message::Error("Invalid number of groups.");
		return false;
	}

	if (no_groups > no_pairs) {
		Message::Warning("Too many groups, using maximum.", false);
		no_groups = no_pairs;
	}

	// Compute average group size.
  float average_size = (no_pairs == no_groups) ? 1.0f : static_cast(float, no_pairs) / no_groups;

	if (average_size < 5.0)
		Message::Warning("Average group size (" + String::Format(average_size) + ") is small.", false);

	int i, j;

	// Verify binarity.
	for (i = 0; i < no_pairs; i++) {
		if (targets[i] != 0 && targets[i] != 1) {
			Message::Error("Element in target vector is not 0 or 1.");
			return false;
		}
	}

	// Initialize data vectors.
	targets_ = targets;
	outputs_ = outputs;
	indices_ = indices;

	targets_mean_.erase(targets_mean_.begin(), targets_mean_.end());
	outputs_mean_.erase(outputs_mean_.begin(), outputs_mean_.end());
	indices_summed_.erase(indices_summed_.begin(), indices_summed_.end());
	group_sizes_.erase(group_sizes_.begin(), group_sizes_.end());

	targets_mean_.reserve(no_groups);
	outputs_mean_.reserve(no_groups);
	indices_summed_.reserve(no_groups);
	group_sizes_.reserve(no_groups);

	Vector(int)     permutation;
	Permuter<float> permuter;

	// Sort vectors by model output.
	permuter.Permute(outputs_, permutation);

	MathKit::Permute(targets_, permutation);
	MathKit::Permute(outputs_, permutation);
	MathKit::Permute(indices_, permutation);

	// Compute group sums.
	for (i = 0; i < no_groups; i++) {

		// Notify user of progress?
		if (progress) {
			if (!message.Progress("Computing calibration group sums...", i, no_groups))
				return false;
		}

		// Determine start and end of group.
		int start = MathKit::Round(i * average_size);
		int size  = MathKit::Round((i + 1) * average_size) - start;

		int   target_sum = 0;
		float output_sum = 0;
		int   index_sum  = 0;

		// Compute group sums.
		for (j = 0; j < size; j++) {
			target_sum += targets_[start + j];
			output_sum += outputs_[start + j];
			index_sum  += indices_[start + j];
		}

		indices_summed_.push_back(index_sum);
		group_sizes_.push_back(size);

		// Avoid numerical quirks.
		if (size == 1) {
			targets_mean_.push_back(static_cast(float, target_sum));
			outputs_mean_.push_back(output_sum);
		}
		else {
			targets_mean_.push_back(static_cast(float, target_sum) / size);
			outputs_mean_.push_back(output_sum / size);
		}

	}

	// Sort vectors by mean targets per group.
	permuter.Permute(targets_mean_,   permutation);

	MathKit::Permute(targets_mean_,   permutation);
	MathKit::Permute(outputs_mean_,   permutation);
	MathKit::Permute(indices_summed_, permutation);
	MathKit::Permute(group_sizes_,    permutation);

	// Set decision class.
	SetDecisionClass(decision_class);

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Save(ofstream &stream, Id id, const String &parameters) const {

	float mean_0s, mean_1s, variance_0s, variance_1s, regression_slope, regression_intercept;

	// Compute stuff.
	float mean_target        = ComputeMeanTarget();
	float mean_output        = ComputeMeanOutput();
	float brier_score        = ComputeBrierScore();
	float covariance_bias    = ComputeBias();
	float covariance_slope   = ComputeSlope(&mean_0s, &mean_1s);
	float covariance_scatter = ComputeScatter(&variance_0s, &variance_1s);

	if (!ComputeLinearRegression(&regression_slope, &regression_intercept)) {
		regression_slope     = Undefined::Float();
		regression_intercept = Undefined::Float();
	}

	// Determine output strings.
	String brier_string                = (brier_score          == Undefined::Float()) ? Undefined::String() : String::Format(brier_score);
	String covariance_bias_string      = (covariance_bias      == Undefined::Float()) ? Undefined::String() : String::Format(covariance_bias);
	String covariance_slope_string     = (covariance_slope     == Undefined::Float()) ? Undefined::String() : String::Format(covariance_slope);
	String covariance_scatter_string   = (covariance_scatter   == Undefined::Float()) ? Undefined::String() : String::Format(covariance_scatter);
	String regression_slope_string     = (regression_slope     == Undefined::Float()) ? Undefined::String() : String::Format(regression_slope);
	String regression_intercept_string = (regression_intercept == Undefined::Float()) ? Undefined::String() : String::Format(regression_intercept);

	// Notify user?
	if (Configuration::Kernel::IsVerbose()) {
		Message message;
		message.Notify("Brier score = " + brier_string);
		message.Notify("Bias = " + covariance_bias_string);
		message.Notify("Slope = " + covariance_slope_string);
		message.Notify("Scatter = " + covariance_scatter_string);
		message.Notify("Regression slope = " + regression_slope_string);
		message.Notify("Regression intercept = " + regression_intercept_string);
	}

	// Save header.
	stream << "% Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " + IdHolder::GetClassname(id) << endl;
	stream << "% {" + parameters + "}" << endl;
	stream << "%" << endl;
	stream << "% Format:" << endl;
	stream << "%    <Mean target/actual outcome per group> <Mean model output per group> <Sum of object indices per group> <Group size>" << endl;
	stream << "%" << endl;
	stream << "% Targets and outputs are binarized with respect to the selected decision class." << endl;
	stream << "%" << endl;
	stream << "% Brier score                      = " << brier_string << endl;
	stream << "% Mean outcome                     = " << String((mean_target == Undefined::Float()) ? Undefined::String() : String::Format(mean_target)) << endl;
	stream << "% Mean prediction                  = " << String((mean_output == Undefined::Float()) ? Undefined::String() : String::Format(mean_output)) << endl;
	stream << "% Mean of model outputs for 0s     = " << String((mean_0s     == Undefined::Float()) ? Undefined::String() : String::Format(mean_0s))     << endl;
	stream << "% Mean of model outputs for 1s     = " << String((mean_1s     == Undefined::Float()) ? Undefined::String() : String::Format(mean_1s))     << endl;
	stream << "% Variance of model outputs for 0s = " << String((variance_0s == Undefined::Float()) ? Undefined::String() : String::Format(variance_0s)) << endl;
	stream << "% Variance of model outputs for 1s = " << String((variance_1s == Undefined::Float()) ? Undefined::String() : String::Format(variance_1s)) << endl;
	stream << "% Bias                             = " << covariance_bias_string << endl;
	stream << "% Slope                            = " << covariance_slope_string << endl;
	stream << "% Scatter                          = " << covariance_scatter_string << endl;
	stream << "%" << endl;
	stream << "% For a reference to bias, slope and scatter, see:" << endl;
	stream << "%" << endl;
  stream << "% H. R. Arkes et al. (1995), \"The Covariance Decomposition of the Probability Score" << endl;
	stream << "% and Its Use in Evaluating Prognostic Estimates\", Medical Decision Making, 15, pp. 120-131." << endl;
	stream << "%" << endl;
	stream << "% Linear regression equation:" << endl;
	stream << "%    <Mean model output per group> = " << regression_slope_string << " * <Mean target/actual outcome per group> + " << regression_intercept_string << endl;
	stream << endl;

	int i;

	// Save data.
	for (i = 0; i < GetNoGroups(); i++) {
		stream << GetMeanTarget(i) << ' ';
		stream << GetMeanOutput(i) << ' ';
		stream << GetIndexSum(i)  << ' ';
		stream << GetGroupSize(i)  << endl;
	}

	return true;
}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CalibrationCurve::Save(const String &filename, Id id, const String &parameters) const {

	ofstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return Save(stream, id, parameters);

}

//-------------------------------------------------------------------
// Method........: ComputeBrierScore
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The Brier score is simply the average squared
//                 difference between target and output.
// Revisions.....:
//===================================================================

float
CalibrationCurve::ComputeBrierScore() const {

	int i, no_pairs = GetNoPairs();

	if (no_pairs == 0)
		return 0.0;

	double sum = 0.0;

	for (i = 0; i < no_pairs; i++) {
		double difference = GetTarget(i) - GetOutput(i);
		sum += difference * difference;
	}

	return static_cast(float, sum / no_pairs);

}

//-------------------------------------------------------------------
// Method........: ComputeMeanTarget/ComputeMeanOutput
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
CalibrationCurve::ComputeMeanTarget() const {
	return MathKit::Mean(GetTargets());
}

float
CalibrationCurve::ComputeMeanOutput() const {
	return MathKit::Mean(GetOutputs());
}

//-------------------------------------------------------------------
// Method........: ComputeBias
// Author........: Aleksander šhrn
// Date..........:
// Description...: Bias is the difference between the average model
//                 prediction and the average outcome.
//
// Comments......: Arkes et al. (1995), "The Covariance Decomposition
//                 of the Probability Score and Its Use in Evaluating
//                 Prognostic Estimates", Medical Decision Making,
//                 15, pp. 120-131.
//
//                 Lower absolute values of bias are better.
// Revisions.....:
//===================================================================

float
CalibrationCurve::ComputeBias() const {

	if (GetNoPairs() == 0)
		return 0.0;

	float mean_output = ComputeMeanOutput();
	float mean_target = ComputeMeanTarget();

	return mean_output - mean_target;

}

//-------------------------------------------------------------------
// Method........: ComputeSlope
// Author........: Aleksander šhrn
// Date..........:
// Description...: Slope is the average model prediction assigned
//                 to those with outcome 1 (the decision class),
//                 minus the average model prediction assigned to those
//                 with outcome 0 (not the decision class).
//
//                 The average model predictions for 0s and 1s are
//                 returned in-place, if specified.
//
// Comments......: See paper by Arkes et al.
//
//                 Higher slope is better.
// Revisions.....:
//===================================================================

float
CalibrationCurve::ComputeSlope(float *mean_0s, float *mean_1s) const {

	int i, no_pairs = GetNoPairs();

	double f0 = 0.0;
	double f1 = 0.0;

	int n0 = 0;
	int n1 = 0;

	for (i = 0; i < no_pairs; i++) {
		float output = GetOutput(i);
		if (GetTarget(i) == 1) {
			f1 += output;
			n1++;
		}
		else {
			f0 += output;
			n0++;
		}
	}

	if (n0 == 0 || n1 == 0) {
		if (mean_0s != NULL)
			*mean_0s = Undefined::Float();
		if (mean_1s != NULL)
			*mean_1s = Undefined::Float();
		return Undefined::Float();
	}

	float m_0s = static_cast(float, f0 / n0);
	float m_1s = static_cast(float, f1 / n1);

	if (mean_0s != NULL)
		*mean_0s = m_0s;
	if (mean_1s != NULL)
		*mean_1s = m_1s;

	return m_1s - m_0s;

}

//-------------------------------------------------------------------
// Method........: ComputeScatter
// Author........: Aleksander šhrn
// Date..........:
// Description...: Scatter is an index of the "noisiness" of the
//                 model predictions. Scatter is a pooled variance
//                 derived from the distribution of model predictions
//                 assigned to those with outcome 1 and the distribution
//                 of model predictions assigned to those with outcome 0.
//                 The variance of each distribution is calculated
//                 separately before they are pooled. This means that
//                 scatter is unrelated to whether or the target event
//                 occurred.
//
//                 The variances of the model predictions for 0s and 1s
//                 are returned in-place, if specified.
//
// Comments......: See paper by Arkes et al.
// Revisions.....:
//===================================================================

float
CalibrationCurve::ComputeScatter(float *variance_0s, float *variance_1s) const {

	int i, no_pairs = GetNoPairs();

	Vector(float) f0;
	Vector(float) f1;

	f0.reserve(no_pairs);
	f1.reserve(no_pairs);

	int n0 = 0;
	int n1 = 0;

	for (i = 0; i < no_pairs; i++) {
		float output = GetOutput(i);
		if (GetTarget(i) == 1) {
			f1.push_back(output);
			n1++;
		}
		else {
			f0.push_back(output);
			n0++;
		}
	}

	float v_0s = MathKit::Variance(f0);
	float v_1s = MathKit::Variance(f1);

	if (variance_0s != NULL)
		*variance_0s = v_0s;
	if (variance_1s != NULL)
		*variance_1s = v_1s;

	if (v_0s == Undefined::Float() || v_1s == Undefined::Float())
		return 0.0;

	if (n0 + n1 == 0)
		return 0.0;

	return ((n0 * v_0s) + (n1 * v_1s)) / (n0 + n1);

}

//-------------------------------------------------------------------
// Method........: ComputeLinearRegression
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given the points on the calibration curve,
//                 computes the slope and intercept of the
//                 corresponding linear regression equation.
// Comments......:
// Revisions.....:
//===================================================================

bool
CalibrationCurve::ComputeLinearRegression(float *slope, float *intercept) const {

	float s, i;

	if (!MathKit::LinearRegression(targets_mean_, outputs_mean_, s, i))
		return false;

	if (slope != NULL)
		*slope = s;
	if (intercept != NULL)
		*intercept = i;

	return true;

}



 
