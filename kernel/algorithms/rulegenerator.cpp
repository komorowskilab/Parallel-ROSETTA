//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960419
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/rulegenerator.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/reducts.h>

//-------------------------------------------------------------------
// Methods for class RuleGenerator.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RuleGenerator::RuleGenerator() {
	idg_          = false;
	filename_idg_ = Undefined::String();
}

RuleGenerator::~RuleGenerator() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RuleGenerator, RULEGENERATOR, Algorithm)

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
RuleGenerator::GetParameters() const {

	String parameters;

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
RuleGenerator::SetParameter(const String &keyword, const String &value) {

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());

	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);

	return false;

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleGenerator::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(REDUCTS);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: UseIDG/UseIDG
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
RuleGenerator::UseIDG() const {
	return idg_;
}

bool
RuleGenerator::UseIDG(bool idg) {
	idg_ = idg;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetIDGFilename/SetIDGFilename
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
RuleGenerator::GetIDGFilename() const {
	return filename_idg_;
}

bool
RuleGenerator::SetIDGFilename(const String &filename) {
	filename_idg_ = filename;
	return true;
}
