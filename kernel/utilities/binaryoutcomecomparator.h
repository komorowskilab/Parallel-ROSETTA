//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BINARYOUTCOMECOMPARATOR_H__
#define __BINARYOUTCOMECOMPARATOR_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/undefined.h>
#include <kernel/basic/string.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: BinaryOutcomeComparator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for doing statistical hypothesis
//                 testing on classifiers with binary outcomes.
// Comments......:
// Revisions.....:
//===================================================================

class BinaryOutcomeComparator {
protected:

	//- Member variables...............................................
	Vector(Vector(int))   targets_;  // Actual outcomes (0 or 1).
	Vector(Vector(float)) outputs_;  // Classifiers' certainty that outcome is 1.

protected:

	//- Constructors...................................................
	BinaryOutcomeComparator();

	//- Helper methods.................................................
  virtual bool          LoadPairs(Vector(int) &targets, Vector(float) &outputs, const String &filename, bool swapped = false) const;

public:

	//- Constructors/destructor........................................
	BinaryOutcomeComparator(const Vector(String) &filenames, const Vector(bool) &swapped);
	virtual ~BinaryOutcomeComparator();

};

#endif 
