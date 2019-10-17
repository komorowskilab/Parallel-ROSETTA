//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 9608/9609
// Description...:
// Revisions.....:
//===================================================================

#ifndef __STANDARDVOTER_H__
#define __STANDARDVOTER_H__

#include <copyright.h>

#include <kernel/algorithms/voter.h>

//-------------------------------------------------------------------
// Class.........: StandardVoter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Classifies a possibly incomplete information vector
//                 by holding an "election" among the rules that fire.
// Revisions.....:
//===================================================================

class StandardVoter : public Voter {
public:

	//- Enumerated types...............................................
	enum Voting {VOTING_SUPPORT, VOTING_SIMPLE};
	enum Normalization {NORMALIZATION_FIRING, NORMALIZATION_ALL};

	//- Textual representations........................................
	static String                    GetString(Voting voting);
	static String                    GetString(Normalization normalization);

protected:

	//- Parameters.....................................................
	Voting                           voting_;                  // Strategy for deciding how many votes a rule gets to cast.
	Normalization                    normalization_;           // Strategy for normalizing the voting result.
	bool                             exclude_generalizations_; // If one rule is a generalization of another rule, let only the most specific rule fire.

protected:

  //- Methods inherited from Voter...................................
	virtual bool                     EliminateRules(Vector(int) &indices, const InformationVector &inf, const Rules &rules, float fraction) const;
	virtual bool                     FillMaps(VoteMap &votemap, RuleMap &rulemap, const Vector(int) &indices, const Rules &rules) const;
	virtual bool                     FillClassification(RuleBasedClassification &classification, const VoteMap &votemap, const RuleMap &rulemap, const Rules &rules) const;

	//- New virtual methods.............................................
	virtual bool                     ExcludeGeneralizations(Vector(int) &indices, const Rules &rules) const;
	virtual int                      GetNoVotes(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
	StandardVoter();
  virtual ~StandardVoter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String                   GetParameters() const;
	virtual bool                     SetParameter(const String &keyword, const String &value);
	virtual StandardVoter				*Clone();

  //- Parameter management...........................................
	Voting                           GetVotingStrategy() const {return voting_;}
	bool                             SetVotingStrategy(Voting voting) {voting_ = voting; return true;}

	Normalization                    GetNormalizationStrategy() const {return normalization_;}
	bool                             SetNormalizationStrategy(Normalization normalization) {normalization_ = normalization; return true;}

	bool                             ExcludeGeneralizations() const {return exclude_generalizations_;}
	bool                             ExcludeGeneralizations(bool exclude) {exclude_generalizations_ = exclude; return true;}

};

#endif

 
