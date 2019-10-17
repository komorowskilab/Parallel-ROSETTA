//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DECISIONTABLES_H__
#define __DECISIONTABLES_H__

#include <copyright.h>

#include <kernel/structures/structures.h>
#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class.........: DecisionTables
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A set of decision tables.
// Revisions.....:
//===================================================================

class DecisionTables : public Structures {
protected:

  //- Constructor....................................................
  DecisionTables(const DecisionTables &in);

public:

  //- Constructors/destructor........................................
  DecisionTables();
  virtual ~DecisionTables();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual Structure *Duplicate() const;

  //- Methods inherited from Structures..............................
  virtual bool       InsertStructure(Structure *structure, int i);

	//- Local methods..................................................
	int                GetNoDecisionTables() const;
	DecisionTable     *GetDecisionTable(int i) const;

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

inline int
DecisionTables::GetNoDecisionTables() const {
	return GetNoStructures();
}

inline DecisionTable *
DecisionTables::GetDecisionTable(int i) const {
	return dynamic_cast(DecisionTable *, GetStructure(i));
}

#endif

 
