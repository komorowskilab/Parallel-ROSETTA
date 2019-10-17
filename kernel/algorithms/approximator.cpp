//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/approximator.h>
#include <kernel/algorithms/partitioner.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/equivalenceclass.h>
#include <kernel/structures/equivalenceclasses.h>
#include <kernel/structures/approximation.h>

#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Methods for class Approximator.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Approximator::Approximator() {
	decision_   = Undefined::Integer();
	attributes_ = Undefined::String();
	precision_  = 0.0f;
}

Approximator::~Approximator() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Approximator, APPROXIMATOR, Algorithm)

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
Approximator::GetParameters() const {

	String parameters, formatted;

	// Decision.
	parameters += Keyword::Decision();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetDecisionValue());

	parameters += Keyword::Separator();

	// Precision.
	parameters += Keyword::Precision();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetPrecision());

	parameters += Keyword::Separator();

	// Attributes.
	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += GetAttributes();

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
Approximator::SetParameter(const String &keyword, const String &value) {

	// Decision.
  if (keyword == Keyword::Decision() && value.IsInteger())
		return SetDecisionValue(value.GetInteger());

	// Precision.
  if (keyword == Keyword::Precision() && value.IsFloat())
		return SetPrecision(value.GetFloat());

	// Attributes.
	if (keyword == Keyword::Attributes())
		return SetAttributes(value);

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
Approximator::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Calculates the rough approximation of the set of
//                 objects with a certain decision, using a certain
//                 set of attributes.  A precision level p can be set.
//
//                 Let m be the rough membership value.  Assign the
//                 eq. classes as follows:
//
//                    if m >= 1 - p then in lower appr.
//                    if m >  p     then in upper appr.
//
//                 Note that p = 0 implies the "traditional" meaning
//                 of the lower/upper approximations.
//
//                 The value of p should lie in the range [0, 0.5].
//
// Comments......: Assumes that library clients use handles.
// Revisions.....: Aš 990630: Redefined range of p to lie in [0, 0.5]
//                            instead of [0.5, 1], following Ziarko's
//                            notation.
//===================================================================

Structure *
Approximator::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Operate on a masked table.
	bool masked = true;

	// Get the decision attribute of the table.
	int decision_attribute = table->GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer()) {
		Message::Error("Unable to determine the table's decision attribute.");
		return NULL;
	}

	// Acquire an installed partioner algorithm.
	Handle<Partitioner> partitioner = dynamic_cast(Partitioner *, ObjectManager::GetIdentifiedAlgorithm(PARTITIONER));

	if (partitioner == NULL) {
		Message::Error("No partitioning algorithm installed.");
		return NULL;
	}

	// Set the partitioning parameters.
	partitioner->SetAttributes(GetAttributes());

	// Calculate the equivalence classes.
	Handle<EquivalenceClasses> partition = dynamic_cast(EquivalenceClasses *, structure.Apply(*partitioner));

	if (partition == NULL) {
		Message::Error("Failed to compute partition.");
		return NULL;
	}

  // Create an approximation structure.
	Handle<Approximation> approximation = Creator::Approximation();

	// Create eq. class sets.
	Handle<EquivalenceClasses> universe = Creator::EquivalenceClasses(); // All objects.
	Handle<EquivalenceClasses> upper    = Creator::EquivalenceClasses(); // Upper approximation.
	Handle<EquivalenceClasses> lower    = Creator::EquivalenceClasses(); // Lower approximation.
	Handle<EquivalenceClasses> outside  = Creator::EquivalenceClasses(); // The universe minus the upper approximation.
	Handle<EquivalenceClasses> boundary = Creator::EquivalenceClasses(); // The upper minus the lower approximation.

	int   i, no_equivalence_classes = partition->GetNoEquivalenceClasses();
	float precision = GetPrecision();

	// Construct the set approximations by examining all eq. classes.
  for (i = 0; i < no_equivalence_classes; i++) {

		// Get the eq. class.
		Handle<EquivalenceClass> eqclass = partition->GetEquivalenceClass(i);

		// Calculate the rough membership value.
		float membership = eqclass->GetMembership(GetDecisionValue(), *table, decision_attribute, masked);

		// Determine "location" of eq. class.
		bool  is_in_lower    = (membership >= 1.0 - precision);
		bool  is_in_upper    = is_in_lower || (membership > precision);
		bool  is_in_boundary = !is_in_lower && is_in_upper;
		bool  is_outside     = !is_in_upper;

		// Place the eq. class where it belongs.
		universe->AppendStructure(eqclass.GetPointer());
		if (is_in_upper)
			upper->AppendStructure(eqclass.GetPointer());
		if (is_in_lower)
			lower->AppendStructure(eqclass.GetPointer());
		if (is_in_boundary)
			boundary->AppendStructure(eqclass.GetPointer());
		if (is_outside)
			outside->AppendStructure(eqclass.GetPointer());
	}

  // Fill the approximation structure.
	approximation->SetDecisionValue(GetDecisionValue());
	approximation->SetUniverse(universe.GetPointer());
	approximation->SetUpperApproximation(upper.GetPointer());
	approximation->SetLowerApproximation(lower.GetPointer());
	approximation->SetBoundaryRegion(boundary.GetPointer());
	approximation->SetOutsideRegion(outside.GetPointer());

	// Return the approximation.
  return approximation.Release();

}

 

Approximator *
Approximator::Clone() {
	return new Approximator;
}
