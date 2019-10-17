//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/ruleexporter.h>

#include <kernel/structures/rules.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class RuleExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RuleExporter::RuleExporter() {
}

RuleExporter::~RuleExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleExporter, RULEEXPORTER, Exporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Ewa Makosa 03.06.2005: Added GeneralRules
//===================================================================

bool
RuleExporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
/*	if (structure.IsA(GENERALRULES))
		return false;
*/	return structure.IsA(RULES);
}


//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================



//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Nicholas Baltzer
// Date..........: 24/11/14
// Description...: Allows parameters in the RuleExporter class.
// Comments......: RuleExporter does not have any parameters. This is here for consistency
// 			of design.
// Revisions.....:
//===================================================================

/*
bool
RuleExporter::SetParameter(const String &keyword, const String &value) {

	return Exporter::SetParameter(keyword, value);

}
*/

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Nicholas Baltzer
// Date..........: 24/11/14
// Description...: Returns a String with the parameters set.
// Comments......: Does not have parameters. Only here for consistency of design
// 			and extendability should there ever be parameters and options added.
// Revisions.....:
//===================================================================
/*
String
RuleExporter::GetParameters() const {

	return Exporter::GetParameters();

}
*/

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Merge code with ReductExporter.
// Revisions.....:
//===================================================================

bool
RuleExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportPrologue(stream, structure);
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
RuleExporter::ExportData(ofstream &stream, const Structure &structure) const {

	Message message;

	bool ok = true;

	// We may have to update some mutable stuff.
	RuleExporter *self = const_cast(RuleExporter *, this);

	// Cast to type verified in Apply method.
	Handle<Rules> rules = dynamic_cast(Rules *, const_cast(Structure *, &structure));

	Structure * dt;
	#pragma omp critical
	{
		dt = rules->FindParent(DECISIONTABLE);
	
	}
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, dt);

	bool masked = true;

	int no_obj = 0;
	no_obj = table->GetNoObjects(masked);
	self->no_objects_ = no_obj;

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

		Handle<Rule> rule = rules->GetRule(i);

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
RuleExporter::ExportEpilogue(ofstream &stream, const Structure &structure) const {
	return Exporter::ExportEpilogue(stream, structure);
}
