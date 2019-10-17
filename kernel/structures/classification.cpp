//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/classification.h>

#include <kernel/basic/algorithm.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Classification.
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

Classification::Classification(const Classification &in) : Structure(in) {
	decision_attribute_ = in.decision_attribute_;
	decisions_          = in.decisions_;
	certainties_        = in.certainties_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Classification::Classification() {
	decision_attribute_ = Undefined::Integer();
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Classification::~Classification() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Classification, CLASSIFICATION, Structure)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
Classification::Clear() {
	decision_attribute_ = Undefined::Integer();
	decisions_.erase(decisions_.begin(), decisions_.end());
	certainties_.erase(certainties_.begin(), certainties_.end());
}

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Classification::Duplicate() const {
	return new Classification(*this);
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetDecisionAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the index of the decision attribute, i.e.
//                 the attribute that the possible classifications
//                 belong to.
// Comments......:
// Revisions.....:
//===================================================================

int
Classification::GetDecisionAttribute() const {
	return decision_attribute_;
}

//-------------------------------------------------------------------
// Method........: SetDecisionAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the index of the decision attribute.
// Comments......:
// Revisions.....:
//===================================================================

bool
Classification::SetDecisionAttribute(int decision_attribute) {
	decision_attribute_ = decision_attribute;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetNoDecisionValues
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of possible decision values
//                 (classes) of the decision attribute.
// Comments......:
// Revisions.....:
//===================================================================

int
Classification::GetNoDecisionValues() const {
	return decisions_.size();
}

//-------------------------------------------------------------------
// Method........: GetDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the specified possible decision value
//                 (class) decision attribute.
// Comments......:
// Revisions.....:
//===================================================================

int
Classification::GetDecisionValue(int i) const {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoDecisionValues())) {
		Message::Error("Index out of range.");
		return Undefined::Integer();
	}
#endif

	return decisions_[i];

}

//-------------------------------------------------------------------
// Method........: InsertDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Classification::InsertDecisionValue(int value, float certainty, int i) {

	int no_decisions = GetNoDecisionValues();

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i > no_decisions)) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	int j;

	// Is the class number already a possible classification?
	for (j = 0; j < no_decisions; j++) {
		if (GetDecisionValue(j) == value) {
			Message::Error(String::Format(value) + " is already a possible decision.");
			return false;
		}
	}

  decisions_.insert(decisions_.begin() + i, value);
	certainties_.insert(certainties_.begin() + i, certainty);

	return true;

}

//-------------------------------------------------------------------
// Method........: AppendDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Classification::AppendDecisionValue(int value, float certainty) {
	return InsertDecisionValue(value, certainty, GetNoDecisionValues());
}

//-------------------------------------------------------------------
// Method........: RemoveDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Classification::RemoveDecisionValue(int i) {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoDecisionValues())) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	decisions_.erase(decisions_.begin() + i);
	certainties_.erase(certainties_.begin() + i);

	return true;

}

//-------------------------------------------------------------------
// Method........: SwapData
// Author........: Aleksander šhrn
// Date..........:
// Description...: Swaps decision value data, used during sorting.
// Comments......:
// Revisions.....:
//===================================================================

void
Classification::SwapData(int i, int j) {
	std::swap(decisions_[i], decisions_[j]);
	std::swap(certainties_[i], certainties_[j]);
}

//-------------------------------------------------------------------
// Method........: Sort
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sorts all decision value data according to their
//                 certainty coefficients.
// Comments......:
// Revisions.....:
//===================================================================

bool
Classification::Sort() {

	// Is a sort redundant?
	if (certainties_.size() < 2)
		return true;

	int i, j;

	// Do a dumb bubble sort, there aren't usually that many elements anyway.
	for (i = 0; i < certainties_.size(); i++) {
		for (j = i + 1; j < certainties_.size(); j++) {
			if (certainties_[j] > certainties_[i]) {
				SwapData(i, j);
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetCertaintyCoefficient
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The certainty coefficients are meant as an abstract numerical
//                 quantity, and should not be confused with
//                 probability or MYCIN-like certainty factors.  How to
//                 interpret a certainty coefficient is to be decided by the
//                 classifiers that produce the classification object.
//
//                 Typically, for rule-based classifiers within this
//                 library, the strength measure will indicate the summed
//                 strengths of all rules indicating a particular
//                 decision, normalised to sum to unity.
// Revisions.....:
//===================================================================

float
Classification::GetCertaintyCoefficient(int i) const {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoDecisionValues())) {
		Message::Error("Index out of range.");
		return 0.0;
	}
#endif

	return certainties_[i];

}

//-------------------------------------------------------------------
// Method........: SetCertaintyCoefficient
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: See comments for the GetCertaintyCoefficient method.
// Revisions.....:
//===================================================================

bool
Classification::SetCertaintyCoefficient(float certainty, int i) {

#ifdef _DEBUG
	// Index in range?
	if ((i < 0) || (i >= GetNoDecisionValues())) {
		Message::Error("Index out of range.");
		return false;
	}
#endif

	certainties_[i] = certainty;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetBinaryOutcomeCoefficient
// Author........: Aleksander šhrn
// Date..........:
// Description...: Processes the classification structure and returns
//                 a model output value in [0, 1] that can be
//                 interpreted in a similar manner as e.g. a logistic
//                 regression (LR) function or a neural network (NN)
//                 model output.
//
//                 Assumes a "virtual" binary outcome (that is, actual
//                 outcome equal to the given focus decision class or not).
//
//                 Naming the focus class C, this method returns "Pr(C)"
//                 as provided by the classification structure.
//
//                 Consider for instance the following classification
//                 structures S1, S2, S3, S4:
//
//                     S1:  Decision class:   Certainty:
//                          0                 0.12
//                          1                 0.67
//                          2                 0.21
//
//                     S2:  Decision class:   Certainty:
//                          0                 0.30
//                          2                 0.70
//
//                     S3:  Decision class:   Certainty:
//                          0                 0.48
//                          1                 0.52
//
//                     S4:  Decision class:   Certainty:
//                          0                 0.35
//                          1                 0.25
//                          2                 0.40
//
//                 Assuming that our focus class is 2, we can binarize them as
//                 below, with 0 signifying different from 2 and 1 signifying equal to 2.
//
//                     S1': Decision class:   Certainty:
//                          0                 0.79
//                          1                 0.21
//
//                     S2': Decision class:   Certainty:
//                          0                 0.30
//                          1                 0.70
//
//                     S3': Decision class:   Certainty:
//                          0                 1.00
//
//                     S4': Decision class:   Certainty:
//                          0                 0.60
//                          1                 0.40
//
//                 Now, ordinarily we'd generate the following predictions:
//
//                     S1': Predicted class:  Certainty:
//                          0                 0.79
//
//                     S2': Predicted class:  Certainty:
//                          1                 0.70
//
//                     S3': Predicted class:  Certainty:
//                          0                 1.00
//
//                     S4': Predicted class:  Certainty:
//                          0                 0.60
//
//                 But if the LR/NN-convention is to be followed, we'd like to return the
//                 certainty values below instead, since the model's output is to be
//                 understod as the classifier's estimate of Pr(Class = 2).
//
//                     S1':                   Certainty':
//                                            0.21
//
//                     S2':                   Certainty':
//                                            0.70
//
//                     S3':                   Certainty':
//                                            0.00
//
//                     S4':                   Certainty':
//                                            0.40
//
// Comments......: If only one classification is made, and the associated
//                 certainty of this is not 1.0 (e.g., if the fallback
//                 classification is invoked), then it is assumed that the
//                 value is in [0, 1]. A warning is generated otherwise.
//
// Revisions.....: Aš 980528: Moved method from BatchClassifier,
//                            simplified logic.
//===================================================================

float
Classification::GetBinaryOutcomeCoefficient(int decision_class) const {

	// How many decision classes are there?
	int no_decisions = GetNoDecisionValues();

	if (no_decisions == 0)
		return Undefined::Float();

	float certainty_sum   = 0.0;
	float certainty_focus = 0.0;

	int i;

	// Determine the certainty coefficient for the focus class.
	for (i = 0; i < no_decisions; i++) {

		float certainty = GetCertaintyCoefficient(i);

		// Skip it if it's not valid.
		if (certainty == Undefined::Float())
			continue;

		if (GetDecisionValue(i) == decision_class)
			certainty_focus += certainty;

		certainty_sum += certainty;

	}

	// Were no valid certainty coefficients supplied?
	if (certainty_sum == 0.0)
		return Undefined::Float();

	// Compute summed certainty of all other classes.
	float certainty_other = certainty_sum - certainty_focus;

	// Normalize, if appropriate.
	if (certainty_sum != 1.0 && certainty_sum != 0.0 && no_decisions > 1)
		certainty_focus /= certainty_sum;

	if (certainty_sum != 1.0 && certainty_sum != 0.0 && no_decisions > 1)
		certainty_other /= certainty_sum;

	// Verify [0, 1] range.
	if (certainty_focus < 0.0 || certainty_focus > 1.0)
		Message::Warning("The certainty of the focus decision class is " + String::Format(certainty_focus) + ", which is outside [0, 1].");

	if (certainty_other < 0.0 || certainty_other > 1.0)
		Message::Warning("The (summed) certainty of the non-focus decision class(es) is " + String::Format(certainty_other) + ", which is outside [0, 1].");

	// Return a LR/NN-style model prediction value in [0, 1].
	if (no_decisions == 1) {
		if (GetDecisionValue(0) == decision_class)
			return certainty_focus;
		else
			return 1 - certainty_other;
	}

	return certainty_focus;

}

//-------------------------------------------------------------------
// Method........: Normalize
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Ensures that certainties sum to unity.
// Revisions.....:
//===================================================================

bool
Classification::Normalize() {

	int    i, no_decisions = GetNoDecisionValues();
	double sum = 0.0;

	for (i = 0; i < no_decisions; i++)
		sum += GetCertaintyCoefficient(i);

	if (sum == 1.0)
		return true;

	if (sum == 0.0)
		return false;

	for (i = 0; i < no_decisions; i++) {
		if (!SetCertaintyCoefficient(GetCertaintyCoefficient(i) / sum, i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNoRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Added for interface uniformity. Must be overloaded
//                 if rules are really used.
// Revisions.....:
//===================================================================

int
Classification::GetNoRules(int /*i*/) const {
	return 0;
}
 
