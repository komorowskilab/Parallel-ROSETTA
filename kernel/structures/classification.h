//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CLASSIFICATION_H__
#define __CLASSIFICATION_H__

#include <copyright.h>

#include <kernel/structures/structure.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: Classification
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Structure returned by (single object) classifiers.
//                 A classification result can be seen as a list of
//                 possible decisions, along with some extra information,
//                 e.g. a measure of evidence/certainty associated with
//                 each decision.
// Comments......: Note that indices to such stuff as extra information
//                 are indices into the set of possible decision values,
//                 and not the decision values themselves.  For instance,
//                 if there are three possible decision values, namely
//                 {4, 12, 56}, the relevant indices would be {0, 1, 2}
//                 (indexing 4, 12, and 56 respectively), and not
//                 {4, 12, 56} themselves.
// Revisions.....:
//===================================================================

class Classification : public Structure {
protected:

	//- Protected member variables.....................................
	int                decision_attribute_; // Index of decision attribute.
	Vector(int)        decisions_;          // Possible decisions.
	Vector(float)      certainties_;        // Certainty coefficients (abstract notion) for each decision.

protected:

  //- Constructors...................................................
//  Classification(const Classification &in);

	//- Protected methods..............................................
	virtual void       SwapData(int i, int j);

public:

  //- Constructors/destructor........................................
  Classification();
  virtual ~Classification();

	// -N 11/06/13 Moved to public for use in batchclassifier.cpp
	//- Constructors...................................................
	Classification(const Classification &in);

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual void       Clear();
	virtual Structure *Duplicate() const;

  //- Decision attribute management..................................
	int                GetDecisionAttribute() const;
	bool               SetDecisionAttribute(int decision_attribute);

  //- Decision value management......................................
	int                GetNoDecisionValues() const;
	int                GetDecisionValue(int i) const;

	virtual bool       InsertDecisionValue(int value, float certainty, int i);
	virtual bool       AppendDecisionValue(int value, float certainty);
	virtual bool       RemoveDecisionValue(int i);

  //- Certainty coefficient management...............................
	float              GetCertaintyCoefficient(int i) const;
	bool               SetCertaintyCoefficient(float certainty, int i);

	float              GetBinaryOutcomeCoefficient(int decision_class) const;

	bool               Normalize();
	bool               Sort();

  //- For interface uniformity.......................................
	virtual int        GetNoRules(int i) const;

};

#endif
 
