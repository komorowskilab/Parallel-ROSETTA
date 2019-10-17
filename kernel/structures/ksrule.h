//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KSRULE_H__
#define __KSRULE_H__

#include <copyright.h>

#include <kernel/structures/rule.h>
#include <kernel/structures/generalizeddecision.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: KSRule
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A simple realization of a rule. See comments for
//                 the superclass Rule wrt. class design.
// Revisions.....:
//===================================================================

class KSRule : public Rule {
private:

  //- Implementation structures......................................
  Vector(int)                        condition_attributes_; // Condition attributes.
  Vector(int)                        condition_values_;     // Condition values.
	int                                condition_support_;    // Support count for the antecedent.
	int                                decision_attribute_;   // Decision attribute.
	GeneralizedDecision                decision_;             // Generalized decision value.

protected:

  //- Copy constructor...............................................
  KSRule(const KSRule &in);

public:

  //- Constructors/destructor........................................
	KSRule();
  virtual ~KSRule();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual Structure                 *Duplicate() const;
	virtual void                       Clear();

  //- Methods inherited from Rule....................................
  virtual int                        GetNoConditionDescriptors() const;

  virtual int                        GetConditionAttribute(int position_no) const;
  virtual int                        GetConditionValue(int position_no) const;

  virtual bool                       AppendConditionDescriptor(int attribute_no, int value);

	virtual int                        GetDecisionAttribute() const;
	virtual bool                       SetDecisionAttribute(int attribute_no);

	virtual const GeneralizedDecision &GetGeneralizedDecision() const;

	virtual int                        GetNoDecisionValues() const;
  virtual int                        GetDecisionValue(int position_no) const;
  virtual bool                       AppendDecisionValue(int value, int support, double stability = 1.0);

	virtual int                        GetSupport(int position_no) const;
	virtual bool                       SetSupport(int position_no, int support, double stability = 1.0);

	virtual double                     GetStability(int position_no) const;

	virtual int                        GetSupport() const;
	virtual bool                       SetSupport(int support, double stability = 1.0);
	virtual double                     GetStability() const;

	virtual bool                       ReserveLHS(int length);
	virtual bool                       ReserveRHS(int length);

};

#endif
 
