//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ADXDECISIONTABLEIMPORTER_H__
#define __ADXDECISIONTABLEIMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/decisiontableimporter.h>

#include <kernel/structures/attribute.h>
#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: AdxDecisionTableImporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....: AÛ 0398: Decomposed.
//===================================================================

class AdxDecisionTableImporter : public DecisionTableImporter {
protected:

	//- Import methods.................................................
	virtual bool       ImportNames(ifstream &stream, Vector(String) &names, int &line_no) const;
	virtual bool       ImportTypes(ifstream &stream, Vector(Attribute::Type) &types, Vector(int) &exponents, int &line_no, int no_attributes) const;
	virtual bool       ImportData(ifstream &stream, DecisionTable &table, bool masked, int &line_no) const;
	virtual bool       InitializeTable(DecisionTable &table, bool masked, const Vector(String) &names, const Vector(Attribute::Type) &types, const Vector(int) &exponents) const;
	virtual bool       GetTokens(const String &line, Vector(String) &tokens) const;
	bool			ImportNamesAndTypes(ifstream &stream, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &exponents, int &line_no, Vector(DecisionTable::Mask) &masks) const;
public:

  //- Constructors/destructor........................................
  AdxDecisionTableImporter();
  virtual ~AdxDecisionTableImporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual Structure *Apply(Structure &structure) const;
	virtual AdxDecisionTableImporter	*Clone();

};

#endif 
