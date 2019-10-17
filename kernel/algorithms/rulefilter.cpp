//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/rulefilter.h>

#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Methods for class RuleFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RuleFilter::RuleFilter() {
}

RuleFilter::~RuleFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleFilter, RULEFILTER, Filter)

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
RuleFilter::GetParameters() const {
	return Filter::GetParameters();
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
RuleFilter::SetParameter(const String &keyword, const String &value) {
	return Filter::SetParameter(keyword, value);
}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleFilter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(RULES);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Fills mutable vectors before calling Apply higher
//                 up.
// Comments......:
// Revisions.....:
//===================================================================

Structure *
RuleFilter::Apply(Structure &structure) const {

	// We're gonna alter mutable members.
	RuleFilter *self = const_cast(RuleFilter *, this);

	Handle<DecisionTable> table;

	// Get nearest decision table.
	if (structure.IsA(DECISIONTABLE))
		table = dynamic_cast(DecisionTable *, &structure);
	else
		table = dynamic_cast(DecisionTable *, structure.FindParent(DECISIONTABLE));

	self->decisions_.erase(self->decisions_.begin(), self->decisions_.end());
	self->cardinalities_.erase(self->cardinalities_.begin(), self->cardinalities_.end());

	bool masked = true;

	// Fill decision/cardinality vectors, if possible.
	if (!table.IsNULL())
		table->GetValueSet(self->decisions_, self->cardinalities_, table->GetDecisionAttribute(masked), masked);

	// Filter.
	return Filter::Apply(structure);

}
