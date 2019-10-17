//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MANUALSCALER_H__
#define __MANUALSCALER_H__

#include <copyright.h>

#include <kernel/algorithms/scaler.h>

#include <kernel/basic/interval.h>

//-------------------------------------------------------------------
// Class.........: ManualScaler
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Enables manual scaling/coarsening/discretization of
//                 a designated attribute. 
//
//                 Having defined a set of intervals with appropriate
//                 mapping values, runs through a decision table and
//                 performs the mapping.
//
// Comments......: This class should perhaps inherit from the 
//                 OrthogonalScaler class, since it operates with
//                 intervals and not hyperplanes.
// Revisions.....:
//===================================================================

class ManualScaler : public Scaler {
protected:

	//- Parameters.....................................................
	int                   attribute_;      // Index of attribute to scale (discretize).
	Vector(Interval)      intervals_;      // Values inside these intervals...
	Vector(int)           values_;         // ...map to these integers.
	bool                  use_dictionary_; // Interpret intervals via dictionary?

public:

  //- Constructors/destructor........................................
  ManualScaler();
  virtual ~ManualScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String       GetParameters() const;
	virtual bool         SetParameter(const String &keyword, const String &value);
  virtual Structure   *Apply(Structure &structure) const;
  virtual ManualScaler	*Clone();

	//- Parameter methods..............................................
	int                  GetAttribute() const {return attribute_;}
	bool                 SetAttribute(int attribute) {attribute_ = attribute; return (attribute_ >= 0);}

	int                  GetNoIntervals() const {return intervals_.size();}

  bool                 InsertInterval(const Interval &interval, int value, int i);
  bool                 AppendInterval(const Interval &interval, int value);
  bool                 RemoveInterval(int i);
	bool                 RemoveAllIntervals();

	const Interval      &GetInterval(int i) const;
	int                  GetValue(int i) const;

	bool                 SetIntervals(const String &intervals);

	bool                 UseDictionary() const {return use_dictionary_;}
	bool                 UseDictionary(bool use_dictionary) {use_dictionary_ = use_dictionary; return true;}

};

#endif 
