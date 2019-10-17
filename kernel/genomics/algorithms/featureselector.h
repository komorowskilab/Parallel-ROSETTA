//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __FEATURESELECTOR_H__
#define __FEATURESELECTOR_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/structures/decisiontable.h>
//-------------------------------------------------------------------
// Class.........: FeatureSelector
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Parent class for feature selection algorithms
// Revisions.....:
//===================================================================

class FeatureSelector : public Algorithm {
protected:
	
	//- Parameters.....................................................
	bool		append_; // Append resulting structures as children of parent structure?
	String		savefeatureselectionfilename_;
	bool		savefeatureselection_;
public:
	
	//- Constructors/destructor........................................
	FeatureSelector();
	virtual ~FeatureSelector();
	
	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
		//- Methods inherited from Algorithm...............................
		virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual bool   IsApplicable(const Structure &structure, bool warn = true) const;
	virtual bool   InitFeatureSelectionTable(DecisionTable &table, DecisionTable &featuretable, Vector(bool) &removes, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &scales) const;
	
	
	//- Parameter management...........................................
	bool           AppendAsChildren() const {return append_;}
	bool           AppendAsChildren(bool append) {append_ = append; return true;}
	bool           GetSaveFeatureSelection() const {return savefeatureselection_;}
	bool           SetSaveFeatureSelection(bool savefeatureselection) {savefeatureselection_ = savefeatureselection; return true;}
	bool           SetSaveFeatureSelectionFilename(const String &savefeatureselectionfilename) {savefeatureselectionfilename_ = savefeatureselectionfilename; return true;}
	const String   &GetSaveFeatureSelectionFilename() const {return savefeatureselectionfilename_;}
	
	
};

#endif 
