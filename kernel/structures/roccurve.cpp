//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/roccurve.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/systemkit.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/cindexcomputer.h>

#include <kernel/system/fstream.h>
#include <kernel/system/math.h>

#include <common/configuration.h>

//-------------------------------------------------------------------
// Methods for class ROCCurve.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ROCCurve::ROCCurve() {
	no_0s_              = 0;
	no_1s_              = 0;
	area_               = Undefined::Float();
	stderr_             = Undefined::Float();
	decision_attribute_ = Undefined::Integer();
	decision_class_     = Undefined::Integer();
}

ROCCurve::ROCCurve(const ROCCurve &in) {
	data_               = in.data_;
	no_0s_              = in.no_0s_;
	no_1s_              = in.no_1s_;
	area_               = in.area_;
	stderr_             = in.stderr_;
	decision_attribute_ = in.decision_attribute_;
	decision_class_     = in.decision_class_;
}

ROCCurve::~ROCCurve() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ROCCurve, ROCCURVE, BinaryOutcomeCurve)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: BinaryOutcomeCurve::Load method not compatible with
//                 ROCCurve::Save method.
// Revisions.....:
//===================================================================

bool
ROCCurve::Load(ifstream &stream) {
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
ROCCurve::Save(ofstream &stream) const {
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
ROCCurve::Duplicate() const {
	return new ROCCurve(*this);
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
ROCCurve::Clear() {

	// Erase data points.
	data_.erase(data_.begin(), data_.end());

	// Invalidate member variables.
	area_               = Undefined::Float();
	stderr_             = Undefined::Float();
	no_0s_              = 0;
	no_1s_              = 0;
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
ROCCurve::Create(const Vector(int) &targets, const Vector(float) &outputs) {
	return Create(targets, outputs, 251);
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 981218: Added optional target/output vectors
//                            as arguments for in-place return. Default NULL.
//                 Aš 990404: Added optional c-index flag as
//                            argument. Default false.
//                 Aš 990607: Binarizes the actual target wrt. the
//                            decision class.
//===================================================================

bool
ROCCurve::Create(const Vector(ICPair) &pairs, int decision_class, int resolution, bool progress, Vector(int) *targets, Vector(float) *outputs, bool cindex) {

	int i, no_pairs = pairs.size();

	// Is vector empty?
	if (no_pairs == 0) {
		Message::Error("No classifications to process.");
		return false;
	}

	Vector(int)   targets_local;
	Vector(float) outputs_local;

	// Use supplied vectors or local ones?
	if (targets == NULL)
		targets = &targets_local;
	if (outputs == NULL)
		outputs = &outputs_local;

	if (!targets->empty())
		targets->erase(targets->begin(), targets->end());
	if (!outputs->empty())
		outputs->erase(outputs->begin(), outputs->end());

	targets->reserve(no_pairs);
	outputs->reserve(no_pairs);

	// Create (actual class (binarized), model output) vectors.
	for (i = 0; i < no_pairs; i++) {
		targets->push_back((pairs[i].first == decision_class) ? 1 : 0);
		outputs->push_back((pairs[i].second == NULL) ? Undefined::Float() : pairs[i].second->GetBinaryOutcomeCoefficient(decision_class));
	}

	// Create ROC curve.
	if (!Create(*targets, *outputs, resolution, progress, cindex))
		return false;

	// Set decision class.
	SetDecisionClass(decision_class);

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates ROC points from a list of pairs (a, b),
//                 where:
//
//                    a = Actual class (binarized wrt. some specified
//                        decision class).
//                    b = Model's predicted certainty for class 1.
//
// Comments......: a in {0, 1}, 0 <= b <= 1
//
//                 Updates the internally needed number of actual
//                 0s and 1s.
//
// Revisions.....: Aš 990404: Added optional c-index flag as
//                            argument. Default false.
//                 Aš 990607: Removed decision class argument,
//                            assumes class 1.
//===================================================================

bool
ROCCurve::Create(const Vector(int) &targets, const Vector(float) &outputs, int resolution, bool progress, bool cindex) {

	Message message;

	// Verify dimensions.
	if (targets.size() != outputs.size())
		return false;

	int i, j, no_pairs = targets.size();

	// Is vector empty?
	if (no_pairs == 0) {
		Message::Error("No pairs to process.");
		return false;
	}

	// Valid resolution?
	if (resolution < 0) {
		Message::Error("Invalid resolution.");
		return false;
	}

	// Initialize data vector.
	data_.erase(data_.begin(), data_.end());
	data_.reserve(resolution + 2);

	bool has_issued_warning1 = false;
	bool has_issued_warning2 = false;

	// Compute a specified number of data points.
	for (i = 0; i < resolution; i++) {

		// Define counters.
		int no_tp = 0; // No. true positives.
		int no_fp = 0; // No. false positives.
		int no_tn = 0; // No. true negatives.
		int no_fn = 0; // No. false negatives.

		// Notify user of progress?
		if (progress) {
			if (!message.Progress("Creating ROC data points...", i, resolution))
				return false;
		}

		// Determine positivity criterion (threshold) for current point.
		float threshold = static_cast(float, i) / (resolution - 1);

		// Process all pairs.
		for (j = 0; j < no_pairs; j++) {

			// Inalid model prediction?
			if (outputs[j] < 0.0 || outputs[j] > 1.0) {
				if (!has_issued_warning1) { // || 1
					cout << "outputs: " << outputs[j] << endl;
					Message::Warning("Ignoring items with invalid model predictions for ROC.", false);
					has_issued_warning1 = true;
				}
				continue;
			}

			// Invalid actual target?
			if (targets[j] != 0 && targets[j] != 1) {
				if (!has_issued_warning2) {
					Message::Warning("Ignoring items with invalid actual targets (not 0 or 1) for ROC.", false);
					has_issued_warning2 = true;
				}
				continue;
			}

			// Determine actual (binarized) and predicted value.
			bool act_flag = (targets[j] == 1);
			bool prd_flag = (outputs[j] >= threshold);

			// Increment counters.
			if (act_flag && prd_flag)
				no_tp++;
			else if (!act_flag && !prd_flag)
				no_tn++;
			else if (!act_flag && prd_flag)
				no_fp++;
			else
				no_fn++;

		}

		// Define counter sums.
		int no_ap = no_tp + no_fn; // No. actual positives.
		int no_pp = no_tp + no_fp; // No. predicted positives.
		int no_an = no_tn + no_fp; // No. actual negatives.
		int no_pn = no_tn + no_fn; // No. predicted negatives.
		int no_tt = no_ap + no_an; // Total tally.

		no_0s_ = no_an;
		no_1s_ = no_ap;

		// Determine sensitivity, specificity, etc.
		float sensitivity = (no_ap == 0) ? Undefined::Float() : static_cast(float, no_tp) / no_ap;
		float specificity = (no_an == 0) ? Undefined::Float() : static_cast(float, no_tn) / no_an;
		float ppvalue     = (no_pp == 0) ? Undefined::Float() : static_cast(float, no_tp) / no_pp;
		float npvalue     = (no_pn == 0) ? Undefined::Float() : static_cast(float, no_tn) / no_pn;
		float accuracy    = (no_tt == 0) ? Undefined::Float() : static_cast(float, no_tp + no_tn) / no_tt;

		// Is sensitivity and specificity defined?
		if (sensitivity == Undefined::Float() || specificity == Undefined::Float())
			continue;

		Vector(float) v;
		v.reserve(6);

		// Determine vector contents.
		v.push_back(1 - specificity);
		v.push_back(sensitivity);
		v.push_back(ppvalue);
		v.push_back(npvalue);
		v.push_back(accuracy);
		v.push_back(threshold);

		// Append to vector.
		data_.push_back(v);

	}

	Vector(float) v00, v11;

	v00.reserve(6);
	v11.reserve(6);

	// Append (0, 0) and (1, 1) points artificially.
	v00.push_back(0.0);                // 1 - specificity
	v00.push_back(0.0);                // sensitivity
	v00.push_back(Undefined::Float()); // ppvalue
	v00.push_back(Undefined::Float()); // npvalue
	v00.push_back(Undefined::Float()); // accuracy
	v00.push_back(Undefined::Float()); // threshold

	v11.push_back(1.0);                // 1 - specificity
	v11.push_back(1.0);                // sensitivity
	v11.push_back(Undefined::Float()); // ppvalue
	v11.push_back(Undefined::Float()); // npvalue
	v11.push_back(Undefined::Float()); // accuracy
	v11.push_back(Undefined::Float()); // threshold

	data_.push_back(v00);
	data_.push_back(v11);

	FloatVectorCompareXY compare;

	// Sort ROC points.
	std::sort(data_.begin(), data_.end(), compare);

	// Set decision class.
	SetDecisionClass(1);

	// Compute statistics.
	if (cindex) {
		CIndexComputer cindex(&targets, &outputs);
		area_ = cindex.GetCIndex();
		if (!cindex.GetStatus()) {
			area_   = Undefined::Float();
			stderr_ = Undefined::Float();
			return false;
		}
    stderr_ = cindex.GetSE();
	}
	else {
		ComputeArea();
    ComputeStandardError();
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ComputeArea
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the trapeziodal estimate of the area under
//                 the ROC curve.
//
// Comments......: Assumes (0, 0) and (1, 1) endpoints are included.
//                 Assumes sorted data.
// Revisions.....:
//===================================================================

bool
ROCCurve::ComputeArea() {

	area_ = Undefined::Float();

	int i, no_points = GetNoPoints();

	// Verify dimensions.
	if (GetNoPoints() < 2)
		return false;

	double area = 0.0;

	// Approximate area under ROC curve with trapezes.
	for (i = 1; i < no_points; i++) {
		double x1 = 1 - GetSpecificity(i - 1);
		double y1 = GetSensitivity(i - 1);
		double x2 = 1 - GetSpecificity(i);
		double y2 = GetSensitivity(i);
		if (x2 != x1)
			area += (x2 - x1) * 0.5 * (y1 + y2);
	}

	area_ = area;

	return true;

}

//-------------------------------------------------------------------
// Method........: ComputeStandardError
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the standard error of the area under the
//                 ROC curve.
//
// Comments......: Assumes area has been computed.
//                 Assumes number of 0s and 1s have been computed.
//
//                 J. Hanley, B. McNeil (1982), "The Meaning and Use
//                 of the Area under a Receiver Operating Characteristic
//                 (ROC) Curve", Radiology, April, Vol. 143, No. 1,
//                 pp. 29-36.
//
// Revisions.....:
//===================================================================

bool
ROCCurve::ComputeStandardError() {

	if (area_ < 0.0 || area_ > 1.0 || no_0s_ == 0 || no_1s_ == 0)
		return false;

	// For clarity, use the same symbols as in the Hanley-McNeil paper.
	int   n_A    = no_1s_;
	int   n_N    = no_0s_;
	float theta  = area_;
	float theta2 = theta * theta;
	float Q1     = theta / (2 - theta);
	float Q2     = 2 * theta2 / (1 + theta);
	float se2    = (theta * (1 - theta) + (n_A - 1) * (Q1 - theta2) + (n_N - 1) * (Q2 - theta2)) / (n_A * n_N);

	stderr_ = sqrt(se2);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNWThreshold
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes and returns the threshold used
//                 to produce the ROC point the closest to (0, 1),
//                 i.e. the NW corner of the ROC curve.
// Comments......:
// Revisions.....:
//===================================================================

float
ROCCurve::GetNWThreshold() const {

	int i, no_points = GetNoPoints();

	// Verify dimensions.
	if (no_points == 0)
		return Undefined::Float();

	int   index   = 0;
	float large   = 999999;
	float closest = large;

	// Evaluate all ROC points.
	for (i = 0; i < no_points; i++) {
		float specificity = GetSpecificity(i);
		float sensitivity = GetSensitivity(i);
		if (specificity != Undefined::Float() && sensitivity != Undefined::Float()) {
			float x  = 1 - specificity;
			float y  = sensitivity;
			//Torgeir 26.02.01 ------------------------
			float d2;
			if (weight_ == 0 || weight_ == Undefined::Float()) {
				d2 = (x * x) + ((y - 1.0) * (y - 1.0));
			} else {
				d2 = (1-sensitivity) + weight_*(1-specificity);
			}
			//Torgeir 26.02.01 ------------------------
			if (d2 < closest) {
				closest = d2;
				index = i;
			}
		}
	}

	// Return "best" threshold.
	if (closest == large)
		return 0.0;
	else
		return GetThreshold(index);

}

//-------------------------------------------------------------------
// Method........: GetAccuracyThreshold
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes and returns the threshold used
//                 to produce the highest accuracy.
// Comments......:
// Revisions.....:
//===================================================================

float
ROCCurve::GetAccuracyThreshold() const {

	int i, no_points = GetNoPoints();

	// Verify dimensions.
	if (no_points == 0)
		return Undefined::Float();

	int   index   = 0;
	float highest = -1;

	// Evaluate all ROC points.
	for (i = 0; i < no_points; i++) {
		if (GetAccuracy(i) != Undefined::Float() && GetAccuracy(i) > highest) {
			highest = GetAccuracy(i);
			index = i;
		}
	}

	// Return "best" threshold.
	if (highest == -1)
		return 0.0;
	else
		return GetThreshold(index);

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
ROCCurve::Save(ofstream &stream, Id id, const String &parameters) const {

	float area    = GetArea();
	float se      = GetStandardError();
	float thr_nw  = GetNWThreshold();
	float thr_acc = GetAccuracyThreshold();

	// Determine output strings.
	String area_string    = (area    == Undefined::Float()) ? Undefined::String() : String::Format(area);
	String se_string      = (se      == Undefined::Float()) ? Undefined::String() : String::Format(se);
	String thr_nw_string  = (thr_nw  == Undefined::Float()) ? Undefined::String() : String::Format(thr_nw);
	String thr_acc_string = (thr_acc == Undefined::Float()) ? Undefined::String() : String::Format(thr_acc);

	// Notify user?
  if (Configuration::Kernel::IsVerbose()) {
		Message message;
    message.Notify("ROC area = " + area_string);
    message.Notify("ROC std. error = " + se_string);
    message.Notify("ROC (0, 1) threshold = " + thr_nw_string);
    message.Notify("ROC accuracy threshold = " + thr_acc_string);
  }

	// Save header.
	stream << "% Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " << IdHolder::GetClassname(id) << endl;
	stream << "% {" << parameters << "}" << endl;
	stream << "%" << endl;
	stream << "%          Predicted" << endl;
	stream << "%          -  +     " << endl;
	stream << "% Actual - TN FP    " << endl;
	stream << "%        + FN TP    " << endl;
	stream << "%" << endl;
	stream << "% Sensitivity = TP / (TP + FN)" << endl;
	stream << "% Specificity = TN / (TN + FP)" << endl;
	stream << "% PPV         = TP / (TP + FP)" << endl;
	stream << "% NPV         = TN / (TN + FN)" << endl;
	stream << "% Accuracy    = (TP + TN) / (TP + FP + TN + FN)" << endl;
	stream << "%" << endl;
	stream << "% Format:" << endl;
	stream << "%    <1 - Specificity> <Sensitivity> <Specificity> <PPV> <NPV> <Accuracy> <Threshold>" << endl;
	stream << "%" << endl;
	stream << "% To plot ROC curve:" << endl;
	stream <<	"%    X-axis = 1st column = 1 - Specificity" << endl;
	stream << "%    Y-axis = 2nd column = Sensitivity" << endl;
	stream << "%" << endl;
	stream << "% Area under ROC curve (trapezoidal approximation)   = " << area_string << endl;
	stream << "% Standard error for area under ROC curve            = " << se_string << endl;
	stream << "% Threshold used for ROC point closest to (0, 1)     = " << thr_nw_string << endl;
	stream << "% Threshold used for ROC point with highest accuracy = " << thr_acc_string << endl;
	stream << "%" << endl;
	stream << "% For a reference to ROC curves, see:" << endl;
	stream << "%" << endl;
	stream << "% J. Hanley, B. McNeil (1982), \"The Meaning and Use of the Area under a Receiver" << endl;
	stream << "% Operating Characteristic (ROC) Curve\", Radiology, April, Vol. 143, No. 1, pp. 29-36." << endl;
	stream << endl;

	int i;

	// Save ROC data.
	for (i = 0; i < GetNoPoints(); i++) {
		(GetSpecificity(i) == Undefined::Float()) ? stream << "NaN "  : stream << 1 - GetSpecificity(i) << ' ';
		(GetSensitivity(i) == Undefined::Float()) ? stream << "NaN "  : stream << GetSensitivity(i)     << " ";
		(GetSpecificity(i) == Undefined::Float()) ? stream << "NaN "  : stream << GetSpecificity(i)     << " ";
		(GetPPV(i)         == Undefined::Float()) ? stream << "NaN "  : stream << GetPPV(i)             << " ";
		(GetNPV(i)         == Undefined::Float()) ? stream << "NaN "  : stream << GetNPV(i)             << " ";
		(GetAccuracy(i)    == Undefined::Float()) ? stream << "NaN "  : stream << GetAccuracy(i)        << " ";
		(GetThreshold(i)   == Undefined::Float()) ? stream << "NaN\n" : stream << GetThreshold(i)       << endl;
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
ROCCurve::Save(const String &filename, Id id, const String &parameters) const {

	ofstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return Save(stream, id, parameters);

}


 
