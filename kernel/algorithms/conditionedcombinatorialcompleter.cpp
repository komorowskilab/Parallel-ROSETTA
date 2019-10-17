//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/conditionedcombinatorialcompleter.h>
#include <kernel/algorithms/combinatorialcompleter.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Methods for class ConditionedCombinatorialCompleter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ConditionedCombinatorialCompleter::ConditionedCombinatorialCompleter() {
	SetCompleter(dynamic_cast(Completer *, ObjectManager::GetIdentifiedAlgorithm(COMBINATORIALCOMPLETER)));
}

ConditionedCombinatorialCompleter::~ConditionedCombinatorialCompleter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ConditionedCombinatorialCompleter, CONDITIONEDCOMBINATORIALCOMPLETER, ConditionedCompleter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overloaded for cosmetic reasons only.
// Revisions.....:
//===================================================================

String
ConditionedCombinatorialCompleter::GetParameters() const {

	if (GetCompleter() == NULL)
		return String("");

	return GetCompleter()->GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overloaded since the completer is fixed.
// Revisions.....:
//===================================================================

bool
ConditionedCombinatorialCompleter::SetParameter(const String &keyword, const String &value) {

	if (GetCompleter() == NULL)
		return false;

	return GetCompleter()->SetParameter(keyword, value);

} 

ConditionedCombinatorialCompleter *
ConditionedCombinatorialCompleter::Clone() {
	return new ConditionedCombinatorialCompleter;
}
