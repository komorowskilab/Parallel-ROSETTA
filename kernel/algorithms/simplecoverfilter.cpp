//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 01.06.2005
// Description...: Simple coverage-based filtering 
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/algorithms/simplecoverfilter.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/utilities/creator.h>
#include <kernel/basic/message.h>
#include <kernel/utilities/discerner.h>
//-------------------------------------------------------------------
// Methods for class SimpleCoverFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

SimpleCoverFilter::SimpleCoverFilter() {
	SetThreshold(2);
	UseIDG(false);
	SetIDGFilename(Undefined::String());
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

SimpleCoverFilter::~SimpleCoverFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(SimpleCoverFilter, SIMPLECOVERFILTER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
SimpleCoverFilter::GetParameters() const {
	
	String parameters;
	// Threshold
	parameters += Keyword::Separator();
	parameters += Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetThreshold());

	// Use IDG?
	parameters += Keyword::Separator();
	parameters += Keyword::IDG();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseIDG());
	
	// IDG filename.
	if (UseIDG()) {
		parameters += Keyword::Separator();
		parameters += Keyword::IDG() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIDGFilename();
	}

	return parameters;
}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Ewa Makosa
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
SimpleCoverFilter::SetParameter(const String &keyword, const String &value) {
	//Threshold
	if (keyword == Keyword::Threshold() && value.IsInteger())
		return SetThreshold(value.GetInteger());

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());
	
	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);

	return false;
	
}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Ewa Makosa
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
SimpleCoverFilter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(RULES);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Ewa Makosa
// Date..........:
// Description...: Takes a rule set as input and filters it according
//                 to a simple coverage criterion.
//
// Comments......: 
// Revisions.....: 
//===================================================================

Structure *
SimpleCoverFilter::Apply(Structure &structure) const {
	// Check input.
	if (!IsApplicable(structure))
		return NULL;
	
	Handle<Rules> rules = dynamic_cast(Rules *, &structure);

	if (rules == NULL)
		return NULL;
	
	Discerner discerner;
	
	Handle<DecisionTable> decisiontable = dynamic_cast(DecisionTable *, rules->FindParent(DECISIONTABLE));
	bool masked = true;
	// Initialize discerner.	
	if (UseIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *decisiontable, masked)) {
			Message::Error("Failed to load IDGs from " + GetIDGFilename() + ".");
			return NULL;
		}
	}
	Filter(decisiontable, discerner, rules);
	return rules.Release();
}

void 
SimpleCoverFilter::Filter(const Handle<DecisionTable> table, 
						  const Discerner &discerner, 
						  Handle<Rules> rules)const {
	
	int rulesNo = rules->GetNoRules();

	if (rulesNo < GetThreshold())
		return;

	int u, i, j, k;
	int objectsNo = table->GetNoObjects(false);
	float fraction = 1.0;
	int sortProperty = 1;
	Handle<InformationVector> inf = Creator::InformationVector();
	UsedRules usedRules;

	rules->Sort(sortProperty, false, table);
	InitializeUsedRules(rulesNo, usedRules);

	for (i = 0; i < objectsNo; i++) {
				
		if (!inf->Create(*table, i, true))
			return;
		u = 0;

		for (j = 0; j < rulesNo; j++) {

			if ((rules->GetRule(j))->Matches(*inf, discerner, fraction)) {
				
				usedRules[j] = true;
				u++;
				if (u == GetThreshold())
					break;
			} 
		}
	}

	RemoveNotUsed(usedRules, rules);
}

void 
SimpleCoverFilter::InitializeUsedRules(int rulesNo, UsedRules &usedRules)const{
	
	for (int i = 0; i < rulesNo; i++) {
		bool used = false;
		usedRules.push_back(used);
	}
};

void 
SimpleCoverFilter::RemoveNotUsed(const UsedRules &usedRules, Handle<Rules> rules)const{
	
	int rulesNo = rules->GetNoRules();

	for (int i = rulesNo - 1; i >= 0 ; i--) {
		if (!usedRules[i])
			rules->RemoveStructure(i);
	}
} 

SimpleCoverFilter *
SimpleCoverFilter::Clone() {
	return new SimpleCoverFilter;
}
