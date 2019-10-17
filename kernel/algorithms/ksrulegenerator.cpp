//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/ksrulegenerator.h>

#include <kernel/structures/reducts.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class KSRuleGenerator.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSRuleGenerator::KSRuleGenerator() {
}

//-------------------------------------------------------------------
// Method........:
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSRuleGenerator::~KSRuleGenerator() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(KSRuleGenerator, KSRULEGENERATOR, RuleGenerator)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990412: Added discerner argument. Implement
//                            later on.
//                 Aš 990727: Implemented discerner argument.
//===================================================================

Structure *
KSRuleGenerator::Apply(Structure &structure) const {

  // Verify argument structure.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<Reducts> reducts = dynamic_cast(Reducts *, &structure);

	int i;

	// Does the reduct set already have rules?  If so, ignore the calculation request and
	// return the already existing rule set. Note that this effectively means that this
	// algorithm will only let one rule set be derived from a reduct set. That can be overcome,
	// however, by calling Rules::Create directly.
	for (i = 0; i < reducts->GetNoChildren(); i++) {
		Handle<Structure> child = reducts->GetChild(i);
		if (!child->IsA(RULES))
			continue;
		return dynamic_cast(Rules *, child.GetPointer());
	}

	// Is the reduct set empty?
	if (reducts->GetNoReducts() == 0) {
		Message::Error("The reduct set is empty.");
		return NULL;
	}

	// Try to find originating decision table.
	Handle<DecisionTable> decisiontable = dynamic_cast(DecisionTable *, structure.FindParent(DECISIONTABLE));

  // Use the supplied backup table?
	if (decisiontable == NULL)
		decisiontable = GetDecisionTable();

	if (decisiontable.IsNULL()) {
		Message::Error("Unable to find decision table to use.");
		return NULL;
	}

	bool masked = true;
	bool append = false;

	Discerner discerner;

	// Initialize discerner.
	if (UseIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *decisiontable, masked)) {
			Message::Error("Failed to load IDGs.");
			return NULL;
		}
	}

	Handle<Rules> rules = Creator::Rules();

	// Create the rules.
	if (!rules->Create(*reducts, *decisiontable, discerner, append))
		return NULL;

	if (reducts->GetReduct(0)->IsModuloDecision())
		rules->SetName("Rules");
	else
		rules->SetName("Patterns");

	return rules.Release();

}

KSRuleGenerator *
KSRuleGenerator::Clone() {
	return new KSRuleGenerator;
}
