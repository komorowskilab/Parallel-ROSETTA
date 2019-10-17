//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/rule.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>

#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/interval.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticLookup
// Author........:
// Date..........:
// Description...: Looks up a cardinality for a given decision,
//                 returns the cardinality in-place.
// Comments......: Simple linear implementation.
// Revisions.....:
//===================================================================

static bool
StaticLookup(int decision, const Vector(int) &decisions, const Vector(int) &cardinalities, int &cardinality) {

	int i;

	// Lookup.
	for (i = 0; i < decisions.size(); i++) {
		if (decisions[i] == decision)
			break;
	}

	// Not found?
	if (i == decisions.size())
		return false;

	// Return in-place.
	cardinality = cardinalities[i];

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticFormatNumber
// Author........:
// Date..........:
// Description...: Used by StaticFormatPrologInterval.
// Comments......:
// Revisions.....:
//===================================================================

static String
StaticFormatNumber(float number) {

	String formatted = String::Format(number);

	// No special formatting needed?
	if (!formatted.Contains('.'))
		return formatted;

	int n = formatted.GetLength();

	// Delete all trailing zeros after the decimal point.
	while (n > 0 && formatted[n - 1] == '0') {
		formatted[n - 1] = '\0';
		n--;
	}

	// Format "129.0" as "129".
	if (formatted[n - 1] == '.')
		formatted[n - 1] = '\0';

	// Did we delete the whole string?
	if (n == 1)
		formatted = String("0");

	return formatted;

}

//-------------------------------------------------------------------
// Method........: StaticFormatPrologInterval
// Author........:
// Date..........:
// Description...: Used by Rule::FormatProlog.
// Comments......:
// Revisions.....: Ewa Makosa 03.06.2005: 
//		   Moved from static file scope to protected class
//		   FormatPrologInterval method
//===================================================================


//-------------------------------------------------------------------
// Methods for class Rule.
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

Rule::Rule(const Rule &/*in*/) {
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Rule::Rule() {
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........:
// Date..........:
// Description...:
// Comments......: Be careful of "Handle loops", as this may cause
//                 memory leaks!
// Revisions.....:
//===================================================================

Rule::~Rule() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Rule, RULE, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent
//===================================================================

//------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::Load(ifstream &stream) {

	Clear();

	int i, a, v, n, s;

	if (!IOKit::Load(stream, n))
		return false;

	if (!ReserveLHS(n))
		return false;

	for (i = 0; i < n; i++) {
		if (!IOKit::Load(stream, a) || !IOKit::Load(stream, v))
			return false;
		if (!AppendConditionDescriptor(a, v))
			return false;
	}

	if (!IOKit::Load(stream, s) || !SetSupport(s))
		return false;

	if (!IOKit::Load(stream, a) || !SetDecisionAttribute(a))
		return false;

	if (!IOKit::Load(stream, n))
		return false;

	if (!ReserveRHS(n))
		return false;

	for (i = 0; i < n; i++) {
		if (!IOKit::Load(stream, v) || !IOKit::Load(stream, s))
			return false;
		if (!AppendDecisionValue(v, s))
			return false;
	}

	return true;

}

//------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::Save(ofstream &stream) const {

	if (!IOKit::Save(stream, GetNoConditionDescriptors()))
		return false;

	int i;

	for (i = 0; i < GetNoConditionDescriptors(); i++) {
		if (!IOKit::Save(stream, GetConditionAttribute(i)))
			return false;
		if (!IOKit::Save(stream, GetConditionValue(i)))
			return false;
	}

	if (!IOKit::Save(stream, GetSupport()))
		return false;

	if (!IOKit::Save(stream, GetDecisionAttribute()))
		return false;

	if (!IOKit::Save(stream, GetNoDecisionValues()))
		return false;

	for (i = 0; i < GetNoDecisionValues(); i++) {
		if (!IOKit::Save(stream, GetDecisionValue(i)))
			return false;
		if (!IOKit::Save(stream, GetSupport(i)))
			return false;
	}

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}
//------------------------------------------------------------------
// Method........: SaveMappedRule
// Author........: Nicholas Baltzer
// Date..........: 07/08/13
// Description...: Saves a rule to stream. The format is mapped via the dictionary
//			to output feature names as strings rather than internal indices.
//			Keep in mind that loading such a set requires reversing this mapping to whatever
//			indices the current dataset uses.
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::SaveMappedRule(ofstream &stream, Handle<DecisionTable> table, bool masked) const {

	//First, save the number of features in the rule on the left hand side.
	if (!IOKit::Save(stream, GetNoConditionDescriptors()))
		return false;

	int i;
	//Go through the features one by one.
	for (i = 0; i < GetNoConditionDescriptors(); i++) {
		//First, save the name of the feature.
		if (!IOKit::Save(stream, table->GetAttributeName(GetConditionAttribute(i), masked)))
			return false;
		//Then save the value.
		if (!IOKit::Save(stream, GetConditionValue(i)))
			return false;
	}
	//Save support of the rule.
	if (!IOKit::Save(stream, GetSupport()))
		return false;
	//save the name of the decision.
	if (!IOKit::Save(stream, table->GetAttributeName(GetDecisionAttribute(), masked)))
		return false;
	//Save the number of possible values for the decision.
	if (!IOKit::Save(stream, GetNoDecisionValues()))
		return false;
	//For each value..
	for (i = 0; i < GetNoDecisionValues(); i++) {
		//Save the value.
		if (!IOKit::Save(stream, GetDecisionValue(i)))
			return false;
		//Then save the support for that value.
		if (!IOKit::Save(stream, GetSupport(i)))
			return false;
	}
	//newline.
	if (!IOKit::Save(stream, '\n'))
		return false;
	//And done. 
	return true;

}


//------------------------------------------------------------------
// Method........: LoadMappedRule
// Author........: Nicholas Baltzer
// Date..........: 08/08/13
// Description...: Loads a rule from input stream, maps it to the internal dictionary
//			of the provided database, and saves it. 
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::LoadMappedRule(ifstream &stream, Handle<DecisionTable> table, bool masked) {
//Rule::LoadMappedRule(String line, Handle<DecisionTable> table, bool masked) {

	//To be safe, clear the rule structure.
	Clear();
	//delimiters.
	const char* delimiters = " \n";
	//Values
	int i, n, s, v, index;
	//Features and values should be Strings.
	String a, string_n, string_v, string_s;

	//Load the number of features in the rule.
	if (!IOKit::Load(stream, n, delimiters))
//	if (!line.GetToken(string_n, delimiters))
		return false;
//	if (string_n.IsInteger()) {
//		n = string_n.GetInteger();
//	}
//	else {
//		return false;
//	}
	//Reserve space for these.
	if (!ReserveLHS(n))
		return false;
	
	for (i = 0; i < n; i++) {
		//Load feature name and feature value.
		if (!IOKit::Load(stream, a) || !IOKit::Load(stream, v))
//		if (!line.GetToken(a, delimiters) || !line.GetToken(string_v, delimiters))
			return false;
		
		//Append the mapped feature<>value pair to the rule.
		index = table->GetAttributeIndex(a, false, masked);
		if (index == Undefined::Integer())
			return false;

		if (!AppendConditionDescriptor(index, v))
				return false;
		
	}
	//Load the support.
	if (!IOKit::Load(stream, s) || !SetSupport(s))
//	if (!line.GetToken(string_s, delimiters) || !SetSupport(string_s.GetInteger()))
		return false;
	//Map and set the decision attribute.
	if (!IOKit::Load(stream, a) || !SetDecisionAttribute(table->GetAttributeIndex(a, false, masked)))
//	if (!line.GetToken(a, delimiters) || !SetDecisionAttribute(table->GetAttributeIndex(a, false, masked)))
		return false;
	//Load the number of decision values.
	if (!IOKit::Load(stream, n))
//	if (!line.GetToken(string_n, delimiters))
		return false;
//	n = string_n.GetInteger();
	//Reserve space for the decision values.
	if (!ReserveRHS(n))
		return false;

	for (i = 0; i < n; i++) {
		//Load a decision value and it's corresponding support.
		if (!IOKit::Load(stream, v, delimiters) || !IOKit::Load(stream, s, delimiters))
//		if (!line.GetToken(string_v, delimiters) || !line.GetToken(string_s, delimiters))
			return false;
		//Append the decision value and it's support.
		if (!AppendDecisionValue(v, s))
//		if (!AppendDecisionValue(string_v.GetInteger(), string_s.GetInteger()))
			return false;
	}

	return true;

}
//------------------------------------------------------------------
// Method........: LoadMappedRuleFromString
// Author........: Nicholas Baltzer
// Date..........: 16/08/13
// Description...: Loads a rule from input string, maps it to the internal dictionary
//			of the provided database, and saves it. 
// Comments......: This version uses a String input rather than a stream input. This is beneficial for
//			terminating on time, and for checking a bunch of stuff. The token function
//			is purged on each failure to avoid getting rule data stuck in the machinery
//			when setting parameters for algorithms. Uses DrainToken and IsInteger as helper
//			functions.
//			***************IMPORTANT********************
//			Only call this function from within a critical(GetToken) section. If you don't,
//			the static token call might be polluted in either direction.
// Revisions.....:
//===================================================================
bool
Rule::LoadMappedRuleFromString(String line, Handle<DecisionTable> table, bool masked) {
//Rule::LoadMappedRule(String line, Handle<DecisionTable> table, bool masked) {

	//To be safe, clear the rule structure.
	Clear();
	//delimiters.
	const char* delimiters = " \n";
	//Values
	int i, n, index;
	//Features and values should be Strings.
	String a, string_v, string_s;

	String token;

	//Load the number of features in the rule.
	if (!line.GetToken(token, delimiters))
		return DrainToken(line, delimiters);
	if (!IsInteger(token))
		return DrainToken(line, delimiters);
	n = token.GetInteger();
	//Reserve space for these.
	if (!ReserveLHS(n))
		return DrainToken(line, delimiters);
	
	for (i = 0; i < n; i++) {
		//Load feature name and feature value.
		if (!line.GetToken(a, delimiters) || !line.GetToken(token, delimiters))
			return DrainToken(line, delimiters);
		
		//Append the mapped feature<>value pair to the rule.
		a.Trim("\"");
		index = table->GetAttributeIndex(a, false, masked);
		if (index == Undefined::Integer())
			return DrainToken(line, delimiters);

		if (!AppendConditionDescriptor(index, token.GetInteger()))
			return DrainToken(line, delimiters);
				
		
	}
	//Load the support.
	if (!line.GetToken(token, delimiters))
		return DrainToken(line, delimiters);
	if (!IsInteger(token))
		return DrainToken(line, delimiters);
	if (!SetSupport(token.GetInteger()))
		return DrainToken(line, delimiters);
	

	//Map and set the decision attribute.
	if (!line.GetToken(a, delimiters))
		return DrainToken(line, delimiters);
	a.Trim("\"");
	index = table->GetAttributeIndex(a, false, masked);
	if (index == Undefined::Integer())
		return DrainToken(line, delimiters);
	if (!SetDecisionAttribute(index))
		return DrainToken(line, delimiters);
	//Load the number of decision values.
	if (!line.GetToken(token, delimiters))
		return DrainToken(line, delimiters);
	if (!IsInteger(token))
		return DrainToken(line, delimiters);
	n = token.GetInteger();
	//Reserve space for the decision values.
	
	if (!ReserveRHS(n))
		return DrainToken(line, delimiters);

	for (i = 0; i < n; i++) {
		//Load a decision value and it's corresponding support.
		if (!line.GetToken(string_v, delimiters) || !line.GetToken(string_s, delimiters))
			return DrainToken(line, delimiters);
		if (!IsInteger(string_v) || !IsInteger(string_s))
			return DrainToken(line, delimiters);

		//Append the decision value and it's support.
		if (!AppendDecisionValue(string_v.GetInteger(), string_s.GetInteger()))
			return DrainToken(line, delimiters);
	}
	//Line ends here, so if we get "" everything is great.
//	if (!line.GetToken(token, delimiters))
	return DrainToken(line, delimiters, true);
	//If not, the line is not done, and something is wrong with the rule structure.
}

//------------------------------------------------------------------
// Method........: IsInteger
// Author........: Nicholas Baltzer
// Date..........: 16/08/13
// Description...: Checks to see if the String provided is a valid integer or not.
//			
// Comments......:
// Revisions.....:
//===================================================================
bool
Rule::IsInteger(String& value) {
	if (value.IsInteger())
		return true;
	return false;
}

//------------------------------------------------------------------
// Method........: DrainToken
// Author........: Nicholas Baltzer
// Date..........: 16/08/13
// Description...: Purges the GetToken buffer, and returns success,
//			which is defaulted to false. Helper function to LoadMappedRulesFromString.
//			
// Comments......:
// Revisions.....:
//===================================================================
bool
Rule::DrainToken(String &line, const char* delimiters, bool success) {
	String token;
	while (line.GetToken(token, delimiters)) {}
	return success;
}

//-------------------------------------------------------------------
// Decision value management.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetDominatingDecisionValue
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the dominating decision value (i.e. the
//                 decision value with the highest associated support
//                 count), and returns in-place both the support and
//                 the position index of this.
//
//                 Ties are resolved arbitrarily.
// Comments......:
// Revisions.....:
//===================================================================

int
Rule::GetDominatingDecisionValue(int &support, int &position_no) const {

	int i, no_decision_values = GetNoDecisionValues();

	// No decision values ?
	if (no_decision_values == 0) {

		// Return error values.
		support     = Undefined::Integer();
		position_no = Undefined::Integer();

		return Undefined::Integer();

	}

	int dominating = GetDecisionValue(0);

	support     = GetSupport(0);
	position_no = 0;

	for (i = 1; i < no_decision_values; i++) {
		int current = GetSupport(i);
		if (current > support) {
			support     = current;
			position_no = i;
			dominating  = GetDecisionValue(i);
		}
	}

	return dominating;

}

//-------------------------------------------------------------------
// Method........: GetAccuracy
// Author........: Aleksander šhrn
// Date..........:
// Description...: An estimate of Pr(RHS(i) | LHS).
// Comments......:
// Revisions.....:
//===================================================================

float
Rule::GetAccuracy(int position_no) const {

	int i, no_decision_values = GetNoDecisionValues();

	// Safe-guard to avoid numerical quirks.
	if ((position_no == 0) && (no_decision_values == 1))
		return 1.0;

	double term = 0.0;
	double sum  = 0.0;

	for (i = 0; i < no_decision_values; i++) {
		int support = GetSupport(i);
		if (i == position_no)
			term = support;
		sum += support;
	}

	return (term / sum);

}

//-------------------------------------------------------------------
// Method........: GetCoverage
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the ratio between the total support of the
//                 decision values and the total number of objects in
//                 the originating table that belong to the same
//                 decision classes.
//
//                 An estimate of Pr(LHS | RHS).
//
// Comments......: A map that maps decision values to their total
//                 counts in the originating table must be supplied.
// Revisions.....:
//===================================================================

float
Rule::GetCoverage(const Vector(int) &decisions, const Vector(int) &cardinalities) const {

	int i, no_decision_values = GetNoDecisionValues();

	// No decision values?
	if (no_decision_values == 0)
		return Undefined::Float();

	int total_support = 0;
	int total_cardinality = 0;

	// Iterate over RHS.
	for (i = 0; i < no_decision_values; i++) {

		// Get decision value.
		int decision = GetDecisionValue(i);

		int cardinality;

		// Lookup cardinality.
		if (!StaticLookup(decision, decisions, cardinalities, cardinality))
			return Undefined::Float();

		// Get support value.
		int support = GetSupport(i);

		// Accumulate.
		total_support += support;
		total_cardinality += cardinality;

	}

	// Avoid numerical quirks.
	if (total_support == 0)
		return 0.0;
	else if (total_support == total_cardinality)
		return 1.0;

	// Avoid division by zero.
	if (total_cardinality == 0)
		return Undefined::Float();

	// Return coverage.
	return static_cast(float, total_support) / total_cardinality;

}

//-------------------------------------------------------------------
// Method........: GetCoverage
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the ratio between the support of the
//                 decision value and the total number of objects in
//                 the originating table that belong to the same
//                 decision class.
//
//                 An estimate of Pr(LHS | RHS(i)).
//
// Comments......: A map that maps decision values to their total
//                 counts in the originating table must be supplied.
// Revisions.....:
//===================================================================

float
Rule::GetCoverage(int position_no, const Vector(int) &decisions, const Vector(int) &cardinalities) const {

	int cardinality;

	// Get decision value.
	int decision = GetDecisionValue(position_no);

	// Lookup cardinality.
	if (!StaticLookup(decision, decisions, cardinalities, cardinality))
		return Undefined::Float();

	// Get support value.
	int support = GetSupport(position_no);

	// Avoid numerical quirks.
	if (support == 0)
		return 0.0;
	else if (support == cardinality)
		return 1.0;

	// Avoid division by zero.
	if (cardinality == 0)
		return Undefined::Float();

	// Return coverage.
	return static_cast(float, support) / cardinality;

}

//------------------------------------------------------------------
// Method........: GetCoverage
// Author........:
// Date..........:
// Description...: Given the total number of objects in the table the
//                 rule/pattern was derived from, returns the
//                 rule's/pattern's LHS support divided by this number.
//
//                 An estimate of Pr(LHS).
// Comments......:
// Revisions.....:
//===================================================================

float
Rule::GetCoverage(int no_objects) const {

	if (no_objects <= 0)
		return Undefined::Float();

	return static_cast(float, GetSupport()) / no_objects;

}

//------------------------------------------------------------------
// Method........: GetCoverage
// Author........:
// Date..........:
// Description...:
// Comments......: A vector that contains the total counts for all
//                 possible decision classes in the originating table
//                 must be supplied.
// Revisions.....:
//===================================================================

float
Rule::GetCoverage(const Vector(int) &cardinalities) const {
	return GetCoverage(MathKit::Sum(cardinalities));
}

//-------------------------------------------------------------------
// Operators
//===================================================================

//-------------------------------------------------------------------
// Operator......: Assignment operator
// Author........:
// Date..........:
// Description...:
// Comments......: Presently, does not copy all associated
//                 quantitative rule/pattern data.
// Revisions.....:
//===================================================================

Rule &
Rule::operator=(const Rule &in) {

	int i;

	// Clear this rule.
	Clear();

	int no_condition_descriptors = in.GetNoConditionDescriptors();

	// Copy the condition attributes and their values.
	for (i = 0; i < no_condition_descriptors; i++)
		AppendConditionDescriptor(in.GetConditionAttribute(i), in.GetConditionValue(i));

	// Copy the decision attribute.
	SetDecisionAttribute(in.GetDecisionAttribute());

	int no_decision_values = in.GetNoDecisionValues();

	// Copy all decision values.
	for (i = 0; i < no_decision_values; i++)
		AppendDecisionValue(in.GetDecisionValue(i));

	Message::Warning("Not all data copied in rule assignment operator.");

	return *this;

}

//-------------------------------------------------------------------
// Operator......: Equality operator
// Author........:
// Date..........:
// Description...:
// Comments......: Does not compare associated numerical data.
// Revisions.....:
//===================================================================

bool
Rule::operator==(const Rule &in) const {

	// Are the two objects physically equal?
	if (&in == this)
		return true;

	int no_condition_descriptors = GetNoConditionDescriptors();

	// Are the conditional terms equal? (Assuming same order).
	if (no_condition_descriptors != in.GetNoConditionDescriptors())
		return false;

	int i;

	for (i = 0; i < no_condition_descriptors; i++)
		if (GetConditionAttribute(i) != in.GetConditionAttribute(i))
			return false;

	for (i = 0; i < no_condition_descriptors; i++)
		if (GetConditionValue(i) != in.GetConditionValue(i))
			return false;

	// Are the decision attributes equal?
	if (GetDecisionAttribute() != in.GetDecisionAttribute())
		return false;

	int no_decision_values = GetNoDecisionValues();

  // Are decision terms equal? (Assuming same order).
	if(GetNoDecisionValues() != in.GetNoDecisionValues())
		return false;

	for (i = 0; i < no_decision_values; i++)
		if (GetDecisionValue(i) != in.GetDecisionValue(i))
			return false;

  return true;

}

//-------------------------------------------------------------------
// Operator......: Inequality operator
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::operator!=(const Rule &in) const {
	return !(*this == in);
}

//-------------------------------------------------------------------
// Operator......: < operator
// Author........:
// Date..........:
// Description...: A rule R is said to be less general (or more
//                 specific) than a rule S, written R < S, if:
//
//                 i)  R's LHS is a proper superset of S's LHS, and
//                 ii) R's RHS is a subset of S's RHS
//
//                 Example:
//
//                   R1: (a, 1)           => (d, 0) || (d, 1)
//                   R2: (a, 1) && (b, 0) => (d, 0)
//                   R3: (a, 1) && (b, 1) =>           (d, 1)
//                   R4: (b, 0)           => (d, 0) || (d, 1)
//
//                   R1 "covers" R2, so R2 < R1.
//                   R1 "covers" R3, so R3 < R1.
//                   R4 "covers" R2, so R2 < R4.
//
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::operator<(const Rule &in) const {

	if (&in == this)
		return false;

	// Get LHS dimensions.
	int no_descriptors_this = GetNoConditionDescriptors();
	int no_descriptors_that = in.GetNoConditionDescriptors();

	// Can we exclude the LHS proper superset condition already now?
	if (no_descriptors_this <= no_descriptors_that)
		return false;

	int i, j;

	// Check LHS superset condition.
	for (i = 0; i < no_descriptors_that; i++) {

		int a = in.GetConditionAttribute(i);
		int v = in.GetConditionValue(i);

		bool found = false;

		for (j = 0; j < no_descriptors_this; j++) {
			if (GetConditionAttribute(j) == a && GetConditionValue(j) == v) {
				found = true;
				break;
			}
		}

		if (!found)
			return false;

	}

	// Get RHS dimensions.
	int no_decisions_this = GetNoDecisionValues();
	int no_decisions_that = in.GetNoDecisionValues();

	// Can we exclude the RHS subset condition already now?
	if (no_decisions_this > no_decisions_that)
		return false;

	if (GetDecisionAttribute() != in.GetDecisionAttribute())
		return false;

	// Check RHS subset condition.
	for (i = 0; i < no_decisions_this; i++) {

		int d = GetDecisionValue(i);

		bool found = false;

		for (j = 0; j < no_decisions_that; j++) {
			if (GetDecisionValue(j) == d) {
				found = true;
				break;
			}
		}

		if (!found)
			return false;

	}

	return true;

}

//-------------------------------------------------------------------
// Operator......: > operator
// Author........:
// Date..........:
// Description...: See < operator.
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::operator>(const Rule &in) const {
	return in < *this;
}



//-------------------------------------------------------------------
// Formatting methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: If a decision table with an associated dictionary
//                 is supplied, this is used.
//
// Revisions.....: Aš 980422: Made formatting more flexible.
//===================================================================

bool
Rule::Format(String &formatted, const DecisionTable *table, bool masked) const {

	// Determine formatting symbols. Consider making these parameters.
	const String rule_prefix         = "";
	const String rule_postfix        = "";
	const String rule_default        = "";                  // Use as formatted rule if none can be built.
	const String antecedent_prefix   = "";      // "IF "
	const String antecedent_postfix  = "";
	const String antecedent_default  = "";      // "(true)" // Use as formatted antecedent if none can be built.
	const String consequent_prefix   = "";
	const String consequent_postfix  = "";
	const String consequent_default  = "";      // "(true)" // Use as formatted consequent if none can be built.
	const String implication         = " => ";  // " THEN "
	const String implication_default = "";                  // Use as implication if formatted antecedent or consequent is empty.
	const String conjunction         = " AND "; // " && "
	const String disjunction         = " OR ";  // " || "
	const String descriptor_prefix   = "";      // "("
	const String descriptor_infix    = "(";     // " == "
	const String descriptor_postfix  = ")";

	// Get rule dimensions.
	int no_conditions = GetNoConditionDescriptors();
	int no_decisions  = GetNoDecisionValues();

	// Get decision attribute.
	int decision_attribute = GetDecisionAttribute();

	String da_name;

	// Get name of decision attribute.
	if (decision_attribute == Undefined::Integer())
		da_name = Undefined::String();
	else if (table == NULL)
		da_name = String::Format(decision_attribute);
	else
    da_name = table->GetAttributeName(decision_attribute, masked);

	String antecedent;
	antecedent.Reserve(500);

	int i;

	// Build conditional part (antecedent, LHS) of rule.
	for (i = 0; i < no_conditions; i++) {

		// Get (attribute, value) pair.
		int attribute = GetConditionAttribute(i);
		int value     = GetConditionValue(i);

		// Get names of attribute and value.
		String a_name = (table == NULL) ? String::Format(attribute) : table->GetAttributeName(attribute, masked);
		String v_name = (table == NULL) ? String::Format(value) : table->GetDictionaryEntry(attribute, value, masked);

		// Append descriptor.
		antecedent += descriptor_prefix;
		antecedent += a_name;
		antecedent += descriptor_infix;
		antecedent += v_name;
		antecedent += descriptor_postfix;

		if (i < (no_conditions - 1))
			antecedent += conjunction;

	}

	if (no_conditions == 0)
		antecedent = antecedent_default;

	// Wrap antecedent.
	if (!antecedent_prefix.IsEmpty())
		antecedent = antecedent_prefix + antecedent;
	antecedent += antecedent_postfix;

	String consequent;
	consequent.Reserve(100);

	// Build decision part (consequent, RHS) of rule.
	for (i = 0; i < no_decisions; i++) {

		// Get decision value.
		int decision = GetDecisionValue(i);

		// Get name of decision value.
		String dv_name = (table == NULL) ? String::Format(decision) : table->GetDictionaryEntry(decision_attribute, decision, masked);

		// Append descriptor.
		consequent += descriptor_prefix;
		consequent += da_name;
		consequent += descriptor_infix;
		consequent += dv_name;
		consequent += descriptor_postfix;

		if (i < (no_decisions - 1))
			consequent += disjunction;

	}

	if (no_decisions == 0)
		consequent = consequent_default;

	// Wrap consequent.
	if (!consequent_prefix.IsEmpty())
		consequent = consequent_prefix + consequent;
	consequent += consequent_postfix;

	formatted.Reserve(antecedent.GetLength() + implication.GetLength() + consequent.GetLength() + rule_prefix.GetLength() + rule_postfix.GetLength());

	// Build formatted rule.
	if (antecedent.IsEmpty() || consequent.IsEmpty()) {
		formatted =  antecedent;
		formatted += implication_default;
		formatted += consequent;
	}
	else {
		formatted =  antecedent;
		formatted += implication;
		formatted += consequent;
	}

	if (formatted.IsEmpty())
		formatted = rule_default;

	// Wrap formatted rule.
	if (!rule_prefix.IsEmpty())
		formatted = rule_prefix + formatted;
	formatted += rule_postfix;

	return true;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Rule::Format(const DecisionTable *table, bool masked) const {

	String formatted;

	if (!Format(formatted, table, masked))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Method........: FormatPrologInterval
// Author........: Ewa Makosa
// Date..........: 03.06.2005
// Description...: Used by Rule::FormatProlog.
// Comments......: Changed this function from static (file scope) to
//		   protected method
// Revisions.....:
//===================================================================

String
Rule::FormatPrologInterval(const String &variablename, const Interval &interval, const String &indent)const {
	
	char lower_delimiter = interval.GetLowerDelimiter();
	char upper_delimiter = interval.GetUpperDelimiter();
	
	float lower_value = interval.GetLowerValue();
	float upper_value = interval.GetUpperValue();
	
	bool lower_is_infinity = (lower_value == -FLT_MAX);
	bool upper_is_infinity = (upper_value == FLT_MAX);
	
	String formatted;
	
	// Don't write "infinity" values.
	if (!lower_is_infinity) {
		String lower_value_formatted = StaticFormatNumber(lower_value);
		if (lower_delimiter == Interval::From())
			formatted += indent + variablename + " > " + lower_value_formatted;
		else
			formatted += indent + variablename + " >= " + lower_value_formatted;
	}
	
	if (!lower_is_infinity && !upper_is_infinity)
		formatted += ",\n";
	
	// Don't write "infinity" values.
	if (!upper_is_infinity) {
		String upper_value_formatted = StaticFormatNumber(upper_value);
		if (upper_delimiter == Interval::To())
			formatted += indent + variablename + " < " + upper_value_formatted;
		else
			formatted += indent + variablename + " <= " + upper_value_formatted;
	}
	
	return formatted;
	
}

//-------------------------------------------------------------------
// Method........: FormatProlog
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: If a decision table with an associated dictionary
//                 is supplied, this is used.
// Revisions.....:
//===================================================================

bool
Rule::FormatProlog(String &formatted, const DecisionTable *table, bool masked) const {

	// Clear formatted string to return.
	formatted = "";

	// Get rule dimensions.
	int no_conditions = GetNoConditionDescriptors();
	int no_decisions  = GetNoDecisionValues();

	// Get decision attribute.
	int decision_attribute = GetDecisionAttribute();

	String daname;

	// Get name of decision attribute.
	if (decision_attribute == Undefined::Integer())
		daname = Undefined::String();
	else if (table == NULL)
		daname = String::Format(decision_attribute);
	else
    daname = table->GetAttributeName(decision_attribute, masked);

	// Uppercase is reserved for variables in Prolog.
	daname.ToLowercase();

	// Do we have to add quotes?
	if (daname.Contains(' ') || daname.Contains(','))
		daname = "\'" + daname + "\'";

	String body;

	int i;

	// Build body of Prolog clause.
	for (i = 0; i < no_conditions; i++) {

		// Get (attribute, value) pair.
		int attribute_no = GetConditionAttribute(i);
		int value        = GetConditionValue(i);

		// Get names of attribute and descriptor.
		String caname = (table == NULL) ? "a" + String::Format(attribute_no) : table->GetAttributeName(attribute_no, masked);
		String dvname = (table == NULL) ? String::Format(value) : table->GetDictionaryEntry(attribute_no, value, masked);

		// Uppercase is reserved for variables in Prolog.
		caname.ToLowercase();
		dvname.ToLowercase();

		// Do we have to add quotes?
		if (caname.Contains(' ') || caname.Contains(','))
			caname = "\'" + caname + "\'";

		// Is the decision value an interval?
		bool is_interval = Interval(dvname).IsValid();

		// Append clause(s) to body.
		if (!is_interval) {
			if (dvname.Contains(' ') || dvname.Contains(','))
				dvname = "\'" + dvname + "\'";
			body += '\t' + caname + "(X, " + dvname + ")";
		}
		else {
			String varname = "V" + String::Format(i);
			body += '\t' + caname + "(X, " + varname + "),\n";
			body += FormatPrologInterval(varname, dvname, "\t\t");
		}

		body += (i < no_conditions - 1) ? ",\n" : ".";

	}

	// Is the RHS empty?
	if (!HasRHS()) {

		// Get quantitative information.
		int    supp = GetSupport();
		double stab = GetStability();

		// Add head to body.
		formatted = "match(X, _, " + String::Format(supp) + ", _, " + String::Format(stab) + ") :-\n" + body;

		return true;

	}

	// Create one Prolog rule for each possible decision value.
	for (i = 0; i < no_decisions; i++) {

		// Get decision value.
		int decision = GetDecisionValue(i);

		// Get name of decision value.
		String dvname = (table == NULL) ? String::Format(decision) : table->GetDictionaryEntry(decision_attribute, decision, masked);

		// Uppercase is reserved for variables in Prolog.
		dvname.ToLowercase();

		// Do we have to add quotes?
		if (dvname.Contains(' ') || dvname.Contains(','))
			dvname = "\"" + dvname + "\"";

		// Get quantitative information.
		int    supp = GetSupport(i);
		float  accu = GetAccuracy(i);
		double stab = GetStability(i);

		// Build head of clause.
		String head = daname + "(X, " + dvname + ", " + String::Format(supp) + ", " + String::Format(accu) + ", " + String::Format(stab) + ")";

		// Append head of clause.
		formatted += head;

		// Is the clause really a fact?
		if (body.IsEmpty()) {
			formatted += '.';
			if (i < no_decisions - 1)
				formatted += "\n\n";
			continue;
		}

		// Append implication sign.
		formatted += " :-\n";

		// Append body of clause.
		formatted += body;

		if (i < no_decisions - 1)
			formatted += "\n\n";

 	}

	return true;

}

//-------------------------------------------------------------------
// Method........: FormatProlog
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Rule::FormatProlog(const DecisionTable *table, bool masked) const {

	String formatted;

	if (!FormatProlog(formatted, table, masked))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Method........: Matches
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the rule "fires" (i.e., is
//                 applicable to or matches) when presented with the
//                 given information vector.
//
//                 A rule fires unless the information vector is in
//                 direct contradiction with the conditional part of
//                 the rule, i.e., there is a definite mismatch.
//
//                 If the information vector is incomplete, it may
//                 happen that a rule's antecedent can only partially
//                 verified for firing.  How such situations are to
//                 be handled can be controlled through setting the
//                 the minimum matching fraction (MMF).  Letting N denote
//                 the set of attributes present in the rule's
//                 antecedent and M denote the set of attributes present
//                 in the information vector, the overlap ratio is defined
//                 as:
//
//                   overlap = cardinality(N intersected with M) / cardinality(N);
//
//                 A MMF of 0.0 means that all missing values
//                 are treated as acceptable (i.e. very tolerant firing), while a MMF
//                 1.0 means that no missing values are accepted (i.e. very
//                 intolerant firing).
//
//                 For a rule with missing values to fire, the following must hold:
//
//                   overlap >= MMF
//
// Comments......: Assumes that the information vector has the same
//                 dimensions and interpretation as a single row in
//                 the decision table from which the reduct (from which
//                 the rule is derived) is calculated.
//
// Revisions.....: Aš 990412: Added discerner argument. Implement it
//                            later.
//                 Aš 990727: MMF = 1 - Tolerance.
//===================================================================

bool
Rule::Matches(const InformationVector &object, const Discerner &discerner, float fraction) const {

#ifdef _DEBUG
	if (fraction < 0.0 || fraction > 1.0) {
		Message::Error("Illegal value passed to Rule::Matches method.");
		return false;
	}
#endif

	int i, no_conditions = GetNoConditionDescriptors();

	// Handle special case of the antecedent being empty.
	if (no_conditions == 0)
		return true;

	int overlap_count = 0;

	// Mismatch?
	for (i = 0; i < no_conditions; i++) {

		// Get values.
		int attribute = GetConditionAttribute(i);
		int value     = GetConditionValue(i);
		int thevalue  = object.GetEntry(attribute);

		// Mismatch as defined by user? (Assuming that discernibility is symmetric, the order
		// of the last two arguments passed to Discerner::Discerns should not matter.)
		if (discerner.Discerns(attribute, value, thevalue))
			return false;

		// Verifiable?
		if (thevalue != Undefined::Integer())
			overlap_count++;

	}

	// Avoid numerical quirks.
	if (fraction == 0.0)
		return true;

	float overlap;

	if (overlap_count == no_conditions)
		overlap = 1.0;
	else if (overlap_count == 0)
		overlap = 0.0;
	else
		overlap = static_cast(float, overlap_count) / no_conditions;

	return (overlap >= fraction);

}

//-------------------------------------------------------------------
// Method........: ReserveLHS/ReserveRHS
// Author........: Aleksander šhrn
// Date..........:
// Description...: Can be used to reserve space, if relevant, prior
//                 to creation. Default implementation does nothing.
// Comments......:
// Revisions.....:
//===================================================================

bool
Rule::ReserveLHS(int /*length*/) {
	return true;
}

bool
Rule::ReserveRHS(int /*length*/) {
	return true;
}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990416: Implemented use of Discerner class.
//===================================================================

bool
Rule::Create(const Reduct &reduct, const DecisionTable &table, const Discerner &discerner, int object_no, bool rhs) {

	bool masked = true;

	// Clear current contents.
	Clear();

	int no_objects = table.GetNoObjects(masked);

	if (object_no < 0 || object_no >= no_objects) {
		Message::Error("Illegal object index passed to Rule::Create method.");
		return false;
	}

	int i, j, no_descriptors = reduct.GetNoAttributes();

	// Reserve space.
	if (!ReserveLHS(no_descriptors))
		return false;

	// Create LHS.
	for (i = 0; i < no_descriptors; i++) {
		int attribute = reduct.GetAttribute(i);
		int value     = table.GetEntry(object_no, attribute, masked);
		if (!AppendConditionDescriptor(attribute, value))
			return false;
	}

	Map(int, int) map;

	int decision_attribute = table.GetDecisionAttribute(masked);

	if (rhs) {
		if (decision_attribute == Undefined::Integer())
			return false;
		if (!SetDecisionAttribute(decision_attribute))
			return false;
	}

	int lhs_support = 0;

	// Compute supports.
	for (i = 0; i < no_objects; i++) {
		for (j = 0; j < no_descriptors; j++) {
			int attribute = reduct.GetAttribute(j);
			int value     = table.GetEntry(i, attribute, masked);
			int thevalue  = table.GetEntry(object_no, attribute, masked);
			if (discerner.Discerns(attribute, thevalue, value))
				break;
		}
		if (j == no_descriptors) {
			if (rhs) {
				int decision_value = table.GetEntry(i, decision_attribute, masked);
				map.insert(Pair(const int, int)(decision_value, 0));
				map[decision_value]++;
			}
			lhs_support++;
		}
	}

	if (!SetSupport(lhs_support, 1.0))
		return false;

	// Create RHS?
	if (rhs) {

		// Reserve space.
		if (!ReserveRHS(map.size()))
			return false;

		// This should be a const_iterator, but VC++ 6.0 won't let me...
		Map(int, int)::iterator it;

		// Append decision values.
		for (it = map.begin(); it != map.end(); it++) {
			int decision_value = (*it).first;
			int rhs_support    = (*it).second;
			if (!AppendDecisionValue(decision_value, rhs_support))
				return false;
		}

	}

	return true;

}
 
