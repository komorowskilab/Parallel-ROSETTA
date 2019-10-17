//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYREDUCTFILTER_H__
#define __MYREDUCTFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductfilter.h>

//-------------------------------------------------------------------
// Class.........: MyReductFilter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Filters away reducts based on a compound
//                 criterion of length, support and content.
// Comments......:
// Revisions.....:
//===================================================================

class MyReductFilter : public ReductFilter {
public:

	//- Enumerated types...............................................
	enum Filtering {FILTERING_LENGTH = 1, FILTERING_SUPPORT = 2, FILTERING_ATTRIBUTE = 4};
	enum Connective {CONNECTIVE_OR, CONNECTIVE_AND};

	//- Textual representations........................................
	static String             GetString(Filtering filtering);
	static String             GetString(Connective connective);

protected:

  //- Parameters.....................................................
	int                       filtering_;                // Pruning strategy/criterions.
	Connective                connective_;               // Connective between terms.

	int                       lower_length_threshold_;   // Remove reduct if length not above this threshold and...
	int                       upper_length_threshold_;   //                  ...not below this threshold.

	int                       lower_support_threshold_;  // Remove reduct if support not above this threshold and..
	int                       upper_support_threshold_;  //                  ...not below this threshold.

	int                       attribute_;                // Remove reduct if reduct contains this attribute .

protected:

	//- Methods inherited from Filter..................................
	virtual bool              Remove(const Structures &structures, int i) const;

public:

  //- Constructors/destructor........................................
  MyReductFilter();
  virtual ~MyReductFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String            GetParameters() const;
	virtual bool              SetParameter(const String &keyword, const String &value);
	virtual MyReductFilter	*Clone();

	//- Parameter management...........................................
	int                       GetFilteringStrategy() const {return filtering_;}
	bool                      SetFilteringStrategy(int filtering) {filtering_ = filtering; return true;}

	Connective                GetFilteringConnective() const {return connective_;}
	bool                      SetFilteringConnective(Connective connective) {connective_ = connective; return true;}

	int                       GetLowerLengthThreshold() const {return lower_length_threshold_;}
	bool                      SetLowerLengthThreshold(int lower_length_threshold) {lower_length_threshold_ = lower_length_threshold; return true;}

	int                       GetUpperLengthThreshold() const {return upper_length_threshold_;}
	bool                      SetUpperLengthThreshold(int upper_length_threshold) {upper_length_threshold_ = upper_length_threshold; return true;}

	int                       GetLowerSupportThreshold() const {return lower_support_threshold_;}
	bool                      SetLowerSupportThreshold(int lower_support_threshold) {lower_support_threshold_ = lower_support_threshold; return true;}

	int                       GetUpperSupportThreshold() const {return upper_support_threshold_;}
	bool                      SetUpperSupportThreshold(int upper_support_threshold) {upper_support_threshold_ = upper_support_threshold; return true;}

	int                       GetAttribute() const {return attribute_;}
	bool                      SetAttribute(int attribute) {attribute_ = attribute; return true;}

};

#endif 
