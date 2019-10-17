//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#ifndef __VOTER_H__
#define __VOTER_H__

#include <copyright.h>

#include <kernel/algorithms/rulebasedclassifier.h>

#include <kernel/basic/map.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class InformationVector;
class RuleBasedClassification;

//-------------------------------------------------------------------
// Class.........: Voter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Classifies a possibly incomplete information vector
//                 by some rule-based voting scheme.
// Revisions.....:
//===================================================================

class Voter : public RuleBasedClassifier {
protected:

	//- Parameters.....................................................
	float                            fraction_;               // Minimal matching fraction for determining if a rule fires.
	bool                             idg_;                    // Use IDG in matching?
	String                           filename_idg_;           // Location of IDG, if any.

protected:

	//- Convenient type definitions....................................
	typedef Map(int, int)            VoteMap;                 // Maps from decision values to vote counts.
	typedef Map(int, Rule::Handles)  RuleMap;                 // Maps from decision values to firing rules.

protected:

	//- Protected methods..............................................
	virtual bool                     EliminateRules(Vector(int) &indices, const InformationVector &inf, const Rules &rules, float fraction) const;
	virtual bool                     FillMaps(VoteMap &votemap, RuleMap &rulemap, const Vector(int) &indices, const Rules &rules) const;
	virtual bool                     FillClassification(RuleBasedClassification &classification, const VoteMap &votemap, const RuleMap &rulemap, const Rules &rules) const;

public:
	// Added for avoiding memory deletes of structure rules. -N 24/05/13
	//-Rules management.................................................
	Rules				GetRuleset() const; // -N

  //- Constructors/destructor........................................
	Voter();
  virtual ~Voter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String                   GetParameters() const;
	virtual bool                     SetParameter(const String &keyword, const String &value);
  virtual Structure               *Apply(Structure &structure) const;
  virtual Voter						*Clone() = 0;

  //- Methods inherited from Classifier..............................
	virtual bool                     SetRules(const Rules *rules);

  //- New virtual methods............................................
	virtual RuleBasedClassification *Classify(const InformationVector &inf) const;
	virtual RuleBasedClassification *Classify(const InformationVector &inf, const Vector(int) &indices) const;

  //- Parameter management...........................................
	float                            GetFraction() const {return fraction_;}
	bool                             SetFraction(float fraction) {fraction_ = fraction; return true;}

	bool                             UseIDG() const {return idg_;}
	bool                             UseIDG(bool idg) {idg_ = idg; return true;}

	const String                    &GetIDGFilename() const {return filename_idg_;}
	bool                             SetIDGFilename(const String &filename) {filename_idg_ = filename; return true;}

};

#endif

 
