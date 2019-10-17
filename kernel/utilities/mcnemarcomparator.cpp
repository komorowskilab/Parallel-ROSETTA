//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/mcnemarcomparator.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class McNemarComparator.
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

McNemarComparator::McNemarComparator() {
	n_a_ = n_b_ = n_c_ = n_d_ = Undefined::Integer();
}

McNemarComparator::McNemarComparator(const Vector(String) &filenames, const Vector(bool) &swap) {
	McNemarComparator(filenames, swap, 0.5, 0.5);
}

McNemarComparator::McNemarComparator(const Vector(String) &filenames, const Vector(bool) &swap, float threshold1, float threshold2) : BinaryOutcomeComparator(filenames, swap) {

	n_a_ = n_b_ = n_c_ = n_d_ = Undefined::Integer();

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
		Create(targets_[0], outputs_[0], outputs_[1], threshold1, threshold2, false);

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

McNemarComparator::~McNemarComparator() {
}

//-------------------------------------------------------------------
// Helper methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ComputeMatrixEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
McNemarComparator::ComputeMatrixEntries(const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, float threshold1, float threshold2, int &n_a, int &n_b, int &n_c, int &n_d) const {

	int i;

	// Verify dimensions.
	if (targets.size() != outputs1.size() || targets.size() != outputs2.size()) {
		Message::Error("Incompatible vector dimensions.");
		return false;
	}

	n_a = n_b = n_c = n_d = 0;

	for (i = 0; i < targets.size(); i++) {
		bool correct1 = (outputs1[i] >= threshold1) ? (targets[i] == 1) : (targets[i] == 0);
		bool correct2 = (outputs2[i] >= threshold2) ? (targets[i] == 1) : (targets[i] == 0);
		if (correct1 && correct2)
			n_d++;
		else if (!correct1 && correct2)
			n_c++;
		else if (correct1 && !correct2)
			n_b++;
		else
			n_a++;
	}

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
// Revisions.....:
//===================================================================

bool
McNemarComparator::Create(const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, float threshold1, float threshold2, bool copy) {

	n_a_ = n_b_ = n_c_ = n_d_ = Undefined::Integer();

	bool error = false;

	// Compute matrix entries.
	if (!ComputeMatrixEntries(targets, outputs1, outputs2, threshold1, threshold2, n_a_, n_b_, n_c_, n_d_)) {
		n_a_ = n_b_ = n_c_ = n_d_ = Undefined::Integer();
		error = true;
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
McNemarComparator::GetZ() const {

	int n_b = GetNb();
	int n_c = GetNc();

	if (n_b == Undefined::Integer() || n_c == Undefined::Integer()) {
		Message::Error("Unable to compute Z-statistic.");
		return Undefined::Float();
	}

	if (n_b == 0 && n_c == 0) {
		Message::Error("Diagonal contingency matrix, Z-statistic not defined.");
		return Undefined::Float();
	}
#ifndef OBSOLETE
	return (abs(n_b - n_c) - 1) / sqrt(n_b + n_c);
#else
	return (abs(n_b - n_c) - 1) / sqrt((double)(n_b + n_c));
#endif

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
McNemarComparator::GetP() const {

	float z = GetZ();

	if (z == Undefined::Float()) {
		Message::Error("Unable to compute p-value.");
		return Undefined::Float();
	}

	float phi = MathKit::CumulativeNormal(z);

	return (2 * (1 - phi));

}
 
