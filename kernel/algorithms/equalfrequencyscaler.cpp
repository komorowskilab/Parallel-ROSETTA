//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/equalfrequencyscaler.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Methods for class EqualFrequencyScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

EqualFrequencyScaler::EqualFrequencyScaler() {
	SetNoIntervals(3);
}

EqualFrequencyScaler::~EqualFrequencyScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(EqualFrequencyScaler, EQUALFREQUENCYSCALER, OrthogonalScaler)

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
EqualFrequencyScaler::GetParameters() const {

	String parameters = OrthogonalScaler::GetParameters();

	parameters += Keyword::Separator();

	parameters += Keyword::Intervals();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoIntervals());

	return parameters;

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
EqualFrequencyScaler::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Intervals() && value.IsInteger())
		return SetNoIntervals(value.GetInteger());

	return OrthogonalScaler::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Methods inherited from OrthogonalScaler.
//===================================================================

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Attempts to create a given number of intervals with
//                 an approximately equal number of objects within each
//                 interval. This may not always be possible, smarter
//                 logic may be added to handle such cases.
// Comments......:
// Revisions.....: Aš 981208: New, simpler implementation.
//                 Aš 990507: Don't flag too few distinct values as
//                            an error.
//                 Aš 990610: Added special case of 1 interval.
//===================================================================

bool
EqualFrequencyScaler::FindCuts(const DecisionTable &table, int attribute_no, bool masked, Cuts &cuts) const {

	if (GetNoIntervals() < 1)
		return false;

	cuts.erase(cuts.begin(), cuts.end());

	// Special case.
	if (GetNoIntervals() == 1) {
		cuts.reserve(2);
		cuts.push_back(-FLT_MAX);
		cuts.push_back(FLT_MAX);
		return true;
	}

	cuts.reserve(GetNoIntervals() - 1);

	int no_objects = table.GetNoObjects(masked);

	Vector(int) values;
	values.reserve(no_objects);

	int i;

	// Collect defined values.
	for (i = 0; i < no_objects; i++) {
		int value = table.GetEntry(i, attribute_no, masked);
		if (value != Undefined::Integer())
			values.push_back(value);
	}

	Map(int, int) histogram;

	// Build histogram.
	if (!MathKit::Distribution(histogram, values))
		return false;

	// Too few distinct values? Don't flag this as an error, simply return an empty cut set.
	if (histogram.size() < GetNoIntervals()) {
		Message::Warning("More intervals than unique values.", false);
		return true;
	}

	// Determine ideal size of each group at the outset.
	int ideal = MathKit::Round(static_cast(float, values.size()) / GetNoIntervals());

	int cardinality = 0;                                      // Cardinality of current interval.
	int interval    = 0;                                      // Index of current interval.
	int objects     = 0;                                      // Number of objects already contained in intervals.

	// These should be const_iterators, but VC++ 6.0 won't let me...
	Map(int, int)::iterator previous;
	Map(int, int)::iterator next;
	Map(int, int)::iterator current;

	// Build intervals.
	for (current = histogram.begin(); !(current == histogram.end()); current++) {

		int value = (*current).first;
		int count = (*current).second;

		objects += count;

		if (cardinality == 0 || cardinality + count <= ideal) { // Definitely include current value in current interval.
			cardinality += count;
		}
		else {                                                  // Wrap up current interval! But inclusion (exclusion) means overflow (underflow)...
			int   diffinclude = abs(cardinality + count - ideal);
			int   diffexclude = abs(ideal - cardinality);
			float cut         = Undefined::Float();
			if (diffexclude < diffinclude) {                      // We're closer to our ideal size if we exclude the value.
				cut = 0.5f * ((*previous).first + value);
				cardinality = count;
			}
			else {                                                // We're closer to our ideal size if we include the value.
				next = current;
				next++;
				if (!(next == histogram.end()))
					cut = 0.5f * (value + (*next).first);
				else
					break;
				cardinality = 0;
			}
			cuts.push_back(cut);
			interval++;
			if (GetNoIntervals() - interval != 0)                 // Dynamically recompute ideal size of remaining intervals.
				ideal = MathKit::Round(static_cast(float, values.size() - objects + cardinality) / (GetNoIntervals() - interval));
		}

		previous = current;

		if (cuts.size() == GetNoIntervals() - 1)
			break;

	}

	return true;

}

EqualFrequencyScaler *
EqualFrequencyScaler::Clone() {
	return new EqualFrequencyScaler;
}
