//-------------------------------------------------------------------
// Author........:
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/classifier.h>

#include <kernel/structures/informationvector.h>

//-------------------------------------------------------------------
// Methods for class Classifier.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Classifier::Classifier() {
}

Classifier::~Classifier() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Classifier, CLASSIFIER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Classifier::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(INFORMATIONVECTOR);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsCompatible
// Author........: Aleksander šhrn
// Date..........:
// Description...: Is the classifier compatible with information vectors
//                 extracted from the given decision table?
// Comments......:
// Revisions.....:
//===================================================================

bool
Classifier::IsCompatible(const DecisionTable &/*table*/, bool /*masked*/) const {
	return true;
}

//-------------------------------------------------------------------
// Method........: Reset
// Author........: Aleksander šhrn
// Date..........:
// Description...: Can be overloaded in case the derived classifier
//                 needs to be reset somehow between classifications.
// Comments......:
// Revisions.....:
//===================================================================

bool
Classifier::Reset() {
	return true;
}

//-------------------------------------------------------------------
// Method........: Initialize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Can be overloaded in case the derived classifier
//                 needs to be initialized somehow before batch
//                 classification commences.
// Comments......:
// Revisions.....:
//===================================================================

bool
Classifier::Initialize(const DecisionTable &/*table*/, bool /*masked*/) {
	return true;
}

//-------------------------------------------------------------------
// Method........: SetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...: To make interface uniform for rule-based and
//                 non-rule-based classifiers.Typically overloaded by
//                 rule-based classifiers.
// Comments......:
// Revisions.....:
//===================================================================

bool
Classifier::SetRules(const Rules * /*rules*/) {
	return false;
}
 
