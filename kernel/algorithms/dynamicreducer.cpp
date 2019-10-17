//-------------------------------------------------------------------
// Author........: Robin Andersson
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/dynamicreducer.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/rulegenerator.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/rules.h>

#include <common/objectmanager.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/discerner.h>

//-------------------------------------------------------------------
// Methods for class DynamicReducer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

DynamicReducer::DynamicReducer(){
	// initialize parameters
	upper_percentage_ = 90;
	lower_percentage_ = 50;
	generalized_ = true;
	no_levels_ = 5;
	no_samples_ = 10;
	seed_ = 52341;
	threshold_ = 0.7f;
	reducer_ = dynamic_cast(Reducer*, ObjectManager::GetIdentifiedAlgorithm(SAVGENETICREDUCER));
	
	// global number of objects
	no_unmasked_objects_ = 0;
}

DynamicReducer::~DynamicReducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DynamicReducer, DYNAMICREDUCER, Reducer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

String
DynamicReducer::GetParameters() const {
	
	String parameters = "";
	
	//Samples
	parameters += Keyword::Samples();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoSamples());
	
	parameters += Keyword::Separator();
	
	//Levels
	parameters += Keyword::Levels();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoLevels());
	
	parameters += Keyword::Separator();
	
	//Seed
	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());
	
	parameters += Keyword::Separator();
	
	// Lower percentage
	parameters += Keyword::Percentage() + Keyword::Dot() + Keyword::Lower();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetLowerPercentage());
	
	parameters += Keyword::Separator();
	
	// Upper percentage
	parameters += Keyword::Percentage() + Keyword::Dot() + Keyword::Upper();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetUpperPercentage());
	
	parameters += Keyword::Separator();
	
	// Generalized
	parameters += Keyword::Generalized();
	parameters += Keyword::Assignment();
	parameters += String::Format(Generalized());
	
	parameters += Keyword::Separator();
	
	// Reducer.
	parameters += Keyword::Reducer();
	parameters += Keyword::Assignment();
	
	if (GetReducer().IsNULL()) {
		parameters += "NULL";
	}
	else {
		parameters += IdHolder::GetClassname(GetReducer()->GetId());
		parameters += Keyword::Separator();
		parameters += GetReducer()->GetParameters();
	}
	
	return parameters;
	
}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

bool
DynamicReducer::SetParameter(const String &keyword, const String &value) {
	
	// Seed.
	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());
	
	// Levels.
	if (keyword == Keyword::Levels() && value.IsInteger())
		return SetNoLevels(value.GetInteger());
	
	// Samples.
	if (keyword == Keyword::Samples() && value.IsInteger())
		return SetNoSamples(value.GetInteger());
	
	// Upper percentage.
	if ((keyword == Keyword::Percentage() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperPercentage(value.GetInteger());
	
	// Lower percentage.
	if ((keyword == Keyword::Percentage() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerPercentage(value.GetInteger());
	
	// Generalized
	if (keyword == Keyword::Generalized() && value.IsBoolean())
		return Generalized(value.GetBoolean());
	
	// Reducer.
	if (keyword == Keyword::Reducer()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (!algorithm.IsNULL() && (algorithm->IsA(DYNAMICREDUCER) || !algorithm->IsA(REDUCER))) {
			Message::Error("Specified reducer is not a non-dynamic reducer.");
			return false;
		}
		
		Handle<Reducer> reducer = dynamic_cast(Reducer*, algorithm.GetPointer());
		return SetReducer(reducer);
	}
	
	// Pass on to reducer?
	if (!GetReducer().IsNULL()) {
		if (GetReducer()->SetParameter(keyword, value))
			return true;
	}
	
	return false;
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

Structure *
DynamicReducer::Apply(Structure &structure) const {
	
	bool masked = true;
	bool unmasked = false;
	
	// Check if input is of expected type.
	if (!IsApplicable(structure)) {
		return NULL;
	}
	
	// Check if reducer is set
	if (GetReducer().IsNULL()) {
		Message::Error("Static reducer not defined.");
		return NULL;
	}

	// Check that parameters are in correct interval
	if (GetNoLevels() < 1 || GetNoSamples() < 1 || GetSeed() < 0 || 
		GetUpperPercentage() < 0 || GetLowerPercentage() < 0 ||
		GetUpperPercentage() > 100 || GetLowerPercentage() > 100 ||
		GetUpperPercentage() < GetLowerPercentage() ||
		GetThreshold() < 0 || GetThreshold() > 1) {
		
		Message::Error("Dynamic reducer: Bad parameter settings, aborting...");
		return NULL;
	}
	
	// Calculate decrease in sample size per level
	int decrease;
	if (GetNoLevels() == 1)
		decrease = 0;
	else
		decrease = MathKit::Round(static_cast(float, (GetUpperPercentage() - GetLowerPercentage())) / 
								  static_cast(float, (GetNoLevels() - 1)));
	
	// Cast to correct type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	
	// compute the number of unmasked objects in the table
	const_cast(DynamicReducer*, this)->no_unmasked_objects_ = table->GetNoObjects(masked);
	
	// Sufficient number of objects?
	if ((decrease * static_cast(float,no_unmasked_objects_)) < 1.0) {
		Message::Error("Unsufficient number of objects for current sample size settings.\nThe sample sizes of the decision table must vary with at least one object.");
		return NULL;
	}
	
	// Create an empty reduct set.
	Handle<Reducts> reducts = Creator::Reducts();
	// Set reduct set name
	String reduct_name = table->GetName();
	reduct_name += ", (" + String::Format(GetNoLevels()) + "*" + String::Format(GetNoSamples());
	reduct_name += ", [" + String::Format(GetLowerPercentage()) + "," + String::Format(GetUpperPercentage()) + "], ";
	reduct_name += String::Format(1.0 - GetThreshold()) + ") - ";
	Generalized()?reduct_name += "generalized dynamic reducts":reduct_name += "dynamic reducts";
	reducts->SetName(reduct_name);
	
	if (reducts.IsNULL()){
		Message::Error("Could not initialize reduct set.\n");
		return NULL;
	}
	
	// Special case: Is the table empty?
	if ((no_unmasked_objects_ == 0) || (table->GetNoAttributes(masked) == 0)) {
		Message::Error("The decision table is empty.");
		return NULL;
	}
	
	// Special case: Are all condition attributes disabled?
	if (table->GetNoAttributes(masked) == 1 && table->GetDecisionAttribute(masked) == 0) {
		if (!AppendReduct(*reducts))
			return NULL;
		return reducts.Release();
	}
	
	// Progess stuff
	int no_sub_tables = GetNoLevels() * GetNoSamples();
	int counter = 1;
	Message message;
	
	// Initialize reduct map for calculation of stability coefficient
	ReductMap reduct_map;
	
	// Compute the unmasked objects indices of the decision table
	int i;
	const_cast(DynamicReducer*, this)->indices_.reserve(no_unmasked_objects_);
	for (i = 0; i < table->GetNoObjects(unmasked); i++){
		if (table->IsObjectEnabled(i))
			const_cast(DynamicReducer*, this)->indices_.push_back(i);
	}
	
	//Initialize random number generator
	RNG rng(GetSeed());
	
	//Compute non-generalized dynamic reducts?
	if (!Generalized()) {
		
		message.Notify("Calculating reducts for whole table...");
		if (!message.Progress("Calculating reducts for whole table...")) {
			Message::Error("Algorithm failure, aborting...");
			return NULL;
		}
		
		// Calculate reducts for sample
		Handle<Reducts> whole_reducts = dynamic_cast(Reducts*, table->Apply(*GetReducer()));
		// Update reduct map, set support of these reducts to 0 (init = true)
		if (whole_reducts.IsNULL()) {
			Message::Error("Failed to calculate reducts for whole table.");
			return NULL;
		}
		UpdateReducts(reduct_map, *whole_reducts, true);
	}
	
	message.Notify("Calculating reducts for sample tables...");
	// Iterate over all sample sizes
	for (i = 0; i < GetNoLevels(); i++){
		
		// Calculate sample size for this level
		int sample_size = GetUpperPercentage() - (i * decrease);
		
		// Compute reducts for the user specified number of samples
		for (int j = 0; j < GetNoSamples(); j++) {
			
			if (!message.Progress("Calculating reducts for sample " + String::Format(counter) + " of " + String::Format(no_sub_tables), counter, no_sub_tables)) {
				Message::Error("Algorithm failure, aborting...");
				return NULL;
			}
			
			// Initialize sample to original decision table
			Handle<DecisionTable> sample = dynamic_cast(DecisionTable*, table->Duplicate());
			// Calculate sample of calculated sample size from decision table
			if (!Sample(*table, *sample, sample_size, rng)) {
				Message::Error("Unable to compute sample of sample size: " + String::Format(sample_size) + "% from table.\n");
				return NULL;
			}
			
			// Calculate reducts for sample
			Handle<Reducts> sample_reducts = dynamic_cast(Reducts*, sample->Apply(*GetReducer()));
			
			if (sample_reducts.IsNULL()) {
				Message::Error("Failed to calculate reducts for sample: " + String::Format(i));
				return NULL;
			}

			// Update reduct map
			UpdateReducts(reduct_map, *sample_reducts);
			
			counter++;
		}
	}
	
	// Calculate dynamic reducts from reduct map
	message.Notify("Calculating dynamic reducts...");
	if (!CalculateDynamicReducts(*table, *reducts, reduct_map)) {
		Message::Error("Failed to calculate dynamic reducts.");
		return NULL;
	}
	
	// Generate rules if object discernibility
	if (GetReducer()->GetDiscernibility() == Reduct::DISCERNIBILITY_OBJECT) {
		
		// Acquire a rule generator.
		Handle<RuleGenerator> rulegenerator = dynamic_cast(RuleGenerator *, ObjectManager::GetIdentifiedAlgorithm(RULEGENERATOR));
		
		if (rulegenerator.IsNULL()) {
			Message::Warning("No rule generators installed.");
			return reducts.Release();
		}
		
		// Generate rules.
		rulegenerator->SetDecisionTable(table.GetPointer());
		Handle<Rules> rules = dynamic_cast(Rules *, reducts->Apply(*rulegenerator));
		rulegenerator->SetDecisionTable(NULL);
		
		if (rules.IsNULL()) {
			Message::Warning("No rules generated.");
			return reducts.Release();
		}
		else {
			rules->SetName("dynamic rules");
		}
		
		// Append rule set as a child to the reduct set.
		if (!reducts->IsParent(rules.GetPointer(), true))
			reducts->AppendChild(rules.GetPointer());
		
	}
	
	return reducts.Release();
}

//-------------------------------------------------------------------
// Method........: UpdateReducts
// Author........: Robin Andersson
// Date..........:
// Description...: Updates the reduct map with the new sample reducts
// Comments......: 
// Revisions.....:
//===================================================================

void
DynamicReducer::UpdateReducts(ReductMap &reduct_map, const Reducts &sample_reducts, bool init /*=false*/) const{
	
	Message message;
	int i;
	ReductMap::iterator iter;
	
	//Update reduct map with sample reducts
	for (i = 0; i < sample_reducts.GetNoReducts(); i++) {
		
		Handle<Reduct> reduct = sample_reducts.GetReduct(i);
		
		// Get reduct representation (string)
		String reduct_string = reduct->Format();
		
		// If reduct is new, add new entry to map
		iter = reduct_map.find(reduct_string);
		if (iter == reduct_map.end()) {
			
			RedItem item(reduct, init?0:1);
			
			Vector(RedItem) newvec;
			newvec.push_back(item);
			
			Entry entry(reduct_string,newvec);
			reduct_map.insert(entry);
		}
		// if we are computing object discernibilty reducts
		else if (GetReducer()->GetDiscernibility() == Reduct::DISCERNIBILITY_OBJECT) {
			
			bool found = false;
			
			//iterate over reducts in vector
			int j;
			for (j = 0; j < iter->second.size(); j++) {
				// does same object relative reduct allready exists?
				if (reduct->GetObject() == iter->second[j].first->GetObject()) {
					found = true;
					iter->second[j].second++;
					break;
				}
			}
			
			// new object relative reduct?
			if (!found) {
				RedItem item(reduct, init?0:1);
				iter->second.push_back(item);
			}
			
		}
		// Otherwise update existing entry
		else {
			iter->second[0].second++;
		}
	}
}

//-------------------------------------------------------------------
// Method........: CalculateDynamicReducts
// Author........: Robin Andersson
// Date..........:
// Description...: Iterates over all sample reducts, and treats a sample
//                 reduct as dynamic if its computed stability coefficient
//                 is above the user specified threshold
// Comments......: Support is set to the stability coefficient in percents
// Revisions.....:
//===================================================================

bool
DynamicReducer::CalculateDynamicReducts(const DecisionTable &table, Reducts &reducts, const ReductMap &reduct_map) const {
	
	Message message;

	int no_sub_tables = GetNoLevels() * GetNoSamples();
	
	ReductMap::const_iterator entry;
	
	// Iterate over all entries in the reduct map
	for (entry = reduct_map.begin(); entry != reduct_map.end(); entry++) {
		
		// Iterate over reducts in vector 
		// (only 1 element if full discernibility type)
		int i;
#ifdef X86_64
		message.Notify("Number of reditems in vector: " + String::Format((int)entry->second.size()));
#else
		message.Notify("Number of reditems in vector: " + String::Format(entry->second.size()));
#endif
		for (i = 0; i < entry->second.size(); i++) {
			
			// Calculate stability coefficient
			float stability = static_cast(float,entry->second[i].second) / static_cast(float,no_sub_tables);
			
			// Add reduct to reduct set if stability coefficient is above the defined threshold
			if (stability >= GetThreshold()) {
				
				Handle<Reduct> reduct = entry->second[i].first;

				//Approximate support to stability coeff * number of objects in the table
				reduct->SetSupport(MathKit::Round(100.0f * stability));
				
				reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);

				if (!reducts.AppendStructure(reduct.Release())){
					Message::Error("Could not append reduct to reduct set.");
					return false;
				}
			}
		}
	}

	return true;
}

//-------------------------------------------------------------------
// Method........: Sample
// Author........: Robin Andersson
// Date..........:
// Description...: Generates a sample from the decision table with
//                 specified sample size
// Comments......: 
// Revisions.....:
//===================================================================

bool
DynamicReducer::Sample(const DecisionTable &table, DecisionTable &sample, const int sample_size, RNG rng) const {
	
	bool unmasked = false;
	bool masked = true;
	
	// Calculate the number of objects to remove from sample table
	int no_remove_objects = no_unmasked_objects_ - 
		MathKit::Round((static_cast(float,sample_size) / 100.0) *
		static_cast(float,no_unmasked_objects_));

	// Generated random indices
	Set(int) removes;
	removes.erase(removes.begin(),removes.end());
	int counter = 0;

	// Generate random object indices to remove
	while (removes.size() < no_remove_objects) {
		// Randomize an unmasked object to remove
		int obj_nr = indices_[rng.DrawInteger(0,indices_.size() - 1)];

		// If not allready generated, add to map
		if (removes.find(obj_nr) == removes.end()) {
			removes.insert(obj_nr);
		}
	}

	// Remove the objects corresponding to the generated indices
	Set(int)::iterator iter = removes.end();
	do {
		iter--;
	
		// Try to remove the object from sample table
		if(!sample.RemoveObject(*iter, masked)) {
			Message::Error("Unable to remove object " + String::Format(*iter) + " from sample table.");
			return false;
		}

	} while (iter != removes.begin());

	return true;
}

//-------------------------------------------------------------------
// Method........: AppendReduct
// Author........: Alexander Ohrn, Robin Andersson
// Date..........:
// Description...: Appends a single reduct that is either empty or
//                 contains the decision attribute. Typically relevant
//                 if all condition attributes in the input table are
//                 masked away.
//
// Comments......: Called from Apply as a special case.
//                 Copied from Reducer, slightly modified...
// Revisions.....:
//===================================================================

bool
DynamicReducer::AppendReduct(Reducts &reducts) const {
	
	// Create an empty reduct.
	Handle<Reduct> reduct = Creator::Reduct();
	
	// Initialize support.
	reduct->SetSupport(1);
	
	reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);
	
	reduct->IsModuloDecision(GetReducer()->ModuloDecision());
	
	// Initialize reduct contents.
	if (!GetReducer()->ModuloDecision())
		reduct->InsertAttribute(0);
	
	// Append reduct.
	return reducts.AppendStructure(reduct.GetPointer());
	
}
 
DynamicReducer *
DynamicReducer::Clone() {
 	return new DynamicReducer;
}
