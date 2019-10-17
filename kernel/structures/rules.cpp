//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/rules.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/reducts.h>

#include <kernel/algorithms/ruleevaluator.h>

#include <kernel/utilities/partitionkit.h>
#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/basic/bits.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#ifdef _OPENMP
#include "omp.h"
#endif

//-------------------------------------------------------------------
// Static stuff (file scope). Hack to use STL sorting.
//===================================================================

static bool         static_compare_ascending_     = true;
static int          static_compare_property_      = 0;
static int          static_compare_no_objects_    = 0;
static Vector(int) *static_compare_decisions_     = NULL;
static Vector(int) *static_compare_cardinalities_ = NULL;

//-------------------------------------------------------------------
// Method........: StaticCreateVectors
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticCreateVectors(const DecisionTable &table, bool masked) {

	// Instantiate vectors?
	if (static_compare_decisions_ == NULL)
		static_compare_decisions_ = new Vector(int);

	if (static_compare_cardinalities_ == NULL)
		static_compare_cardinalities_ = new Vector(int);

	// Clear current contents.
	static_compare_decisions_->erase(static_compare_decisions_->begin(), static_compare_decisions_->end());
	static_compare_cardinalities_->erase(static_compare_cardinalities_->begin(), static_compare_cardinalities_->end());

	// Determine decision attribute.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	// Fill vectors.
	if (!table.GetValueSet(*static_compare_decisions_, *static_compare_cardinalities_, decision_attribute, masked))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticDestroyVectors
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static void
StaticDestroyVectors() {

#if 0 // Causes some weird debug assertion...
	if (static_compare_decisions_ != NULL)
		delete static_compare_decisions_;

	if (static_compare_cardinalities_ != NULL)
		delete static_compare_cardinalities_;
#endif

}

//-------------------------------------------------------------------
// STL comparator methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Comparison operator
// Author........: Aleksander šhrn
// Date..........:
// Description...: Compares two rules according to the value of
//                 one of their specified properties.
// Comments......: Assumes static stuff has been properly set.
// Revisions.....: Aš 981102: Generalized to handle empty RHS.
//                 Aš 980426: Dropped fall-through stuff.
//===================================================================

bool
Rules::Compare::operator()(const Handle<Structure> &a, const Handle<Structure> &b) const {

	if (a == b)
		return false;

	Handle<Rule> rule_a = dynamic_cast(Rule *, const_cast(Structure *, a.GetPointer()));
	Handle<Rule> rule_b = dynamic_cast(Rule *, const_cast(Structure *, b.GetPointer()));

	int no_decisions_a = rule_a->GetNoDecisionValues();
	int no_decisions_b = rule_b->GetNoDecisionValues();

#if 0
	bool has_rhs_a = (no_decisions_a > 0);
	bool has_rhs_b = (no_decisions_b > 0);
#endif
	
	int i;

	float property_a = 0.0;
	float property_b = 0.0;

	int support_a  = 0; // Dummy, not currently in use.
  int support_b  = 0; // Dummy, not currently in use.
	int position_a = 0; // Dummy, not currently in use.
	int position_b = 0; // Dummy, not currently in use.

	switch (static_compare_property_) {
		case 0:  // Decision (dominating).
		         property_a = rule_a->GetDominatingDecisionValue(support_a, position_a);
		         property_b = rule_b->GetDominatingDecisionValue(support_b, position_b);
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
						   break;
						 */
						 break;
		case 1:  // Support (LHS).
             property_a = rule_a->GetSupport();
             property_b = rule_b->GetSupport();
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
							 break;
						 */
						 break;
		case 2:  // Support (RHS).
	           for (i = 0; i < no_decisions_a; i++)
						   property_a = std::max(property_a, static_cast(float, rule_a->GetSupport(i)));
	           for (i = 0; i < no_decisions_b; i++)
						   property_b = std::max(property_b, static_cast(float, rule_b->GetSupport(i)));
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
							 break;
						 */
						 break;
		case 3:
		case 4:  // Accuracy (RHS)
	           for (i = 0; i < no_decisions_a; i++)
						   property_a = std::max(property_a, rule_a->GetAccuracy(i));
	           for (i = 0; i < no_decisions_b; i++)
						   property_b = std::max(property_b, rule_b->GetAccuracy(i));
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
							 break;
						 */
						 break;
		case 5:  // Coverage (LHS).
						 property_a = rule_a->GetCoverage(static_compare_no_objects_);
						 property_b = rule_b->GetCoverage(static_compare_no_objects_);
						 /*
					   if (property_a != property_b)   // If equal, fall-through.
						   break;
						 */
						 break;
		case 6:  // Coverage (RHS).
			       if (static_compare_decisions_ != NULL && static_compare_cardinalities_ != NULL) {
	             for (i = 0; i < no_decisions_a; i++)
						     property_a = std::max(property_a, static_cast(float, rule_a->GetCoverage(i, *static_compare_decisions_, *static_compare_cardinalities_)));
	             for (i = 0; i < no_decisions_b; i++)
						     property_b = std::max(property_b, static_cast(float, rule_b->GetCoverage(i, *static_compare_decisions_, *static_compare_cardinalities_)));
						 }
						 /*
						 if (property_a != property_b) // If equal, fall-through.
							 break;
						 */
						 break;
		case 7:  // Stability (LHS)
						 property_a = rule_a->GetStability();
						 property_b = rule_b->GetStability();
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
							 break;
						 */
						 break;
		case 8:  // Stability (RHS)
	           for (i = 0; i < no_decisions_a; i++)
						   property_a = std::max(property_a, static_cast(float, rule_a->GetStability(i)));
	           for (i = 0; i < no_decisions_b; i++)
						   property_b = std::max(property_b, static_cast(float, rule_b->GetStability(i)));
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
							 break;
						 */
						 break;
		case 9:  // Length (LHS).
		         property_a = rule_a->GetNoConditionDescriptors();
		         property_b = rule_b->GetNoConditionDescriptors();
						 /*
						 if (property_a != property_b)   // If equal, fall-through.
							 break;
						 */
						 break;
		case 10: // Length (RHS).
		         property_a = no_decisions_a;
		         property_b = no_decisions_b;
						 break;
		default: return false;
	}

	if (static_compare_ascending_)
		return (property_a < property_b);
	else
		return (property_a > property_b);

}

//-------------------------------------------------------------------
// Methods for class Rules.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Rules::Rules(const Rules &in) : Structures(in) {
}

Rules::Rules() {
}

Rules::~Rules() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Rules, RULES, Structures)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
Rules::Duplicate() const {
	return new Rules(*this);
}

//-------------------------------------------------------------------
// Method........: FindMember
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Eliminate this method with time -- should be
//                 implemented on a more general level.
// Revisions.....:
//===================================================================

int
Rules::FindMember(const Structure *member, bool physical) const {

	// Physical membership is implemented elsewhere.
	if (physical)
		return Structure::FindMember(member, physical);

	if (member == NULL)
		return Undefined::Integer();

	if (!member->IsA(RULE))
		return Undefined::Integer();

	// Ugly, but safe.
	Handle<Rule> rule = dynamic_cast(Rule *, const_cast(Structure *, member));

	int i, no_rules = GetNoRules();

	// Check logical membership.
	for (i = 0; i < no_rules; i++) {
		if (*rule == *GetRule(i))
			return i;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Methods inherited from Structures.
//===================================================================

//------------------------------------------------------------------
// Method........: InsertStructure
// Author........: Aleksander šhrn
// Date..........:
// Description...: Overloaded to check for consistency/homogenity.
//                 Ensures that only rules are inserted.
// Comments......: AppendStructure is not necessary to overload, since
//                 Structures::AppendStructure is implemented via the
//                 InsertStructure method.
// Revisions.....:
//===================================================================

bool
Rules::InsertStructure(Structure *structure, int i) {

	if (structure == NULL) {
		Message::Error("Cannot insert a NULL rule.");
		return false;
	}

	if (!structure->IsA(RULE)) {
		Message::Error("Cannot insert a non-rule into a rule set.");
		return false;
	}

	return Structures::InsertStructure(structure, i);

}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Sort
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sorts the rules according to one of their specified
//                 properties.
//
//                    0 = Decision (dominating)
//                    1 = Support (LHS)
//                    2 = Support (RHS)
//                 3, 4 = Accuracy (RHS)
//                    5 = Coverage (LHS)
//                    6 = Coverage (RHS)
//                    7 = Stability (LHS)
//                    8 = Stability (RHS)
//                    9 = Length (LHS)
//                   10 = Length (RHS)
//
// Comments......: Options 3 and 4 are the same, just in case a LHS
//                 equivalent should become relevant.
// Revisions.....:
//===================================================================

bool
Rules::Sort(int property, bool ascending, const Handle<DecisionTable> table) {

	bool masked = true;

	// Check property parameter.
	if (property < 0 || property > 10)
		return false;

	// Set static stuff.
	static_compare_property_   = property;
	static_compare_ascending_  = ascending;
	static_compare_no_objects_ = (!table.IsNULL()) ? table->GetNoObjects(masked) : 0;

	if (property == 6) {
		if (table == NULL)
			return false;
		if (!StaticCreateVectors(*table, masked))
			return false;
	}

	Compare comparator;
	Message message;

	message.Notify("Sorting rules...");

	// Sort.
	std::sort(structures_.begin(), structures_.end(), comparator);

	// Cleanup static stuff.
	StaticDestroyVectors();

	return true;

}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Rules::Evaluate(const RuleEvaluator &evaluator, Vector(float) &qualities) const {

	int i, no_rules = GetNoRules();

	qualities.erase(qualities.begin(), qualities.end());
	qualities.reserve(no_rules);

	for (i = 0; i < no_rules; i++)
		qualities.push_back(evaluator.Evaluate(*GetRule(i)));

	return true;

}

//-------------------------------------------------------------------
// Method........: Matches
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a vector of indices of rules
//                 that fire when presented with the specified
//                 information vector.
//
//                 The (in-place) returned vector should be sorted,
//                 as clients may rely on this.
// Comments......:
// Revisions.....:
//===================================================================

bool
Rules::Matches(const InformationVector &object, const Discerner &discerner, float fraction, Vector(int) &indices) const {

	int i, no_rules = GetNoRules();

	// Clear index vector.
	indices.erase(indices.begin(), indices.end());
	indices.reserve(no_rules);

	// Loop over all rules.
	for (i = 0; i < no_rules; i++) {
		if (GetRule(i)->Matches(object, discerner, fraction))
			indices.push_back(i);
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: LoadRulesFromFile
// Author........: Nicholas Baltzer
// Date..........: 07/08/13
// Description...:
// Comments......: Takes a ruleset file (rcl) and loads it into a ruleset.
//			The decision table is needed to map the features into
//			the internal dictionaryi, but is not otherwise not modified 
//			in any way.
// Revisions.....:
//===================================================================
/*
bool
Rules::LoadRulesetFromFile(const String sourcefile, Handle<DecisionTable> table, bool masked) {

	//Cannot return inside a critical section.
	bool success = false;
	#pragma omp critical(loadclassifier)
	{	
		//Load the file.
	        ifstream stream;
	        if (!IOKit::Open(stream, sourcefile)) {
	                Message::Error("Rules::LoadRulesFromFile -> Failed to open "+ sourcefile);
			//No file to load. Abort.
	        } 
	       	else {
			//Keep track of the line for future reference.
			int i = 1;
			
			while (!IOKit::IsEOF(stream)) { 
	        		//Create a new rule.
				Handle<Rule> rule = Creator::Rule();
				//Load the rule from the file.
			        if (!rule->LoadMappedRule(stream, table, masked)) {
					//Didn't work. However, there may be other rules in the set that work, so we can't abort.
					Message::Error("Rules::LoadRulesetFromFile -> Failed to load rule on line " + String::Format(i));
				}
				//Append the rule to the ruleset.
		        	else if (!AppendStructure(rule.GetPointer())) {
					//Didn't work.
					Message::Error("Rules::LoadRulesetFromFile -> Failed to append rule to ruleset");
				}
				else {
					//Atleast one rule has been loaded into the ruleset, so we have a success.
					success = true;
				}
				//Next line;
				++i;
				
				
	        	}
    	
		}
		//And done.
		IOKit::Close(stream);
	}
	//Success means that at least one rule was added to the ruleset.
        return success;       
}   
*/
bool
Rules::LoadRulesetFromFile(const String sourcefile, Handle<DecisionTable> table, bool masked) {

	//Cannot return inside a critical section.
	bool success = false;
	#pragma omp critical(loadclassifier)
	{	
		//Load the file.
	        ifstream stream;
	        if (!IOKit::Open(stream, sourcefile)) {
	                Message::Error("Rules::LoadRulesFromFile -> Failed to open "+ sourcefile);
			//No file to load. Abort.
	        } 
	       	else {
			//Keep track of the line for future reference.
			Message message;
			int nLoaded = 0, nFailed = 0;
			String line;			
			int line_no = 0;
			#pragma omp critical(GetToken)
			{
			while (IOKit::LoadLine(stream, line, &line_no)) { 
	        		//Create a new rule.
				Handle<Rule> rule = Creator::Rule();
				//Load the rule from the file.
			        if (!rule->LoadMappedRuleFromString(line, table, masked)) {
					//Didn't work. However, there may be other rules in the set that work, so we can't abort.
					Message::Warning("Rules::LoadRulesetFromFile -> Failed to load rule on line " + String::Format(line_no));
					++nFailed;
				}
				//Append the rule to the ruleset.
		        	else if (!AppendStructure(rule.GetPointer())) {
					//Didn't work.
					Message::Error("Rules::LoadRulesetFromFile -> Failed to append rule to ruleset");
				}
				else {
					//Atleast one rule has been loaded into the ruleset, so we have a success.
					success = true;
					++nLoaded;
				}
				//Next line;
//				++i;
				
				
	        	}
			}
			message.Notify("Ruleset: " + sourcefile + "\nLoaded:\t" + String::Format(nLoaded) + " \nFailed:\t" + String::Format(nFailed) + "\n Total:\t" + String::Format(line_no - 1));
    	
		}
		//And done.
		IOKit::Close(stream);
	}
	//Success means that at least one rule was added to the ruleset.
        return success;       
}   


//-------------------------------------------------------------------
// Method........: CreateF
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Quick initial implementation. It is easy to lower
//                 the current computational complexity somewhat, but
//                 that is left for later should it be needed. For now,
//                 use Rule::Create as it is, and spend a little more
//                 time than strictly needed to compute the support
//                 counts.
// Revisions.....:
//===================================================================

bool
Rules::Create(const Reduct &reduct, const DecisionTable &table, const Discerner &discerner, bool append) {

	bool masked = true;

	int no_objects    = table.GetNoObjects(masked);

	// Clear current rule set?
	if (!append) 
		Clear();

	Vector(int) indices;
	int         no_partitions;
	Vector(int) cardinalities;

	// Compute the equivalence classes.
	if (!PartitionKit::ComputePartitionIndices(indices, no_partitions, reduct, table, masked, &cardinalities))
		return false;

	Bits processed(no_partitions, false);

	int i;

	// Create one rule per equivalence class.
	for (i = 0; i < no_objects; i++) {

		// Already processed this eq. class?
		if (processed.GetState(indices[i]))
			continue;

		Handle<Rule> rule = Creator::Rule();

		if (!rule->Create(reduct, table, discerner, i, reduct.IsModuloDecision())) {
			Message::Error("Failed to create rule from reduct.");
			return false;
		}

		if (!AppendStructure(rule.GetPointer())) {
			Message::Error("Failed to add new rule to rule set.");
			return false;
		}

		// Flag this eq. class as processed.
		processed.SetState(indices[i], true);

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: For each reduct in the reduct set, creates one or
//                 more rules (depending on the reduct type) and adds
//                 these to the rule set.
// Comments......: 
// Revisions.....: Aš 000331: Added check for duplicates.
//===================================================================

bool
Rules::Create(const Reducts &reducts, const DecisionTable &table, const Discerner &discerner, bool append) {


	if (!append)
		Clear();

	int i, j, no_reducts = reducts.GetNoReducts();

	if (no_reducts == 0)
		return true;

	Message message;

	if (reducts.GetReduct(0)->IsModuloDecision(true)) // TRH 02.06.04
		message.Notify("Computing rules...");
	else
		message.Notify("Computing patterns...");


{
	// Process all reducts. Dispatch on reduct type.
	for (i = 0; i < no_reducts; i++) {
		if (!message.Progress("Create rule " + String::Format(i + 1) + " of " + String::Format(no_reducts) + "...", i, no_reducts))
			break;

		Reduct::Discernibility type = reducts.GetReduct(i)->GetDiscernibilityType();

		// Discernibility relative to the full system? 
		if (type == Reduct::DISCERNIBILITY_FULL) {

			// Create one rule per equivalence class.
			if (!Create(*reducts.GetReduct(i), table, discerner, true)) {
				Message::Error("Failed to create rules from reduct.");
				return false;
			}
		}

		// Discernibility relative to an object?
		else if (type == Reduct::DISCERNIBILITY_OBJECT) {

			Handle<Rule> rule = Creator::Rule();
			// Create one rule from a particular object.
			if (!rule->Create(*(reducts.GetReduct(i)), table, discerner, reducts.GetReduct(i)->GetObject(), reducts.GetReduct(i)->IsModuloDecision())) {
				Message::Error("Failed to create rule from reduct.");
				return false;
			}
			bool is_unique = true;

			// Avoid duplicates..
			for (j = GetNoRules() - 1; j >= 0; j--) {
				if (*rule == *GetRule(j)) {
					is_unique = false;
					break;
				}
			}
			if (is_unique && !AppendStructure(rule.GetPointer())) {
				Message::Error("Failed to add new rule to rule set.");
				return false;
			}
		}
		// Not implemented, whatever the type is.
		else {
			Message::Error("Discernibility type unknown.");
			return false;
		}
	}
}
	return true;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetDescriptorSupports
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the individual supports of each
//                 descriptor (attribute-value pairs) that is in use
//                 by the rule set.
// Comments......: Done in two passes to allow use of reserve method.
// Revisions.....:
//===================================================================

bool
Rules::GetDescriptorSupports(bool rhs, DVMap &descriptors) const {

	// Clear input map.
	descriptors.erase(descriptors.begin(), descriptors.end());

	int i, j, no_rules = GetNoRules();

	if (no_rules == 0)
		return true;

	DIMap dimap;

	// Build index map.
	for (i = 0; i < no_rules; i++) {

		Handle<Rule> rule = GetRule(i);

		// Get length of rule.
		int lhs_length = rule->GetNoConditionDescriptors();
		int rhs_length = rule->GetNoDecisionValues();

		Rule::Descriptor descriptor(0, 0);

		// Build LHS descriptors.
		for (j = 0; j < lhs_length; j++) {

			descriptor.first  = rule->GetConditionAttribute(j);
			descriptor.second = rule->GetConditionValue(j);

			// Update map.
			dimap.insert(Pair(const Rule::Descriptor, int)(descriptor, 0));
			dimap[descriptor]++;

		}

		// Don't process RHS?
		if (!rhs)
			continue;

		// Build RHS descriptors.
		for (j = 0; j < rhs_length; j++) {

			descriptor.first  = rule->GetDecisionAttribute();
			descriptor.second = rule->GetDecisionValue(j);

			// Update map.
			dimap.insert(Pair(const Rule::Descriptor, int)(descriptor, 0));
			dimap[descriptor]++;

		}

	}

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	DIMap::iterator iterator = dimap.begin();

	// Initialize vector map.
	while (!(iterator == dimap.end())) {
		descriptors.insert(Pair(const Rule::Descriptor, Vector(int))((*iterator).first, Vector(int)()));
		descriptors[(*iterator).first].reserve((*iterator).second);
		iterator++;
	}

	// Fill vector map.
	for (i = 0; i < no_rules; i++) {

		Handle<Rule> rule = GetRule(i);

		// Get length of rule.
		int lhs_length = rule->GetNoConditionDescriptors();
		int rhs_length = rule->GetNoDecisionValues();

		// Get LHS support of rule.
		int lhs_support = rule->GetSupport();

		Rule::Descriptor descriptor(0, 0);

		// Collect LHS descriptor supports.
		for (j = 0; j < lhs_length; j++) {

			descriptor.first  = rule->GetConditionAttribute(j);
			descriptor.second = rule->GetConditionValue(j);

			// Update vector.
			descriptors[descriptor].push_back(lhs_support);

		}

		// Don't process RHS?
		if (!rhs)
			continue;

		// Collect RHS descriptor supports.
		for (j = 0; j < rhs_length; j++) {

			// Get RHS support of rule.
			int rhs_support = rule->GetSupport(j);

			descriptor.first  = rule->GetDecisionAttribute();
			descriptor.second = rule->GetDecisionValue(j);

			// Update vector.
			descriptors[descriptor].push_back(rhs_support);

		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNoDescriptors
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of descriptors (attribute-value
//                 pairs) that are in use by the rule set.
//
//                 Provides a crude measure of the complexity of the
//                 rule set.
// Comments......:
// Revisions.....:
//===================================================================

int
Rules::GetNoDescriptors(bool rhs, int &no_unique) const {

	no_unique = 0;

	DVMap descriptors;

	// Get individual supports.
	if (!GetDescriptorSupports(rhs, descriptors))
		return 0;

	// Return (in-place) the number of unique descriptors.
	no_unique = descriptors.size();

	int sum = 0;

	// Accumulate descriptor counts.
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	DVMap::iterator it = descriptors.begin();

	while (!(it == descriptors.end())) {
		sum += (*it).second.size();
		it++;
	}

	return sum;

}
 
