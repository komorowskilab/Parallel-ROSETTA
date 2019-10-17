//-------------------------------------------------------------------
// Author........: Staal Vinterbo
// Date..........:
// Description...: Implements the calculation of ROC areas with
//                 standard error.
//
//                 @ARTICLE{hanley:roc82,
//                   author = "James A. Hanley and Barbara J. McNeil",
//                   title = "The meaning and use of the area under the ROC curve",
//                   journal = "Radiology",
//                   year = 1982,
//                 }
//
// Revisions.....: AÛ: Made code adhere to coding standards.
//===================================================================

#ifndef __CINDEXCOMPUTER_H__
#define __CINDEXCOMPUTER_H__

#include <copyright.h>

#include <kernel/basic/vector.h>
//06/10/14 NB -- for NULL with C++11.
#include <cstddef>
//-------------------------------------------------------------------
// Class.........: CIndexComputer
// Author........: Staal Vinterbo
// Date..........:
// Description...: Calculates the concordant pairs index.
// Comments......:
// Revisions.....:
//===================================================================

class CIndexComputer {
private:

  //- Implementation structures......................................
  bool                 computed_;  // Is the c-index computed?
  const Vector(int)   *actuals_;   // Actual outcome classes, 0 or 1.
  const Vector(float) *predicted_; // Model outputs of certainty that objects belong to class 1.
  float                cindex_;    // The concordant pairs index, cached.
  float                se_;        // The standard error of the c-index, cached.
  bool                 error_;     // Status flag.

  //- STL helper.....................................................
  struct Compare {
  private:
    const Vector(float) *v_;
    Compare() {v_ = NULL;}
  public:
    bool operator()(int i, int j) const {
      return ((*v_)[i] < (*v_)[j]);
    }
    Compare(const Vector(float) *v) {v_ = v;}
  };

public:

  //- Constructors/destructor........................................
  CIndexComputer(const Vector(int) *actuals, const Vector(float) *predicted);
  virtual ~CIndexComputer();

  //- C-index methods................................................
  float                GetCIndex();
  float                GetSE();

  bool                 GetStatus() const;

};


#endif
 
