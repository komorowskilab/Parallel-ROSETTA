//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYREDUCTSHORTENER_H__
#define __MYREDUCTSHORTENER_H__

#include <copyright.h>

#include <kernel/algorithms/reductshortener.h>

//-------------------------------------------------------------------
// Class.........: MyReductShortener
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Shortens each reduct in a set of reducts according to
//                 various criteria.  By shortening is meant removing one
//                 or more attributes from a reduct.
//
// Comments......:
// Revisions.....: AÛ 0797 - Allow compund criteria.
//===================================================================

class MyReductShortener : public ReductShortener {
public:

	//- Enumerated types...............................................
	enum Shortening {SHORTENING_PERCENTAGE = 1, SHORTENING_OCCURRENCE = 2, SHORTENING_COMBINEDOCCURRENCE = 4};

	//- Textual representations........................................
	static String                  GetString(MyReductShortener::Shortening shortening);

protected:

  //- Parameters.....................................................
	int                            shortening_;          // Shortening strategy/criterion.
	float                          lower_percentage_;    // Remove attribute if occurrence percentage not above this threshold...
	float                          upper_percentage_;    // ...and not below this threshold.
	int                            attribute_;           // Remove this attribute.
	int                            primary_attribute_;   // Remove this attribute...
	int                            secondary_attribute_; // ...if it occurs together with this one.

public:

  //- Constructors/destructor........................................
  MyReductShortener();
  virtual ~MyReductShortener();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String                 GetParameters() const;
	virtual bool                   SetParameter(const String &keyword, const String &value);
	virtual Structure             *Apply(Structure &structure) const;
	virtual MyReductShortener		*Clone();

	//- Parameter management...........................................
	int                            GetShorteningStrategy() const {return shortening_;}
	bool                           SetShorteningStrategy(int shortening) {shortening_ = shortening; return true;}

	float                          GetLowerPercentage() const {return lower_percentage_;}
	bool                           SetLowerPercentage(float lower_percentage) {lower_percentage_ = lower_percentage; return true;}

	float                          GetUpperPercentage() const {return upper_percentage_;}
	bool                           SetUpperPercentage(float upper_percentage) {upper_percentage_ = upper_percentage; return true;}

	int                            GetAttribute() const {return attribute_;}
	bool                           SetAttribute(int attribute) {attribute_ = attribute; return true;}

	int                            GetPrimaryAttribute() const {return primary_attribute_;}
	bool                           SetPrimaryAttribute(int primary_attribute) {primary_attribute_ = primary_attribute; return true;}

	int                            GetSecondaryAttribute() const {return secondary_attribute_;}
	bool                           SetSecondaryAttribute(int secondary_attribute) {secondary_attribute_ = secondary_attribute; return true;}

};

#endif 
