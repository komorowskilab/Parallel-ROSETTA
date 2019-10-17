//-------------------------------------------------------------------
// Author........: Aleksander ?hrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/discernibilityfunctionexporter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/discernibilitymatrix.h>
#include <kernel/structures/discernibilityfunction.h>

#include <kernel/utilities/discerner.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Static helpers (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticGetFunctionName
// Author........: Aleksander ?hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static String
StaticGetFunctionName(const DecisionTable &table, int i, int names) {

	String name;

	if (names == Undefined::Integer())
		name = String::Format(i);
	else
		name = table.GetEntry(i, names, true, false);

	name = "f(" + name + ")";

	return name;

}

//-------------------------------------------------------------------
// Methods for class DiscernibilityFunctionExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DiscernibilityFunctionExporter::DiscernibilityFunctionExporter() {
	discernibility_ = Reduct::DISCERNIBILITY_OBJECT;
	simplify_       = true;
}

DiscernibilityFunctionExporter::~DiscernibilityFunctionExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DiscernibilityFunctionExporter, DISCERNIBILITYFUNCTIONEXPORTER, DiscernibilityExporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander ?hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
DiscernibilityFunctionExporter::GetParameters() const {

	String parameters;

	// Discernibility.
	parameters += Keyword::Discernibility();
	parameters += Keyword::Assignment();
	parameters += Reduct::GetString(GetDiscernibilityType());

	parameters += Keyword::Separator();

	// Object selection.
	if (GetDiscernibilityType() == Reduct::DISCERNIBILITY_OBJECT) {
		parameters += GetObjectSelector().GetParameters();
		parameters += Keyword::Separator();
	}

	// Simplify.
	parameters += Keyword::Simplify();
	parameters += Keyword::Assignment();
	parameters += String::Format(Simplify());

	parameters += Keyword::Separator();

	return parameters + DiscernibilityExporter::GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander ?hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityFunctionExporter::SetParameter(const String &keyword, const String &value) {

	// Object selection.
	if (GetObjectSelector().SetParameter(keyword, value))
		return true;

	// Discernibility.
	if (keyword == Keyword::Discernibility()) {
		if (value == Reduct::GetString(Reduct::DISCERNIBILITY_FULL))
			return SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);
		if (value == Reduct::GetString(Reduct::DISCERNIBILITY_OBJECT))
			return SetDiscernibilityType(Reduct::DISCERNIBILITY_OBJECT);
		return false;
	}

	// Simplify.
	if (keyword == Keyword::Simplify() && value.IsBoolean())
		return Simplify(value.GetBoolean());

	return DiscernibilityExporter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Methods inherited from DiscernibilityExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander ?hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityFunctionExporter::ExportData(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const {

	// Dispatch.
	switch (GetDiscernibilityType()) {
		case Reduct::DISCERNIBILITY_FULL   : return ExportSystemFunction(stream, discerner, table, masked, name_index_unmasked);
		case Reduct::DISCERNIBILITY_OBJECT : return ExportObjectFunctions(stream, discerner, table, masked, name_index_unmasked);
	}

	return false;

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportSystemFunction
// Author........: Aleksander ?hrn
// Date..........:
// Description...: Computes, formats and exports the discernibility
//                 functions for the full system.
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityFunctionExporter::ExportSystemFunction(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const {

	Message message;

	DiscernibilityMatrix   matrix;
	DiscernibilityFunction function;

	bool  copy      = false;
	bool  reduce    = false;
	bool  simplify  = false;
	float precision = Undefined::Float();
	bool  weighted  = false;

	// Compute function.
	if (Simplify()) {
		message.Notify("Building and simplifying discernibility function...");
		if (!function.CreateSimplified(table, masked, ModuloDecision(), discerner, NULL, precision, NULL, weighted))
			return false;
	}
	else {
		message.Notify("Computing discernibility matrix...");
		if (!matrix.Create(table, masked, ModuloDecision(), discerner, precision))
			return false;
		if (!function.Create(matrix, copy, reduce, simplify, weighted))
			return false;
	}

	// Baptize.
	function.SetName("f(" + table.GetName() + ")");

	message.Notify("Exporting discernibility function, " + String::Format(function.GetNoSums()) + " sums...");

	// Export function.
	if (!function.Save(stream, &table, masked))
		return false;

	stream << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportObjectFunctions
// Author........: Aleksander ?hrn
// Date..........:
// Description...: Computes, formats and exports the discernibility
//                 functions for all selected objects.
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityFunctionExporter::ExportObjectFunctions(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const {

	Vector(int) selected;

	// Select objects to export.
	if (!GetObjectSelector().Select(selected, table, masked)) {
		Message::Error("Failed to perform object selection.");
		return false;
	}

	if (selected.empty()) {
		Message::Error("No objects match selection criteria.");
		return false;
	}

	Message message;

	DiscernibilityMatrix         matrix;
	GeneralizedDecision::Handles decisions;

	bool  copy      = false;
	bool  reduce    = false;
	bool  simplify  = false;
	float precision = Undefined::Float();
	bool  weighted  = false;

	if (Simplify()) {
		message.Notify("Initializing discernibility matrix...");
		if (!matrix.Initialize(table, masked, ModuloDecision()))
			return false;
		message.Notify("Computing generalized decisions...");
		if (ModuloDecision() && !table.GetGeneralizedDecisions(decisions, discerner, masked))
			return false;
	}
	else {
		message.Notify("Creating discernibility matrix...");
		if (!matrix.Create(table, selected, masked, ModuloDecision(), discerner, precision))
			return false;
	}

	int i;

	// Process all selected objects.
	for (i = 0; i < selected.size(); i++) {

		// Determine function name.
		String name = StaticGetFunctionName(table, selected[i], name_index_unmasked);

		if (!message.Progress("Exporting " + name + "...", i, selected.size()))
			break;

		DiscernibilityFunction function;

		if (Simplify()) {
			message.Notify("Building and simplifying " + name + "...");
			if (!function.CreateSimplified(table, selected[i], masked, ModuloDecision(), discerner, ModuloDecision() ? &decisions : NULL, Undefined::Float(), &matrix, weighted))
				return false;
		}
		else {
			message.Notify("Building " + name + "...");
			if (!function.Create(matrix, selected[i], copy, reduce, simplify))
				return false;
		}

		// Baptize.
		function.SetName(name);

		message.Notify("Exporting " + name + ", " + String::Format(function.GetNoSums()) + " sums...");

		// Export function.
		if (!function.Save(stream, &table, masked))
			return false;

		stream << endl;

	}

	return true;

}

DiscernibilityFunctionExporter *
DiscernibilityFunctionExporter::Clone() {
	return new DiscernibilityFunctionExporter;
}
