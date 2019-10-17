//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/holte1rreducer.h>
#include <kernel/algorithms/rulegenerator.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/rules.h>

#include <kernel/utilities/creator.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Methods for class Holte1RReducer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Holte1RReducer::Holte1RReducer() {
}

Holte1RReducer::~Holte1RReducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Holte1RReducer, HOLTE1RREDUCER, Reducer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: For now, this algorithm does not take any parameters.
// Revisions.....:
//===================================================================

String
Holte1RReducer::GetParameters() const {
	return String("");
}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: For now, this algorithm does not take any parameters.
// Revisions.....:
//===================================================================

bool
Holte1RReducer::SetParameter(const String &/*keyword*/, const String &/*value*/) {
	return false;
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: For now, this algorithm does not take any parameters.
//                 Consider adding IDG support, though, for the sake of
//                 the rule generator.
// Revisions.....:
//===================================================================

Structure *
Holte1RReducer::Apply(Structure &structure) const {

	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to correct type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Create en empty reduct set.
	Handle<Reducts> reducts = Creator::Reducts();

	if (reducts == NULL)
		return NULL;

	bool masked = true;

	// Get number of attributes.
	int i, no_attributes = table->GetNoAttributes(masked);

	// Create one "reduct" per condition attribute.
	for (i = 0; i < no_attributes; i++) {

		if (table->IsDecision(i, masked))
			continue;

		// Create an empty reduct.
		Handle<Reduct> reduct = Creator::Reduct();

		// Initialize support.
		reduct->SetSupport(1);

		// Initialize disc. type. For now, set full discernibility so that
		// rules can be generated afterwards.
		reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);

		// Initialize reduct contents.
		reduct->InsertAttribute(i);

		// Add reduct to reduct set.
		reducts->AppendStructure(reduct.GetPointer());

	}

	reducts->SetName("Singletons");

	// Acquire a rule generator.
	Handle<RuleGenerator> rulegenerator = dynamic_cast(RuleGenerator *, ObjectManager::GetIdentifiedAlgorithm(RULEGENERATOR));

	if (rulegenerator == NULL) {
		Message::Warning("No rule generators installed.");
		return reducts.Release();
	}

	// Generate rules.
	rulegenerator->SetDecisionTable(table.GetPointer());
	Handle<Rules> rules = dynamic_cast(Rules *, reducts->Apply(*rulegenerator));
	rulegenerator->SetDecisionTable(NULL);

	if (rules == NULL) {
		Message::Warning("No rules generated.");
		return reducts.Release();
	}
	else {
		rules->SetName("1R");
	}

	// Append rule set as a child to the reduct set.
	if (!reducts->IsParent(rules.GetPointer(), true))
		reducts->AppendChild(rules.GetPointer());

	return reducts.Release();

}

Holte1RReducer *
Holte1RReducer::Clone() {
	return new Holte1RReducer;
}
