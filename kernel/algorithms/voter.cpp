//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/voter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/informationvector.h>
#include <kernel/structures/rulebasedclassification.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/message.h>

// 14/05/13 -N
#ifdef _OPENMP
#include <omp.h>
#endif

//-------------------------------------------------------------------
// Methods for class Voter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Voter::Voter() {
	fraction_        = 0.0;
	idg_             = false;
	filename_idg_    = Undefined::String();
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Voter::~Voter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Voter, VOTER, RuleBasedClassifier)

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
Voter::GetParameters() const {

	String parameters;

	// Get parameters higher up.
	parameters += RuleBasedClassifier::GetParameters();

	parameters += Keyword::Separator();

	// Min. matching fraction.
	parameters += Keyword::Fraction();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetFraction());

	parameters += Keyword::Separator();

	// Use IDG?
	parameters += Keyword::IDG();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseIDG());

	// IDG filename.
	if (UseIDG()) {
		parameters += Keyword::Separator();
		parameters += Keyword::IDG() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIDGFilename();
	}

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
Voter::SetParameter(const String &keyword, const String &value) {

	// Fraction.
	if (keyword == Keyword::Fraction() && value.IsFloat())
		return SetFraction(value.GetFloat());

	// Fraction, backwards compatibility..
	if (keyword == Keyword::Tolerance() && value.IsFloat())
		return SetFraction(1 - value.GetFloat());

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());

	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);

	// Perhaps higher up?
	return RuleBasedClassifier::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes an information vector as input, classifies
//                 it according to the preset rules, and creates and
//                 returns a rule-based classification structure.
//
// Comments......: Assumes that library clients use handles.
// Revisions.....:
//===================================================================

Structure *
Voter::Apply(Structure &structure) const {

	// Check input type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<InformationVector> inf = dynamic_cast(InformationVector *, &structure);


		// Are any rules present?
		if (GetRules() == NULL) {
			Message::Warning("Cannot classify, no classification rules set.", false);
			return NULL;
		}
	
		if (GetRules()->GetNoRules() == 0) {
			Message::Warning("Empty rule set.", false);
			return NULL;
		}

/* 
#ifdef _DEBUG
	// Check dimensions.
  Handle<DecisionTable> table = dynamic_cast(DecisionTable *, GetRules()->FindParent(DECISIONTABLE));

	if (table.IsNULL()) {
		Message::Debug("Unable to access the rules' originating decision table.");
		return NULL;
	}

	bool masked = true;

	if (inf->GetNoAttributes() != table->GetNoAttributes(masked)) {
		Message::Debug("Dimensionality mismatch between the information vector\n"
			             "to classify and the rules' originating decision table.");
		return NULL;
	}
#endif
  */

	return Classify(*inf);

}

//-------------------------------------------------------------------
// Methods inherited from RuleBasedClassifier.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Voter::SetRules(const Rules *rules) {
	rules_ = rules;
   return (!rules_.IsNULL());	//sus
//	return true;
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Classify
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes an information vector as input, classifies
//                 it according to the preset rules, and creates and
//                 returns a rule-based classification structure.
// Comments......:
// Revisions.....:
//===================================================================

RuleBasedClassification *
Voter::Classify(const InformationVector &inf) const {

#ifdef _DEBUG

	if (GetRules() == NULL) {
		Message::Warning("Cannot classify, no classification rules set.", false);
		return NULL;
	}

	if (GetRules()->GetNoRules() == 0) {
		Message::Warning("Empty rule set.", false);
		return NULL;
	}
#endif

	Vector(int) indices;

	// Determine the set of rules that fire.
	if (!EliminateRules(indices, inf, *GetRules(), GetFraction()))
		return NULL;

	return Classify(inf, indices);

}

Rules
Voter::GetRuleset() const {
	Rules *rules = new Rules;
	*rules = *GetRules();
	return *rules;
}
//-------------------------------------------------------------------
// Method........: Classify
// Author........: Aleksander šhrn
// Date..........:
// Description...: Called from other Classify method. Explicitly
//                 provides the set of firing rules (or rather, their
//                 indices).
// Comments......:
// Revisions.....: -N 21/05/13 Moved GetRules() call to variable
//			with a Duplicate, since this part has some
//			issues with parallel execution.
//===================================================================

RuleBasedClassification *
Voter::Classify(const InformationVector &/*inf*/, const Vector(int) &indices) const {

#ifdef _DEBUG
	if (GetRules() == NULL) {
		Message::Warning("Cannot classify, no classification rules set.", false);
		return NULL;
	}

	if (GetRules()->GetNoRules() == 0) {
		Message::Warning("Empty rules set.", false);
		return NULL;
	}
#endif

	// Did any rules fire?
	if (indices.empty()) {
		Message::Debug("No rules fired.");
		return NULL;
	}

	VoteMap votemap; // Maps from decision values to total number of votes (for that decision).
	RuleMap rulemap; // Maps from decision values to firing rules.

	// Fill maps.
	//Assign ruleset. *GetRules() generates a whole set of parallel problems. -N	
	//Handle<Rules> ruleset = dynamic_cast(Rules *, GetRules()->Duplicate());	
	//Rules rules = GetRuleset();
	if (!FillMaps(votemap, rulemap, indices, *GetRules()))
		return NULL;

	Handle<RuleBasedClassification> classification = Creator::RuleBasedClassification();

	// Fill classification structure.
	//No need to grab the ruleset again. -N
//	if (ruleset == *GetRules())
	if (!FillClassification(*classification, votemap, rulemap, *GetRules()))
		return NULL;

	classification->Sort();

	//ruleset = NULL;

	return classification.Release();

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: EliminateRules
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the indices of those rules that
//                 fire.
//
// Comments......:
// Revisions.....: Aš 990727: Added discerner initialization. Note
//                            that the rules' parent table and the
//                            table the inf. vector is culled from
//                            must be compatible dictionarywise.
//===================================================================

bool
Voter::EliminateRules(Vector(int) &indices, const InformationVector &inf, const Rules &rules, float fraction) const {

	Discerner discerner;

	// Initialize discerner, if specified. (Quite extravagant to reload the IDG file for each
	// information vector, but...)
	if (UseIDG()) {

		// Try to trace back to the originating decision table. Needed to parse IDG file.
		Handle<DecisionTable> table = dynamic_cast(DecisionTable *, rules.FindParent(DECISIONTABLE));

		bool masked = true;

		if (table == NULL) {
			Message::Warning("Initialization of IDG failed, could not find parent decision table.", false);
		}
		else {
			if (!discerner.LoadIDGs(GetIDGFilename(), *table, masked)) {
				Message::Error("Failed to load IDG. Illegal file or dictionary mismatch?");
				return false;
			}
		}

	}

	// Determine indices of rules that fire.
	return rules.Matches(inf, discerner, fraction, indices);

}

//-------------------------------------------------------------------
// Method........: FillMaps
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given the indices of those rules that fire, returns
//                 (in-place) filled maps for these.
//
//                 The VoteMap maps from decision values to total
//                 number of votes (for that decision).
//
//                 The RuleMap maps from decision values to firing
//                 rules.
//
// Comments......: Should be overloaded according to the voting
//                 scheme.
// Revisions.....:
//===================================================================

bool
Voter::FillMaps(VoteMap &/*votemap*/, RuleMap &/*rulemap*/, const Vector(int) &/*indices*/, const Rules &/*rules*/) const {
	return false;
}

//-------------------------------------------------------------------
// Method........: FillClassification
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Should be overloaded according to the voting
//                 scheme.
// Revisions.....:
//===================================================================

bool
Voter::FillClassification(RuleBasedClassification &/*classification*/, const VoteMap &/*votemap*/, const RuleMap &/*rulemap*/, const Rules &/*rules*/) const {
	return false;
}
 
