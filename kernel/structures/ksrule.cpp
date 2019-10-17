//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/ksrule.h>

//-------------------------------------------------------------------
// Methods for class KSRule.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSRule::KSRule(const KSRule &in) : Rule(in) {
  condition_attributes_ = in.condition_attributes_;
  condition_values_     = in.condition_values_;
	condition_support_    = in.condition_support_;
	decision_attribute_   = in.decision_attribute_;
	decision_             = in.decision_;
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSRule::KSRule() {
	decision_attribute_ = Undefined::Integer();
	condition_support_  = 0;
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSRule::~KSRule() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(KSRule, KSRULE, Rule)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//------------------------------------------------------------------
// Method........: Duplicate
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
KSRule::Duplicate() const {
	return new KSRule(*this);
}

//------------------------------------------------------------------
// Method........: Clear
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
KSRule::Clear() {

	if (!condition_attributes_.empty())
		condition_attributes_.erase(condition_attributes_.begin(), condition_attributes_.end());
	if (!condition_values_.empty())
		condition_values_.erase(condition_values_.begin(), condition_values_.end());

	decision_.Clear();

	decision_attribute_ = Undefined::Integer();
	condition_support_  = 0;

}

//-------------------------------------------------------------------
// Methods inherited from Rule.
//===================================================================

//------------------------------------------------------------------
// Method........: GetNoConditionDescriptors
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetNoConditionDescriptors() const {
	return condition_attributes_.size();
}

//------------------------------------------------------------------
// Method........: GetConditionAttribute
// Author........:
// Date..........:
// Description...: Returns the specified attribute in the parent
//                 reduct.
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetConditionAttribute(int position_no) const {

#ifdef _DEBUG
	// Is the index argument legal?
	if ((position_no < 0) || (position_no >= GetNoConditionDescriptors())) {
		Message::Error("Rule position index out of range.");
		return Undefined::Integer();
	}
#endif

	return condition_attributes_[position_no];

}

//------------------------------------------------------------------
// Method........: GetConditionValue
// Author........:
// Date..........:
// Description...: Returns the specified condition value.
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetConditionValue(int position_no) const {

#ifdef _DEBUG
	// Is the index argument legal?
	if ((position_no < 0) || (position_no >= GetNoConditionDescriptors())) {
		Message::Error("Rule position index out of range.");
		return Undefined::Integer();
	}
#endif

	return condition_values_[position_no];

}

//------------------------------------------------------------------
// Method........: AppendConditionDescriptor
// Author........:
// Date..........:
// Description...: Appends an (attribute, value) pair to the
//                 condition part of the rule.
//
// Comments......:
// Revisions.....:
//===================================================================

bool
KSRule::AppendConditionDescriptor(int attribute_no, int value) {

	condition_attributes_.push_back(attribute_no);
	condition_values_.push_back(value);

	return true;

}

//------------------------------------------------------------------
// Method........: SetDecisionAttribute
// Author........:
// Date..........:
// Description...: Sets the given attribute number as the decision
//                 attribute.  The attribute number is relative to
//                 the origin table.
// Comments......:
// Revisions.....:
//===================================================================

bool
KSRule::SetDecisionAttribute(int attribute_no) {
	decision_attribute_ = attribute_no;
	return true;
}

//------------------------------------------------------------------
// Method........: GetDecisionAttribute
// Author........:
// Date..........:
// Description...: Returns the rule's decision attribute (if any).
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetDecisionAttribute() const {
	return decision_attribute_;
}

//------------------------------------------------------------------
// Method........: GetGeneralizedDecision
// Author........:
// Date..........:
// Description...: Returns the number of decision values.
// Comments......:
// Revisions.....:
//===================================================================

const GeneralizedDecision &
KSRule::GetGeneralizedDecision() const {
	return decision_;
}

//------------------------------------------------------------------
// Method........: GetNoDecisionValues
// Author........:
// Date..........:
// Description...: Returns the number of decision values.
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetNoDecisionValues() const {
	return decision_.GetNoDecisions();
}

//------------------------------------------------------------------
// Method........: AppendDecisionValue
// Author........:
// Date..........:
// Description...: Appends a possible decision value to the rule.
//
//                 The support argument refers to number of
//                 objects in the table that (match the conditional
//                 descriptors and) have the specified decision value.
//
// Comments......: Consider augmenting the number of function arguments.
// Revisions.....:
//===================================================================

bool
KSRule::AppendDecisionValue(int value, int support, double /*stability*/) {
	return decision_.AppendDecision(value, support);
}

//------------------------------------------------------------------
// Method........: GetDecisionValue
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetDecisionValue(int position_no) const {
	return decision_.GetDecision(position_no);
}

//------------------------------------------------------------------
// Method........: GetSupport
// Author........:
// Date..........:
// Description...: Returns the number of objects that the decision
//                 value in the specified position in the rule's RHS
//                 applies to (supports), given the rule's LHS.
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetSupport(int position_no) const {
	return decision_.GetSupport(position_no);
}

//------------------------------------------------------------------
// Method........: GetStability
// Author........:
// Date..........:
// Description...: Returns the stability of the decision value in the specified
//                 position in the rule's RHS applies to (supports),
//                 given the rule's LHS.
//
// Comments......: Stability not supported by this subclass.
// Revisions.....:
//===================================================================

double
KSRule::GetStability(int /*position_no*/) const {
	return 1.0;
}

//------------------------------------------------------------------
// Method........: SetSupport
// Author........:
// Date..........:
// Description...: Sets the number of objects that the rule
//                 applies to (supports).
//
// Comments......: Stability not supported by this subclass.
// Revisions.....:
//===================================================================

bool
KSRule::SetSupport(int /*position_no*/, int /*support*/, double /*stability*/) {
	Message::Error("KSRule::SetSupport not implemented.");
	return false;
}

//------------------------------------------------------------------
// Method........: GetSupport
// Author........:
// Date..........:
// Description...: Returns the number of objects in the original
//                 decision table the LHS matches.
//
//                 Typically relevant in case the rule is really an
//                 abstract pattern, i.e. does not have a RHS.
// Comments......:
// Revisions.....:
//===================================================================

int
KSRule::GetSupport() const {
	return condition_support_;
}

//------------------------------------------------------------------
// Method........: SetSupport
// Author........:
// Date..........:
// Description...: Typically relevant in case the rule is really an
//                 abstract pattern, i.e. does not have a RHS.
//
// Comments......: Stability not supported by this subclass.
// Revisions.....:
//===================================================================

bool
KSRule::SetSupport(int support, double /*stability*/) {
	condition_support_ = support;
	return true;
}

//------------------------------------------------------------------
// Method........: GetStability
// Author........:
// Date..........:
// Description...: Only relevant if the RHS is missing. Returns
//                 the stability coefficient of the LHS.

// Comments......: Stability not supported by this subclass.
// Revisions.....:
//===================================================================

double
KSRule::GetStability() const {
	return 1.0;
}

//-------------------------------------------------------------------
// Method........: ReserveLHS/ReserveRHS
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reserves space prior to creation.
// Comments......:
// Revisions.....:
//===================================================================

bool
KSRule::ReserveLHS(int length) {

	condition_attributes_.reserve(length);
	condition_values_.reserve(length);

	return true;

}

bool
KSRule::ReserveRHS(int length) {
	return decision_.Reserve(length);
}
 
