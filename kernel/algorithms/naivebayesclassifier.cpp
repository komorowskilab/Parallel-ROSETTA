//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/naivebayesclassifier.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/informationvector.h>
#include <kernel/structures/classification.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/rules.h>

#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class NaiveBayesClassifier.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

NaiveBayesClassifier::NaiveBayesClassifier() {
	decision_attribute_ = Undefined::Integer();
}

NaiveBayesClassifier::~NaiveBayesClassifier() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(NaiveBayesClassifier, NAIVEBAYESCLASSIFIER, Classifier)

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
NaiveBayesClassifier::GetParameters() const {

	String parameters;

	parameters += Keyword::DecisionTable();
	parameters += Keyword::Assignment();

	if (!GetDecisionTable().IsNULL())
		parameters += GetDecisionTable()->GetName();
	else
		parameters += "NULL";

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
NaiveBayesClassifier::SetParameter(const String &keyword, const String &/*value*/) {

	// The master table cannot currently be set this way.
	if (keyword == Keyword::DecisionTable())
		return false;

	// Unknown keyword or illegal value.
	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes that library clients use handles.
// Revisions.....:
//===================================================================

Structure *
NaiveBayesClassifier::Apply(Structure &structure) const {

	// Check input type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<InformationVector> inf = dynamic_cast(InformationVector *, &structure);

	int i, no_attributes = inf->GetNoAttributes();

	// Briefly verify compatibility.
	if (no_attributes != conditionals_.size())
		return NULL;

	if (decision_attribute_ < 0 || decision_attribute_ >= no_attributes)
		return NULL;

	// Create result structure.
	Handle<Classification> classification = Creator::Classification();

	PPMap::const_iterator it1;
	CPMap::const_iterator it2;

	// Consider each possible decision value in turn.
	for (it1 = priors_.begin(); it1 != priors_.end(); it1++) {

		int    decision    = (*it1).first;
		double probability = (*it1).second;

		// Compute product of conditional probabilities.
		for (i = 0; i < no_attributes; i++) {

			// Only consider condition attributes.
			if (i == decision_attribute_)
				continue;

			int condition = inf->GetEntry(i);

			// Skip missing values.
			if (condition == Undefined::Integer())
				continue;

			IIPair iipair(condition, decision);

			// Lookup conditional probability.
			it2 = conditionals_[i].find(iipair);

			if (it2 == conditionals_[i].end()) {
				probability = 0.0;
				break;
			}

			probability *= (*it2).second;

		}

		if (probability != 0.0)
			classification->AppendDecisionValue(decision, probability);

	}

	if (classification->GetNoDecisionValues() > 0) {
		classification->SetDecisionAttribute(decision_attribute_);
		classification->Normalize();
		classification->Sort();
		return classification.Release();
	}

	return NULL;

}

//-------------------------------------------------------------------
// Methods inherited from Classifier.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsCompatible
// Author........: Aleksander šhrn
// Date..........:
// Description...: Is this classifier compatible with information vectors
//                 extracted from the given decision table?
// Comments......:
// Revisions.....:
//===================================================================

bool
NaiveBayesClassifier::IsCompatible(const DecisionTable &table, bool masked) const {

	// Check compatibility between tables.
	if (GetDecisionTable() == NULL) {
		Message::Error("No master decision table set.", false);
		return false;
	}

	if (GetDecisionTable().GetPointer() == &table)
		return true;

	if (GetDecisionTable()->GetNoAttributes(masked) != table.GetNoAttributes(masked)) {
		Message::Error("Mismatch in decision table dimensions.", false);
		return false;
	}

	// May the coding schemes be different?
	if (!table.HasDictionary() && !GetDecisionTable()->HasDictionary()) {       // None of the tables have associated dictionaries.
		return true;
	}
	else if ((table.HasDictionary() && !GetDecisionTable()->HasDictionary()) ||	// Only one table has an associated dictionary.
		       (!table.HasDictionary() && GetDecisionTable()->HasDictionary())) {
		Message::Warning("The coding schemes used by the decision tables may be different!", false);
		return true;
	}
	else {                                                                      // Both tables have associated dictionaries.
		Handle<Dictionary> dict1 = table.GetDictionary();
		Handle<Dictionary> dict2 = GetDecisionTable()->GetDictionary();
		if (!(dict1->IsCompatible(*dict2))) {
			Message::Warning("Possibly different coding schemes in use by decision tables.", false);
			return true;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Initialize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes probabilities.
//
// Comments......: The table given as an argument is the table of
//                 objects to be classified, not the table to
//                 compute probabilities from.
// Revisions.....:
//===================================================================

bool
NaiveBayesClassifier::Initialize(const DecisionTable &/*table*/, bool masked) {

	int i, j;

	// Clear probability data structures.
	conditionals_.erase(conditionals_.begin(), conditionals_.end());
	priors_.erase(priors_.begin(), priors_.end());

	Handle<DecisionTable> master = GetDecisionTable();

	// Is a table to compute probabilities from provided?
	if (master == NULL) {
		Message::Error("No master decision table set.", false);
		return false;
	}

	// Get index of decision attribute.
	decision_attribute_ = master->GetDecisionAttribute(masked);

	if (decision_attribute_ == Undefined::Integer())
		return false;

	int no_objects    = master->GetNoObjects(masked);
	int no_attributes = master->GetNoAttributes(masked);

	// Compute prior counts.
	for (i = 0; i < no_objects; i++) {
		int decision = master->GetEntry(i, decision_attribute_, masked);
		priors_.insert(Pair(const int, double)(decision, 0.0));
		priors_[decision] += 1.0;
	}

	conditionals_.reserve(no_attributes);

	// Compute conditional counts and normalize them.
	for (j = 0; j < no_attributes; j++) {

		conditionals_.push_back(CPMap());

		// Only consider condition attributes.
		if (j == decision_attribute_)
			continue;

		for (i = 0; i < no_objects; i++) {
			int condition = master->GetEntry(i, j, masked);
			int decision  = master->GetEntry(i, decision_attribute_, masked);
			IIPair iipair(condition, decision);
			conditionals_[j].insert(Pair(const IIPair, double)(iipair, 0.0));
			conditionals_[j][iipair] += 1.0;
		}

		CPMap::iterator it1;

		for (it1 = conditionals_[j].begin(); it1 != conditionals_[j].end(); it1++) {
			(*it1).second /= priors_[(*it1).first.second];
		}

	}

	PPMap::iterator it2;

	// Normalize prior counts.
	for (it2 = priors_.begin(); it2 != priors_.end(); it2++) {
		(*it2).second /= no_objects;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overloading this method does not make much sense
//                 since this is not a rule-based classifier, but
//                 this hack should enable this classifier to be used in
//                 the specialized command script language.
// Revisions.....:
//===================================================================

bool
NaiveBayesClassifier::SetRules(const Rules *rules) {

	// This ensures that this object does not keep the table unnecessarily alive.
	// Side effect: GetParameters cannot return the table name.
	if (rules == NULL)
		return SetDecisionTable(NULL);

	// Backtrack to the decision table the rules were derived from.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE));

	return SetDecisionTable(table);

}

NaiveBayesClassifier *
NaiveBayesClassifier::Clone() {
	return new NaiveBayesClassifier;
}
