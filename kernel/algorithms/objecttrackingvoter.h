//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 9901
// Description...:
// Revisions.....:
//===================================================================

#ifndef __OBJECTTRACKINGVOTER_H__
#define __OBJECTTRACKINGVOTER_H__

#include <copyright.h>

#include <kernel/algorithms/voter.h>

#include <kernel/basic/bits.h>

//-------------------------------------------------------------------
// Class.........: ObjectTrackingVoter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: For each rule that fires, a trace is made back to
//                 which object(s) the rule was derived from. The
//                 set union of these tracked objects is computed,
//                 and the final voting is done on the basis of this.
//                 In effect, this means that the originating objects
//                 do the voting (with one vote each), and the set of
//                 objects that get to vote is defined through the set
//                 of rules that fire.
// Revisions.....:
//===================================================================

class ObjectTrackingVoter : public Voter {
protected:

	//- Member variables...............................................
	Vector(Bits *)       tracked_; // Mutable. For each rule, indicates a track of the objects in its support base.

protected:

  //- Methods inherited from Voter...................................
	virtual bool         FillMaps(VoteMap &votemap, RuleMap &rulemap, const Vector(int) &indices, const Rules &rules) const;
	virtual bool         FillClassification(RuleBasedClassification &classification, const VoteMap &votemap, const RuleMap &rulemap, const Rules &rules) const;

public:

  //- Constructors/destructor........................................
	ObjectTrackingVoter();
  virtual ~ObjectTrackingVoter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Method inherited from Classifier...............................
	virtual bool         Initialize(const DecisionTable &table, bool masked);
	virtual bool         Reset();

	//- Methods inherited from Algorithms........................
	virtual ObjectTrackingVoter		*Clone();

};

#endif

 
