//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BOOLEANSOPFUNCTION_H__
#define __BOOLEANSOPFUNCTION_H__

#include <copyright.h>

#include <kernel/structures/booleanfunction.h>

//-------------------------------------------------------------------
// Class.........: BooleanSOPFunction
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class BooleanSOPFunction : public BooleanFunction {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<BooleanSOPFunction>) Handles;

protected:

	//- Constructors...................................................
  BooleanSOPFunction(const BooleanSOPFunction &in);

public:

  //- Constructors and destructor....................................
	BooleanSOPFunction();
  virtual ~BooleanSOPFunction();

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
	int                        GetNoProducts() const                                                    {return GetNoComponents();}

	const Bits                *GetProduct(int i) const                                                  {return GetComponent(i);}
	bool                       SetProduct(int i, const Bits *product, bool copy, float weight = 1.0)    {return SetComponent(i, product, copy, weight);}

	bool                       InsertProduct(const Bits *product, int i, bool copy, float weight = 1.0) {return InsertComponent(product, i, copy, weight);}
	bool                       AppendProduct(const Bits *product, bool copy, float weight = 1.0)        {return AppendComponent(product, copy, weight);}
	bool                       RemoveProduct(int i)                                                     {return RemoveComponent(i);}
	bool                       RemoveAllProducts()                                                      {return RemoveAllComponents();}

};

#endif 
