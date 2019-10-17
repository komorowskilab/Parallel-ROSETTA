//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __LOADFEATURESELECTION_H__
#define __LOADFEATURESELECTION_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>

//-------------------------------------------------------------------
// Class.........: LoadFeatureSelection
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Loads a feature selection from a file. (on a decision table)
// Revisions.....:
//===================================================================

class LoadFeatureSelection : public FeatureSelector {
protected:
	
	// Parameters......................................................
	String				 filename_;
public:
	
	//- Constructors/destructor........................................
	LoadFeatureSelection();
	virtual ~LoadFeatureSelection();
	
	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
		//- Methods inherited from Algorithm...............................
		virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual LoadFeatureSelection	*Clone();
	
	//- Local parameter methods........................................
	
	const String      &GetFilename() const {return filename_;}
	bool               SetFilename(const String &filename) {filename_ = filename; return true;}
	
	
};

#endif 
