//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __TTEST_H__
#define __TTEST_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>

//-------------------------------------------------------------------
// Class.........: TTest
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs a student's t test on a decision table
// Revisions.....:
//===================================================================

class TTest : public FeatureSelector {
protected:

	//- Parameters.....................................................
	int                attributes_;             // Seed to the RNG.
	float              threshold_; // Normalised splitting percentage.
	bool							 bootstrap_;
	int								 straps_;
	float							 pthreshold_;
	int								 seed_;
	bool						   generateseed_;
	// add the appropiate stuff

public:

	//- Constructors/destructor........................................
	TTest();
	virtual ~TTest();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual	TTest	*Clone();

	//- Parameter management...........................................
	int                GetAttributes() const {return attributes_;}
	bool               SetAttributes(int attributes) {attributes_ = attributes; return true;}

	float              GetThreshold() const {return threshold_;}
	bool               SetThreshold(float threshold) {float f = threshold; threshold_ = f; return true;}

	int                GetStraps() const {return straps_;}
	bool               SetStraps(int straps) {straps_ = straps; return true;}

	int                GetSeed() const {return seed_;}
	virtual bool               SetSeed(int seed) {seed_ = seed; return true;}

	float              GetPThreshold() const {return pthreshold_;}
	bool               SetPThreshold(float threshold) {float f = threshold;  pthreshold_=f; return true;}

	virtual bool               GetGenerateSeed() const {return generateseed_;}
	virtual bool               SetGenerateSeed(bool generateseed) {generateseed_=generateseed; return true;}



	bool               GetBootStrap() const {return bootstrap_;}
	bool               SetBootStrap(bool bootstrap) {bootstrap_=bootstrap; return true;}

	// add our get and set functions
};

#endif
 
