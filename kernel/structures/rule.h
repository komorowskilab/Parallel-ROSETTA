//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULE_H__
#define __RULE_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/reduct.h>

#include <kernel/basic/map.h>
#include <kernel/basic/pair.h>
#include <kernel/basic/interval.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Discerner;
class InformationVector;

//-------------------------------------------------------------------
// Class.........: Rule
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for rules.
//
//                 A rule is on the form LHS => RHS where LHS (the
//                 antecedent) is a conjunction of descriptors and
//                 RHS (the consequent) is a disjunction (or more
//                 precisely, a set known as the generalized decision
//                 value).
//
//                 If RHS is missing the rule is not strictly
//                 a rule since it has no decision, but is then often
//                 denoted a "pattern" or "template".
//
// Comments......: The class design is currently not as clean as it
//                 should be, mainly due to historical reasons and to
//                 make integration with legacy code simpler.
//
//                 Currently, the Rule class is a compound class that
//                 represents both general patterns or templates, as well
//                 as decision rules, i.e.:
//
//                    General patterns or templates:
//								       (a1 = v1) AND ... AND (an = vn)
//
//                    Decision rules:
//                       (a1 = v1) AND ... AND (an = vn) =>
//                          (d in [d1, .., dm])
//
//                 The class design should be decomposed into two classes,
//                 e.g., Template and DecisionRule.
//
//                 Also, not currently modelled by the compound Rule class are
//                 association rules, i.e., rules where the consequent is also
//                 a conjunction of descriptors involving different attributes.
//
//                    Association rules:
//								       (a1 = v1) AND ... AND (an = vn) =>
//                          (am = vm) AND ... AND (ak = vk)
//
//                 Such rules should be modelled by a third class in this
//                 family, e.g., AssociationRule.
//
//                 To do: Define a decent Descriptor class and use this. 
//                        Such a class could also hold the binary relation 
//                        in use for that descriptor, e.g., "=", ">=" or 
//                        "!=". Now, "=" is always assumed.
//
// Revisions.....:
//===================================================================

class Rule : public Structure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<Rule>) Handles;    // Represents a vector of rule pointers (handles).
	typedef Pair(int, int)       Descriptor; // Represents an (attribute, value) pair.

protected:

  //- Copy constructor...............................................
  Rule(const Rule &in);

  // Ewa Makosa 03.06.2005...........................................
  // Moved function StaticFormatPrologInterval from static file scope
  // to protected method.............................................
  String	     FormatPrologInterval(const String &variablename, const Interval &interval, const String &indent)const;

public:

  //- Constructors/destructor........................................
	Rule();
  virtual ~Rule();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent..............................
  virtual bool        Load(ifstream &stream);
  virtual bool        Save(ofstream &stream) const;

	// -N 07/08/13 Saves a rule with mapped features and decisions.
	virtual bool	SaveMappedRule(ofstream &stream, Handle<DecisionTable> table, bool masked) const;
	virtual bool	LoadMappedRule(ifstream &stream, Handle<DecisionTable> table, bool masked);
	virtual bool	LoadMappedRuleFromString(String line, Handle<DecisionTable> table, bool masked);
	bool		IsInteger(String& line);
	bool		DrainToken(String& line, const char* delimiters, bool success=false);	
	//- Condition (LHS) management.....................................
  virtual int         GetNoConditionDescriptors() const = 0;

  virtual int         GetConditionAttribute(int position_no) const = 0;
  virtual int         GetConditionValue(int position_no) const = 0;

  virtual bool        AppendConditionDescriptor(int attribute_no, int value) = 0;

  //- Decision attribute (RHS) management............................
	virtual int         GetDecisionAttribute() const = 0;
	virtual bool        SetDecisionAttribute(int attribute_no) = 0;

	//- Decision value (RHS) management................................
	virtual int         GetNoDecisionValues() const = 0;
  virtual int         GetDecisionValue(int position_no) const = 0;
  virtual int         GetDominatingDecisionValue(int &support, int &position_no) const;
  virtual bool        AppendDecisionValue(int value, int support = 1, double stability = 1.0) = 0;

	//- Quant. value management, complete rules (RHS, given LHS).......
	virtual int         GetSupport(int position_no) const = 0;
	virtual bool        SetSupport(int position_no, int support, double stability = 1.0) = 0;

	virtual float       GetAccuracy(int position_no) const;
	virtual double      GetStability(int position_no) const = 0;

	virtual float       GetCoverage(const Vector(int) &decisions, const Vector(int) &cardinalities) const;
	virtual float       GetCoverage(int position_no, const Vector(int) &decisions, const Vector(int) &cardinalities) const;

	//- Quant. value management, patterns (LHS only, RHS missing)......
	virtual int         GetSupport() const = 0;
	virtual bool        SetSupport(int support, double stability = 1.0) = 0;

	virtual double      GetStability() const = 0;

	virtual float       GetCoverage(int no_objects) const;
  virtual float       GetCoverage(const Vector(int) &cardinalities) const;

	//- Querying methods...............................................
	bool                IsDeterministic() const;

	bool                HasLHS() const;
	bool                HasRHS() const;

	virtual bool        Matches(const InformationVector &object, const Discerner &discerner, float fraction) const;

	//- Operators......................................................
  Rule               &operator=(const Rule &in);

  bool                operator==(const Rule &in) const;
  bool                operator!=(const Rule &in) const;

	bool                operator<(const Rule &in) const;
	bool                operator>(const Rule &in) const;

	//- Formatting.....................................................
	// Ewa Makosa 03.06.2005...........................................
	// Made Format function virtual....................................
	virtual bool        Format(String &formatted, const DecisionTable *table = NULL, bool masked = true) const;
	String              Format(const DecisionTable *table = NULL, bool masked = true) const;
	bool                FormatProlog(String &formatted, const DecisionTable *table = NULL, bool masked = true) const;
	String              FormatProlog(const DecisionTable *table = NULL, bool masked = true) const;

	//- Creation.......................................................
	virtual bool        Create(const Reduct &reduct, const DecisionTable &table, const Discerner &discerner, int object_no, bool rhs);

	virtual bool        ReserveLHS(int length);
	virtual bool        ReserveRHS(int length);

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

//------------------------------------------------------------------
// Method........: IsDeterministic
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Rule::IsDeterministic() const {
	return (GetNoDecisionValues() <= 1);
}

//------------------------------------------------------------------
// Method........: HasLHS/HasRHS
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Rule::HasLHS() const {
	return (GetNoConditionDescriptors() > 0);
}

inline bool
Rule::HasRHS() const {
	return (GetNoDecisionValues() > 0);
}

#endif
 
