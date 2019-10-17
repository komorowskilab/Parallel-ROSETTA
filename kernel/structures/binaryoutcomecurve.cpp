//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/binaryoutcomecurve.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/permuter.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class BinaryOutcomeCurve.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

BinaryOutcomeCurve::BinaryOutcomeCurve() {
}

BinaryOutcomeCurve::BinaryOutcomeCurve(const BinaryOutcomeCurve &/*in*/) {
}

BinaryOutcomeCurve::~BinaryOutcomeCurve() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BinaryOutcomeCurve, BINARYOUTCOMECURVE, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BinaryOutcomeCurve::Load(ifstream &stream) {
	return Load(stream, false);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BinaryOutcomeCurve::Load(ifstream &stream, bool swap) {

	Vector(int)   targets;
	Vector(float) outputs;

	bool sort = false;

	// Load the pairs...
	if (!Load(stream, targets, outputs, swap, sort))
		return false;

	// ... and create the curve.
	if (!Create(targets, outputs))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BinaryOutcomeCurve::Load(const String &filename, bool swap) {

	ifstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return Load(stream, swap);

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...: Loads a set of (target, output) pairs, as
//                 output from a ROSETTA calibration curve with
//                 a single object per group.
//
//                 A third number per line may also be supplied, which
//                 indicates a sorting key the pairs are to be sorted
//                 by. The sorting key is typically an object index.
//                 This helps one to "auto-align" the pairs from
//                 different classifiers.
//
//                 Sorting can be turned off by the sort flag, even
//                 if sorting keys are present.
//
//                 The pairs are read as (output, target) if the
//                 swap flag is set.
// Comments......:
// Revisions.....: Aš 981214: Moved from BinaryOutcomeComparator.
//===================================================================

bool
BinaryOutcomeCurve::Load(ifstream &stream, Vector(int) &targets, Vector(float) &outputs, bool swap, bool sort) {

	Vector(int) keys;

	targets.erase(targets.begin(), targets.end());
	outputs.erase(outputs.begin(), outputs.end());

	keys.reserve(1000);
	targets.reserve(1000);
	outputs.reserve(1000);

	String line;

	// Process all lines.
	while (IOKit::LoadLine(stream, line)) {

		String token;
		String token1 = Undefined::String();
		String token2 = Undefined::String();
		String token3 = Undefined::String();

		// Extract the first two or three tokens.
		while (line.GetToken(token)) {
			if (token1 == Undefined::String())
				token1 = token;
			else if (token2 == Undefined::String())
				token2 = token;
			else if (token3 == Undefined::String())
				token3 = token;
		}

		// Swap the order of the first two tokens?
		if (swap) {
			String tmptok = token1;
			token1 = token2;
			token2 = tmptok;
		}

		// Verify token types.
		if (!token1.IsInteger() || !token2.IsFloat()) {
			Message::Error("Unable to parse line (" + line + ").");
			return false;
		}

		targets.push_back(token1.GetInteger());
		outputs.push_back(token2.GetFloat());

		// Verify key type, if present and necessary.
		if (sort && (token3 != Undefined::String()) && (!token3.IsInteger())) {
			Message::Error("Unable to parse sorting key (" + line + ").");
			return false;
		}

		if (sort && (token3 != Undefined::String()))
			keys.push_back(token3.GetInteger());

	}

	if (sort && !keys.empty() && (keys.size() != targets.size()))
		Message::Error("Not all sorting keys supplied, no sorting performed.");

	// Sort pairs according to (presumably) unique keys?
	if (sort && (keys.size() == targets.size())) {
		Vector(int)   permutation;
		Permuter<int> permuter;
		permuter.Permute(keys,    permutation);
		MathKit::Permute(targets, permutation);
		MathKit::Permute(outputs, permutation);
	}

	return true;

}
 
