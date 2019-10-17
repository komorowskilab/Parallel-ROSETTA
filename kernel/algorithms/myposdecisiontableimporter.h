//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYPOSDECISIONTABLEIMPORTER_H__
#define __MYPOSDECISIONTABLEIMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableimporter.h>

#include <kernel/basic/map.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class DiscernibilityFunction;

//-------------------------------------------------------------------
// Class.........: MyPOSDecisionTableImporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Constructs a binary decision table from a
//                 description of a Boolean POS discernibility
//                 function.
// Revisions.....:
//===================================================================

class MyPOSDecisionTableImporter : public DecisionTableImporter {
protected:

	//- Import methods.................................................
	virtual bool       BuildDecisionTable(DecisionTable &table, const DiscernibilityFunction &function) const;
	virtual bool       BuildDictionary(DecisionTable &table, const Map(String, int) &names) const;

public:

  //- Constructors/destructor........................................
  MyPOSDecisionTableImporter();
  virtual ~MyPOSDecisionTableImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual Structure *Apply(Structure &structure) const;
	virtual MyPOSDecisionTableImporter	*Clone();

  //- New methods....................................................
	virtual bool       Parse(ifstream &stream, DecisionTable &table) const;
	virtual bool       Parse(const String &function, DecisionTable &table) const;

};

#endif 
