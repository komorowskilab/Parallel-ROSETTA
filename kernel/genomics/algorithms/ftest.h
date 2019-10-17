//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __FTEST_H__
#define __FTEST_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>

//-------------------------------------------------------------------
// Class.........: FTest
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs f-test
// Revisions.....:
//===================================================================

class FTest : public FeatureSelector {
protected:

	//- Parameters.....................................................
	int                attributes_;             
	float              threshold_; 
	bool							 bootstrap_;
	int								 straps_;
	float							 pthreshold_;
	int								 seed_;
	bool						   generateseed_;
	// add the appropiate stuff

public:

	//- Constructors/destructor........................................
	FTest();
	virtual ~FTest();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual	FTest	*Clone();

	//- Parameter management...........................................
	int                GetAttributes() const {return attributes_;}
	bool               SetAttributes(int attributes) {attributes_ = attributes; return true;}

	int                GetSeed() const {return seed_;}
	bool               SetSeed(int seed) {seed_ = seed; return true;}


	float              GetThreshold() const {return threshold_;}
	bool               SetThreshold(float threshold) {float f = threshold;  threshold_ = f; return true;}

	int                GetStraps() const {return straps_;}
	bool               SetStraps(int straps) {straps_ = straps; return true;}

	bool               GetGenerateSeed() const {return generateseed_;}
	bool               SetGenerateSeed(bool generateseed) {generateseed_=generateseed; return true;}


	float              GetPThreshold() const {return pthreshold_;}
	bool               SetPThreshold(float threshold) {float f = threshold ; pthreshold_=f; return true;}

	bool               GetBootStrap() const {return bootstrap_;}
	bool               SetBootStrap(bool bootstrap) {bootstrap_=bootstrap; return true;}

};

#endif
 
