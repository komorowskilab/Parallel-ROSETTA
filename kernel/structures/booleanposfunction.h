//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BOOLEANPOSFUNCTION_H__
#define __BOOLEANPOSFUNCTION_H__

#include <copyright.h>

#include <kernel/structures/booleanfunction.h>

//-------------------------------------------------------------------
// Class.........: BooleanPOSFunction
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class BooleanPOSFunction : public BooleanFunction {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<BooleanPOSFunction>) Handles;

protected:

	//- Constructors...................................................
  BooleanPOSFunction(const BooleanPOSFunction &in);

public:

  //- Constructors and destructor....................................
	BooleanPOSFunction();
  virtual ~BooleanPOSFunction();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure..............................
  virtual Structure         *Duplicate() const;

	//- Methods inherited from BooleanFunction .......................
	virtual bool               Create(const String &function, const DecisionTable &table, bool masked);
	virtual bool               Create(const String &function, Map(String, int) &names);

	virtual bool               Format(String &formatted, const DecisionTable *table = NULL, bool masked = true) const;
	virtual String             Format(const DecisionTable *table = NULL, bool masked = true) const;

	virtual bool               Load(ifstream &stream, const DecisionTable *table = NULL, bool masked = true);
	virtual bool               Save(ofstream &stream, const DecisionTable *table = NULL, bool masked = true) const;

	//- Synonym methods................................................
	int                        GetNoSums() const                                                {return GetNoComponents();}

	const Bits                *GetSum(int i) const                                              {return GetComponent(i);}
	bool                       SetSum(int i, const Bits *sum, bool copy, float weight = 1.0)    {return SetComponent(i, sum, copy, weight);}

	bool                       InsertSum(const Bits *sum, int i, bool copy, float weight = 1.0) {return InsertComponent(sum, i, copy, weight);}
	bool                       AppendSum(const Bits *sum, bool copy, float weight = 1.0)        {return AppendComponent(sum, copy, weight);}
	bool                       RemoveSum(int i)                                                 {return RemoveComponent(i);}
	bool                       RemoveAllSums()                                                  {return RemoveAllComponents();}

};

#endif 
