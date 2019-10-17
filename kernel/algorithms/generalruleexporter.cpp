//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/generalruleexporter.h>

#include <kernel/structures/generalrules.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class RuleExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

GeneralRuleExporter::GeneralRuleExporter() {
}

GeneralRuleExporter::~GeneralRuleExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(GeneralRuleExporter, GENERALRULEEXPORTER, RuleExporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Ewa Makosa: Added GeneralRules
//===================================================================

bool
GeneralRuleExporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(GENERALRULES);
}

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Merge code with ReductExporter.
// Revisions.....:
//===================================================================

bool
GeneralRuleExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {
	return RuleExporter::ExportPrologue(stream, structure);
}

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRuleExporter::ExportData(ofstream &stream, const Structure &structure) const {

	Message message;

	bool ok = true;

	// We may have to update some mutable stuff.
	GeneralRuleExporter *self = const_cast(GeneralRuleExporter *, this);

	// Cast to type verified in Apply method.
	Handle<GeneralRules> rules = dynamic_cast(GeneralRules *, const_cast(Structure *, &structure));

	// Get originating decision table, if any.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE));

	bool masked = true;

	self->no_objects_ = table->GetNoObjects(masked);
	self->decisions_.erase(self->decisions_.begin(), self->decisions_.end());
	self->cardinalities_.erase(self->cardinalities_.begin(), self->cardinalities_.end());

	// Fill decision/cardinality vectors, if possible.
	if (!table.IsNULL())
		table->GetValueSet(self->decisions_, self->cardinalities_, table->GetDecisionAttribute(masked), masked);

	int i, no_rules = rules->GetNoRules();

	// Do the actual exporting.
	for (i = 0; i < no_rules; i++) {

		if (!message.Progress("Exporting rules...", i, no_rules))
			break;

		Handle<GeneralRule> rule = rules->GetRule(i);

		// Export the rule (format specifics are overloaded).
		if (!ExportRule(stream, i, *rule)) {
			Message::Error("Failed to export rule.");
			ok = false;
			break;
		}

	}

	// Make sure all windows are popped down.
	/*
	if (!ok)
		Message::Reset();
	*/

	return ok;
}
//-------------------------------------------------------------------
// Method........: ExportEpilogue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRuleExporter::ExportEpilogue(ofstream &stream, const Structure &structure) const {
	return RuleExporter::ExportEpilogue(stream, structure);
}

//-------------------------------------------------------------------
// Methods inherited from RuleExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportRule
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRuleExporter::ExportRule(ofstream &/*stream*/, int /*rule_no*/, const Rule &/*rule*/) const {
	return false;
}
 
