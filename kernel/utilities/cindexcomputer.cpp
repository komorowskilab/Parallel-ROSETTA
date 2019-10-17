//-------------------------------------------------------------------
// Author........: Staal Vinterbo
// Date..........:
// Description...: Implements the calculation of ROC areas with
//                 standard error
//
//                 @ARTICLE{hanley:roc82,
//                   author = "James A. Hanley and Barbara J. McNeil",
//                   title = "The meaning and use of the area under the ROC curve",
//                   journal = "Radiology",
//                   year = 1982,
//                 }
//
// Revisions.....: Aš: Made code adhere to coding standards.
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/cindexcomputer.h>

#include <kernel/system/math.h>

#include <kernel/basic/undefined.h>
#include <kernel/basic/algorithm.h>

//-------------------------------------------------------------------
// Methods for class CIndexComputer
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

CIndexComputer::CIndexComputer(const Vector(int) *actuals, const Vector(float) *predicted) {
	computed_  = false;
	actuals_   = actuals;
	predicted_ = predicted;
	cindex_    = Undefined::Float();
	se_        = Undefined::Float();
	error_     = false;
}

CIndexComputer::~CIndexComputer() {
}

//-------------------------------------------------------------------
// Method........: GetCIndex
// Author........: Staal Vinterbo
// Date..........:
// Description...: Returns cached c-index. Calculates it if not
//                 done previously.
// Comments......:
// Revisions.....: Aš: Added some comments, made code adhere to
//                     library standards.
//===================================================================

float
CIndexComputer::GetCIndex() {

  int i;

	// Return c-index, if cached.
  if (computed_)
    return cindex_;

	// Stuff not properly initialized?
  if (predicted_ == NULL || actuals_ == NULL || predicted_->size() != actuals_->size() || predicted_->size() == 0) {
    error_ = true;
    return Undefined::Float();
  }

  Vector(int) indices;

  indices.reserve(predicted_->size());

	// Create index vector.
  for (i = 0; i < predicted_->size(); i++)
    indices.push_back(i);

  Compare comparator(predicted_);

	// Sort indices according to predictions.
  std::sort(indices.begin(), indices.end(), comparator);

  int no_1s = 0;
  int no_0s = 0;

  // Count the number of ones.
  for (i = 0; i < predicted_->size(); i++) {
    if ((*actuals_)[i] == 1)
      no_1s++;
  }

  // Count the number of zeros.
  no_0s = actuals_->size() - no_1s;

  int no_0s_below = 0; // Number of 0s ranked below current.
  int no_1s_below = 0; // Number of 1s ranked below current.
  int start       = 0; // Start of current "iso-rank window".
  int stop        = 0; // Stop of current "iso-rank window".

  double Q1 = 0.0;
  double Q2 = 0.0;
  double W  = 0.0;

  while (start < predicted_->size()) {

    // Number of 1s inside "iso-rank window" (IRW).
    int no_1s_inside = 0;

    // Determine IRW.
    while (stop < predicted_->size() && (*predicted_)[indices[stop]] == (*predicted_)[indices[start]]) {
      if ((*actuals_)[indices[stop]] == 1)
        no_1s_inside++;
      stop++;
    }

    int length = stop - start;                              // Length of IRW.
    int no_0s_inside = length - no_1s_inside;               // Number of 0s in IRW
    int no_1s_above = no_1s - (no_1s_below + no_1s_inside); // Number of 1s ranked above current.
    int no_0s_above = no_0s - (no_0s_below + no_0s_inside); // Number of 0s ranked above current.

		/*
    // Number of concordant pairs an IRW 1-element is in, plus half the ties.
    double concordant_1 = no_0s_below + (0.5 * no_0s_inside);

    // Number of discordant pairs an IRW 1-element is in, plus half the ties.
    double discordant_1 = no_0s_above + (0.5 * no_0s_inside);

    // Number of concordant pairs an IRW 0-element is in, plus half the ties.
    double concordant_0 = no_1s_above + (0.5 * no_1s_inside);

    // Number of discordant pairs an IRW 0-element is in, plus half the ties.
    double discordant_0 = no_1s_below + (0.5 * no_1s_inside);
		*/

    double w = no_1s_inside * (no_0s_below + (0.5 * no_0s_inside));

    W += w;

    double q2 = no_1s_inside * ((no_0s_below * no_0s_below) + (no_0s_below * no_0s_inside) + ((1.0 / 3.0) * (no_0s_inside * no_0s_inside)));

    Q2 += q2;

    double q1 = no_0s_inside * ((no_1s_above * no_1s_above) + (no_1s_above * no_1s_inside) + ((1.0 / 3.0) * (no_1s_inside * no_1s_inside)));

    Q1 += q1;

		// Sanity check.
		if (no_1s_above + no_1s_inside + no_1s_below != no_1s) {
      error_ = true;
      return Undefined::Float();
    }

    no_1s_below += no_1s_inside;
		no_0s_below += no_0s_inside;

    start = stop;

  }

  int    no_pairs = no_1s * no_0s;
  double cindex   = W / no_pairs;
  double cindex2  = cindex * cindex;

  Q1 /= (no_pairs * no_1s);
  Q2 /= (no_pairs * no_0s);

  double n1 = cindex * (1.0 - cindex);
  double n2 = (no_1s - 1) * (Q1 - cindex2);
  double n3 = (no_0s - 1) * (Q2 - cindex2);

  se_       = static_cast(float, sqrt((n1 + n2 + n3) / no_pairs));
  cindex_   = static_cast(float, cindex);
  computed_ = true;

  return cindex_;

}

//-------------------------------------------------------------------
// Method........: GetSE
// Author........: Staal Vinterbo
// Date..........:
// Description...: Returns cached standard error. Calculates it if not
//                 done previously.
// Comments......:
// Revisions.....:
//===================================================================

float
CIndexComputer::GetSE() {

	// Return SE, if cached.
  if (computed_)
    return se_;

	// Compute AUC/SE.
  GetCIndex();

	if (!GetStatus())
		se_ = Undefined::Float();

  return se_;

}

//-------------------------------------------------------------------
// Method........: GetStatus
// Author........: Staal Vinterbo
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CIndexComputer::GetStatus() const {
	return (error_ == false);
}
 
