//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __APPLICATOR_H__
#define __APPLICATOR_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

#include <kernel/structures/structure.h>

//-------------------------------------------------------------------
// Class.........: Applicator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Applies an algorithm to a structure, using exceptions.
// Revisions.....:
//===================================================================

class Applicator {
protected:

	//- The objects....................................................
	Handle<Algorithm> algorithm_;
	Handle<Structure> structure_;

	//- Constructor....................................................
	Applicator();

public:

	//- Constructor....................................................
	Applicator(Algorithm *algorithm, Structure *structure);

	//- Application....................................................
  Structure *Apply();

};

//-------------------------------------------------------------------
// Methods for class Applicator (inlined).
//===================================================================

//-------------------------------------------------------------------
// Method........: Empty constructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: Made protected in order to render it inaccessible.
// Revisions.....:
//===================================================================

inline
Applicator::Applicator() {
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline
Applicator::Applicator(Algorithm *algorithm, Structure *structure) {
	algorithm_ = algorithm;
	structure_ = structure;
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Applies the algorithm to the structure.  If the
//                 application fails, an exception is thrown.
// Comments......:
// Revisions.....:
//===================================================================

inline Structure *
Applicator::Apply() {

	if (algorithm_ == NULL)
		throw "Algorithm is NULL.";

	if (structure_ == NULL)
		throw "Structure is NULL.";

	Handle<Structure> result = structure_->Apply(*algorithm_);

	if (result == NULL)
		throw "Application failed.";

	return result.Release();

}

#endif
 
