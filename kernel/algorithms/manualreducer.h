//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MANUALREDUCER_H__
#define __MANUALREDUCER_H__

#include <copyright.h>

#include <kernel/algorithms/reducer.h>

//-------------------------------------------------------------------
// Class.........: ManualReducer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: An algorithmic shell around Creator::Reduct.
// Revisions.....:
//===================================================================

class ManualReducer : public Reducer {
protected:

	// Parameters......................................................
  String             attributes_; // Attribute subset, e.g., "{bird, penguin}" or "{0, 3}".

public:

  //- Constructors/destructor........................................
  ManualReducer();
  virtual ~ManualReducer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
  virtual Structure *Apply(Structure &structure) const;
  virtual ManualReducer *Clone();

	//- Local parameter methods........................................
  const String      &GetAttributes() const {return attributes_;}
	bool               SetAttributes(const String &attributes) {attributes_ = attributes; return true;}

};

#endif 
