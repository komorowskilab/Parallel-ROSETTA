//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PRINCIPALCOMPONENTANALYSIS_H__
#define __PRINCIPALCOMPONENTANALYSIS_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>
#include <kernel/genomics/utilities/statkit.h>
#include <kernel/genomics/utilities/nrutil.h>

//-------------------------------------------------------------------
// Class.........: PrincipalComponentAnalysis
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs a principal component analysis on a decision table.
// Revisions.....:
//===================================================================

class PrincipalComponentAnalysis : public FeatureSelector {
protected:

	// Parameters......................................................
	int        attributes_; // Attribute subset, e.g., "{bird, penguin}" or "{0, 3}".
	bool			 precalculated_;
	
 
	//	DecisionTable * subtable1;
public:

	//- Constructors/destructor........................................
	PrincipalComponentAnalysis();
	virtual ~PrincipalComponentAnalysis();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	   virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual	PrincipalComponentAnalysis	*Clone();
	
//	virtual bool			 InitFeatureSelectionTable(DecisionTable &table, DecisionTable &featuretable, Vector(bool) &removes, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &scales) const;
	//Vector (float) eigenvalues;
	virtual Structure *Apply(Structure &structure) const;

	//- Local parameter methods........................................
	int       GetAttributes() const {return attributes_;}
	bool      SetAttributes(int attributes) {attributes_ = attributes; return true;}
	bool       GetPrecalculated() const {return precalculated_;}
	bool      SetPrecalculated(bool precalculated)  {precalculated_=precalculated; return true;} 

	bool		MakeAnalysis(DecisionTable &hej, DecisionTable &pcatable, int size) const;
	bool Transform(DecisionTable &pcaTable, DecisionTable &dataTable, DecisionTable &newTable, int components) const;
	bool PreCalculate(Structure &strucure, Vector (float) &eigenvalues) ;
};

#endif
 
