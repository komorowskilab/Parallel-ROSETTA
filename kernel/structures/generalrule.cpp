//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 2005.05.31
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/utilities/discerner.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>
#include <kernel/utilities/iokit.h>
#include <kernel/structures/generalrule.h>
#include <kernel/basic/interval.h>

//-------------------------------------------------------------------
// Methods for class generalrule.
//===================================================================

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
GeneralRule::GeneralRule(const GeneralRule &in) : Rule(in) {
	
	condition_attributes_ = in.condition_attributes_;
	
	for (int desc=0; desc<in.GetNoConditionDescriptors(); desc++){
		GeneralRule::ValueSet values = in.GetConditionValues(desc);
		condition_values_.push_back(values);
	}
	
	condition_support_    = in.condition_support_;
	decision_attribute_   = in.decision_attribute_;
	decision_             = in.decision_;
}


GeneralRule::GeneralRule()
{
	SetDecisionAttribute(Undefined::Integer());
	SetSupport(0);
}

GeneralRule::~GeneralRule(){}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(GeneralRule, GENERALRULE, Rule)

//-------------------------------------------------------------------
// Methods inherited from Persistent
//===================================================================

//------------------------------------------------------------------
// Method........: Load
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRule::Load(ifstream &stream) {
	
	Clear();
	
	int i, j, a, v, n, nv, s;
	
	if (!IOKit::Load(stream, n))
		return false;
	
	if (!ReserveLHS(n))
		return false;
	
	for (i = 0; i < n; i++) {
		
		if (!IOKit::Load(stream, a))
			return false;
		
		if (!IOKit::Load(stream, nv))
			return false;
		
		GeneralRule::ValueSet values;
		for (j = 0; j < nv; j++) {
			if (!IOKit::Load(stream, v))
				return false;
			values.insert(v);
		}
		
		if (!AppendConditionDescriptor(a, values))
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
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRule::Save(ofstream &stream) const {
	
	if (!IOKit::Save(stream, GetNoConditionDescriptors()))
		return false;
	
	int i;
	
	for (i = 0; i < GetNoConditionDescriptors(); i++) {
		if (!IOKit::Save(stream, GetConditionAttribute(i)))
			return false;
		if (!IOKit::Save(stream, GetNoConditionValues(i)))
			return false;
		GeneralRule::ValueSet::const_iterator it;

		for (it = GetConditionValues(i).begin(); it != GetConditionValues(i).end(); it++){
			if (!IOKit::Save(stream, *it))
				return false;
		}
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

//-------------------------------------------------------------------
// Method........: Matches
// Author........: Ewa Makosa
// Date..........:
// Description...: See description in Rule class
//
// Comments......: Overloaded Matches from class Rule to define
//					matching of attribute value if one of the values
//					of a general descriptor matches the value in the
//					information vector given
//
// Revisions.....: 
//===================================================================

bool
GeneralRule::Matches(const InformationVector &object, const Discerner &discerner, float fraction) const {

#ifdef _DEBUG
	if (fraction < 0.0 || fraction > 1.0) {
		Message::Error("Illegal value passed to GeneralRule::Matches method.");
		return false;
	}
#endif
	
	int i, no_conditions = GetNoConditionDescriptors();
	
	// Handle special case of the antecedent being empty.
	if (no_conditions == 0){
		return true;
	}
	
	int overlap_count = 0;
	
	// Mismatch?
	for (i = 0; i < no_conditions; i++) {
		
		// Get values.
		int attribute = GetConditionAttribute(i);
		int thevalue  = object.GetEntry(attribute);
		int value;
		bool found = false;
		ValueSet::const_iterator it;

		for (it = GetConditionValues(i).begin(); it != GetConditionValues(i).end(); it++){
		
			value     = *it;
			if (!discerner.Discerns(attribute, value, thevalue)){//the same value was found
				
				found = true;
				break;
			}
		}

		if (!found)//we went through the whole set of values without finding any undiscernible value
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


GeneralRule &
GeneralRule::operator=(const GeneralRule &in) {
	
	int i;
	
	// Clear this rule.
	Clear();
	
	int no_condition_descriptors = in.GetNoConditionDescriptors();
	
	// Copy the condition attributes and their values.
	for (int desc=0; desc<in.GetNoConditionDescriptors(); desc++){
		
		GeneralRule::ValueSet values;
		
		GeneralRule::ValueSet::const_iterator it = in.GetConditionValueIterator(desc);
		for (int val = 0; val < in.GetNoConditionValues(desc); val++){
			values.insert(*it);
			it++;
		}

		AppendConditionDescriptor(in.GetConditionAttribute(desc), values);
	}	

	// Copy the decision attribute.
	SetDecisionAttribute(in.GetDecisionAttribute());
	
	int no_decision_values = in.GetNoDecisionValues();
	
	// Copy all decision values.
	for (i = 0; i < no_decision_values; i++)
		AppendDecisionValue(in.GetDecisionValue(i), in.GetSupport(i));
	
	SetSupport(in.GetSupport(), 1.0);

	return *this;
}

bool
GeneralRule::operator==(const GeneralRule &in) const {
	
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
		
		for (i = 0; i < no_condition_descriptors; i++){
		
			if (GetNoConditionValues(i) != in.GetNoConditionValues(i))
				return false;
			
			GeneralRule::ValueSet::const_iterator it_this, it_that; 
			it_this = GetConditionValueIterator(i);
			it_that = in.GetConditionValueIterator(i);
			
			for (int val = 0; val < GetNoConditionValues(i); val++){
				if ((*it_this) != (*it_that))
					return false;
				it_this++;
				it_that++;
			}
		}
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
// Operator......: < operator
// Author........:
// Date..........:
// Description...: A rule R is said to be less general (or more
//                 specific) than a rule S, written R < S, if:
//
//                 i)  R's LHS is a proper superset of S's LHS, and
//                 ii) R's RHS is a subset of S's RHS
//
//					In case of general rules, 
//					i) LHS of R is a proper subset of S's LHS if 
//					a set A_r of all attributes of R is a subset of 
//					S's attributes and if for all those attributes 
//					from A_r their value sets are equal to value sets
//					of responding attributes of S
//					ii) The same as in case of a classic rule
//					
// Comments......: See description in Rule class
// Revisions.....:
//===================================================================

bool
GeneralRule::operator<(const GeneralRule &in) const {
	
	if (&in == this)
		return false;
	
	// Get LHS dimensions.
	int no_descriptors_this = GetNoConditionDescriptors();
	int no_descriptors_that = in.GetNoConditionDescriptors();
	
	// Can we exclude the LHS proper superset condition already now?
	if (no_descriptors_this <= no_descriptors_that)
		return false;
	
	int i, j;
	bool found_a, found_v;

	// Check LHS superset condition.
	for (i = 0; i < no_descriptors_that; i++) {
		
		int a = in.GetConditionAttribute(i);
		int val_this, val_that, v;

		found_a = false;

		GeneralRule::ValueSet::const_iterator it_this, it_that;
		it_that = in.GetConditionValueIterator(i);
		
		for (j = 0; j < no_descriptors_this; j++) {

			if (GetConditionAttribute(j) == a){

				found_a = true;
				
				for (val_that = 0; val_that<in.GetNoConditionValues(i); val_that++){
			
					v = *it_that;
			
					found_v = false;
					
					it_this = GetConditionValueIterator(j);

					for (val_this = 0; val_this<GetNoConditionValues(j); val_this++){
			
						if ((*it_this) == v) {
							found_v = true;
							break;
						}
						it_this++;
					}
					if (!found_v)
						return false;
			
					it_that++;
				}
			}
			
		}
		if (!found_a)
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
// Method........: AttrLHSSubsetOf
// Author........: Ewa Makosa
// Date..........:
// Description...: checks if a set of attributes from the LHS 
//					is a subset of attributes of ins LHS
//					
// Comments......: 
// Revisions.....: 
//===================================================================
 
bool 
GeneralRule::AttrLHSSubsetOf(const Handle<GeneralRule> in)const{

	// Get LHS dimensions.
	int no_descriptors_r1 = GetNoConditionDescriptors();
	int no_descriptors_r2 = in->GetNoConditionDescriptors();

	// Can we exclude the LHS proper superset condition already now?
	if (no_descriptors_r1 > no_descriptors_r2)
		return false;

	int i, j;

	// Check LHS superset condition.
	for (i = 0; i < no_descriptors_r1; i++) {

		int a = GetConditionAttribute(i);
	
		bool found = false;

		for (j = 0; j < no_descriptors_r2; j++) {
			if (in->GetConditionAttribute(j) == a) {
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
GeneralRule::Duplicate() const {
	return new GeneralRule(*this);
}


//------------------------------------------------------------------
// Method........: RemoveConditionDescriptor
// Author........: Ewa Makosa
// Date..........:
// Description...: Removes an (attribute, value) pair from the
//                 condition part of the rule at the attribute_no position
//
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRule::RemoveConditionDescriptor(int position_no){
	
	condition_attributes_.erase(condition_attributes_.begin() +position_no);
	condition_values_[position_no].erase(condition_values_[position_no].begin(), condition_values_[position_no].end());
	condition_values_.erase(condition_values_.begin() +position_no);
	return true;
};

//-------------------------------------------------------------------
// Method........: Reevaluate
// Author........: Ewa Makosa
// Date..........:
// Description...: Calculate all the rules specific values
// Comments......:
// Revisions.....: 
//===================================================================

bool
GeneralRule::Reevaluate(const Discerner &discerner, const DecisionTable &decisiontable, bool rhs) {
	
	bool masked = true;
	
	// Clear current decision settings.
	decision_.Clear();
	SetSupport(0);
	
	int decision_attribute = decisiontable.GetDecisionAttribute(masked);
	
	if (rhs) {
		if (decision_attribute == Undefined::Integer())
			return false;
		if (!SetDecisionAttribute(decision_attribute))
			return false;
	}
	
	int no_objects = decisiontable.GetNoObjects(masked);
	
	int i, j;
	
	Map(int, int) map;
	
	int lhs_support = 0;
	
	// Compute supports.
	
	for (i = 0; i < no_objects; i++) {

		for (j = 0; j < GetNoConditionDescriptors(); j++) {

			int attribute = GetConditionAttribute(j);
			int thevalue  = decisiontable.GetEntry(i, attribute, masked);
			bool found = false;
			GeneralRule::ValueSet::const_iterator it;
			for (it = GetConditionValues(j).begin(); it != GetConditionValues(j).end(); it++){
		
				int value = *it;
		
				//have we found one of the values matching te objects value?
				if (!discerner.Discerns(attribute, thevalue, value)){
					found = true;
					break;
				}
			}
			//none of the values matched?
			if (!found)
				break;
		}
		if (j == GetNoConditionDescriptors()) {//for each desc we have found its value
		
			if (rhs) {
				int decision_value = decisiontable.GetEntry(i, decision_attribute_, masked);
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


//-------------------------------------------------------------------
// Methods inherited from Rule.
//===================================================================

//------------------------------------------------------------------
// Method........: Clear
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
GeneralRule::Clear() {
	
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
bool
GeneralRule::Format(String &formatted, const DecisionTable *table, bool masked) const {
	
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
	const String values         = " OR ";  // " || "
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
	
	int i, lastNo, count;
	// Build conditional part (antecedent, LHS) of rule.
	for (i = 0; i < no_conditions; i++) {
		
		// Get (attribute, value) pair.
		int attribute = GetConditionAttribute(i);
		String a_name = (table == NULL) ? String::Format(attribute) : table->GetAttributeName(attribute, masked);
		int value;
		String v_name;
		antecedent += descriptor_prefix;
		antecedent += a_name;
		antecedent += descriptor_infix;
		
		lastNo = GetNoConditionValues(i) - 1;
		count = 0;
		GeneralRule::ValueSet::const_iterator it;
		for (it = GetConditionValues(i).begin(); it != GetConditionValues(i).end();it++){
			
			value     = *it;
			v_name = (table == NULL) ? String::Format(value) : table->GetDictionaryEntry(attribute, value, masked);
			
			// Append descriptor.
			antecedent += v_name;
			if (count != lastNo)
				antecedent += values;
			count++;
		}
		
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
			consequent += values;
		
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
// Method........: FormatProlog
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......: If a decision table with an associated dictionary
//                 is supplied, this is used.
// Revisions.....:
//===================================================================

bool
GeneralRule::FormatProlog(String &formatted, const DecisionTable *table, bool masked) const {
	
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
	
	String body = "";
	
	int i, j, b, amount;
	//all possible value combinations
	Vector(String) bodies;

	// Build all possible bodies of Prolog clause.
	for (i = 0; i < no_conditions; i++) {
	
		int attribute_no = GetConditionAttribute(i);
		String caname = (table == NULL) ? "a" + String::Format(attribute_no) : table->GetAttributeName(attribute_no, masked);
		caname.ToLowercase();

		// Do we have to add quotes?
		if (caname.Contains(' ') || caname.Contains(','))
			caname = "\'" + caname + "\'";
		
		GeneralRule::ValueSet::const_iterator it = GetConditionValueIterator(i);

		//the existing bodies should be all copied as many times as there are NoConditionDisjunctionValues(i)
		amount = bodies.size();
		for (j = 0; j < GetNoConditionValues(i) - 1; j++){
			for (b = 0; b < amount; b++){
				String body_copy = bodies[b];
				bodies.push_back(body_copy);
			}
		}
		
		//Message::Warning(caname+" has "+String::Format(GetNoConditionDisjunctionValues(i))+" values ");
		for (j = 0; j < GetNoConditionValues(i); j++){
			// Get (attribute, value) pair.
			int value        = *it;
			it++;
			// Get names of attribute and descriptor.
			String dvname = (table == NULL) ? String::Format(value) : table->GetDictionaryEntry(attribute_no, value, masked);
			//Message::Warning(" Value "+String::Format(value)+" of an attribute "+caname+" got name "+dvname);
			// Uppercase is reserved for variables in Prolog.
		
			dvname.ToLowercase();
		
		
			// Is the decision value an interval?
			bool is_interval = Interval(dvname).IsValid();
		
			// Append clause(s) to amount of bodies
			if (amount == 0){
				if (!is_interval) {
					if (dvname.Contains(' ') || dvname.Contains(','))
						dvname = "\'" + dvname + "\'";
					bodies.push_back('\t' + caname + "(X, " + dvname + ")"+((i < no_conditions - 1) ? ",\n" : "."));
				}
				else {
					String varname = "V" + String::Format(i);
					bodies.push_back('\t' + caname + "(X, " + varname + "),\n"+ FormatPrologInterval(varname, dvname, "\t\t")+((i < no_conditions - 1) ? ",\n" : "."));
				}
			}else{
				for (b = 0; b < amount; b++){
					if (!is_interval) {
						if (dvname.Contains(' ') || dvname.Contains(','))
							dvname = "\'" + dvname + "\'";
						bodies[j*amount + b] += '\t' + caname + "(X, " + dvname + ")";
					}
					else {
						String varname = "V" + String::Format(i);
						bodies[j*amount + b] += '\t' + caname + "(X, " + varname + "),\n";
						bodies[j*amount + b] += FormatPrologInterval(varname, dvname, "\t\t");
					}
					bodies[j*amount + b] += (i < no_conditions - 1) ? ",\n" : ".";
				}
			}
		
		}
		
	}
	
	// Is the RHS empty?
	if (!HasRHS()) {
		
		// Get quantitative information.
		int    supp = GetSupport();
		double stab = GetStability();
		
		// Add head to bodies.
		for (b = 0; b < amount; b++){
			formatted += "match(X, _, " + String::Format(supp) + ", _, " + String::Format(stab) + ") :-\n" + bodies[b];
		}
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
		
		amount = bodies.size();
		for (b = 0; b < amount; b++){
			// Append head of clause.
			formatted += head;
			
			// Is the clause really a fact?
			if (bodies[b].IsEmpty()) {
				formatted += '.';
				if (i < no_decisions - 1)
					formatted += "\n\n";
				continue;
			}
			
			// Append implication sign.
			formatted += " :-\n";
			
			// Append body of clause.
			formatted += bodies[b];
			
			if ((i < no_decisions - 1) || (b < amount -1))
				formatted += "\n\n";
		}
		
	}
	
	return true;
	
}

//------------------------------------------------------------------
// Method........: GetNoConditionDescriptors
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
GeneralRule::GetNoConditionDescriptors() const {
	//	Message::Warning("GeneralRule: GetNoConditionDescriptors "+String::Format(condition_attributes_.size()));
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
GeneralRule::GetConditionAttribute(int position_no) const {
	
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
GeneralRule::GetConditionValue(int position_no) const {
	
	Message::Error("GeneralRule:  GetConditionValue(int position_no) not implemented.");
	return Undefined::Integer();
	
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
GeneralRule::AppendConditionDescriptor(int attribute_no, int value) {
	
	condition_attributes_.push_back(attribute_no);
	GeneralRule::ValueSet values;
	values.insert(value);
	condition_values_.push_back(values);
	
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
GeneralRule::SetDecisionAttribute(int attribute_no) {
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
GeneralRule::GetDecisionAttribute() const {
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
GeneralRule::GetGeneralizedDecision() const {
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
GeneralRule::GetNoDecisionValues() const {
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
GeneralRule::AppendDecisionValue(int value, int support, double /*stability*/) {
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
GeneralRule::GetDecisionValue(int position_no) const {
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
GeneralRule::GetSupport(int position_no) const {
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
GeneralRule::GetStability(int /*position_no*/) const {
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
GeneralRule::SetSupport(int /*position_no*/, int /*support*/, double /*stability*/) {
	Message::Error("GeneralRule::SetSupport not implemented.");
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
GeneralRule::GetSupport() const {
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
GeneralRule::SetSupport(int support, double /*stability*/) {
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
GeneralRule::GetStability() const {
	return 1.0;
}

//-------------------------------------------------------------------
// Method........: ReserveLHS/ReserveRHS
// Author........: Ewa Makosa
// Date..........:
// Description...: Reserves space prior to creation.
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralRule::ReserveLHS(int length) {
	condition_attributes_.reserve(length);
	condition_values_.reserve(length);
	
	return true;	
}

bool
GeneralRule::ReserveRHS(int length) {
	return decision_.Reserve(length);
}


int
GeneralRule::GetNoConditionValues(int position_no) const{
	return condition_values_[position_no].size();
};

GeneralRule::ValueSet::const_iterator
GeneralRule::GetConditionValueIterator(int position_no)const{
	
#ifdef _DEBUG
	// Is the index argument legal?
	if ((position_no < 0) || (position_no >= GetNoConditionDescriptors())) {
		Message::Error("GeneralRule position index out of range.");
		return NULL;
	}
#endif
	return condition_values_[position_no].begin();
};

const GeneralRule::ValueSet&
GeneralRule::GetConditionValues(int position_no)const{
	
#ifdef _DEBUG
	// Is the index argument legal?
	if ((position_no < 0) || (position_no >= GetNoConditionDescriptors())) {
		Message::Error("Rule position index out of range.");
		return condition_values_[0];
	}
#endif
	return condition_values_[position_no];
};

bool
GeneralRule::AppendConditionDescriptor(int attribute_no, const GeneralRule::ValueSet &values){
	condition_attributes_.push_back(attribute_no);
	condition_values_.push_back(values);
	return true;
};


bool
GeneralRule::AddConditionValue(int position_no, int value){
	condition_values_[position_no].insert(value);
	return true;
};

 
