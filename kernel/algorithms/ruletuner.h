//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 29.01.2005
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULETUNER_H__
#define __RULETUNER_H__

#include <copyright.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/generalrule.h>
#include <kernel/structures/generalrules.h>
#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structures;

//-------------------------------------------------------------------
// Class.........: DecisionSet
// Author........: Ewa Makosa
// Description...: An aiding class for gruping rules accoring to 
//					their generalized decision
//
// Revisions.....: 
//===================================================================
class DecisionSet{
public:

	typedef STLSet(int) ISet;

	ISet decisions_;

	//bool changed_;

	bool operator==(const DecisionSet &in)const {
		// Are the two dictionaries physically equal?
		if (&in == this)
			return true;
		if (in.decisions_ != decisions_)
			return false;
		return true;
	}

	bool operator<(const DecisionSet& in)const{
		int no_dec = GetNoDecisions();
		
		// Same number of attributes?
		if (in.GetNoDecisions() != no_dec)
			return (no_dec < in.GetNoDecisions());
		
		return (ToString() < in.ToString());
	}

	int GetNoDecisions()const{
		return decisions_.size();
	}

	ISet::const_iterator Begin()const{
		return decisions_.begin();
	}

	ISet::const_iterator End()const{
		return decisions_.end();
	}

	void Insert(int i){
		decisions_.insert(i);
	}
	
	String ToString()const{
		String representation;
		ISet::const_iterator it;
		for (it = decisions_.begin(); it != decisions_.end(); it ++){
			representation += String::Format(*it);
		}
		return representation;
	}

/*	void SetChanged(){
		changed_ = true;
	}

	void SetUnchanged(){
		changed_ = false;
	}*/
};

//-------------------------------------------------------------------
// Class.........: RuleTuner
// Author........: Ewa Makosa
// Description...: A ruletuner is an algorithm that operates on a set of
//                 structures, and changes members of this set - preformes
//			       a fine tuning
//               
//
// Revisions.....: 
//===================================================================

class RuleTuner : public Algorithm {

protected:

	//- Parameters.....................................................
	float					threshold_;        // the threshold for one rule quality.
	bool                    idg_;              // Use indiscernibility definition graph (IDG)?
	String                  filename_idg_;     // IDG location, if relevant.

protected:

	float					ObtainRuleAccuracy(const Rule &rule) const;
	//- Tuning.............................................
	virtual Structure*		Tune(const Structures &structures) const = 0;
	Handle<GeneralRules>	ConvertRules(const Structures &structures) const;
	Handle<GeneralRule>		InitializeRule(const Handle<Rule> rule) const;
	Handle<GeneralRule>		InitializeRule(const Handle<GeneralRule> rule)const;
public:

	//- Constructors/destructor........................................
	RuleTuner();
	virtual ~RuleTuner();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String			GetParameters() const;
	virtual bool			SetParameter(const String &keyword, const String &value);
	virtual bool			IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure		*Apply(Structure &structure) const;

	//- Parameter management...........................................
	virtual	float			GetThreshold() const {return threshold_;}
	virtual	bool            SetThreshold(float threshold) {threshold_ = threshold; return true;}
	
	virtual bool            UseIDG() const;
	virtual bool            UseIDG(bool idg);
	
	virtual const String	&GetIDGFilename() const;
	virtual bool            SetIDGFilename(const String &filename);
};

#endif

 
