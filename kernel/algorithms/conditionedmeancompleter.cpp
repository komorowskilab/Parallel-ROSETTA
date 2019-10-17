//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/conditionedmeancompleter.h>
#include <kernel/algorithms/meancompleter.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Methods for class ConditionedMeanCompleter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

ConditionedMeanCompleter::ConditionedMeanCompleter() {
	SetCompleter(dynamic_cast(Completer *, ObjectManager::GetIdentifiedAlgorithm(MEANCOMPLETER)));
}

ConditionedMeanCompleter::~ConditionedMeanCompleter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ConditionedMeanCompleter, CONDITIONEDMEANCOMPLETER, ConditionedCompleter)

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
ConditionedMeanCompleter::GetParameters() const {

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
ConditionedMeanCompleter::SetParameter(const String &keyword, const String &value) {

	if (GetCompleter() == NULL)
		return false;

	return GetCompleter()->SetParameter(keyword, value);

} 

ConditionedMeanCompleter *
ConditionedMeanCompleter::Clone() {
	return new ConditionedMeanCompleter;
}
