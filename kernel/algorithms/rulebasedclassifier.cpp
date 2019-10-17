//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/rulebasedclassifier.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/dictionary.h>

#include <kernel/basic/message.h>

#include <common/objectmanager.h>

#include "omp.h"

//-------------------------------------------------------------------
// Methods for class RuleBasedClassifier.
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

RuleBasedClassifier::RuleBasedClassifier() {

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

RuleBasedClassifier::~RuleBasedClassifier() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleBasedClassifier, RULEBASEDCLASSIFIER, Classifier)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Abstract, can only be called from subclasses.
// Revisions.....:
//===================================================================

String
RuleBasedClassifier::GetParameters() const {

	String parameters;

	parameters += Keyword::Rules();
	parameters += Keyword::Assignment();

	if (GetRules() != NULL)
		parameters += GetRules()->GetName();
	else
		parameters += "NULL";

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Abstract, can only be called from subclasses.
// Revisions.....:
//===================================================================

bool
RuleBasedClassifier::SetParameter(const String &keyword, const String &/*value*/) {

	// Rules cannot be set this way.
	if (keyword == Keyword::Rules())
		return false;

	// Unknown keyword or illegal value.
	return false;

}
//-------------------------------------------------------------------
// Methods inherited from Classifier.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsCompatible
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Does a very crude check of the applicability of the
//                 rules to inf. vectors extracted from the given
//                 decision table.
//
// Revisions.....: Aš 971106: Changed a warning message to a debug
//                            message.
//===================================================================

bool
RuleBasedClassifier::IsCompatible(const DecisionTable &table, bool masked) const {

	// Rules use indices relative to a masked table.
	if (!masked)
		return false;

  // Have the rules been set?
	if (GetRules() == NULL) {
		Message::Error("No rules have been set.");
		return false;
	}

	// Is the rule set empty?
	if (GetRules()->GetNoRules() == 0) {
		Message::Error("The rule set is empty.");
		return false;
	}

  // Get the originating table of rule set.
	Handle<DecisionTable> origin = dynamic_cast(DecisionTable *, GetRules()->FindParent(DECISIONTABLE));

	if (origin == NULL) {
		Message::Debug("Could not trace back to the rule set's originating decision table.\n"
			             "Unable to verify applicability, assuming it is safe to proceed.");
		return true;
	}

	// Do a very quick pre-check.
	if (origin == &table)
		return true;

	int no_attributes_1 = table.GetNoAttributes(masked);
	int no_attributes_2 = origin->GetNoAttributes(masked);

	// Are the dimensions incompatible?
	if (no_attributes_1 != no_attributes_2) {
		Message::Error("Mismatch in number of attributes used by the rules and the decision table.", false);
		return false;
	}

	// May the coding schemes be different?
	if (!table.HasDictionary() && !origin->HasDictionary()) {       // None of the tables have associated dictionaries.
		return true;
	}
	else if ((table.HasDictionary() && !origin->HasDictionary()) ||	// Only one table has an associated dictionary.
		       (!table.HasDictionary() && origin->HasDictionary())) {
		Message::Warning("The coding schemes used by the rules and the decision table may be different!", false);
		return true;
	}
	else {                                                          // Both tables have associated dictionaries.
		Handle<Dictionary> dict1 = table.GetDictionary();
		Handle<Dictionary> dict2 = origin->GetDictionary();
		if (!(dict1->IsCompatible(*dict2))) {
			Message::Warning("Possibly different coding schemes in use by the rules and the decision table.", false);
			return true;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Side effect if the rules are NULL: GetParameters
//                 cannot return the name of the rule set.
// Revisions.....:
//===================================================================

bool
RuleBasedClassifier::SetRules(const Rules *rules) {
	rules_ = rules;
	return (!rules_.IsNULL());
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: -N 20/05/13 Added the OPENMP version.
//		   -N 21/05/13 Removed OPENMP version, as it caused
//			a race condition on double criticals.
//===================================================================

Rules *
RuleBasedClassifier::GetRules() const {
	return const_cast(Rules *, rules_.GetPointer());
		
}
