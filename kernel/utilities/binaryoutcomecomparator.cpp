//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/binaryoutcomecomparator.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/permuter.h>

#include <kernel/structures/binaryoutcomecurve.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class BinaryOutcomeComparator.
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

BinaryOutcomeComparator::BinaryOutcomeComparator() {
}

BinaryOutcomeComparator::BinaryOutcomeComparator(const Vector(String) &filenames, const Vector(bool) &swap) {

	targets_.erase(targets_.begin(), targets_.end());
	outputs_.erase(outputs_.begin(), outputs_.end());

	targets_.reserve(filenames.size());
	outputs_.reserve(filenames.size());

	// Verify dimensions.
	if (filenames.size() != swap.size()) {
		Message::Error("Dimension mismatch in arguments passed to BinaryOutcomeComparator constructor.");
		return;
	}

	int i, last = -1;

	// Load all (target, output) pairs.
	for (i = 0; i < filenames.size(); i++) {
		targets_.push_back(Vector(int)());
		outputs_.push_back(Vector(float)());
		last++;
		if (!LoadPairs(targets_[last], outputs_[last], filenames[i], swap[i])) {
			Message::Error("Failed to load data from " + filenames[i] + ".");
			targets_.erase(targets_.begin() + last);
			outputs_.erase(outputs_.begin() + last);
			last--;
		}
	}

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BinaryOutcomeComparator::~BinaryOutcomeComparator() {
}

//-------------------------------------------------------------------
// Helper methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: LoadPairs
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 981214: Code shared with BinaryOutcomeCurve.
//===================================================================

bool
BinaryOutcomeComparator::LoadPairs(Vector(int) &targets, Vector(float) &outputs, const String &filename, bool swap) const {

	if (!IOKit::Exists(filename)) {
		Message::Error(filename + " does not exist.");
		return false;
	}

	ifstream stream(filename.GetBuffer());

	return BinaryOutcomeCurve::Load(stream, targets, outputs, swap, true);

}


 
