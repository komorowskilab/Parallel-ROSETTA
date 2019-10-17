//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 2005.05.31
// Description...: General rule is a more general representation than
//					classic decision rules. Attribute values are of 
//					form a = (v_1 OR v_2 OR...OR v_n) instead of a=v
// Revisions.....:
//===================================================================

// generalrule.h: interface for the GeneralRule class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GENERALRULE_H__
#define __GENERALRULE_H__

#include <copyright.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/generalizeddecision.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/set.h>

//-------------------------------------------------------------------
// Class.........: GeneralRule
// Author........: Ewa Makosa
// Date..........:
// Description...: A realization of a general rule. See comments for
//                 the superclass Rule wrt. class design.
// Revisions.....:
//===================================================================
	
class GeneralRule : public Rule {

public:

	typedef STLSet(int)			ValueSet;	// Represents a set of values in the general case /Ewa Makosa, 16.03.2005
	typedef Pair(int, ValueSet)	Descriptor; // Represents an (attribute, value set) pair.

private:
	
	//- Implementation structures......................................
	typedef Vector(ValueSet) Conjunction;

	Vector(int)				condition_attributes_; // Condition attributes.
	Conjunction             condition_values_;     // Condition values.
	int                     condition_support_;    // Support count for the antecedent.
	int                     decision_attribute_;   // Decision attribute.
	GeneralizedDecision     decision_;             // Generalized decision value.

protected:

	//- Copy constructor...............................................
	GeneralRule(const GeneralRule &in);

public:
	
	//- Constructors/destructor........................................	
	GeneralRule();
	virtual ~GeneralRule();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
	//- Methods inherited from Persistent..............................
	virtual bool						Load(ifstream &stream);
	virtual bool						Save(ofstream &stream) const;

	//- Methods inherited from Structure...............................
	virtual Structure					*Duplicate() const;
	virtual void						Clear();
	
	//- Methods inherited from Rule....................................
	//- Condition (LHS) management......................
	virtual int							GetNoConditionDescriptors() const;
	virtual int							GetConditionAttribute(int position_no) const;
	virtual int							GetConditionValue(int position_no) const;
	virtual bool						AppendConditionDescriptor(int attribute_no, int value);
	
	//- Condition (LHS) management:: General-specific methods..........
	virtual int							GetNoConditionValues(int position_no) const;
	virtual ValueSet::const_iterator			GetConditionValueIterator(int position_no) const;
	virtual const ValueSet&				GetConditionValues(int position_no) const;
	virtual bool						AppendConditionDescriptor(int attribute_no, const GeneralRule::ValueSet &values);
	virtual bool						AddConditionValue(int position_no, int value);

	//- Condition (LHS) management:: Shortable-specific methods........
	virtual bool						RemoveConditionDescriptor(int position_no);

	//- Decision attribute (RHS) management............................
	virtual int							GetDecisionAttribute() const;
	virtual bool						SetDecisionAttribute(int attribute_no);
	
	//- Decision value (RHS) management................................
	virtual const GeneralizedDecision	&GetGeneralizedDecision() const;	
	virtual int							GetNoDecisionValues() const;
	virtual int							GetDecisionValue(int position_no) const;
	virtual bool						AppendDecisionValue(int value, int support, double stability = 1.0);
	
	//- Quant. value management, complete rules .......................
	virtual int							GetSupport(int position_no) const;
	virtual bool						SetSupport(int position_no, int support, double stability = 1.0);
	virtual double						GetStability(int position_no) const;
	virtual int							GetSupport() const;
	virtual bool						SetSupport(int support, double stability = 1.0);
	virtual double						GetStability() const;

	//- Querying methods...............................................	
	virtual bool						Matches(const InformationVector &object, const Discerner &discerner, float fraction) const;
	
	//- Operators......................................................
	GeneralRule &						operator=(const GeneralRule &in);
	bool								operator==(const GeneralRule &in) const;
	bool								operator<(const GeneralRule &in) const;
	
	bool								AttrLHSSubsetOf(const Handle<GeneralRule> in)const;

	//- Formatting.....................................................
	virtual bool						Format(String &formatted, const DecisionTable *table = NULL, bool masked = true) const;
	virtual bool						FormatProlog(String &formatted, const DecisionTable *table = NULL, bool masked = true) const;

	//- Creation.......................................................
	virtual bool						ReserveLHS(int length);
	virtual bool						ReserveRHS(int length);
	virtual bool						Reevaluate(const Discerner &discerner, const DecisionTable &decisiontable, bool rhs);

};
#endif  
