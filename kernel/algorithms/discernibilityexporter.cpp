//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/discernibilityexporter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/indiscernibilitygraph.h>

#include <kernel/utilities/discerner.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Static stuff (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticGetNameIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the index (unmasked) of the attribute with
//                 the specified name.
//
//                 Also checks that attribute is valid for unique
//                 object naming (with a masked table).
//
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticGetNameIndex(int &index, const String &name, const DecisionTable &table) {

	index = Undefined::Integer();

	if (name == Undefined::String())
		return true;

	bool masked = false;

	// Get index.
	index = table.GetAttributeIndex(name, true, masked);

	if (index == Undefined::Integer()) {
		Message::Error("\"" + name + "\" is not a valid attribute name.");
		return false;
	}

	Vector(int) values, cardinalities;

	// Get value set.
	if (!table.GetValueSet(values, cardinalities, index, masked)) {
		Message::Error("Unable to get value set of attribute " + name + ".");
		return false;
	}

	// Verify "1-1"-ness with the masked table.
	if (values.size() != table.GetNoObjects(true)) {
		Message::Error("Attribute " + name + " does not define unique object names.");
		return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Methods for class DiscernibilityExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DiscernibilityExporter::DiscernibilityExporter() {
	idg_          = false;
	filename_idg_ = Undefined::String();
	name_         = false;
	id_name_      = Undefined::String();
	modulo_       = false;
}

DiscernibilityExporter::~DiscernibilityExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DiscernibilityExporter, DISCERNIBILITYEXPORTER, DecisionTableExporter)

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
DiscernibilityExporter::GetParameters() const {

	String parameters;

	// Use IDG?
	parameters += Keyword::IDG();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseIDG());

	parameters += Keyword::Separator();

	// IDG filename.
	if (UseIDG()) {

		parameters += Keyword::IDG() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIDGFilename();

		parameters += Keyword::Separator();

	}

	// Use names?
	parameters += Keyword::Names();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseIDAttribute());

	parameters += Keyword::Separator();

	// Name of 1-1 attribute for object naming.
	if (UseIDAttribute()) {

		parameters += Keyword::Names() + Keyword::Dot() + Keyword::Attribute();
		parameters += Keyword::Assignment();
		parameters += GetIDAttributeName();

		parameters += Keyword::Separator();

	}

	// Modulo decision.
	parameters += Keyword::Modulo() + Keyword::Dot() + Keyword::Decision();
	parameters += Keyword::Assignment();
	parameters += String::Format(ModuloDecision());

	parameters += Keyword::Separator();

	return parameters + DecisionTableExporter::GetParameters();

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
DiscernibilityExporter::SetParameter(const String &keyword, const String &value) {

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());

	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);

	// Names.
	if (keyword == Keyword::Names() && value.IsBoolean())
		return UseIDAttribute(value.GetBoolean());

	// Name of 1-1 attribute for object naming.
	if (keyword == Keyword::Names() + Keyword::Dot() + Keyword::Attribute())
		return SetIDAttributeName(value);

	// Modulo decision.
	if (keyword == Keyword::Modulo() + Keyword::Dot() + Keyword::Decision() && value.IsBoolean())
		return ModuloDecision(value.GetBoolean());

	return DecisionTableExporter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	stream << "% Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " + IdHolder::GetClassname(GetId()) << endl;
	stream << "% {" + GetParameters() + "}" << endl;
	stream << "%" << endl;
	stream << "% " << structure.GetName() << endl;
	stream << endl;

	return true;

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
DiscernibilityExporter::ExportData(ofstream &stream, const Structure &structure) const {

	// Cast to verified type.
	Handle<DecisionTable> table = const_cast(DecisionTable *, dynamic_cast(const DecisionTable *, &structure));

	bool masked = true;

	Discerner discerner;

	// Load IDGs, if specified.
	if (UseIDG() && !discerner.LoadIDGs(GetIDGFilename(), *table, masked)) {
		Message::Error("Failed to load IDGs from " + GetIDGFilename() + ".");
		return false;
	}

	int name_index_unmasked = Undefined::Integer();

	// Deduce index and verify validity of naming attribute.
	if (UseIDAttribute() && !StaticGetNameIndex(name_index_unmasked, GetIDAttributeName(), *table))
		return false;

	// Current implementation cannot handle object masking.
	if (table->GetNoObjects(masked) != table->GetNoObjects(!masked)) {
		Message::Error("Current implementation cannot handle object masking.");
		return false;
	}

	return ExportData(stream, discerner, *table, masked, name_index_unmasked);

}

//-------------------------------------------------------------------
// Methods inherited from DecisionTableExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportInformationVector
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityExporter::ExportInformationVector(ofstream &/*stream*/, const DecisionTable &/*table*/, int /*object_no*/, bool /*masked*/) const {
	return false;
}

