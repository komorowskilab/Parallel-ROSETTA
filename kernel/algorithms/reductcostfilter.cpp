//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reductcostfilter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class ReductCostFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ReductCostFilter::ReductCostFilter() {
	SetCostFilename(Undefined::String());
	SetThreshold(100.0);
	SetDefaultCost(0.0);
	SetLogFilename(Undefined::String());
}

ReductCostFilter::~ReductCostFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReductCostFilter, REDUCTCOSTFILTER, ReductFilter)

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
ReductCostFilter::GetParameters() const {

	String parameters;

	// Cost, filename.
	parameters += Keyword::Cost() + Keyword::Dot() + Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetCostFilename();

	parameters += Keyword::Separator();

	// Default.
	parameters += Keyword::Default();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetDefaultCost());

	parameters += Keyword::Separator();

	// Threshold.
	parameters += Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetThreshold());

	parameters += Keyword::Separator();

	// Log, filename.
	parameters += Keyword::Log() + Keyword::Dot() + Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetLogFilename();

	parameters += Keyword::Separator();

	return parameters + ReductFilter::GetParameters();

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
ReductCostFilter::SetParameter(const String &keyword, const String &value) {

	// Cost, filename,
	if (keyword == Keyword::Cost() + Keyword::Dot() + Keyword::Filename())
		return SetCostFilename(value);

	// Cost, filename (for backwards compatibility).
	if (keyword == Keyword::Filename())
		return SetCostFilename(value);

	// Threshold.
	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetThreshold(value.GetFloat());

	// Default.
	if (keyword == Keyword::Default() && value.IsFloat())
		return SetDefaultCost(value.GetFloat());

	// Log, filename,
	if (keyword == Keyword::Log() + Keyword::Dot() + Keyword::Filename())
		return SetLogFilename(value);

	return ReductFilter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: GetOutputFilenames
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ReductCostFilter::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	filenames.push_back(GetLogFilename());

	return true;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ReductCostFilter::Apply(Structure &structure) const {

	// This method is conceptually const only.
	ReductCostFilter *self = const_cast(ReductCostFilter *, this);

	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, structure.FindParent(DECISIONTABLE));

	// Load cost information.
	if (!self->costs_.Load(GetCostFilename(), *table, GetDefaultCost())) {
		Message::Error("Failed to load cost information.");
		return NULL;
	}

	// Erase mutable bookkeeping stuff.
	(self->statistics_).erase((self->statistics_).begin(), (self->statistics_).end());
	(self->rankings_).erase((self->rankings_).begin(), (self->rankings_).end());
	(self->reducts_).erase((self->reducts_).begin(), (self->reducts_).end());

	(self->statistics_).reserve(structure.GetNoStructures());
	(self->rankings_).reserve(structure.GetNoStructures());
	(self->reducts_).reserve(structure.GetNoStructures());

	// Do the filtering.
	Handle<Structure> result = Filter::Apply(structure);

	// Save log, calculate and print statistics.
	if (!self->SaveLog(*table))
		Message::Warning("Failed to save log file.");

	// Clean up in general.
	(self->statistics_).erase((self->statistics_).begin(), (self->statistics_).end());
	(self->rankings_).erase((self->rankings_).begin(), (self->rankings_).end());
	(self->reducts_).erase((self->reducts_).begin(), (self->reducts_).end());

	return result.Release();

}

//-------------------------------------------------------------------
// Methods inherited from Filter.
//===================================================================

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the specified reduct should be removed
//                 from the reduct set.
// Comments......:
// Revisions.....:
//===================================================================

bool
ReductCostFilter::Remove(const Structures &structures, int i) const {

	if (!structures.IsA(REDUCTS))
		return false;

	Handle<Reducts> reducts = dynamic_cast(Reducts *, const_cast(Structures *, &structures));
	Handle<Reduct>  reduct  = reducts->GetReduct(i);

	// Compute total cost.
	float cost = costs_.GetCost(*reduct);

	String formatted;
	bool   masked = true;

	// Format reduct.
	if (!reduct->Format(formatted, dynamic_cast(DecisionTable *, reducts->FindParent(DECISIONTABLE)), masked))
		formatted = Undefined::String();

	// Update bookkeeping structures.
	ReductCostFilter *self = const_cast(ReductCostFilter *, this);

	// Update bookkeeping structures.
	(self->statistics_).push_back(cost);
	(self->rankings_).push_back(std::make_pair(i, cost));
	(self->reducts_).push_back(ISPair(i, formatted) /* std::make_pair(i, formatted) */);

	// Return removal decision.
	return (cost > GetThreshold());

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SaveLog
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ReductCostFilter::SaveLog(const DecisionTable &table) const {

	// Compute statistics.
	float mean    = MathKit::Mean(statistics_);
	float median  = MathKit::Median(statistics_);
	float stddev  = MathKit::StandardDeviation(statistics_);
	float minimum = MathKit::Minimum(statistics_);
	float maximum = MathKit::Maximum(statistics_);

	Message message;

	// Output to user.
	message.Notify("Cost.Mean    = " + (mean    == Undefined::Float() ? Undefined::String() : String::Format(mean)));
	message.Notify("Cost.Median  = " + (median  == Undefined::Float() ? Undefined::String() : String::Format(median)));
	message.Notify("Cost.StdDev  = " + (stddev  == Undefined::Float() ? Undefined::String() : String::Format(stddev)));
	message.Notify("Cost.Minimum = " + (minimum == Undefined::Float() ? Undefined::String() : String::Format(minimum)));
	message.Notify("Cost.Maximum = " + (maximum == Undefined::Float() ? Undefined::String() : String::Format(maximum)));

	ofstream stream;

	if (!IOKit::Open(stream, GetLogFilename())) {
		Message::Error("Failed to open " + GetLogFilename() + ".");
		return false;
	}

	bool masked = true;

	// Save log header.
	stream << "% Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " + IdHolder::GetClassname(GetId()) << endl;
	stream << "% {" + GetParameters() + "}" << endl;
	stream << "%" << endl;
	stream << "% Note that the indices below are 0-based." << endl;
	stream << "%" << endl;

	int i, longest = 0;

	for (i = 0; i < table.GetNoAttributes(masked); i++) {
		/*
		if (costs_.GetCost(i) == GetDefaultCost())
			continue;
		*/
		String name = table.GetAttributeName(i, masked);
		if (name.GetLength() > longest)
			longest = name.GetLength();
	}

	for (i = 0; i < table.GetNoAttributes(masked); i++) {
		/*
		if (costs_.GetCost(i) == GetDefaultCost())
			continue;
		*/
		String name = table.GetAttributeName(i, masked);
		name = "Cost(" + name + ")";
		name.Pad(' ', longest + 6);
		stream << "% " << name << " = " << costs_.GetCost(i) << endl;
	}

	stream << endl;

	// Save cost statistics.
	stream << "Cost.Mean    = " << (mean    == Undefined::Float() ? Undefined::String() : String::Format(mean)) << endl;
	stream << "Cost.Median  = " << (median  == Undefined::Float() ? Undefined::String() : String::Format(median)) << endl;
	stream << "Cost.StdDev  = " << (stddev  == Undefined::Float() ? Undefined::String() : String::Format(stddev)) << endl;
	stream << "Cost.Minimum = " << (minimum == Undefined::Float() ? Undefined::String() : String::Format(minimum)) << endl;
	stream << "Cost.Maximum = " << (maximum == Undefined::Float() ? Undefined::String() : String::Format(maximum)) << endl << endl;

	// We need to update (sort) the mutable bookkeeping stuff.
	ReductCostFilter *self = const_cast(ReductCostFilter *, this);

	IFPairCompareSecondDescending comparator1;
	ISPairCompareFirstAscending   comparator2;

	message.Notify("Sorting costs...");

	// Sort rankings and formatted reducts.
	std::sort((self->rankings_).begin(), (self->rankings_).end(), comparator1);
	std::sort((self->reducts_).begin(),  (self->reducts_).end(),  comparator2);

	message.Notify("Saving costs to log...");

	// Save rankings.
	for (i = 0; i < rankings_.size(); i++) {

		int   index = rankings_[i].first;
		float value = rankings_[i].second;

		String formatted_i;
		String formatted_v;
		String formatted_r;

		// Format output.
		formatted_i = "Reduct #" + String::Format(index);

		formatted_v = (value == Undefined::Float()) ? Undefined::String() : String::Format(value);
		formatted_v.Pad(' ', 11);

		formatted_r = reducts_[index].second;

		// Save to stream.
		stream << formatted_v << formatted_i << " = " << formatted_r << endl;

	}

	return true;

}
 
ReductCostFilter *
ReductCostFilter::Clone() {
	return new ReductCostFilter;
}
