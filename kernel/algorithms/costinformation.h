//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __COSTINFORMATION_H__
#define __COSTINFORMATION_H__

#include <copyright.h>

#include <kernel/basic/vector.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Reduct;
class String;

//-------------------------------------------------------------------
// Class.........: CostInformation
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class that keeps information about
//                 attribute costs.
//
// Comments......: To do: Add support for shared costs.
// Revisions.....:
//===================================================================

class CostInformation {
protected:

  //- Member variables..............................................
	Vector(float)         costs_;

protected:

	//- Constructors...................................................
  CostInformation(const CostInformation &in);

public:

  //- Constructor and destructor....................................
	CostInformation();
  virtual ~CostInformation();

  //- Local methods.................................................
  virtual bool          Load(ifstream &stream, const DecisionTable &table, float defaultcost);
  virtual bool          Load(const String &filename, const DecisionTable &table, float defaultcost);

	const Vector(float)  &GetCosts() const {return costs_;}

	virtual float         GetCost(int attribute) const;
	virtual float         GetCost(const Vector(int) &attributes) const;
	virtual float         GetCost(const Reduct &attributes) const;

};

#endif 
