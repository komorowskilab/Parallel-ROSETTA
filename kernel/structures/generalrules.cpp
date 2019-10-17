//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........:
// Description...: Class for general rules. Reimplements methods of 
//					Rules for the general ones
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/generalrules.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/reducts.h>
#include <kernel/utilities/discerner.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
//#include <kernel/basic/bits.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class GeneralRules.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

GeneralRules::GeneralRules(const GeneralRules &in) : Rules(in) {
}

GeneralRules::GeneralRules() {
}

GeneralRules::~GeneralRules() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(GeneralRules, GENERALRULES, Rules)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

Structure *
GeneralRules::Duplicate() const {
	return new GeneralRules(*this);
}


//-------------------------------------------------------------------
// Methods inherited from Structures.
//===================================================================


//-------------------------------------------------------------------
// Method........: CreateF
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......: Returns an error -- General Rules are not created 
//					from reducts
// Revisions.....:
//===================================================================

bool
GeneralRules::Create(const Reduct &reduct, const DecisionTable &table, const Discerner &discerner, bool append) {
	
	Message::Error("Creation of general rules not implemented.");

	return false;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Ewa Makosa
// Date..........:
// Description...: Returns an error -- General Rules are not created 
//					from reducts
// Comments......: 
// Revisions.....: 
//===================================================================

bool
GeneralRules::Create(const Reducts &reducts, const DecisionTable &table, const Discerner &discerner, bool append) {

	Message::Error("Creation of general rules not implemented.");

	return false;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetDescriptorSupports
// Author........: Ewa Makosa
// Date..........:
// Description...: Returns (in-place) the individual supports of each
//                 descriptor (attribute-value set pairs) that is in use
//                 by the rule set.
// Comments......: Done in two passes to allow use of reserve method.
//					Implements the method for general rules
// Revisions.....:
//===================================================================

bool
GeneralRules::GetDescriptorSupports(bool rhs, DVMap &descriptors) const {

	// Clear input map.
	descriptors.erase(descriptors.begin(), descriptors.end());

	int i, j, no_rules = GetNoRules();

	if (no_rules == 0)
		return true;

	DIMap dimap;

	// Build index map.
	for (i = 0; i < no_rules; i++) {

		Handle<GeneralRule> rule = GetRule(i);

		// Get length of rule.
		int lhs_length = rule->GetNoConditionDescriptors();
		int rhs_length = rule->GetNoDecisionValues();

		GeneralRule::Descriptor descriptor(0, GeneralRule::ValueSet());

		// Build LHS descriptors.
		for (j = 0; j < lhs_length; j++) {

			descriptor.first  = rule->GetConditionAttribute(j);
 			descriptor.second = rule->GetConditionValues(j);

			// Update map.
			dimap.insert(Pair(const GeneralRule::Descriptor, int)(descriptor, 0));
			dimap[descriptor]++;

		}

		// Don't process RHS?
		if (!rhs)
			continue;

		// Build RHS descriptors.
	
		for (j = 0; j < rhs_length; j++) {

			descriptor.first  = rule->GetDecisionAttribute();
			GeneralRule::ValueSet vs;
			vs.insert(rule->GetDecisionValue(j));
			descriptor.second = vs;

			// Update map.
			dimap.insert(Pair(const GeneralRule::Descriptor, int)(descriptor, 0));
			dimap[descriptor]++;

		}

	}

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	DIMap::iterator iterator = dimap.begin();

	// Initialize vector map.
	while (!(iterator == dimap.end())) {
		descriptors.insert(Pair(const GeneralRule::Descriptor, Vector(int))((*iterator).first, Vector(int)()));
		descriptors[(*iterator).first].reserve((*iterator).second);
		iterator++;
	}

	// Fill vector map.
	for (i = 0; i < no_rules; i++) {

		Handle<GeneralRule> rule = GetRule(i);

		// Get length of rule.
		int lhs_length = rule->GetNoConditionDescriptors();
		int rhs_length = rule->GetNoDecisionValues();

		// Get LHS support of rule.
		int lhs_support = rule->GetSupport();

		GeneralRule::Descriptor descriptor(0, GeneralRule::ValueSet());

		// Collect LHS descriptor supports.
		for (j = 0; j < lhs_length; j++) {

			descriptor.first  = rule->GetConditionAttribute(j);
			descriptor.second = rule->GetConditionValues(j);

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
			GeneralRule::ValueSet vs;
			vs.insert(rule->GetDecisionValue(j));
			descriptor.second = vs;


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
GeneralRules::GetNoDescriptors(bool rhs, int &no_unique) const {

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

//-------------------------------------------------------------------
// Method........: FindMember
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......: Eliminate this method with time -- should be
//                 implemented on a more general level.
// Revisions.....:
//===================================================================

int
GeneralRules::FindMember(const Structure *member, bool physical) const {

	// Physical membership is implemented elsewhere.
	if (physical)
		return Structure::FindMember(member, physical);

	if (member == NULL)
		return Undefined::Integer();

	if (!member->IsA(GENERALRULE))
		return Undefined::Integer();

	// Ugly, but safe.
	Handle<GeneralRule> rule = dynamic_cast(GeneralRule *, const_cast(Structure *, member));

	int i, no_rules = GetNoRules();

	// Check logical membership.
	for (i = 0; i < no_rules; i++) {
		if (*rule == *GetRule(i))
			return i;
	}

	return Undefined::Integer();

}

 
