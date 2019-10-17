//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PARTITIONER_H__
#define __PARTITIONER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: Partitioner
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Partitions a decision table, i.e. determines and
//                 returns its equivalence classes.
// Revisions.....:
//===================================================================

class Partitioner : public Algorithm {
protected:

	//- Parameters.....................................................
	String             attributes_; // Attribute subset to partition by, e.g., "{bird, penguin}" or "{0, 3}".

public:

  //- Constructors/destructor........................................
  Partitioner();
  virtual ~Partitioner();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure *Apply(Structure &structure) const;
	virtual Partitioner	*Clone();

	//- Parameter management...........................................
	const String      &GetAttributes() const {return attributes_;}
	bool               SetAttributes(const String &attributes) {attributes_ = attributes; return true;}

};

#endif 
