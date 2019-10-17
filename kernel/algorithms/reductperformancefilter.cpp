//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reductperformancefilter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/batchclassification.h>

#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/iokit.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Static stuff, file scope (ugly hack).
//===================================================================

static ofstream *static_stream = NULL;

//-------------------------------------------------------------------
// Methods for class ReductPerformanceFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ReductPerformanceFilter::ReductPerformanceFilter() {
	SetCriterion(CRITERION_DIAGONAL);
	SetIndex(0);
	SetThreshold(0.5);
	SetFilename(Undefined::String());
}

ReductPerformanceFilter::~ReductPerformanceFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReductPerformanceFilter, REDUCTPERFORMANCEFILTER, ReductFilter)

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
ReductPerformanceFilter::GetParameters() const {

	String parameters;

	// Ratio.
	parameters += Keyword::Ratio();
	parameters += Keyword::Assignment();
	parameters += GetString(GetCriterion());

	parameters += Keyword::Separator();

	// Index.
	parameters += Keyword::Index();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetIndex());

	parameters += Keyword::Separator();

	// Threshold.
	parameters += Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetThreshold());

	parameters += Keyword::Separator();

	// Filename.
	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();

	parameters += Keyword::Separator();

	// RGDecisionTable.
	parameters += Keyword::RuleGenerator() + Keyword::Dot() + Keyword::DecisionTable();
	parameters += Keyword::Assignment();

	if (GetRGDecisionTable() == NULL)
		parameters += "NULL";
	else
		parameters += GetRGDecisionTable()->GetName();

	parameters += Keyword::Separator();

	// BCDecisionTable.
	parameters += Keyword::BatchClassifier() + Keyword::Dot() + Keyword::DecisionTable();
	parameters += Keyword::Assignment();

	if (GetBCDecisionTable() == NULL)
		parameters += "NULL";
	else
		parameters += GetBCDecisionTable()->GetName();

	parameters += Keyword::Separator();

	// RuleGenerator.
	parameters += Keyword::RuleGenerator();
	parameters += Keyword::Assignment();
	
	if (GetRuleGenerator() == NULL) {
		parameters += "NULL";
	}
	else {
		parameters += IdHolder::GetClassname(GetRuleGenerator()->GetId());
		parameters += Keyword::Separator();
		parameters += GetRuleGenerator()->GetParameters();
	}

	parameters += Keyword::Separator();

	// BatchClassifier.
	parameters += Keyword::BatchClassifier();
	parameters += Keyword::Assignment();

	if (GetBatchClassifier() == NULL) {
		parameters += "NULL";
	}
	else {
		parameters += IdHolder::GetClassname(GetBatchClassifier()->GetId());
		parameters += Keyword::Separator();
		parameters += GetBatchClassifier()->GetParameters();
	}

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
ReductPerformanceFilter::SetParameter(const String &keyword, const String &value) {

	// Ratio.
	if (keyword == Keyword::Ratio()) {
		if (value == GetString(CRITERION_ROW))
			return SetCriterion(CRITERION_ROW);
		if (value == GetString(CRITERION_COLUMN))
			return SetCriterion(CRITERION_COLUMN);
		if (value == GetString(CRITERION_DIAGONAL))
			return SetCriterion(CRITERION_DIAGONAL);
		return false;
	}

	// Index.
	if (keyword == Keyword::Index() && value.IsInteger())
		return SetIndex(value.GetInteger());

	// Threshold.
	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetThreshold(value.GetFloat());

	// Filename.
	if (keyword == Keyword::Filename())
		return SetFilename(value);

	// RGDecisionTable (including backwards compatibility).
	if ((keyword == Keyword::RGDecisionTable()) ||
		  (keyword == Keyword::RuleGenerator() + Keyword::Dot() + Keyword::DecisionTable())) {
		String value_lc(value);
		value_lc.ToLowercase();
		if (value_lc == "null")
			return SetRGDecisionTable(Handle<DecisionTable>(NULL));
		Message::Error("RGDecisionTable parameter cannot currently be set this way.");
		return false;
	}

	// BCDecisionTable (including backwards compatibility).
	if ((keyword == Keyword::BCDecisionTable()) ||
		  (keyword == Keyword::BatchClassifier() + Keyword::Dot() + Keyword::DecisionTable())) {
		String value_lc(value);
		value_lc.ToLowercase();
		if (value_lc == "null")
			return SetBCDecisionTable(Handle<DecisionTable>(NULL));
		Message::Error("BCDecisionTable parameter cannot currently be set this way.");
		return false;
	}

	// RuleGenerator.
	if (keyword == Keyword::RuleGenerator()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (algorithm == NULL || !algorithm->IsA(RULEGENERATOR))
			return false;
		return SetRuleGenerator(dynamic_cast(RuleGenerator *, algorithm.GetPointer()));
	}

	// BatchClassifier.
	if (keyword == Keyword::BatchClassifier()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (algorithm == NULL || !algorithm->IsA(BATCHCLASSIFIER))
			return false;
		return SetBatchClassifier(dynamic_cast(BatchClassifier *, algorithm.GetPointer()));
	}

	// Pass on to RuleGenerator?
	if (GetRuleGenerator() != NULL) {
		if (GetRuleGenerator()->SetParameter(keyword, value))
			return true;
	}

	// Pass on to BatchClassifier?
	if (GetBatchClassifier() != NULL) {
		if (GetBatchClassifier()->SetParameter(keyword, value))
			return true;
	}

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
ReductPerformanceFilter::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	if (GetFilename() != Undefined::String())
		filenames.push_back(GetFilename());

	return true;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: If no decision tables are set for use with RG or BC,
//                 tries to trace back to a parent table of the input
//                 and uses this.
// Revisions.....:
//===================================================================

Structure *
ReductPerformanceFilter::Apply(Structure &structure) const {

	// This method is conceptually const only.
	ReductPerformanceFilter *self = const_cast(ReductPerformanceFilter *, this);

	// Reset mutable bookkeeping stuff.
	(self->scores_).erase((self->scores_).begin(), (self->scores_).end());
	(self->scores_).reserve(structure.GetNoStructures());
	(self->only_scores_).erase((self->only_scores_).begin(), (self->only_scores_).end());
	(self->only_scores_).reserve(structure.GetNoStructures());

	// Check that vital parameters are set.
	if (GetRuleGenerator() == NULL) {
		Message::Error("No RuleGenerator specified.");
		return NULL;
	}

	if (GetBatchClassifier() == NULL) {
		Message::Error("No BatchClassifier specified.");
		return NULL;
	}

	if (GetBatchClassifier()->GetClassifier() == NULL) {
		Message::Error("The BatchClassifier has no Classifier set.");
		return NULL;
	}

	bool use_parent_rg = (GetRGDecisionTable() == NULL);
	bool use_parent_bc = (GetBCDecisionTable() == NULL);

	// Use parent tables if possible and necessary.
	if (use_parent_rg)
		self->SetRGDecisionTable(dynamic_cast(DecisionTable *, structure.FindParent(DECISIONTABLE)));
	if (use_parent_bc)
		self->SetBCDecisionTable(dynamic_cast(DecisionTable *, structure.FindParent(DECISIONTABLE)));

	if (GetRGDecisionTable() == NULL) {
		Message::Error("No RGDecisionTable specified and parent not found.");
		return NULL;
	}

	if (GetBCDecisionTable() == NULL) {
		Message::Error("No BCDecisionTable specified and parent not found.");
		return NULL;
	}

	ofstream stream;

	// Initialize log file, if filename is set.
	if (GetFilename() != Undefined::String()) {

		if (!IOKit::Open(stream, GetFilename())) {
			Message::Error("Failed to open " + GetFilename() + " for output.");
			return NULL;
		}

		// Write file header.
		stream << "% Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
		stream << "%" << endl;
		stream << "% " + IdHolder::GetClassname(GetId()) << endl;
		stream << "% {" + GetParameters() + "}" << endl;
		stream << "%" << endl;
		stream << "% Note that the indices below are 0-based." << endl << endl;

	}

	// Ugly hack, to make stream accessible from Remove and CalculateStatistics method.
	static_stream = &stream;

	// Do the filtering.
	Handle<Structure> result = Filter::Apply(structure);

	if (use_parent_rg)
		self->SetRGDecisionTable(Handle<DecisionTable>(NULL));
	if (use_parent_bc)
		self->SetBCDecisionTable(Handle<DecisionTable>(NULL));

	// Calculate and print statistics.
	CalculateStatistics();

	// Clean up ugly hack.
	static_stream = NULL;

	// Clean up in general.
	(self->scores_).erase((self->scores_).begin(), (self->scores_).end());
	(self->only_scores_).erase((self->only_scores_).begin(), (self->only_scores_).end());

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
ReductPerformanceFilter::Remove(const Structures &structures, int i) const {

	if (!structures.IsA(REDUCTS))
		return false;

	Handle<Reducts> reducts = dynamic_cast(Reducts *, const_cast(Structures *, &structures));

	// Duplicate the specified reduct only.
	Handle<Reducts> reduct = dynamic_cast(Reducts *, reducts->DuplicateStructure(i));

	if (reduct == NULL) {
		Message::Error("Failed to duplicate reduct number " + String::Format(i) + " from reduct set.");
		return false;
	}

	// Generate rules from the reduct, using RG table.
	GetRuleGenerator()->SetDecisionTable(GetRGDecisionTable());

	Handle<Rules> rules = dynamic_cast(Rules *, reduct->Apply(*GetRuleGenerator()));

	GetRuleGenerator()->SetDecisionTable(Handle<DecisionTable>(NULL));

	if (rules == NULL) {
		Message::Error("Failed to generate rules from reduct number " + String::Format(i) + ".");
		return false;
	}

	// How many rules are there?
	int no_rules = rules->GetNoRules();

	// Apply BC to BC table, using generated set of rules.
	if (!GetBatchClassifier()->SetRules(rules.GetPointer())) {
		Message::Error("Failed to pass rules generated from reduct number " + String::Format(i) + " to BatchClassifier.\n"
			             "Are you sure the BatchClassifier has been assigned a rule-based Classifier?");
		return false;
	}

	Handle<BatchClassification> batchclassification = dynamic_cast(BatchClassification *, GetBCDecisionTable()->Apply(*GetBatchClassifier()));

	GetBatchClassifier()->SetRules(NULL);

	if (batchclassification == NULL) {
		Message::Error("Failed to generate confusion matrix from reduct number " + String::Format(i) + ".");
		return false;
	}

	float value;
	bool  index_ok = true;

	// Extract specified value from confusion matrix.
	switch (GetCriterion()) {
		case CRITERION_DIAGONAL: value = batchclassification->GetConfusionMatrix().GetDiagonalRatio();
			                       break;
		case CRITERION_ROW:      if (GetIndex() >= batchclassification->GetConfusionMatrix().GetDimension()) {
		                           Message::Error("Specified index larger than dimension of confusion matrix for reduct number " + String::Format(i) + ".");
			                         index_ok = false;
														 }
														 else {
															 value = batchclassification->GetConfusionMatrix().GetRowRatio(GetIndex());
														 }
			                       break;
		case CRITERION_COLUMN:   if (GetIndex() >= batchclassification->GetConfusionMatrix().GetDimension()) {
		                           Message::Error("Specified index larger than dimension of confusion matrix for reduct number " + String::Format(i) + ".");
			                         index_ok = false;
														 }
			                       else {
			                         value = batchclassification->GetConfusionMatrix().GetColumnRatio(GetIndex());
														 }
			                       break;
		default:                 Message::Error("Specified performance criterion not implemented.");
			                       index_ok = false;
	}

	if (!index_ok)
		return false;

	if (value == Undefined::Float()) {
		Message::Warning("Score for reduct number " + String::Format(i) + " is undefined.", false);
		return false;
	}

	// Get ROC area (if any).
	float roc_area = batchclassification->GetROCArea();

	String formatted_r;

	bool masked = true;

	// Format reduct.
	if (!reducts->GetReduct(i)->Format(formatted_r, dynamic_cast(DecisionTable *, reducts->FindParent(DECISIONTABLE)), masked))
		formatted_r = Undefined::String();

	// We need to update the mutable bookkeeping stuff.
	ReductPerformanceFilter *self = const_cast(ReductPerformanceFilter *, this);

	ReductScore score;

	// Store values for summary rankings/statistics.
	score.score_  = value;
	score.auc_    = batchclassification->GetROCArea();
	score.reduct_ = formatted_r;
	score.index_  = i;

	(self->scores_).push_back(score);
	(self->only_scores_).push_back(score.score_);

	// Make removal decision.
	bool remove = (value <= GetThreshold());

	// Dump information to logfile, if open.
	if (static_stream != NULL) {

		String formatted_m;

		String indent(' ', 5);

		// Format matrix.
		if (!batchclassification->GetConfusionMatrix().Format(formatted_m, indent))
			formatted_m = Undefined::String();

		// Write log file entry.
		*static_stream << "Reduct #" << score.index_ << ":" << endl;
		*static_stream << indent << score.reduct_ << endl;
		*static_stream << "#Rules:" << endl;
		*static_stream << indent << no_rules << endl;
		if (roc_area != Undefined::Float()) {
			*static_stream << "Area under ROC curve:" << endl;
			*static_stream << indent << score.auc_ << endl;
		}
		*static_stream << "Performance:" << endl;
		*static_stream << formatted_m << endl;

	}

	// Return removal decision.
	return remove;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: CalculateStatistics
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
ReductPerformanceFilter::CalculateStatistics() const {

	if (only_scores_.empty())
		return;

	float mean    = MathKit::Mean(only_scores_);
	float median  = MathKit::Median(only_scores_);
	float stddev  = MathKit::StandardDeviation(only_scores_);
	float minimum = MathKit::Minimum(only_scores_);
	float maximum = MathKit::Maximum(only_scores_);

	String name;

	// Extract name of score quantity.
	switch (GetCriterion()) {
		case CRITERION_DIAGONAL: name = "Diagonal"; break;
		case CRITERION_ROW:      name = "Row[" + String::Format(GetIndex()) + "]"; break;
		case CRITERION_COLUMN:   name = "Column[" + String::Format(GetIndex()) + "]"; break;
		default:                 name = "Score";
	}

	Message message;

	message.Notify(name + ".Mean    = " + (mean    == Undefined::Float() ? Undefined::String() : String::Format(mean)));
	message.Notify(name + ".Median  = " + (median  == Undefined::Float() ? Undefined::String() : String::Format(median)));
	message.Notify(name + ".StdDev  = " + (stddev  == Undefined::Float() ? Undefined::String() : String::Format(stddev)));
	message.Notify(name + ".Minimum = " + (minimum == Undefined::Float() ? Undefined::String() : String::Format(minimum)));
	message.Notify(name + ".Maximum = " + (maximum == Undefined::Float() ? Undefined::String() : String::Format(maximum)));

	String indent(' ', 5);

	// Write to log file.
	if (static_stream != NULL) {

		// Write summary statistics.
		*static_stream << "Statistics:" << endl;
		*static_stream << indent << name + ".Mean    = " + (mean    == Undefined::Float() ? Undefined::String() : String::Format(mean)) << endl;
		*static_stream << indent << name + ".Median  = " + (median  == Undefined::Float() ? Undefined::String() : String::Format(median)) << endl;
		*static_stream << indent << name + ".StdDev  = " + (stddev  == Undefined::Float() ? Undefined::String() : String::Format(stddev)) << endl;
		*static_stream << indent << name + ".Minimum = " + (minimum == Undefined::Float() ? Undefined::String() : String::Format(minimum)) << endl;
		*static_stream << indent << name + ".Maximum = " + (maximum == Undefined::Float() ? Undefined::String() : String::Format(maximum)) << endl << endl;

		// Any scores present? If not, we're done.
		if (scores_.empty())
			return;

		// We need to update (sort) the mutable bookkeeping stuff.
		ReductPerformanceFilter *self = const_cast(ReductPerformanceFilter *, this);

		ReductScoreComparator1 comparator1;

		message.Notify("Sorting scores...");

		// Sort by performance scores.
		std::sort((self->scores_).begin(), (self->scores_).end(), comparator1);

		message.Notify("Saving ranking to log...");

		*static_stream << "Ranking:" << endl;

		int  i;
		bool sort_by_auc = false;

		// Write rankings.
		for (i = 0; i < scores_.size(); i++) {

			String formatted_i;
			String formatted_v;
			String formatted_r;

			// Format output.
			formatted_i = "Reduct #" + String::Format(scores_[i].index_);

			formatted_v = String::Format(scores_[i].score_);
			formatted_v.Pad(' ', 11);

			formatted_r = scores_[i].reduct_;

			// Write to stream.
			*static_stream << indent << formatted_v << formatted_i << " = " << formatted_r << endl;

			if (scores_[i].auc_ != Undefined::Float())
				sort_by_auc = true;

		}

		// Write ranked AUC values, too, or are we done?
		if (!sort_by_auc)
			return;

		ReductScoreComparator2 comparator2;

		message.Notify("Sorting AUC scores...");

		// Sort by AUC.
		std::sort((self->scores_).begin(), (self->scores_).end(), comparator2);

		message.Notify("Saving ranking to log...");

		*static_stream << endl << "ROC areas:" << endl;

		// Write ROC areas.
		for (i = 0; i < scores_.size(); i++) {

			String formatted_i;
			String formatted_v;
			String formatted_r;

			// Format output.
			formatted_i = "Reduct #" + String::Format(scores_[i].index_);

			formatted_v = String::Format(scores_[i].auc_);
			formatted_v.Pad(' ', 11);

			formatted_r = scores_[i].reduct_;

			// Write to stream.
			*static_stream << indent << formatted_v << formatted_i << " = " << formatted_r << endl;

		}

	}

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
ReductPerformanceFilter::GetString(ReductPerformanceFilter::Criterion criterion) {

	switch (criterion) {
		case CRITERION_ROW:      return "Row";
		case CRITERION_COLUMN:   return "Column";
		case CRITERION_DIAGONAL: return "Diagonal";
		default:                 return Undefined::String();
	}

}
 
ReductPerformanceFilter *
ReductPerformanceFilter::Clone() {
	return new ReductPerformanceFilter;
}
