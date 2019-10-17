//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...: Implements the statistical hypothesis testing
//                 described in:
//
//                 @ARTICLE{hanley:roc83,
//                   author = "James A. Hanley and Barbara J. McNeil",
//                   title = "A Method of Comparing the Areas under Receiver Operating
//                            Characteristic Curves Derived from the Same Cases",
//                   journal = "Radiology",
//                   year = 1983,
//                   volume = 148,
//                   month = sep,
//                   pages = "839--843",
//                 }
//
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/hanleymcneilcomparator.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/structures/roccurve.h>

//-------------------------------------------------------------------
// Static stuff (file scope):
//===================================================================

#if defined(_MSC_VER) // Microsoft Visual C++.
#pragma warning(disable:4305)
#endif

static float static_r_[][12] = {
// 0.700 0.725 0.750 0.775 0.800 0.825 0.850 0.875 0.900 0.925 0.950 0.975
	{0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01, 0.01}, // 0.02
	{0.04, 0.04, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02}, // 0.04
	{0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.04, 0.04, 0.04, 0.03, 0.02}, // 0.06
	{0.07, 0.07, 0.07, 0.07, 0.07, 0.06, 0.06, 0.06, 0.06, 0.05, 0.04, 0.03}, // 0.08
	{0.09, 0.09, 0.09, 0.09, 0.08, 0.08, 0.08, 0.07, 0.07, 0.06, 0.06, 0.04}, // 0.10
	{0.11, 0.11, 0.11, 0.10, 0.10, 0.10, 0.09, 0.09, 0.08, 0.08, 0.07, 0.05}, // 0.12
	{0.13, 0.12, 0.12, 0.12, 0.12, 0.11, 0.11, 0.11, 0.10, 0.09, 0.08, 0.06}, // 0.14
	{0.14, 0.14, 0.14, 0.14, 0.13, 0.13, 0.13, 0.12, 0.11, 0.11, 0.09, 0.07}, // 0.16
	{0.16, 0.16, 0.16, 0.16, 0.15, 0.15, 0.14, 0.14, 0.13, 0.12, 0.11, 0.09}, // 0.18
	{0.18, 0.18, 0.18, 0.17, 0.17, 0.17, 0.16, 0.15, 0.15, 0.14, 0.12, 0.10}, // 0.20
	{0.20, 0.20, 0.19, 0.19, 0.19, 0.18, 0.18, 0.17, 0.16, 0.15, 0.14, 0.11}, // 0.22
	{0.22, 0.22, 0.21, 0.21, 0.21, 0.20, 0.19, 0.19, 0.18, 0.17, 0.15, 0.12}, // 0.24
	{0.24, 0.23, 0.23, 0.23, 0.22, 0.22, 0.21, 0.20, 0.19, 0.18, 0.16, 0.13}, // 0.26
	{0.26, 0.25, 0.25, 0.25, 0.24, 0.24, 0.23, 0.22, 0.21, 0.20, 0.18, 0.15}, // 0.28
	{0.27, 0.27, 0.27, 0.26, 0.26, 0.25, 0.25, 0.24, 0.23, 0.21, 0.19, 0.16}, // 0.30
	{0.29, 0.29, 0.29, 0.28, 0.28, 0.27, 0.26, 0.26, 0.24, 0.23, 0.21, 0.18}, // 0.32
	{0.31, 0.31, 0.31, 0.30, 0.30, 0.29, 0.28, 0.27, 0.26, 0.25, 0.23, 0.19}, // 0.34
	{0.33, 0.33, 0.32, 0.32, 0.31, 0.31, 0.30, 0.29, 0.28, 0.26, 0.24, 0.21}, // 0.36
	{0.35, 0.35, 0.34, 0.34, 0.33, 0.33, 0.32, 0.31, 0.30, 0.28, 0.26, 0.22}, // 0.38
	{0.37, 0.37, 0.36, 0.36, 0.35, 0.35, 0.34, 0.33, 0.32, 0.30, 0.28, 0.24}, // 0.40
	{0.39, 0.39, 0.38, 0.38, 0.37, 0.36, 0.36, 0.35, 0.33, 0.32, 0.29, 0.25}, // 0.42
	{0.41, 0.40, 0.40, 0.40, 0.39, 0.38, 0.38, 0.37, 0.35, 0.34, 0.31, 0.27}, // 0.44
	{0.43, 0.42, 0.42, 0.42, 0.41, 0.40, 0.39, 0.38, 0.37, 0.35, 0.33, 0.29}, // 0.46
	{0.45, 0.44, 0.44, 0.43, 0.43, 0.42, 0.41, 0.40, 0.39, 0.37, 0.35, 0.30}, // 0.48
	{0.47, 0.46, 0.46, 0.45, 0.45, 0.44, 0.43, 0.42, 0.41, 0.39, 0.37, 0.32}, // 0.50
	{0.49, 0.48, 0.48, 0.47, 0.47, 0.46, 0.45, 0.44, 0.43, 0.41, 0.39, 0.34}, // 0.52
	{0.51, 0.50, 0.50, 0.49, 0.49, 0.48, 0.47, 0.46, 0.45, 0.43, 0.41, 0.36}, // 0.54
	{0.53, 0.52, 0.52, 0.51, 0.51, 0.50, 0.49, 0.48, 0.47, 0.45, 0.43, 0.38}, // 0.56
	{0.55, 0.54, 0.54, 0.53, 0.53, 0.52, 0.51, 0.50, 0.49, 0.47, 0.45, 0.40}, // 0.58
	{0.57, 0.56, 0.56, 0.55, 0.55, 0.54, 0.53, 0.52, 0.51, 0.49, 0.47, 0.42}, // 0.60
	{0.59, 0.58, 0.58, 0.57, 0.57, 0.56, 0.55, 0.54, 0.53, 0.51, 0.49, 0.45}, // 0.62
	{0.61, 0.60, 0.60, 0.59, 0.59, 0.58, 0.58, 0.57, 0.55, 0.54, 0.51, 0.47}, // 0.64
	{0.63, 0.62, 0.62, 0.62, 0.61, 0.60, 0.60, 0.59, 0.57, 0.56, 0.53, 0.49}, // 0.66
	{0.65, 0.64, 0.64, 0.64, 0.63, 0.62, 0.62, 0.61, 0.60, 0.58, 0.56, 0.51}, // 0.68
	{0.67, 0.66, 0.66, 0.66, 0.65, 0.65, 0.64, 0.63, 0.62, 0.60, 0.58, 0.54}, // 0.70
	{0.69, 0.69, 0.68, 0.68, 0.67, 0.67, 0.66, 0.65, 0.64, 0.63, 0.60, 0.56}, // 0.72
	{0.71, 0.71, 0.70, 0.70, 0.69, 0.69, 0.68, 0.67, 0.66, 0.65, 0.63, 0.59}, // 0.74
	{0.73, 0.73, 0.72, 0.72, 0.72, 0.71, 0.71, 0.70, 0.69, 0.67, 0.65, 0.61}, // 0.76
	{0.75, 0.75, 0.75, 0.74, 0.74, 0.73, 0.73, 0.72, 0.71, 0.70, 0.68, 0.64}, // 0.78
	{0.77, 0.77, 0.77, 0.76, 0.76, 0.76, 0.75, 0.74, 0.73, 0.72, 0.70, 0.67}, // 0.80
	{0.79, 0.79, 0.79, 0.79, 0.78, 0.78, 0.77, 0.77, 0.76, 0.75, 0.73, 0.70}, // 0.82
	{0.82, 0.81, 0.81, 0.81, 0.81, 0.80, 0.80, 0.79, 0.78, 0.77, 0.76, 0.73}, // 0.84
	{0.84, 0.84, 0.83, 0.83, 0.83, 0.82, 0.82, 0.81, 0.81, 0.80, 0.78, 0.75}, // 0.86
	{0.86, 0.86, 0.86, 0.85, 0.85, 0.85, 0.84, 0.84, 0.83, 0.82, 0.81, 0.79}, // 0.88
	{0.88, 0.88, 0.88, 0.88, 0.87, 0.87, 0.87, 0.86, 0.86, 0.85, 0.84, 0.82}  // 0.90
};

#if defined(_MSC_VER) // Microsoft Visual C++.
#pragma warning(default:4305)
#endif

//-------------------------------------------------------------------
// Methods for class HanleyMcNeilComparator.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructors.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

HanleyMcNeilComparator::HanleyMcNeilComparator() {

	auc1_   = Undefined::Float();
	auc2_   = Undefined::Float();
	se1_    = Undefined::Float();
	se2_    = Undefined::Float();
	corr0s_ = Undefined::Float();
	corr1s_ = Undefined::Float();
	r_      = Undefined::Float();

}

HanleyMcNeilComparator::HanleyMcNeilComparator(const Vector(String) &filenames, const Vector(bool) &swap, Correlation correlation, bool cindex) : BinaryOutcomeComparator(filenames, swap) {

	auc1_   = Undefined::Float();
	auc2_   = Undefined::Float();
	se1_    = Undefined::Float();
	se2_    = Undefined::Float();
	corr0s_ = Undefined::Float();
	corr1s_ = Undefined::Float();
	r_      = Undefined::Float();

	bool error = false;

	// Check that things went OK further up.
	if (targets_.size() != 2 || outputs_.size() != 2) {
		Message::Error("The number of classifiers is not 2.");
		error = true;
	}
	else {
		if (targets_[0] != targets_[1]) {
			Message::Error("The actual outcomes for the two classifiers do not match up.");
			error = true;
		}
	}

	if (!error)
		Create(targets_[0], outputs_[0], outputs_[1], correlation, false, cindex);

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

HanleyMcNeilComparator::~HanleyMcNeilComparator() {
}

//-------------------------------------------------------------------
// Helper methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ComputeROCQuantities
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990404: Added c-index option.
//                 Aš 990607: Target class of 1 is now implied.
//===================================================================

bool
HanleyMcNeilComparator::ComputeROCQuantities(const Vector(int) &targets, const Vector(float) &outputs, float &auc, float &se, bool cindex) const {

	ROCCurve roc;

	if (!roc.Create(targets, outputs, 251, true, cindex))
		return false;

	auc = roc.GetArea();
	se  = roc.GetStandardError();

#if 0
	roc.Structure::Save(String("c:/temp/hmc_roc.txt"));
#endif

	return true;

}

//-------------------------------------------------------------------
// Method........: ComputeCorrelations
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
HanleyMcNeilComparator::ComputeCorrelations(Correlation correlation, const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, float &corr0s, float &corr1s) const {

	if (targets.size() != outputs1.size() || targets.size() != outputs2.size()) {
		Message::Error("Incompatible vector dimensions.");
		return false;
	}

	Vector(float) outputs1_0s; outputs1_0s.reserve(targets.size());
	Vector(float) outputs1_1s; outputs1_1s.reserve(targets.size());
	Vector(float) outputs2_0s; outputs2_0s.reserve(targets.size());
	Vector(float) outputs2_1s; outputs2_1s.reserve(targets.size());

	int i;

	// Build vectors.
	for (i = 0; i < targets.size(); i++) {
		if (targets[i] == 1) {
			outputs1_1s.push_back(outputs1[i]);
			outputs2_1s.push_back(outputs2[i]);
		}
		else {
			outputs1_0s.push_back(outputs1[i]);
			outputs2_0s.push_back(outputs2[i]);
		}
	}

	// Compute correlations.
	switch (correlation) {
		case CORRELATION_PEARSON: corr0s = MathKit::PearsonCorrelation(outputs1_0s, outputs2_0s);
															corr1s = MathKit::PearsonCorrelation(outputs1_1s, outputs2_1s);
															break;
		case CORRELATION_KENDALL: corr0s = MathKit::KendallCorrelation(outputs1_0s, outputs2_0s);
															corr1s = MathKit::KendallCorrelation(outputs1_1s, outputs2_1s);
															break;
		default:                  corr0s = Undefined::Float();
		                          corr1s = Undefined::Float();
	}

	return (corr0s != Undefined::Float() && corr1s != Undefined::Float());

}

//-------------------------------------------------------------------
// Method........: ComputeRValue
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given mean AUC and mean correlation, looks up
//                 the r-coefficient from Hanley-McNeil's table.
// Comments......:
// Revisions.....:
//===================================================================

bool
HanleyMcNeilComparator::ComputeRValue(float auc1, float auc2, float corr0s, float corr1s, float &r) const {

	// Do a sanity check.
	if (auc1 < 0.0 || auc1 > 1.0 || auc2 < 0.0 || auc2 > 1.0 || corr0s < -1.0 || corr0s > 1.0 || corr1s < -1.0 || corr1s > 1.0) {
		Message::Error("Invalid AUC or correlation values for computing r-coefficient.");
		return false;
	}

	float auc  = 0.5 * (auc1 + auc2);
	float corr = 0.5 * (corr0s + corr1s);

	// Do a range check.
	if (auc < 0.700 || auc > 0.975 || corr < 0.02 || corr > 0.90) {
		Message::Error("Mean AUC or mean correlation outside range of Hanley-McNeil lookup table.");
		return false;
	}

	const double auc_delta = 0.025;
	const double auc_start = 0.700;

	// Lookup AUC index.
	double index_auc   = (auc - auc_start) / auc_delta;
	int    index_auc_l = static_cast(int, floor(index_auc));
	int    index_auc_r = static_cast(int, ceil(index_auc));
	double diff_auc_l  = auc - (auc_start + (index_auc_l * auc_delta));
	double diff_auc_r  = (auc_start + (index_auc_r * auc_delta)) - auc;

	const double corr_delta = 0.02;
	const double corr_start = 0.02;

	// Lookup correlation index.
	double index_corr   = (corr - corr_start) / corr_delta;
	int    index_corr_l = static_cast(int, floor(index_corr));
	int    index_corr_r = static_cast(int, ceil(index_corr));
	double diff_corr_l  = corr - (corr_start + (index_corr_l * corr_delta));
	double diff_corr_r  = (corr_start + (index_corr_r * corr_delta)) - corr;

	/*
	// For now, do a closest match. Consider smooth interpolation.
	if ((diff_auc_l < diff_auc_r) && (diff_corr_l < diff_corr_r))
		r = static_r_[index_corr_l][index_auc_l];
	else if ((diff_auc_l >= diff_auc_r) && (diff_corr_l < diff_corr_r))
		r = static_r_[index_corr_l][index_auc_r];
	else if ((diff_auc_l < diff_auc_r) && (diff_corr_l >= diff_corr_r))
		r = static_r_[index_corr_r][index_auc_l];
	else if ((diff_auc_l >= diff_auc_r) && (diff_corr_l >= diff_corr_r))
		r = static_r_[index_corr_r][index_auc_r];
	else
		r = Undefined::Float();

	return true;
	*/

	// Lookup candidates.
	float r_ll = static_r_[index_corr_l][index_auc_l];
	float r_lr = static_r_[index_corr_l][index_auc_r];
	float r_rl = static_r_[index_corr_r][index_auc_l];
	float r_rr = static_r_[index_corr_r][index_auc_r];

	// Normalize index lookup errors.
	double diff_auc_l_n  = diff_auc_l  / (diff_auc_l  + diff_auc_r);
	double diff_auc_r_n  = diff_auc_r  / (diff_auc_l  + diff_auc_r);
	double diff_corr_l_n = diff_corr_l / (diff_corr_l + diff_corr_r);
	double diff_corr_r_n = diff_corr_r / (diff_corr_l + diff_corr_r);

	// Compute interpolation weights.
	double weight_ll = (1.0 - diff_corr_l_n) * (1.0 - diff_auc_l_n);
	double weight_lr = (1.0 - diff_corr_l_n) * (1.0 - diff_auc_r_n);
	double weight_rl = (1.0 - diff_corr_r_n) * (1.0 - diff_auc_l_n);
	double weight_rr = (1.0 - diff_corr_r_n) * (1.0 - diff_auc_r_n);

	r = (weight_ll * r_ll) + (weight_lr * r_lr) + (weight_rl * r_rl) + (weight_rr * r_rr);

	return true;

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990404: Added c-index option.
//===================================================================

bool
HanleyMcNeilComparator::Create(const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, Correlation correlation, bool copy, bool cindex) {

	auc1_   = Undefined::Float();
	auc2_   = Undefined::Float();
	se1_    = Undefined::Float();
	se2_    = Undefined::Float();
	corr0s_ = Undefined::Float();
	corr1s_ = Undefined::Float();
	r_      = Undefined::Float();

	bool error = false;

	// Verify dimensions.
	if (targets.size() != outputs1.size() || targets.size() != outputs2.size()) {
		Message::Error("Incompatible vector dimensions.");
		error = true;
	}

	// Compute ROC quantities.
	if (!error) {
		if (!ComputeROCQuantities(targets, outputs1, auc1_, se1_, cindex)) {
			auc1_ = Undefined::Float();
			se1_  = Undefined::Float();
			error = true;
		}
	}

	if (!error) {
		if (!ComputeROCQuantities(targets, outputs2, auc2_, se2_, cindex)) {
			auc2_ = Undefined::Float();
			se2_  = Undefined::Float();
			error = true;
		}
	}

	// Compute correlations.
	if (!error) {
		if (!ComputeCorrelations(correlation, targets, outputs1, outputs2, corr0s_, corr1s_)) {
			corr0s_ = Undefined::Float();
			corr1s_ = Undefined::Float();
			error   = true;
		}
	}

	// Compute AUC correlation modifier.
	if (!error) {
		if (!ComputeRValue(auc1_, auc2_, corr0s_, corr1s_, r_)) {
			r_    = Undefined::Float();
			error = true;
		}
	}

	// Copy vectors?
	if (copy) {
		targets_.erase(targets_.begin(), targets_.end());
		outputs_.erase(outputs_.begin(), outputs_.end());
		targets_.reserve(2);
		outputs_.reserve(2);
		targets_.push_back(targets);
		targets_.push_back(targets);
		outputs_.push_back(outputs1);
		outputs_.push_back(outputs2);
	}

	return error;

}

//-------------------------------------------------------------------
// Method........: GetZ
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
HanleyMcNeilComparator::GetZ() const {

	float auc1 = GetAUC1();
	float auc2 = GetAUC2();
	float se1  = GetSE1();
	float se2  = GetSE2();
	float r    = GetR();

	if (auc1 == Undefined::Float() || se1 == Undefined::Float() ||
		  auc2 == Undefined::Float() || se2 == Undefined::Float() || r == Undefined::Float()) {
		Message::Error("Unable to compute Z-statistic.");
		return Undefined::Float();
	}

	return (auc1 - auc2) / sqrt((se1 * se1) + (se2 * se2) - (2 * r * se1 * se2));

}

//-------------------------------------------------------------------
// Method........: GetP
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
HanleyMcNeilComparator::GetP() const {

	float z = GetZ();

	if (z == Undefined::Float()) {
		Message::Error("Unable to compute p-value.");
		return Undefined::Float();
	}

	float phi = MathKit::CumulativeNormal((z < 0.0) ? -z : z);

	return (2 * (1 - phi));

}
//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
HanleyMcNeilComparator::GetString(Correlation correlation) {

	switch (correlation) {
		case CORRELATION_PEARSON: return "Pearson";
		case CORRELATION_KENDALL: return "Kendall";
		default:                  return Undefined::String();
	}

}
 
