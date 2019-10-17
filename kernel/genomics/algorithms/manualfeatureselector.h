//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MANUALFEATURESELECTOR_H__
#define __MANUALFEATURESELECTOR_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>

//-------------------------------------------------------------------
// Class.........: ManualFeatureSelector
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs a manual feature selection
// Revisions.....:
//===================================================================

class ManualFeatureSelector : public FeatureSelector {
protected:
	
	// Parameters......................................................
	String         attributes_; // ie "1010101" 1 = keep 0=throw away
public:
	
	//- Constructors/destructor........................................
	ManualFeatureSelector();
	virtual ~ManualFeatureSelector();
	
	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
		//- Methods inherited from Algorithm...............................
		virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	
	virtual Structure *Apply(Structure &structure) const;
	virtual	ManualFeatureSelector	*Clone();
	
	//- Local parameter methods........................................
	const String      &GetAttributes() const {return attributes_;}
	bool               SetAttributes(const String &attributes) {attributes_ = attributes; return true;}
};

#endif 
