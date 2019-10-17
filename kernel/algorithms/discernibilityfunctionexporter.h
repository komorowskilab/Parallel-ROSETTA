//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DISCERNIBILITYFUNCTIONEXPORTER_H__
#define __DISCERNIBILITYFUNCTIONEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/discernibilityexporter.h>
#include <kernel/algorithms/objectselector.h>

#include <kernel/structures/reduct.h>

//-------------------------------------------------------------------
// Class.........: DiscernibilityFunctionExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Computes and exports user-defined POS discernibility
//                 functions.
// Revisions.....:
//===================================================================

class DiscernibilityFunctionExporter : public DiscernibilityExporter {
protected:

	//- Implementation structures......................................
	Reduct::Discernibility discernibility_; // Type of discernibility to consider.
	ObjectSelector         selector_;       // Handles object selection.
	bool                   simplify_;       // Simplify functions?

protected:

  //- Methods inherited from DiscernibilityExporter..................
	virtual bool           ExportData(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const;

  //- New methods....................................................
	virtual bool           ExportSystemFunction(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const;
	virtual bool           ExportObjectFunctions(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const;

public:

  //- Constructors/destructor........................................
  DiscernibilityFunctionExporter();
  virtual ~DiscernibilityFunctionExporter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String         GetParameters() const;
	virtual bool           SetParameter(const String &keyword, const String &value);
	virtual DiscernibilityFunctionExporter	*Clone();

	//- Parameter management...........................................
	Reduct::Discernibility GetDiscernibilityType() const {return discernibility_;}
	bool                   SetDiscernibilityType(Reduct::Discernibility discernibility) {discernibility_ = discernibility; return true;}

	ObjectSelector        &GetObjectSelector() {return selector_;}
	const ObjectSelector  &GetObjectSelector() const {return selector_;}
	bool                   SetObjectSelector(ObjectSelector &selector) {selector_ = selector; return true;}

	bool                   Simplify() const {return simplify_;}
	bool                   Simplify(bool simplify) {simplify_ = simplify; return true;}

};

#endif 
