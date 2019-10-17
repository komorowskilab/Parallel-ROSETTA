//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/approximation.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticSetPointers
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the pointers (handles) in one array to point
//                 similarly as they do in the other.  Used for
//                 duplication and assignment.
// Comments......: Assumes the array to set has been emptied.
// Revisions.....:
//===================================================================

static bool
StaticSetPointers(Handle<EquivalenceClasses> set1, const Handle<EquivalenceClasses> universe1,
									const Handle<EquivalenceClasses> set2, const Handle<EquivalenceClasses> universe2) {

	int i, no_classes = set2->GetNoEquivalenceClasses();

	for (i = 0; i < no_classes; i++) {

		// Find out to where into the universe the pointer points.
		int index = universe2->FindMember(set2->GetEquivalenceClass(i));

		if (index == Undefined::Integer()) {
			Message::Error("Could not set pointer array properly.");
			return false;
		}

		// Set the other pointer accordingly.
    set1->AppendStructure(universe1->GetEquivalenceClass(index));

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticSaveSet
// Author........: Aleksander šhrn
// Date..........:
// Description...: Saves the size and indices only of the given set.
//                 Pointers can then easily be reconstructed later.
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticSaveSet(ofstream &stream, const Handle<EquivalenceClasses> set, const Handle<EquivalenceClasses> universe) {

	int i, no_classes = set->GetNoEquivalenceClasses();

	if (!IOKit::Save(stream, no_classes))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	for (i = 0; i < no_classes; i++) {

		// Find out to where into the universe the pointer points.
		int index = universe->FindMember(set->GetEquivalenceClass(i));

		if (index == Undefined::Integer()) {
			Message::Error("Could not locate pointer into universe.");
			return false;
		}

		// Save the index only.
		if (!IOKit::Save(stream, index))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticLoadSet
// Author........: Aleksander šhrn
// Date..........:
// Description...: Inverse of StaticSaveSet.
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticLoadSet(ifstream &stream, Handle<EquivalenceClasses> set, const Handle<EquivalenceClasses> universe) {

	int i, no_classes, index;

	// Load the number of eq. classes to append.
	if (!IOKit::Load(stream, no_classes))
		return false;

	for (i = 0; i < no_classes; i++) {

		// Load the index.
		if (!IOKit::Load(stream, index))
			return false;

		// Append an eq. class.
		if (!set->AppendStructure(universe->GetEquivalenceClass(index)))
			return false;

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticGetRatio
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Description...: Returns the ratio between the number of objects in
//                 the given eq. class set that are correctly classified
//                 by the approximation, and the total number of objects
//                 with the decision value that was attempted approximated.
//
// Comments......: The counter and denominator of the ratio can optionally
//                 be returned in-place.
//
//                 So that sensitivity and specificty computation share the
//                 same implementation.
// Revisions.....:
//===================================================================

static float
StaticGetRatio(Handle<EquivalenceClasses> set, int decision, bool equality,
							 int *no_correct, int *no_total, const DecisionTable *table) {

	if (no_correct != NULL)
		*no_correct = 0;

	if (no_total != NULL)
		*no_total = 0;

	// Was the originating decision table supplied?
	if (table == NULL) {
		Message::Error("Could not acquire originating decision table.");
		return Undefined::Float();
	}

	// Operate on a masked table.
	bool masked = true;

	// Acquire the index of the table's decision attribute.
	int decision_attribute = table->GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer()) {
		Message::Error("Could not acquire the index of the decision attribute in the originating decision table.");
		return Undefined::Float();
	}

  int no_classes = set->GetNoEquivalenceClasses();
	int counter    = 0;
	int i, j;

	// Run through the set and count how many objects that are correctly classified.
	for (i = 0; i < no_classes; i++) {
		Handle<EquivalenceClass> eqclass = set->GetEquivalenceClass(i);
		int no_objects = eqclass->GetNoObjects();
		for (j = 0; j < no_objects; j++) {
			int object_no = eqclass->GetObject(j);
			int entry     = table->GetEntry(object_no, decision_attribute, masked);
		  if (equality && (entry == decision))
				counter++;
			else if (!equality && (entry != decision))
				counter++;
		}
	}

	if (no_correct != NULL)
		*no_correct = counter;

	int no_objects  = table->GetNoObjects(masked);
	int denominator = 0;

	// Run through the table and count how many objects that actually belong (or don't belong)
	// to the set of object with the decision that was attempted approximated.
	for (i = 0; i < no_objects; i++) {
		int entry = table->GetEntry(i, decision_attribute, masked);
		if (equality && (entry == decision))
			denominator++;
		else if (!equality && (entry != decision))
			denominator++;
	}

	if (no_total != NULL)
		*no_total = denominator;

	// Avoid division by zero.
	if (denominator == 0)
		return Undefined::Float();

	// Safe-guard to avoid numerical quirks.
	if (counter == denominator)
		return 1.0;

	return static_cast(float, counter) / denominator;

}

//-------------------------------------------------------------------
// Methods for class Approximation.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Approximation::Approximation(const Approximation &in) : AnnotatedStructure(in) {

	decision_ = in.decision_;

	// Copy (i.e. physically duplicate) the equivalence class sets.
	universe_ = dynamic_cast(EquivalenceClasses *, in.universe_->Duplicate());

	// Instantiate eq. class sets.
	lower_    = Creator::EquivalenceClasses();
	upper_    = Creator::EquivalenceClasses();
	boundary_ = Creator::EquivalenceClasses();
	outside_  = Creator::EquivalenceClasses();

	// Set pointers into the universe.
	StaticSetPointers(lower_,    universe_, in.lower_,    in.universe_);
	StaticSetPointers(upper_,    universe_, in.upper_,    in.universe_);
	StaticSetPointers(boundary_, universe_, in.boundary_, in.universe_);
	StaticSetPointers(outside_,  universe_, in.outside_,  in.universe_);

}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Approximation::Approximation() {

	decision_ = Undefined::Integer();

	// Instantiate eq. class sets.
	universe_ = Creator::EquivalenceClasses();
	lower_    = Creator::EquivalenceClasses();
	upper_    = Creator::EquivalenceClasses();
	boundary_ = Creator::EquivalenceClasses();
	outside_  = Creator::EquivalenceClasses();

}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The handle mechanism should take care of the
//                 garbage collection.
// Revisions.....:
//===================================================================

Approximation::~Approximation() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Approximation, APPROXIMATION, AnnotatedStructure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Approximation::Load(ifstream &stream) {

	// Clear current contents.
	Clear();

	// Load stuff higher up.
	if (!AnnotatedStructure::Load(stream))
		return false;

	// Load the decision value.
	if (!IOKit::Load(stream, decision_))
		return false;

	// Load the universe (the basic building blocks).
	if (!universe_->Load(stream))
		return false;

	// Load the indices and set the pointers.
	if (!StaticLoadSet(stream, lower_,    universe_) ||
		  !StaticLoadSet(stream, upper_,    universe_) ||
			!StaticLoadSet(stream, boundary_, universe_) ||
			!StaticLoadSet(stream, outside_,  universe_))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Approximation::Save(ofstream &stream) const {

	// Save stuff higher up.
	if (!AnnotatedStructure::Save(stream))
		return false;

	// Save the decision value.
	if (!IOKit::Save(stream, decision_))
		return false;

	// Save the universe (i.e. the primitive building blocks).
	if (!universe_->Save(stream))
		return false;

	// Save sizes and indices into the universe only (eases later reconstruction).
	if (!StaticSaveSet(stream, lower_,    universe_) ||
      !StaticSaveSet(stream, upper_,    universe_) ||
			!StaticSaveSet(stream, boundary_, universe_) ||
			!StaticSaveSet(stream, outside_,  universe_))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Approximation::Duplicate() const {
  return new Approximation(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
Approximation::Clear() {

	// Clear stuff higher up.
	AnnotatedStructure::Clear();

	// Clear the decision value.
  decision_ = Undefined::Integer();

	// Clear the eq. class sets.
	universe_->Clear();
	lower_->Clear();
	upper_->Clear();
	boundary_->Clear();
	outside_->Clear();

}

//-------------------------------------------------------------------
// Method........: Assignment operator
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assign annotation/parent stuff from higher up,
//                 too...?
// Revisions.....:
//===================================================================

Approximation &
Approximation::operator=(const Approximation &in) {

	// Clear present contents.
	Clear();

	// Copy the decision value.
	decision_ = in.decision_;

	// Copy (i.e. physically duplicate) the equivalence class sets.
	universe_ = dynamic_cast(EquivalenceClasses *, in.universe_->Duplicate());

	// Set pointers into the universe.
	StaticSetPointers(lower_,    universe_, in.lower_,    in.universe_);
	StaticSetPointers(upper_,    universe_, in.upper_,    in.universe_);
	StaticSetPointers(boundary_, universe_, in.boundary_, in.universe_);
	StaticSetPointers(outside_,  universe_, in.outside_,  in.universe_);

	return *this;

}

//-------------------------------------------------------------------
// Method........: Equality operator
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Don't bother to test the "redundant" sets.
// Revisions.....:
//===================================================================

bool
Approximation::operator==(const Approximation &in) const {

	if (this == &in)
		return true;

#if 0
	// Is it the same set that's approximated?
	if (GetDecisionValue() != in.GetDecisionValue())
		return false;
#endif

	int this_size = GetLowerApproximation()->GetNoEquivalenceClasses();
	int that_size = in.GetLowerApproximation()->GetNoEquivalenceClasses();

	if (this_size != that_size)
		return false;

	int i;

	// Are the lower boundaries the same?
	for (i = 0; i < this_size; i++) {
		if (*(GetLowerApproximation()->GetEquivalenceClass(i)) != *(in.GetLowerApproximation()->GetEquivalenceClass(i)))
			return false;
	}

	this_size = GetUpperApproximation()->GetNoEquivalenceClasses();
	that_size = in.GetUpperApproximation()->GetNoEquivalenceClasses();

	if (this_size != that_size)
		return false;

	// Are the upper boundaries the same?
	for (i = 0; i < this_size; i++) {
		if (*(GetUpperApproximation()->GetEquivalenceClass(i)) != *(in.GetUpperApproximation()->GetEquivalenceClass(i)))
			return false;
	}

	this_size = GetUniverse()->GetNoEquivalenceClasses();
	that_size = in.GetUniverse()->GetNoEquivalenceClasses();

	if (this_size != that_size)
		return false;

	// Are the universes the same?
	for (i = 0; i < this_size; i++) {
		if (*(GetUniverse()->GetEquivalenceClass(i)) != *(in.GetUniverse()->GetEquivalenceClass(i)))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Inequality operator
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Approximation::operator!=(const Approximation &in) const {
	return !(*this == in);
}

//-------------------------------------------------------------------
// Method........: IsRough
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Approximation::IsRough() const {

	if ((GetUpperApproximation() == NULL) || (GetLowerApproximation() == NULL)) {
		Message::Error("The upper or lower approximation is NULL.");
		return false;
	}

	int upper_no_classes = GetUpperApproximation()->GetNoEquivalenceClasses();
	int lower_no_classes = GetLowerApproximation()->GetNoEquivalenceClasses();

	return (upper_no_classes != lower_no_classes);

}

//-------------------------------------------------------------------
// Method........: IsCrisp
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Approximation::IsCrisp() const {
	return !IsRough();
}

//-------------------------------------------------------------------
// Method........: GetSensitivity
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the sensitivity of the approximation.
// Comments......:
// Revisions.....:
//===================================================================

float
Approximation::GetSensitivity(int *no_correct, int *no_total) const {
	return StaticGetRatio(lower_, decision_, true, no_correct, no_total, dynamic_cast(DecisionTable *, FindParent(DECISIONTABLE)));
}

//-------------------------------------------------------------------
// Method........: GetSpecificity
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Computes the specificity of the approximation.
// Revisions.....:
//===================================================================

float
Approximation::GetSpecificity(int *no_correct, int *no_total) const {
	return StaticGetRatio(outside_, decision_, false, no_correct, no_total, dynamic_cast(DecisionTable *, FindParent(DECISIONTABLE)));
}

//-------------------------------------------------------------------
// Method........: GetAccuracy
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the accuracy of the approximation.
// Comments......: Accuracy can be seen as a weighting between
//                 sensitivity and specificity. Can be optimized.
// Revisions.....:
//===================================================================

float
Approximation::GetAccuracy(int *no_correct, int *no_total) const {

	if (no_correct != NULL)
		*no_correct = 0;

	if (no_total != NULL)
		*no_total = 0;

	int no_correct_sensitivity = 0;
	int no_correct_specificity = 0;
	int no_total_sensitivity   = 0;
	int no_total_specificity   = 0;

	// Calculate sensitivity.
	float sensitivity = GetSensitivity(&no_correct_sensitivity, &no_total_sensitivity);

	if (sensitivity == Undefined::Float())
		return Undefined::Float();

	// Calculate specificity.
	float specificity = GetSpecificity(&no_correct_specificity, &no_total_specificity);

	if (specificity == Undefined::Float())
		return Undefined::Float();

	int counter     = no_correct_sensitivity + no_correct_specificity;
	int denominator = no_total_sensitivity + no_total_specificity;

	if (no_correct != NULL)
		*no_correct = counter;

	if (no_total != NULL)
		*no_total = denominator;

	return static_cast(float, counter) / denominator;

}
 
