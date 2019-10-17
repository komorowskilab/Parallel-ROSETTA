//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __GENERALRULES_H__
#define __GENERALRULES_H__

#include <copyright.h>

#include <kernel/structures/generalrule.h>
#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Discerner;
class InformationVector;

//-------------------------------------------------------------------
// Class.........: GeneralRules
// Author........: Ewa Makosa
// Date..........:
// Description...: A set of general rules.
// Revisions.....:
//===================================================================

class GeneralRules : public Rules {

protected:

  //- Constructor....................................................
  GeneralRules(const GeneralRules &in);

protected:

	//- Type definitions...............................................
	typedef Map(GeneralRule::Descriptor, int) DIMap;         // Maps descriptors to integers.

public:

	//- Type definitions...............................................
	typedef Map(GeneralRule::Descriptor, Vector(int)) DVMap; // Maps descriptors to integer vectors.

public:

	//- Constructors/destructor........................................
	GeneralRules();
	virtual ~GeneralRules();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Structure...............................
	virtual Structure *Duplicate() const;
	virtual int        FindMember(const Structure *member, bool physical) const;

	//- Virtual methods............................................
	virtual bool       Create(const Reduct &reduct, const DecisionTable &table, const Discerner &discerner, bool append);
	virtual bool       Create(const Reducts &reducts, const DecisionTable &table, const Discerner &discerner, bool append);

	//- Local methods..................................................
	bool               GetDescriptorSupports(bool rhs, DVMap &descriptors) const;
	int                GetNoDescriptors(bool rhs, int &no_unique) const;
	GeneralRule        *GetRule(int i) const;

};

inline GeneralRule *
GeneralRules::GetRule(int i) const {

	return dynamic_cast(GeneralRule *, GetStructure(i));
}
#endif

 
