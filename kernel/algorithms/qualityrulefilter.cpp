//-------------------------------------------------------------------
// Author........: Thomas ×gotnes/Aleksander šhrn
// Date..........: 981111
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/qualityrulefilter.h>
#include <kernel/algorithms/keyword.h>

//-------------------------------------------------------------------
// Methods for class QualityRuleFilter
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

QualityRuleFilter::QualityRuleFilter() {
	SetFilteringStrategy(FILTERING_TORGO);
	SetMichalskiWeight(0.5);
	SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_NONE);
	UseUpper(false);
	UseLower(true);
	SetUpper(1.0);
	SetLower(0.0);
	RemoveUndefinedRules(true);
}

QualityRuleFilter::~QualityRuleFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(QualityRuleFilter, QUALITYRULEFILTER, RuleFilter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Thomas ×gotnes
// Date..........: 981111
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

String
QualityRuleFilter::GetParameters() const {

	String parameters;

	// Filtering.
	parameters += Keyword::Filtering();
	parameters += Keyword::Assignment();
	parameters += GetString(GetFilteringStrategy());

	parameters += Keyword::Separator();

	// Filtering parameters.
	if (GetFilteringStrategy() == FILTERING_MICHALSKI) {
		parameters += Keyword::Filtering() + Keyword::Dot() + Keyword::Bias();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetMichalskiWeight());
		parameters += Keyword::Separator();
	}

	if (GetFilteringStrategy() == FILTERING_COLEMAN) {
		parameters += Keyword::Filtering() + Keyword::Dot() + Keyword::Normalization();
		parameters += Keyword::Assignment();
		parameters += ColemanRuleEvaluator::GetString(GetColemanNormalization());
		parameters += Keyword::Separator();
	}

	// Upper threshold.
	if (UseUpper()) {
		parameters += Keyword::Upper() + Keyword::Dot() + Keyword::Threshold();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpper());
		parameters += Keyword::Separator();
	}

	// Lower threshold.
	if (UseLower()) {
		parameters += Keyword::Lower() + Keyword::Dot() + Keyword::Threshold();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLower());
		parameters += Keyword::Separator();
	}

	// Remove undefined rules?
	parameters += Keyword::Remove() + Keyword::Dot() + Keyword::Undefined();
	parameters += Keyword::Assignment();
	parameters += String::Format(RemoveUndefinedRules());

	parameters += Keyword::Separator();

	// Stuff higher up.
	parameters += RuleFilter::GetParameters();

	parameters += Keyword::Separator();

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Thomas ×gotnes
// Date..........: 981111
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

bool
QualityRuleFilter::SetParameter(const String &keyword, const String &value) {

	// Filtering.
	if (keyword == Keyword::Filtering()) {
		if (value == GetString(FILTERING_MICHALSKI))
			return SetFilteringStrategy(FILTERING_MICHALSKI);
		if (value == GetString(FILTERING_TORGO))
			return SetFilteringStrategy(FILTERING_TORGO);
		if (value == GetString(FILTERING_BRAZDIL))
			return SetFilteringStrategy(FILTERING_BRAZDIL);
		if (value == GetString(FILTERING_PEARSON))
			return SetFilteringStrategy(FILTERING_PEARSON);
		if (value == GetString(FILTERING_G2))
			return SetFilteringStrategy(FILTERING_G2);
		if (value == GetString(FILTERING_J))
			return SetFilteringStrategy(FILTERING_J);
		if (value == GetString(FILTERING_COHEN))
			return SetFilteringStrategy(FILTERING_COHEN);
		if (value == GetString(FILTERING_COLEMAN))
			return SetFilteringStrategy(FILTERING_COLEMAN);
		if (value == GetString(FILTERING_KONONENKO))
			return SetFilteringStrategy(FILTERING_KONONENKO);
		return false;
	}

	// Filtering parameters.
	if (keyword == Keyword::Filtering() + Keyword::Dot() + Keyword::Bias() && value.IsFloat())
		return SetMichalskiWeight(value.GetFloat());

	if (keyword == Keyword::Filtering() + Keyword::Dot() + Keyword::Normalization()) {
		if (value == ColemanRuleEvaluator::GetString(ColemanRuleEvaluator::NORMALIZATION_NONE))
			return SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_NONE);
		if (value == ColemanRuleEvaluator::GetString(ColemanRuleEvaluator::NORMALIZATION_C1))
			return SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_C1);
		if (value == ColemanRuleEvaluator::GetString(ColemanRuleEvaluator::NORMALIZATION_C2))
			return SetColemanNormalization(ColemanRuleEvaluator::NORMALIZATION_C2);
		return false;
	}

	// Upper/lower boundaries
	if (keyword == Keyword::Upper() && value.IsBoolean())
		return UseUpper(value.GetBoolean());

	if (keyword == Keyword::Lower() && value.IsBoolean())
		return UseLower(value.GetBoolean());

	if (keyword == Keyword::Upper() + Keyword::Dot() + Keyword::Threshold() && value.IsFloat())
		return SetUpper(value.GetFloat());

	if (keyword == Keyword::Lower() + Keyword::Dot() + Keyword::Threshold() && value.IsFloat())
		return SetLower(value.GetFloat());

	// Remove undefined rules?
	if (keyword == Keyword::Remove() + Keyword::Dot() + Keyword::Undefined() && value.IsBoolean())
		return RemoveUndefinedRules(value.GetBoolean());

	return RuleFilter::SetParameter(keyword, value);

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
QualityRuleFilter::Apply(Structure &structure) const {

	// This method is conceptually const only.
	Handle<QualityRuleFilter> self = const_cast(QualityRuleFilter *, this);

	// Instantiate evaluator object.
	self->evaluator_ = CreateEvaluator(structure);

	if (evaluator_ == NULL) {
		Message::Error("Unable to instantiate evaluator (" + GetString(GetFilteringStrategy()) + ").");
		return NULL;
	}

	// Filter.
	Structure *result = RuleFilter::Apply(structure);

	// Reset evaluator object.
	self->evaluator_ = NULL;

	return result;

}

//-------------------------------------------------------------------
// Methods inherited from Filter.
//===================================================================

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Thomas ×gotnes/Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Use instantiated evaluator object.
//===================================================================

bool
QualityRuleFilter::Remove(const Structures &structures, int i) const {

	if (evaluator_ == NULL)
		return false;

	if (!structures.IsA(RULES))
		return false;

	Handle<Rules> rules = dynamic_cast(Rules *, const_cast(Structures *, &structures));
	Handle<Rule>  rule  = rules->GetRule(i);

	// Get quality
	float quality = evaluator_->Evaluate(*rule);

	// Remove rules with undefined quality?
	if (quality == Undefined::Float())
		return RemoveUndefinedRules();

	// Less than lower?
	if (UseLower() && quality < GetLower())
		return true;

	// Greater than upper?
	if (UseUpper() && quality > GetUpper())
		return true;

	return false;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Thomas ×gotnes
// Date..........: 981111
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
QualityRuleFilter::GetString(Filtering filtering) {

	switch (filtering) {
		case FILTERING_MICHALSKI: return "Michalski";
		case FILTERING_TORGO:		  return "Torgo";
		case FILTERING_BRAZDIL:		return "Brazdil";
		case FILTERING_PEARSON:		return "Pearson";
		case FILTERING_G2:				return "G2";
		case FILTERING_J:					return "J";
		case FILTERING_COHEN:			return "Cohen";
		case FILTERING_COLEMAN:		return "Coleman";
		case FILTERING_KONONENKO:	return "Kononenko";
		default:                  return Undefined::String();
	}

}

//-------------------------------------------------------------------
// Method........: CreateEvaluator
// Author........: Aleksander šhrn
// Date..........: 981124
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Handle<RuleEvaluator>
QualityRuleFilter::CreateEvaluator(const Structure &structure) const {

	Handle<DecisionTable> table;

	// Get decision table "nearest" input structure.
	if (structure.IsA(DECISIONTABLE))
		table = dynamic_cast(DecisionTable *, const_cast(Structure *, &structure));
	else
		table = dynamic_cast(DecisionTable *, const_cast(Structure *, structure.FindParent(DECISIONTABLE)));

	if (table == NULL) {
		Message::Error("Unable to get nearest decision table.");
		return NULL;
	}

	Handle<RuleEvaluator> evaluator;

	// Instantiate evaluator object.
	switch (GetFilteringStrategy()) {
		case FILTERING_MICHALSKI: evaluator = new MichalskiRuleEvaluator(*table, weight_, 1.0 - weight_); break;
		case FILTERING_TORGO:     evaluator = new TorgoRuleEvaluator(*table);                             break;
		case FILTERING_BRAZDIL:   evaluator = new BrazdilRuleEvaluator(*table);                           break;
		case FILTERING_PEARSON:   evaluator = new PearsonRuleEvaluator(*table);                           break;
		case FILTERING_G2:        evaluator = new G2RuleEvaluator(*table);                                break;
		case FILTERING_J:         evaluator = new JRuleEvaluator(*table);                                 break;
		case FILTERING_COHEN:     evaluator = new CohenRuleEvaluator(*table);                             break;
		case FILTERING_COLEMAN:   evaluator = new ColemanRuleEvaluator(*table, normalization_);           break;
		case FILTERING_KONONENKO: evaluator = new KononenkoRuleEvaluator(*table);                         break;
	}

	return evaluator;

}
 
QualityRuleFilter *
QualityRuleFilter::Clone() {
	return new QualityRuleFilter;
}
