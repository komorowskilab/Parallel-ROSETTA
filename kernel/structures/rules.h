//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULES_H__
#define __RULES_H__

#include <copyright.h>

#include <kernel/structures/structures.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/utilities/iokit.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Discerner;
class InformationVector;
class Reduct;
class Reducts;
class RuleEvaluator;

//-------------------------------------------------------------------
// Class.........: Rules
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A set of rules.
// Revisions.....:
//===================================================================

class Rules : public Structures {
private:

	//- STL helpers, comparators.......................................
	struct Compare {
		bool operator()(const Handle<Structure> &a, const Handle<Structure> &b) const;
	};

protected:

  //- Constructor....................................................
	//Added as a non-protected copy constructor. -N
//  Rules(const Rules &in);

protected:

	//- Type definitions...............................................
	typedef Map(Rule::Descriptor, int) DIMap;         // Maps descriptors to integers.

public:

	//- Type definitions...............................................
	typedef Map(Rule::Descriptor, Vector(int)) DVMap; // Maps descriptors to integer vectors.

public:

  //- Constructors/destructor........................................
  Rules();
  virtual ~Rules();
  //- Constructor -N 20/05/13.........................................
	//No longer protected. -N
	Rules(const Rules &in);

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual Structure *Duplicate() const;
	virtual int        FindMember(const Structure *member, bool physical) const;

  //- Methods inherited from Structures..............................
  virtual bool       InsertStructure(Structure *structure, int i);

  //- New virtual methods............................................
	virtual bool       Sort(int property, bool ascending, const Handle<DecisionTable> table = NULL);
	virtual bool       Evaluate(const RuleEvaluator &evaluator, Vector(float) &qualities) const;
	virtual bool       Matches(const InformationVector &object, const Discerner &discerner, float fraction, Vector(int) &indices) const;

	virtual bool       Create(const Reduct &reduct, const DecisionTable &table, const Discerner &discerner, bool append);
	virtual bool       Create(const Reducts &reducts, const DecisionTable &table, const Discerner &discerner, bool append);
	// -N 06/08/13 for Classifier.Load
	virtual bool	LoadRulesetFromFile(const String sourcefile, Handle<DecisionTable> table, bool masked);

	//- Local methods..................................................
	int                GetNoRules() const;
	Rule              *GetRule(int i) const;

	bool               HasRHS(bool all) const;
	bool               GetDescriptorSupports(bool rhs, DVMap &descriptors) const;
	int                GetNoDescriptors(bool rhs, int &no_unique) const;

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

inline int
Rules::GetNoRules() const {
	return GetNoStructures();
}

inline Rule *
Rules::GetRule(int i) const {
	
	Structure* str;
	Rule* rule;
	str = GetStructure(i);
	rule = dynamic_cast(Rule *, str);
	return rule;
//	return dynamic_cast(Rule*, GetStructure(i));
}
//-------------------------------------------------------------------
// Method........: HasRHS
// Author........:
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

inline bool
Rules::HasRHS(bool all) const {

	int i, no_rules = GetNoRules();

	bool exists = false;

	for (i = 0; i < no_rules; i++) {
		if (GetRule(i)->HasRHS()) {
			exists = true;
			if (!all)
				return true;
		}
		else {
			if (all)
				return false;
		}
	}

	return exists;

}

#endif

 
