//-------------------------------------------------------------------
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....: Robin Andersson
//                 restructuring of the algorithm and
//                 added support of randomization and iteration over
//                 parameter settings
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/system/fstream.h>
#include <kernel/system/math.h>

#include <kernel/genomics/algorithms/cvclassifier.h>
#include <kernel/genomics/algorithms/templatescaler.h>

#include <kernel/structures/project.h>
#include <kernel/structures/projectmanager.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/classification.h>
#include <kernel/structures/textfile.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/roccurve.h>

#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/batchclassifier.h>

#include <kernel/sav/algorithms/savgeneticreducer.h>

#include <kernel/basic/set.h>
#include <kernel/basic/message.h>
#include <kernel/basic/algorithm.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/iokit.h>

#include <common/objectmanager.h>


//-------------------------------------------------------------------
// Methods for class TemplateScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

CVClassifier::CVClassifier() {
	
	// initialize varaiables and parameter settings
	
	//template length
	SetTemplateLength(2);
	SetTemplateLengthTo(2);
	SetTemplateLengthStep(1);
	// constant template length
   	SetConstantTemplateLength(3);
	SetConstantTemplateLengthTo(3);
	SetConstantTemplateLengthStep(1);
	// required increase/decrease
	SetCreaseThr(0.6f);
	SetCreaseThrTo(0.6f);
	SetCreaseThrStep(0.1f);
	// constant threshold
	SetConstantThr(0.2f);
	SetConstantThrTo(0.2f);
	SetConstantThrStep(0.1f);
	// start end threshold
	SetStartEndThr(0.1f);
	SetStartEndThrTo(0.1f);
	SetStartEndThrStep(0.1f);
	// tolerance threshold
	SetToleranceThr(0.2f);
	SetToleranceThrTo(0.2f);
	SetToleranceThrStep(0.1f);
	//obsolete parameter?
	supervised_ = false;
	// Reducer
	SetApproximate(true);
	SetApproximateFraction(0.9f);
	SetIDG(true);
	SetIDGFilename(String("idg-not-symmetric.txt"));
	// CV
	SetCVLog(true);
	SetLogFilename(String("cv.txt"));
	SetCV(10);
	SetIterateCV(false);
	SetIterateCVLogFilename(String("params.txt"));
	iterations_ = 1;
	// seed
	SetSeed(1);
	// weight on false positives
	SetWeight(0.0f);
	//Ranomizations
	SetNoRandomization(0);
	SetRandomizationLogFilename(String("rands.txt"));
	//- Rule pruning
	SetRulePruning(false);
	SetRulePruningAccuracy(0.0f);
	SetRulePruningCoverage(0.0f);
	//- Feature selection
	coverage_thr_ = 2;
	accuracy_thr_ = 0.5f;
	max_features_thr_ = 50;
	min_features_thr_ = 0;
	
	reducer_ = dynamic_cast(Reducer*, ObjectManager::GetIdentifiedAlgorithm(SAVGENETICREDUCER));
	//reducer_->SetParameters("DISCERNIBILITY=Object; MODULO.DECISION=T; PRECOMPUTE=F; BRT=F; BRT.PRECISION=0.1; SELECTION=All; IDG=" + String::Format(GetIDG()) +"; IDG.FILENAME=" + GetIDGFilename() + "; APPROXIMATE=T; FRACTION=0.9; SIZE.KEEP=256; KEEP.LEVELS=3;");
	
}

CVClassifier::~CVClassifier() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(CVClassifier, CVCLASSIFIER, Algorithm)

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
CVClassifier::GetParameters() const {
	
	String parameters = "";
	
	// CV
	parameters += Keyword::Number();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetCV());
	parameters += Keyword::Separator();
	
	parameters += Keyword::Log() + Keyword::Dot() + Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetLogFilename();
	parameters += Keyword::Separator();
	
	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());
	parameters += Keyword::Separator();
	
	// parameter iteration
	parameters += Keyword::CV() + Keyword::Dot() + Keyword::Iterate();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetIterateCV());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::CV() + Keyword::Dot() + Keyword::Iterate() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIterateCVLogFilename();
		parameters += Keyword::Separator();
	}
	
	// template length
	parameters += Keyword::TemplateLength();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetTemplateLength());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::TemplateLength() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetTemplateLengthTo());
		parameters += Keyword::Separator();
		
		parameters += Keyword::TemplateLength() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetTemplateLengthStep());
		parameters += Keyword::Separator();
	}
	
	// constant template length
	parameters += Keyword::ConstantTemplateLength();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetConstantTemplateLength());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetConstantTemplateLengthTo());
		parameters += Keyword::Separator();
		
		parameters += Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetConstantTemplateLengthStep());
		parameters += Keyword::Separator();
	}
	
	// required increase/decrease
	parameters += Keyword::CreaseThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetCreaseThr());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::CreaseThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetCreaseThrTo());
		parameters += Keyword::Separator();
		
		parameters += Keyword::CreaseThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetCreaseThrStep());
		parameters += Keyword::Separator();
	}
	
	// constant threshold
	parameters += Keyword::ConstantThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetConstantThr());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::ConstantThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetConstantThrTo());
		parameters += Keyword::Separator();
		
		parameters += Keyword::ConstantThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetConstantThrStep());
		parameters += Keyword::Separator();
	}
	
	// start end threshold
	parameters += Keyword::StartEndThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetStartEndThr());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::StartEndThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetStartEndThrTo());
		parameters += Keyword::Separator();
		
		parameters += Keyword::StartEndThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetStartEndThrStep());
		parameters += Keyword::Separator();
	}
	
	// tolerance threshold
	parameters += Keyword::ToleranceThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetToleranceThr());
	parameters += Keyword::Separator();
	
	if (GetIterateCV()) {
		parameters += Keyword::ToleranceThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetToleranceThrTo());
		parameters += Keyword::Separator();
		
		parameters += Keyword::ToleranceThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetToleranceThrStep());
		parameters += Keyword::Separator();
	}
	
	// Randomization	
	parameters += Keyword::Randomization() + Keyword::Dot() + Keyword::Number();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoRandomization());
	parameters += Keyword::Separator();
	
	if (GetNoRandomization() > 0) {
		parameters += Keyword::Randomization() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetRandomizationLogFilename();
		parameters += Keyword::Separator();
	}
	
	// Rule pruning
	parameters += Keyword::RulePruning();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetRulePruning());
	parameters += Keyword::Separator();
	
	if (GetRulePruning()) {
		parameters += Keyword::RulePruning() + Keyword::Dot() + Keyword::Accuracy();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetRulePruningAccuracy());
		parameters += Keyword::Separator();
		
		parameters += Keyword::RulePruning() + Keyword::Dot() + Keyword::Coverage();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetRulePruningCoverage());
		parameters += Keyword::Separator();
	}
	
	// weight on false positives
	parameters += Keyword::Weight();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetWeight());
	
	// Reducer
	// -------
	
	parameters += Keyword::IDG();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetIDG());
	parameters += Keyword::Separator();
	
	if (GetIDG()) {
		parameters += Keyword::IDG() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIDGFilename();
		parameters += Keyword::Separator();
	}
	
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
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Robin Andersson
//===================================================================

bool
CVClassifier::SetParameter(const String &keyword, const String &value) {
	
	// CV
	if (keyword == Keyword::Number() && value.IsInteger())
		return SetCV(value.GetInteger());
	
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Log() && value.IsBoolean())
		return SetCVLog(value.GetBoolean());
	
	if (keyword == Keyword::Log() + Keyword::Dot() + Keyword::Filename())
		return SetLogFilename(value);
	
	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());
	
	// parameter iteration
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Iterate() && value.IsBoolean())
		return SetIterateCV(value.GetBoolean());
	
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Iterate() + Keyword::Dot() +
		Keyword::Log() + Keyword::Dot() + Keyword::Filename())
		return SetIterateCVLogFilename(value);
	
	// template length
	if (keyword == Keyword::TemplateLength() && value.IsInteger())
		return SetTemplateLength(value.GetInteger());
	
	if (keyword == Keyword::TemplateLength() + Keyword::Dot() + Keyword::To() && value.IsInteger())
		return SetTemplateLengthTo(value.GetInteger());
	
	if (keyword == Keyword::TemplateLength() + Keyword::Dot() + Keyword::Step() && value.IsInteger())
		return SetTemplateLengthStep(value.GetInteger());
	
	// constant template length
	if (keyword == Keyword::ConstantTemplateLength() && value.IsInteger())
		return SetConstantTemplateLength(value.GetInteger());
	
	if (keyword == Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::To() && value.IsInteger())
		return SetConstantTemplateLengthTo(value.GetInteger());
	
	if (keyword == Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::Step() && value.IsInteger())
		return SetConstantTemplateLengthStep(value.GetInteger());
	
	// required increase/decrease
	if (keyword == Keyword::CreaseThr() && value.IsFloat())
		return SetCreaseThr(value.GetFloat());
	
	if (keyword == Keyword::CreaseThr() + Keyword::Dot() + Keyword::To() && value.IsFloat())
		return SetCreaseThrTo(value.GetFloat());
	
	if (keyword == Keyword::CreaseThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat())
		return SetCreaseThrStep(value.GetFloat());
	
	// constant threshold
	if (keyword == Keyword::ConstantThr() && value.IsFloat())
		return SetConstantThr(value.GetFloat());
	
	if (keyword == Keyword::ConstantThr() + Keyword::Dot() + Keyword::To() && value.IsFloat())
		return SetConstantThrTo(value.GetFloat());
	
	if (keyword == Keyword::ConstantThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat())
		return SetConstantThrStep(value.GetFloat());
	
	// start end threshold
	if (keyword == Keyword::StartEndThr() && value.IsFloat())
		return SetStartEndThr(value.GetFloat());
	
	if (keyword == Keyword::StartEndThr() + Keyword::Dot() + Keyword::To() && value.IsFloat())
		return SetStartEndThrTo(value.GetFloat());
	
	if (keyword == Keyword::StartEndThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat())
		return SetStartEndThrStep(value.GetFloat());
	
	// tolerance threshold
	if (keyword == Keyword::ToleranceThr() && value.IsFloat())
		return SetToleranceThr(value.GetFloat());
	
	if (keyword == Keyword::ToleranceThr() + Keyword::Dot() + Keyword::To() && value.IsFloat())
		return SetToleranceThrTo(value.GetFloat());
	
	if (keyword == Keyword::ToleranceThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat())
		return SetToleranceThrStep(value.GetFloat());
	
	// Randomization	
	if (keyword == Keyword::Randomization() + Keyword::Dot() + Keyword::Number() && value.IsInteger())
		return SetNoRandomization(value.GetInteger());
	
	if (keyword == Keyword::Randomization() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename())
		return SetRandomizationLogFilename(value);
	
	// Rule pruning
	if (keyword == Keyword::RulePruning() && value.IsBoolean())
		return SetRulePruning(value.GetBoolean());
	
	if (keyword == Keyword::RulePruning() + Keyword::Dot() + Keyword::Accuracy() && value.IsFloat())
		return SetRulePruningAccuracy(value.GetFloat());
	
	if (keyword == Keyword::RulePruning() + Keyword::Dot() + Keyword::Coverage() && value.IsFloat())
		return SetRulePruningCoverage(value.GetFloat());
	
	// weight on false positives
	if (keyword == Keyword::Weight() && value.IsFloat())
		return SetWeight(value.GetFloat());
	
	// Reducer.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return SetIDG(value.GetBoolean());
	
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);
	
	if (keyword == Keyword::Reducer()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (!algorithm.IsNULL() && !algorithm->IsA(REDUCER)) {
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
// Method........: IsApplicable
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CVClassifier::IsApplicable(const Structure &structure, bool /*warn*/) const {
	
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Method........: CleanUp
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================
void
CVClassifier::CleanUp() {
	Handle<CVClassifier> self = const_cast(CVClassifier *, this);	
	self->indices_.erase(self->indices_.begin(), self->indices_.end());
	self->gd_.erase(self->gd_.begin(), self->gd_.end());
}


//-------------------------------------------------------------------
// Method........: Apply
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Robin Andersson 040629
//===================================================================

Structure *
CVClassifier::Apply(Structure &structure) const {
	
	Message message;
	
	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;
	
	// Cast to verified type.
	Handle<DecisionTable> input = dynamic_cast(DecisionTable *, &structure);
	
	// Check if any conditional attributes
	if (input->GetNoAttributes(true) < 1) {
		Message::Error("Table has no condition attributes.", false);
		return NULL;
	}
	
	// Check and modify reducer
	if (GetReducer().IsNULL() || !GetReducer()->IsA(REDUCER)) {
		Message::Error("Reducer algortihm is not set or not a proper reducer.");
		return NULL;
	}
	
	// Set important parameters, can override user settings...
	if (!GetReducer()->SetParameters("DISCERNIBILITY=Object; MODULO.DECISION=T; SELECTION=All")) {
		Message::Error("Needed parameter settings not supported by reducer " + IdHolder::GetClassname(GetReducer()->GetId()) + "\nor by one of it's internal algorithms. Aborting...");
		return NULL;
	}
	
	Handle<CVClassifier> self = const_cast(CVClassifier *, this);
	
	// Check if IDG is supported
	if (GetIDG() && !GetReducer()->SetParameters("IDG=" + String::Format(GetIDG()) +"; IDG.FILENAME=" + GetIDGFilename(), false)) {
		Message::Debug("Use of IDG not supported...");
		self->SetIDG(false);
	}
	
	message.Notify("CV LEARNING");
	
	// Initiate new map for association of AUCs for decision classes
	// with current parameter settings
	AnnotationData bestParamForClass;
	bestParamForClass.erase(bestParamForClass.begin(), bestParamForClass.end());
	
	RandomizationData randCalc;
	randCalc.erase(randCalc.begin(), randCalc.end());
	
	// Run iteration without randomization...
	// If iteration over parameters is not selected by the user, CVIter
	// will iterate one time only.
	
	if (!self->CVIter(*input, true, bestParamForClass, randCalc)){
		self->CleanUp();
		return NULL;
	}
	
	// Run selected number of randomizations together with selected number of
	// iterations over parameter settings
	if (GetNoRandomization() > 0) {
		
		Handle<DecisionTable> rand_table = dynamic_cast(DecisionTable *, input->Duplicate());
		
		message.Notify("CV RANDOMIZATION");
		
		// Run randomizations...
		for (int i = 0; i < GetNoRandomization(); i++) {
			
			message.Notify("CV RANDOMIZATION: " + String::Format(i + 1));
			// Randomize the annotations in the decision table
			Randomize(*rand_table, i);
			
			if (!self->CVIter(*rand_table,false,bestParamForClass,randCalc)){
				randCalc.clear();
				bestParamForClass.clear();
				self->CleanUp();
				return NULL;
			}
			
			rand_table->RemoveAllChildren();
			
			rand_table = dynamic_cast(DecisionTable *, input->Duplicate());
		}
		
		WriteRandResults(bestParamForClass, randCalc);
		AddTextFile(*input, GetRandomizationLogFilename());
		randCalc.clear();
	}
	
	bestParamForClass.clear();
	self->CleanUp();
	
	return input.Release();
}

//-------------------------------------------------------------------
// New methods
//===================================================================

//-------------------------------------------------------------------
// Method........: Randomize
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================
bool
CVClassifier::Randomize(DecisionTable &table, int seed) const {
	
	RNG rng(seed);
	
	bool masked = true;
	
	int i, j, k;
	int no_objects = table.GetNoObjects(masked);
	int no_attrib = table.GetNoAttributes(masked);
	
	Vector(int) decision_values;
	Vector(int) decision_cardinalities;
	table.GetValueSet(decision_values, decision_cardinalities, table.GetDecisionAttribute(masked), masked);
	
	Vector(Vector(String)) track(decision_values.size(), Vector(String)());
	
	for (i = 0; i < no_objects; i++) {
		
		String gene = table.GetEntry(i, 0, true, false);
		
		int index;
		int iter = 0;
		while (true) {
			index = rng.DrawInteger(0, decision_cardinalities.size()-1);
			
			if (decision_cardinalities[index] <= 0) continue;
			
			bool found = false;
			for (k = 0; k < track[index].size(); k++) {
				if (track[index][k] == gene) {
					found = true;
					break;
				}
			}
			if (!found) break;
			
			iter++;
			if (iter > 1000) break;
		}	
		
		decision_cardinalities[index]--;
		if (!table.SetEntry(i, table.GetDecisionAttribute(masked), decision_values[index], masked))
			return false;
		track[index].push_back(gene);
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: CVIter
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Robin Andersson 040629:
//                 Added formal parameters (annotData & randCalc)
//                 Best parametersettings are output to the param file
//===================================================================

bool
CVClassifier::CVIter(DecisionTable &table, bool output, AnnotationData &annotData, RandomizationData &randCalc) {
	
	// no iteration?
	if (!GetIterateCV())
		return CV(*const_cast(DecisionTable *, &table), output, annotData, randCalc, 1);
	
	int i = 0;
	Message m;
	
	// remember initial template parameter values
	int t_length = GetTemplateLength();
	int c_t_length = GetConstantTemplateLength();
	float c_thr = GetCreaseThr();
	float const_thr = GetConstantThr();
	float start_end_thr = GetStartEndThr();
	float tol_thr = GetToleranceThr();
	
	// Iterate over parameter settings
	for (int t_length_iter = t_length; t_length_iter <= GetTemplateLengthTo(); t_length_iter += GetTemplateLengthStep()) {
		for (int c_t_length_iter = c_t_length; c_t_length_iter <= GetConstantTemplateLengthTo(); c_t_length_iter += GetConstantTemplateLengthStep()) {
			for (float c_thr_iter = c_thr; c_thr_iter <= GetCreaseThrTo(); c_thr_iter += GetCreaseThrStep()) {
				for (float const_thr_iter = const_thr; const_thr_iter <= GetConstantThrTo(); const_thr_iter += GetConstantThrStep()) {
					for (float start_end_thr_iter = start_end_thr; start_end_thr_iter <= GetStartEndThrTo(); start_end_thr_iter += GetStartEndThrStep()) {
						for (float tol_thr_iter = tol_thr; tol_thr_iter <= GetToleranceThrTo(); tol_thr_iter += GetToleranceThrStep()) {
							
							// change template parameter values
							SetTemplateLength(t_length_iter);
							SetConstantTemplateLength(c_t_length_iter);
							SetCreaseThr(c_thr_iter);
							SetConstantThr(const_thr_iter);
							SetStartEndThr(start_end_thr_iter);
							SetToleranceThr(tol_thr_iter);
							
							i++;
							m.Notify("CV Iteration: " + String::Format(i));
							
							// Run cross validation with actual template parameter values
							if (!CV(*const_cast(DecisionTable *, &table), output, annotData, randCalc, i)) {
								return false;
							}
							table.RemoveAllChildren();
						}
					}
				}
			}
		}
	}
	
	//Reset template parameters
	SetTemplateLength(t_length);
	SetConstantTemplateLength(c_t_length);
	SetCreaseThr(c_thr);
	SetConstantThr(const_thr);
	SetStartEndThr(start_end_thr);
	SetToleranceThr(tol_thr);
	
	// Print result to paramlog file if this feature is selected
	if (output) {
		
		iterations_ = i;
		
		// Write parameter settings result to textfile
		WriteParamsResult(annotData);
		
		// Add the printed file as child to the decision table
		AddTextFile(table,GetIterateCVLogFilename());
	}
	
	return true;
}

//-------------------------------------------------------------------
// Method........: CV
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Some restructuring + added update of annotation data
//                 randomization data
//===================================================================

bool
CVClassifier::CV(DecisionTable &table, bool output, AnnotationData &annotData, RandomizationData &randCalc, int iter /*=0*/) {
	
	bool masked = true;
	RNG rng(GetSeed());
	
	Message message;
	
	int i, j;
	
	// Do sampling for cv
	if (!InitializeSamplingScheme(*const_cast(DecisionTable *, &table), masked, rng))
		return false;
	
	// Too high approximate fraction?
	if (GetApproximateFraction() > 0.9)
		SetApproximateFraction(0.9f);
	
	// Templatize data
	Handle<DecisionTable> training_template = Creator::DecisionTable();
	Handle<DecisionTable> testing_template = Creator::DecisionTable();
	
	// Split decision table into training template and testing template
	if (!PreprocessData(*const_cast(DecisionTable *, &table), *training_template, *testing_template)) {
		return false;
	}
	
	Vector(int) decision_values;
	Vector(int) decision_cardinalities;
	training_template->GetValueSet(decision_values, decision_cardinalities, training_template->GetDecisionAttribute(true), true);
	
	// Print IDG
	if (GetIDG()) {
		ofstream idg;
		if (!IOKit::Open(idg,GetIDGFilename())) {
			Message::Error("Unable to open output file.");
			return false;
		}
		
		for (j = 0; j < training_template->GetNoAttributes(true)-1; j++) {
			
			String name = training_template->GetAttributeName(j, true);
			
			idg<< "begin " + name + "\n";
			idg<< "\tnodes Undefined *\n";
			idg<< "\tUndefined -> *\n";
			idg<< "\tmake-reflexive\n";
			idg<< "end " + name + "\n";
		}
		idg.close();
	}
	
	Vector(ICPair) classification_details; 
	classification_details.erase(classification_details.begin(), classification_details.end());
	classification_details.reserve(training_template->GetNoObjects(masked));
	
	// Remove possibly existing entries in the generalized decision vector
	gd_.erase(gd_.begin(), gd_.end());
	gd_.reserve(training_template->GetNoObjects(masked));
	both_ = 0;
	
	// Run GetCV() number of crossvalidations
	for (i = 0; i < GetCV(); i++) {
		
		String i_string = String::Format(i + 1);
		message.Notify("CV " + i_string);
		
		// Initialize training and test set
		Handle<DecisionTable> training = dynamic_cast(DecisionTable *, training_template->Duplicate());
		Handle<DecisionTable> testing = dynamic_cast(DecisionTable *, testing_template->Duplicate());
		
		// Select training and test set
		if (!SampleTables(i, rng, *training, *testing, masked)){
			return false;
		}
		
		// Find gd for test set
		if (!SetGeneralizedDecision(*training, *testing)){
			return false;
		}
		
		// Do classification
		if (!CVOnce(*training, *testing, classification_details, i+1, masked)){
			return false;
		}
		
		training->RemoveAllChildren();
		testing->RemoveAllChildren();
	}
	
	// ------ Robin Andersson 040629 --------
	if (output || GetNoRandomization() > 0) {
		
		// Collect and output results
		float auc = 0.0f;
		int no_classes = 0;
		Vector(Vector(float)) results(training_template->GetNoObjects(masked),Vector(float)());
		
		// Calculate statistics for current training template and classification details
		if (!CalculateStatistics(*training_template, classification_details, decision_values, results, auc, no_classes)){
			return false;
		}
		
		//Output statistical results?
		if (output) {
			
			if (!WriteResultsToFile(*training_template, decision_values, results, auc, no_classes, iter)){
				return false;
			}
			
			// Update annotation data
			UpdateAnnotData(*training_template, decision_values, no_classes, results, annotData);
			
			// Only add textfile to project if not iterating over parameter settings
			// otherwise too memory demanding
			if (!GetIterateCV()) {
				
				String filename = String::Format(auc) + "-" + GetLogFilename();
				//Add the textfile to the projecttree
				AddTextFile(table, filename);
			}
		}
		
		//Update randomization result
		else if (GetNoRandomization() > 0) {
			//First time?
			if (randCalc.size() == 0) {
				for (i = 0; i < decision_values.size(); i++) {
					Pair(const String, int) p(training_template->GetDictionaryEntry(training_template->GetDecisionAttribute(true), decision_values[i], true),0);
					randCalc.insert(p);
				}
			}
			// Update randomization data
			UpdateRandData(*training_template, decision_values, no_classes, results, annotData, randCalc);
		}
	}
	
	training_template->RemoveAllChildren();
	testing_template->RemoveAllChildren();
	
	return true;	
}

//-------------------------------------------------------------------
// Method........: CalculateStatistics
// Author........: Robin Andersson
// Date..........: 040628
// Description...:
// Comments......: Moved stuff from WriteResultToFile into new method
// Revisions.....:
//===================================================================
bool
CVClassifier::CalculateStatistics(const DecisionTable &data, Vector(ICPair) &classification_details, Vector(int) &decision_values, Vector(Vector(float)) &results, float &auc, int &no_classes) {
	
	Message message;
	int i, j, k;
	
	bool masked = false;
	
	for (i = 0; i < decision_values.size(); i++) {
#ifdef X86_64
		if (!message.Progress("ROC calculations " + String::Format((int)i + 1) + " of " + String::Format((int)decision_values.size()) + "...", i, decision_values.size()))
#else
		if (!message.Progress("ROC calculations " + String::Format(i + 1) + " of " + String::Format(decision_values.size()) + "...", i, decision_values.size()))
#endif
			return false;
		
		int roc_class = decision_values[i];
		
		for (j = 0; j < classification_details.size(); j++) {
			
			if (classification_details[j].first != roc_class) {
				for (k = 0; k < gd_[j].size(); k++) {
					if (gd_[j][k] == roc_class) {
						classification_details[j].first = roc_class;
						break;
					}
				}
			}
		}
		
		// Create ROC curve and calculate statistics 
		
		ROCCurve roc;
		Vector(int)   roc_targets;
		Vector(float) roc_outputs;
		
		roc.SetWeight(GetWeight());
		
		int  roc_resolution = 251;
		bool roc_progress   = true;
		if (!roc.Create(classification_details, roc_class, roc_resolution, roc_progress, &roc_targets, &roc_outputs)) {
			Message::Warning("Generation of ROC curve failed.");
			return false;
		} 
		
		float sensitivity_nw	= Undefined::Float();
		float specificity_nw	= Undefined::Float();
		float tp = Undefined::Float(), fp = Undefined::Float(), tn = Undefined::Float(), fn = Undefined::Float();
		roc.GetTPFPTNFN(tp, fp, tn, fn);
		if (tp+fn < 4)
			continue;
		
		// update results vector
		
		//if (roc.GetArea() >= 0.5) {
		results[no_classes].push_back(roc.GetArea());
		auc += roc.GetArea();
		//} else {
		//	results[no_classes].push_back(1-roc.GetArea());
		//	auc += 1-roc.GetArea();
		//}
		results[no_classes].push_back(roc.GetStandardError());
		results[no_classes].push_back(roc.GetNWThreshold());
		roc.GetNWAccuracy(sensitivity_nw,specificity_nw);
		results[no_classes].push_back(sensitivity_nw);
		results[no_classes].push_back(specificity_nw);
		results[no_classes].push_back(tp);
		results[no_classes].push_back(fp);
		results[no_classes].push_back(tn);
		results[no_classes].push_back(fn);
		
		no_classes++;
	}
	
	auc = auc/no_classes;
	return true;
}

//-------------------------------------------------------------------
// Method........: WriteResultsToFile
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CVClassifier::WriteResultsToFile(const DecisionTable &data, Vector(int) &decision_values, Vector(Vector(float)) &results, float auc, int no_classes, int iter /*=0*/) const {
	
	Message message;
	int i, j;
	
	Vector(float) avg(10,0); 
	
	String fileEnding = "";
	if (GetIterateCV()) {
		fileEnding += "(iter_";
		fileEnding += String::Format(iter);
		fileEnding += ")-";
	}
	
	String filename = String::Format(auc) + "-" + fileEnding + GetLogFilename();
	ofstream log;
	
	if (!IOKit::Open(log, filename)) {
		Message::Error("Unable to open output file: " + filename);
		return false;
	}
	
	String approx = GetApproximate()?"YES (" + String::Format(GetApproximateFraction()) + ")":"NO";
	String prune = GetRulePruning()?"YES":"NO";
	String rand = GetNoRandomization() > 0?"YES":"NO";
	
	log<< "ABBREVIATIONS                                 PARAMETERS FOR CV\n";
	log<< " AUC      = Area Under the ROC Curve           APPROXIMATE REDUCTS: " + approx + "\n";
	log<< " SE       = Standard error                     CROSS VALIDATION: " + String::Format(GetCV()) + "\n";
	log<< " THR.     = Threshold                          WEIGHT ON FP: " + String::Format(GetWeight()) + "\n";
	log<< " SENS.    = Sensitivity (TP/(TP + FN))         REPRODUCEBILITY: " + String::Format(GetSeed()) + "\n";
	log<< " SPEC.    = Specificity (TN/(TN + FP))         RANDOMIZATION: " + rand + "\n";	
	log<< " TP       = True positives\n";
	log<< " FP       = False positives                   PARAMETERS FOR TEMPLATE LANGUAGE\n";
	log<< " TN       = True negatives                     LENGTH: " + String::Format(GetTemplateLength()) + "\n";
	log<< " FN       = False negatives                    INCREASE/DECREASE: " + String::Format(GetCreaseThr()) + "\n";
	log<< "                                               INCREASE/DECREASE AT START/STOP INT.: " + String::Format(GetStartEndThr()) + "\n";
	log<< "                                               INCREASE/DECREASE IN 'WRONG' DIRECTION: " + String::Format(GetToleranceThr()) + "\n";
	log<< "                                               CONSTANT: LENGTH: " + String::Format(GetConstantTemplateLength()) + "\n";
	log<< "                                               CONSTANT: ALLOWED VARIATION FROM MEAN: " + String::Format(GetConstantThr()) + "\n\n";
	log<< "                                              RULE PRUNING: " + prune + "\n";
	if (GetRulePruning()) {
		log<< "                                               COVERAGE: " + String::Format(GetRulePruningCoverage()) + "\n";
		log<< "                                               ACCURACY: " + String::Format(GetRulePruningAccuracy()) + "\n";
	}
	log<< "\n\n";
	
	log<< "PROCESS                                                               AUC       SE        THR.      SENS.     SPEC.     TP        FP        TN        FN\n";
	log<< "--------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	for (i = 0; i < no_classes; i++) {
		String result = data.GetDictionaryEntry(data.GetDecisionAttribute(true), decision_values[i], true);
		int l = result.GetLength();
		int k;
		for (k = 0; k < 70 - l; k++)
			result = result + " ";
		//// fix: Robin ////
		if (l >= 70)
			result = result + " ";
		////////////////////
		for (j = 0; j < 9; j++) {
			
			if (j > 4) {
				result = result + String::Format(static_cast(int,results[i][j]));
				avg[j] = avg[j] + results[i][j];
			} else  {
				result = result + String::Format(results[i][j]);
				avg[j] = avg[j] + results[i][j];
			}
			l = result.GetLength();
			for (k = 0; k < 70 + 10*(j+1) - l; k++)
				result = result + " ";
			//// Fix: Robin ////////////////
			if (l >= (70 + ((j+1) * 10)))
				result = result + " ";
			////////////////////////////////
		}
		log<< result + "\n";
	}
	
	for (j = 0; j < 5; j++) {
		avg[j] = avg[j]/no_classes;
	}
	log<< "--------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	String result = "AVERAGE/SUM                                                           ";
	int k;
	for (j = 0; j < 9; j++) {
		if (j > 4)
			result = result + String::Format(static_cast(int,avg[j]));
		else
			result = result + String::Format(avg[j]);
		int l = result.GetLength();
		for (k = 0; k < 70 + 10*(j+1) - l; k++)
			result = result + " ";
		//// Fix: Robin ////////////////
		if (l >= (70 + ((j+1) * 10)))
			result = result + " ";
		////////////////////////////////
	}
	log<< result + "\n";
	
	log<< "\nPrecision: " + String::Format(avg[5]/(avg[5]+avg[6])) + "\n";
	log<< "Coverage : " + String::Format(avg[5]/(avg[5]+avg[8])) + "\n";
	
	log.close();
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: WriteParamsResult
// Author........: Robin Andersson
// Date..........: 040629
// Description...: Outputs the results of iterating over template
//                 parameters
// Comments......: 
// Revisions.....:
//===================================================================
void
CVClassifier::WriteParamsResult(AnnotationData &annotData) const {
	
	ofstream res;
	if (!IOKit::Open(res, GetIterateCVLogFilename())) {
		Message::Error("Unable to open output file: " + GetIterateCVLogFilename() + ".");
		return;
	}
	
	String approx = GetApproximate()?"YES (" + String::Format(GetApproximateFraction()) + ")":"NO";
	String prune = GetRulePruning()?"YES":"NO";
	
	res << "######################################\n";
	res << "#  BEST AUC VALUES FOR EACH PROCESS  #\n";
	res << "######################################\n";
	res << "\n";
#ifdef X86_64
	res << "Number of decision classes: " + String::Format((int)annotData.size()) + "\n";
#else
	res << "Number of decision classes: " + String::Format(annotData.size()) + "\n";
#endif
	res << "\n";
	res << "Number of iterations: " + String::Format(iterations_) + "\n";
	res << "\n";
	res<< "ABBREVIATIONS                                     PARAMETERS FOR CV\n";
	res<< " AUC      = Area Under the ROC Curve                APPROXIMATE REDUCTS: " + approx + "\n";
	res<< " SE       = Standard error                          CROSS VALIDATION: " + String::Format(GetCV()) + "\n";
	res<< " THR      = Threshold                               WEIGHT ON FP: " + String::Format(GetWeight()) + "\n";
	res<< " SENS     = Sensitivity (TP/(TP + FN))              REPRODUCEBILITY: " + String::Format(GetSeed()) + "\n";
	res<< " SPEC     = Specificity (TN/(TN + FP))\n";          	
	res<< " TP       = True positives\n";
	res<< " FP       = False positives                       RULE PRUNING: " + prune + "\n";
	if (GetRulePruning()) {
		res<< " TN       = True negatives                          COVERAGE: " + String::Format(GetRulePruningCoverage()) + "\n";
		res<< " FN       = False negatives                         ACCURACY: " + String::Format(GetRulePruningAccuracy()) + "\n";
	}
	else {
		res<< " TN       = True negatives\n";
		res<< " FN       = False negatives\n";
	}
	res<< " TL       = Template length\n";
	res<< " CTL      = Constant template length\n";
	res<< " I/D      = Required increase/decrease\n";
	res<< " F/L:I/D  = Required increase/decrease\n";
	res<< "            during the first and the\n";
	res<< "            last sub-intervals\n";
	res<< " MVA      = Maximum allowed variations\n";
	res<< "            from average\n";
	res<< " MCW      = Maximum allowed change in\n";
	res<< "            the wrong direction\n";
	res<< "\n\n";
	
	res << "PROCESS                                                               AUC       SE        THR       SENS      SPEC      TP        FP        TN        FN        TL        CTL       I/D       F/L:I/D   MVA       MCW       \n";
	res << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	typedef AnnotationData::const_iterator CI;
	for (CI pair = annotData.begin(); pair != annotData.end(); pair++) {
		String line = pair->first;
		int length = line.GetLength();
		int l;
		for (l = 0; l < 70 - length; l++)
			line += " ";
		
		// Fix:
		if (length >= 70)
			line += " ";
		
		for (int m = 0; m < pair->second.size(); m++) {
			
			if (m > 4 && m < 11)
				line += String::Format(static_cast(int,pair->second[m]));
			else
				line += String::Format(pair->second[m]);
			
			length = line.GetLength();
			
			for (l = 0; l < 70 + 10*(m+1) - length; l++)
				line += " ";
			
			if (length >= (70 + ((m+1) * 10)))
				line += " ";
		}
		
		line += "\n";
		res << line;
	}
	
	res.close();
}

//-------------------------------------------------------------------
// Method........: WriteRandResults
// Author........: Robin Andersson
// Date..........: 040629
// Description...: Outputs the results of randomization
// Comments......: 
// Revisions.....: 
//===================================================================
void
CVClassifier::WriteRandResults(AnnotationData &annotData, RandomizationData &randCalc) const {
	
	ofstream res;
	if (!IOKit::Open(res, GetRandomizationLogFilename())) {
		Message::Error("Unable to open output file: " + GetRandomizationLogFilename() + ".");
		return;
	}
	
	String approx = GetApproximate()?"YES (" + String::Format(GetApproximateFraction()) + ")":"NO";
	String prune = GetRulePruning()?"YES":"NO";
	
	res << "####################################\n";
	res << "#  P-VALUES FOR BEST PARAMS AUC'S  #\n";
	res << "####################################\n";
	res << "\n";
#ifdef X86_64
	res << "Number of decision classes: " + String::Format((int)annotData.size()) + "\n";
#else
	res << "Number of decision classes: " + String::Format(annotData.size()) + "\n";
#endif
	res << "\n";
	res << "Number of iterations: " + String::Format(iterations_) + "\n";
	res << "\n";
	res<< "ABBREVIATIONS                                     PARAMETERS FOR CV\n";
	res<< " AUC      = Area Under the ROC Curve                APPROXIMATE REDUCTS: " + approx + "\n";
	res<< " SE       = Standard error                          CROSS VALIDATION: " + String::Format(GetCV()) + "\n";
	res<< " THR      = Threshold                               WEIGHT ON FP: " + String::Format(GetWeight()) + "\n";
	res<< " SENS     = Sensitivity (TP/(TP + FN))              REPRODUCIBILITY: " + String::Format(GetSeed()) + "\n";
	res<< " SPEC     = Specificity (TN/(TN + FP))              NR OF RANDOMIZATIONS: " + String::Format(GetNoRandomization()) + "\n";	
	res<< " TP       = True positives\n";
	res<< " FP       = False positives                       RULE PRUNING: " + prune + "\n";
	if (GetRulePruning()) {
		res<< " TN       = True negatives                          COVERAGE: " + String::Format(GetRulePruningCoverage()) + "\n";
		res<< " FN       = False negatives                         ACCURACY: " + String::Format(GetRulePruningAccuracy()) + "\n";
	}
	else {
		res<< " TN       = True negatives\n";
		res<< " FN       = False negatives\n";
	}
	res<< "\n\n";
	
	res << "PROCESS                                                               AUC       p-value             SE        THR       SENS      SPEC      TP        FP        TN        FN        \n";
	res << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	
	typedef AnnotationData::const_iterator CI;
	int totalNoIter = GetNoRandomization();// * iterations_;
	
	for (CI pair = annotData.begin(); pair != annotData.end(); pair++) {
		String line = pair->first;
		int length = line.GetLength();
		int l;
		for (l = 0; l < 70 - length; l++)
			line += " ";
		
		// Fix:
		if (length >= 70)
			line += " ";
		
		//Write AUC-value
		line += String::Format(pair->second[0]);
		length = line.GetLength();
		for (l = 0; l < 80 - length; l++)
			line += " ";
		// Fix:
		if (length >= 80)
			line += " ";
		
		
		//Write p-value
		int betterRands = randCalc[pair->first];
		float pvalue = 0.0f;
		if (betterRands != 0)
			pvalue = static_cast(float, betterRands) / static_cast(float, totalNoIter);
		
		line += String::Format(pvalue);
		line += "(" + String::Format(betterRands) + "/" + String::Format(totalNoIter) + ")";
		length = line.GetLength();
		for (l = 0; l < 100 - length; l++)
			line += " ";
		// Fix:
		if (length >= 100)
			line += " ";
		
		for (int m = 1; m < pair->second.size()-6; m++) { //Don't write the parameter settings...
			
			line += String::Format(pair->second[m]);
			
			length = line.GetLength();
			
			for (l = 0; l < 100 + 10*m - length; l++)
				line += " ";
			
			if (length >= (100 + (m * 10)))
				line += " ";
		}
		
		line += "\n";
		res << line;
	}
	
	res.close();
}

//-------------------------------------------------------------------
// Method........: AddTextFile
// Author........: Robin Andersson
// Date..........: 040629
// Description...: Adds a textfile to the decision table
// Comments......: 
// Revisions.....: 
//===================================================================
void
CVClassifier::AddTextFile(DecisionTable &table, String filename) const {
	
	ifstream stream;
	
	if (!IOKit::Open(stream, filename)) {
		Message::Warning("Unable to add file: " + filename + " to project.");
	}
	
	Handle<TextFile> textfile = new TextFile;
	textfile->Load(stream);
	textfile->SetFilename(filename);
	textfile->SetName(filename);
	table.AppendChild(textfile.GetPointer());
	stream.close();
}

//-------------------------------------------------------------------
// Method........: UpdateRandData
// Author........: Robin Andersson
// Date..........: 040628
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void
CVClassifier::UpdateRandData(const DecisionTable &data, Vector(int) &decision_values, int no_classes, Vector(Vector(float)) &results, AnnotationData &annotData, RandomizationData &randCalc){
	
	AnnotationData::iterator iterator;
	// Compare each AUC value of best params settings with AUC's of randomization
	for (int i = 0; i < no_classes; i++) {
		
		String dec_class = data.GetDictionaryEntry(data.GetDecisionAttribute(true), decision_values[i], true);
		iterator = annotData.find(dec_class);
		
		// Is AUC value of randomization better than best params AUC?
		if (iterator->second[0] < results[i][0]) {
			
			// Only remember better randomization AUC if same parameter settings
			if (GetIterateCV()) {
				if (MathKit::Round(iterator->second[9]) == GetTemplateLength() &&
					MathKit::Round(iterator->second[10]) == GetConstantTemplateLength() &&
					iterator->second[11] == GetCreaseThr() &&
					iterator->second[12] == GetStartEndThr() &&
					iterator->second[13] == GetConstantThr() &&
					iterator->second[14] == GetToleranceThr())
					
					// increment the number of better randomization results
					randCalc[dec_class]++;
			}
			else {
				// increment the number of better randomization results
				randCalc[dec_class]++;
			}
		}
	}
}

//-------------------------------------------------------------------
// Method........: UpdateAnnotData
// Author........: Robin Andersson
// Date..........: 040628
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
bool
CVClassifier::UpdateAnnotData(const DecisionTable &data, Vector(int) &decision_values, int no_classes, Vector(Vector(float)) &results, AnnotationData &annotData){
	
	int i, j;
	
	AnnotationData::iterator iterator;
	
	for (i = 0; i < no_classes; i++) {
		
		String dec_class = data.GetDictionaryEntry(data.GetDecisionAttribute(true), decision_values[i], true);
		
		iterator = annotData.find(dec_class);
		
		// Is the decision class previously recorded?
		if (iterator == annotData.end()) {
			
			Vector(float) newVec(15,0.0f);
			
			for (j = 0; j < 9; j++) {
				
				if (j > 4) 
					newVec[j] = static_cast(int,results[i][j]);
				else
					newVec[j] = results[i][j];
			}
			
			//Update parameters:
			
			newVec[9] = static_cast(float,GetTemplateLength());
			newVec[10] = static_cast(float,GetConstantTemplateLength());
			newVec[11] = GetCreaseThr();
			newVec[12] = GetStartEndThr();
			newVec[13] = GetConstantThr();
			newVec[14] = GetToleranceThr();
			
			Pair(const String, Vector(float)) p(dec_class,newVec);
			annotData.insert(p);
		}
		//Is the AUC value of the decision class better than the previous?
		else if (iterator->second[0] < results[i][0]) {
			
			//Update the map with the new data
			
			for (j = 0; j < 9; j++) {
				
				if (j > 4) 
					iterator->second[j] = static_cast(int,results[i][j]);
				else
					iterator->second[j] = results[i][j];
			}
			
			//Update template parameter values:
			iterator->second[9] = static_cast(float,GetTemplateLength());
			iterator->second[10] = static_cast(float,GetConstantTemplateLength());
			iterator->second[11] = GetCreaseThr();
			iterator->second[12] = GetStartEndThr();
			iterator->second[13] = GetConstantThr();
			iterator->second[14] = GetToleranceThr();
		}
	}
	
	return true;
}

//-------------------------------------------------------------------
// Method........: PreprocessData
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
bool
CVClassifier::PreprocessData(DecisionTable &data, DecisionTable &train, DecisionTable &test) const {
	
	Message message;
	
	String template_parameters = "TEMPLATELENGTH =" + String::Format(GetTemplateLength()) +
		"; CONSTANTTEMPLATELENGTH = " + String::Format(GetConstantTemplateLength()) +
		"; CREASETHR =" + String::Format(GetCreaseThr()) +
		"; CONSTANTTHR = " + String::Format(GetConstantThr()) +
		"; STARTENDTHR = " + String::Format(GetStartEndThr()) +
		"; TOLERANCETHR = " + String::Format(GetToleranceThr());
	
	// Run TemplateScaler supervised
	Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(TEMPLATESCALER);			
	algorithm->SetParameters(template_parameters + "; SUPERVISED = " + String::Format(supervised_) + ";");
	
	//RA: debugging:
	Handle<DecisionTable> train_tmp = dynamic_cast(DecisionTable *,data.Apply(*algorithm));
	train = train_tmp.GetReference();
	
	//originally:
	//train = *dynamic_cast(DecisionTable *,data.Apply(*algorithm));
	
	if (&train == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	
	//RA: debugging:
	Handle<DecisionTable> test_tmp = dynamic_cast(DecisionTable *, train.Duplicate());
	test = test_tmp.GetReference();
	
	//originally:
	//test = *dynamic_cast(DecisionTable *, dynamic_cast(DecisionTable *, &train)->Duplicate());
	
	//}
	
	//Needed?
	//data.AppendChild(dynamic_cast(DecisionTable *, &train));
	//data.AppendChild(dynamic_cast(DecisionTable *, &test));
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: Discretize
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
bool
CVClassifier::Discretize(DecisionTable &train, DecisionTable &test, bool masked) const {
	
	Message message;
	
	int j;
	
	// Run descritization alg.
	Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(ENTROPYSCALER);
	algorithm->SetParameters("MODE=Save; MASK=T; FILENAME=cut1.txt");
	
	//RA: debugging
	Handle<DecisionTable> train_tmp = dynamic_cast(DecisionTable *, train.Apply(*algorithm));
	train = train_tmp.GetReference();
	//originally:
	//train = *dynamic_cast(DecisionTable *, train.Apply(*algorithm));
	
	if (&train == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	
	// Get undiscetized attributes and mask them
	Vector(DecisionTable::Mask) mask_train;
	Vector(DecisionTable::Mask) mask_test;
	
	if (!train.GetAttributeMasks(mask_train)) {
		return false;
	}
	if (!test.GetAttributeMasks(mask_test)) {
		return false;
	}
	for (j = 1; j < train.GetNoAttributes(false)-1; j++) {
		if (train.GetAttributeType(j, false) != train.GetAttributeType(0, false)) {
			mask_train[j] = DecisionTable::MASK_DISABLED;
			mask_test[j] = DecisionTable::MASK_DISABLED;
		}
	}
	if (!train.SetAttributeMasks(mask_train)) {
		return false;
	}
	
	// Apply cut to test set
	algorithm = ObjectManager::GetIdentifiedAlgorithm(ORTHOGONALFILESCALER);			
	algorithm->SetParameters("MODE=Load; MASK=T; FILENAME=cut1.txt");
	
	//RA: debugging
	Handle<DecisionTable> test_tmp = dynamic_cast(DecisionTable *, test.Apply(*algorithm));
	test = test_tmp.GetReference();
	//originally:
	//test = *dynamic_cast(DecisionTable *, test.Apply(*algorithm));
	
	if (&test == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	
	// Mask remaining attributes
	if (!test.SetAttributeMasks(mask_test)) {
		return false;
	}
	
	if (GetIDG()) {
		ofstream idg;
		if (!IOKit::Open(idg,GetIDGFilename())) {
			Message::Error("Unable to open output file.");
			return false;
		}
		for (j = 0; j < train.GetNoAttributes(true); j++) {
			String name = train.GetAttributeName(j, true);
			
			idg<< "begin " + name + "\n";
			idg<< "\tnodes Undefined *\n";
			idg<< "\tUndefined -> *\n";
			idg<< "\tmake-reflexive\n";
			idg<< "end " + name + "\n";
		}
		idg.close();
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: FeatureSelection
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......: Currently not in use
// Revisions.....:
//===================================================================
bool
CVClassifier::FeatureSelection(DecisionTable &train, DecisionTable &test, int iteration, bool masked) const {
	
	int i, j, k, l, m;
	
	masked = false;
	
	Message message;
	
	bool log = true;
	
	int no_objects = train.GetNoObjects(masked);
	int no_attributes = train.GetNoAttributes(masked);
	
	Vector(DecisionTable::Mask) mask;
	
	if (!train.GetAttributeMasks(mask))
		return false;
	
	for (k = 0; k < mask.size()-1; k++)
		mask[k] = DecisionTable::MASK_DISABLED;
	
	// Track gene-word relationships and compute correlations
	
	ofstream corr;
	if (log && !IOKit::Open(corr, "correlations-" + String::Format(iteration+1) + ".txt")) {
		Message::Error("Unable to open output file.");
		return false;
	}
	
	// Object relatert
	
	Vector(Vector(int)) decisions; 
	decisions.erase(decisions.begin(), decisions.end());
	decisions.reserve(no_objects);
	for (i = 0; i < no_objects; i++) {
		decisions.push_back(Vector(int)());
		for (j = 0; j < no_objects; j++) {
			if (train.GetEntry(i, 0, masked) == train.GetEntry(j, 0, masked))
				decisions[i].push_back(train.GetEntry(j, train.GetDecisionAttribute(masked), masked));
		}
	}
	
	Vector(Vector(FIPair)) tracked(no_objects,Vector(FIPair)());
	
	// Create y-vector
	Vector(int) y;
	y.erase(y.begin(), y.end());
	y.reserve(no_objects);
	for (i = 0; i < no_objects; i++) {
		
		if (!message.Progress("Computing correlation for object " + String::Format(i + 1) + " of " + String::Format(no_objects) + "...", i, no_objects))
			break;
		
		int symbol = train.GetEntry(i, 0, masked);
		
		y.erase(y.begin(), y.end());
		
		for (k = 0; k < no_objects; k++) {
			int d = train.GetEntry(k, train.GetDecisionAttribute(masked), masked);
			
			bool found = false;
			for (l = 0; l < decisions[i].size(); l++) {
				for (m = 0; m < decisions[k].size(); m++) {
					if (decisions[k][m] == decisions[i][l]) {
						found = true;
						break;
					}
				}
				if (found) break;
			}
			if (found) {
				y.push_back(1);
			} else {
				y.push_back(0);
			}
		}
		
		// Create x-vector
		Vector(int) x;
		x.reserve(no_objects);
		
		// Run through all attributes 
		for (j = 1; j < no_attributes-1; j++) {
			if (train.GetEntry(i, j, true, masked) == "Undefined")
				continue;
			
			int value = train.GetEntry(i, j, masked);
			
			x.erase(x.begin(), x.end());
			
			for (k = 0; k < no_objects; k++) {
				if (train.GetEntry(k, j, masked) == value) {
					x.push_back(1);
				} else {
					x.push_back(0);
				}
			}
			
			FIPair pair;
			float corr = 0;
			if (true) {
				int p = 0;
				for (int t = 0; t < x.size(); t++) {
					if (x[t] == 1) {
						p++;
						if (y[t] == 1)
							corr++;
					}
				}
				if (corr < coverage_thr_)
					continue;
				corr = corr/p;
			} else {
				corr = MathKit::PearsonCorrelation(x, y);
			}
			if (corr == Undefined::Float())
				continue;
			pair.first = corr;
			pair.second = j;
			tracked[i].push_back(pair);
		}
		
		FIPairCompareFirst comparator;
		std::sort(tracked[i].begin(), tracked[i].end(), comparator);
		
		if (log) {
			corr<< train.GetEntry(i, 0, true, masked) + "  " + String::Format(i+1) + "/" + String::Format(no_objects) + " GO: ";
			corr<< " GO: ";
			for (k = 0; k < decisions[i].size(); k++) {
				corr << " " + train.GetDictionaryEntry(train.GetDecisionAttribute(masked), decisions[i][k], masked);
			}
			corr << "\n";
		}
		for (k = 0; k < tracked[i].size() && k < max_features_thr_; k++) {
			if (tracked[i][k].first < accuracy_thr_ && k+1 > min_features_thr_) break;
			mask[tracked[i][k].second] = DecisionTable::MASK_ENABLED;
			if (log)
				corr<< "  " + train.GetAttributeName(tracked[i][k].second, masked) + " = " + String::Format(tracked[i][k].first) + "\n";
		}
	}
	
	// Set mask
	if (!train.SetAttributeMasks(mask))
		return false;
	if (!test.SetAttributeMasks(mask))
		return false;
	
	if (log)
		corr<< "\n\nNo. attributes: " + String::Format(train.GetNoAttributes(true)-1) + "\n";
	
	// Print IDG
	if (GetIDG()) {
		ofstream idg;
		if (!IOKit::Open(idg,GetIDGFilename())) {
			Message::Error("Unable to open output file.");
			return false;
		}
		for (j = 0; j < train.GetNoAttributes(true)-1; j++) {
			
			String name = train.GetAttributeName(j, true);
			
			idg<< "begin " + name + "\n";
			idg<< "\tnodes Undefined *\n";
			idg<< "\tUndefined -> *\n";
			idg<< "\tmake-reflexive\n";
			idg<< "end " + name + "\n";
		}
		idg.close();
	}
	
	// Empty Objects?
	int empty_obj = 0;
	for (i = train.GetNoObjects(true)-1; i >= 0 ; i--) {
		bool found = false;
		for (j = 0; j < train.GetNoAttributes(true)-1; j++) {
			if (train.GetEntry(i, j, true, true) != "Undefined") {
				found = true;
				break;
			}
		}
		if (!found) {
			empty_obj++;
			train.RemoveObject(i,masked);
		}
	}
	if (log)
		corr <<"Empty training genes: " + String::Format(empty_obj) + "\n";
	
	Vector(int) empty;
	empty.erase(empty.begin(), empty.end());
	empty.reserve(test.GetNoObjects(masked));
	
	empty_obj = 0;
	for (i = test.GetNoObjects(true)-1; i >= 0 ; i--) {
		bool found = false;
		for (j = 0; j < test.GetNoAttributes(true)-1; j++) {
			if (test.GetEntry(i, j, true, true) != "Undefined") {
				found = true;
				break;
			}
		}
		if (!found) {
			empty_obj++;
			empty.push_back(test.GetEntry(i, test.GetDecisionAttribute(masked),masked)); 
			//test.RemoveObject(i,masked);
		}
	}
	if (log) {
		corr<< "Empty testing genes: " + String::Format(empty_obj) + "\n";
		for (k = 0; k < empty.size(); k++) {
			corr << " " + test.GetDictionaryEntry(test.GetDecisionAttribute(masked), empty[k], masked);
		}
	}
	
	// Remove classes with few examples
	if (false) {
		Vector(int) decision_values;
		Vector(int) decision_cardinalities;
		train.GetValueSet(decision_values, decision_cardinalities, train.GetDecisionAttribute(masked), masked);
		
		for (i = train.GetNoObjects(masked)-1; i >= 0 ; i--) {
			for (j = 0; j < decision_values.size(); j++) {
				if (train.GetEntry(i, train.GetDecisionAttribute(masked),masked) == decision_values[j]
					&& decision_cardinalities[j] < 5) {
					train.RemoveObject(i,masked);
					break;
				}
			}
		}
		for (i = test.GetNoObjects(masked)-1; i >= 0 ; i--) {
			for (j = 0; j < decision_values.size(); j++) {
				if (test.GetEntry(i, test.GetDecisionAttribute(masked),masked) == decision_values[j]
					&& decision_cardinalities[j] < 5) {
					test.RemoveObject(i,masked);
					break;
				}
			}
		}
	}
	
	corr.close();
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: SetGeneralizedDecision
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
bool
CVClassifier::SetGeneralizedDecision(const DecisionTable &train, DecisionTable &test) {
	
	int i,j;
	
	Message message;
	
	int decision_attribute = test.GetNoAttributes(false)-1;
	
	for (i = 0; i < test.GetNoObjects(false); i++) {
		gd_.push_back(Vector(int)());
		
		/*
		String gene1 = test.GetEntry(i,0,true,false);
		String annot1 = test.GetEntry(i,decision_attribute,true,false);
		*/
		
		for (j = test.GetNoObjects(false) - 1; j >= i ; j--) {
			if (test.GetEntry(i,0,false) == test.GetEntry(j,0,false)) {
				gd_[gd_.size()-1].push_back(test.GetEntry(j, decision_attribute, false));
				if (i != j) {
					test.RemoveObject(j,false);
				}
			}
		}
		
		/*
		String gene2;
		String annot2;
		*/
		
		bool found = false;
		for (j = 0; j < train.GetNoObjects(false); j++) {
			if (test.GetEntry(i,0,false) == train.GetEntry(j,0,false)) {
				found = true;
				gd_[gd_.size()-1].push_back(train.GetEntry(j, decision_attribute, false));
				
				/*
				gene2 = train.GetEntry(j,0,true,false);
				annot2 = train.GetEntry(j,decision_attribute,true,false);
				*/
			}
		}
		if (found) {
			both_++;
			//message.Notify(String::Format(both_) + ": " + gene1 + ": " + annot1 + "==" + gene2 + ": " + annot2);
		}
	}
	
	return true;
}

//-------------------------------------------------------------------
// Method........: CVOnce
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
CVClassifier::CVOnce(DecisionTable &train, DecisionTable &test, Vector(ICPair) &classification_details, const int iteration, const bool masked) const {
	
	Message message;
	Handle<Algorithm> algorithm;
	int i;
	
	// Remove empty objects in training set
	for (i = train.GetNoObjects(false)-1; i >= 0 ; i--) {
		bool found = false;
		for (int j = 1; j < train.GetNoAttributes(false)-1; j++) {
			if (train.GetEntry(i, j,true, false) != "Undefined") {
				found = true;
				break;
			}
		}
		if (!found) {
			train.RemoveObject(i,false);
		}
	}
	
	// TRAINING
	
	//- Find reducts
	Handle<Structure> reducts = const_cast(DecisionTable *, &train)->Apply(*GetReducer());
	if (reducts.IsNULL()) {
		message.Error("Failed to apply " + IdHolder::GetClassname(GetReducer()->GetId()) + ".");
		return false;
	}
	
	//needed?
	//const_cast(DecisionTable *, &train)->AppendChild(reducts.GetPointer());
	
	
	// Generate Rules
	algorithm = ObjectManager::GetIdentifiedAlgorithm(RULEGENERATOR);
	algorithm->SetParameters("");
	
	Handle<Rules> rules = dynamic_cast(Rules *,const_cast(Structure *, reducts->Apply(*algorithm)));
	if (rules.IsNULL()) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	
	//needed?
	//reducts->AppendChild(rules.GetPointer());
	
	// Prune rules
	if (GetRulePruning()) {
		
		rules = PruneRules(*const_cast(DecisionTable *, &train), *rules);
		if (rules.IsNULL()){
			return false;
		}
		
		//needed?
		//reducts->AppendChild(rules.GetPointer());
	}
	
	// TESTING
	
	Vector(int) decision_values;
	Vector(int) decision_cardinalities;
	test.GetValueSet(decision_values, decision_cardinalities, test.GetDecisionAttribute(true), true);
	int largest_class; int largest_class_size = 0;
	for (i = 0; i < decision_values.size(); i++) {
		if (decision_cardinalities[i] > largest_class_size) {
			largest_class_size = decision_cardinalities[i];
			largest_class = decision_values[i];
		}
	}
	
	//- Classify
	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *,ObjectManager::GetIdentifiedAlgorithm(BATCHCLASSIFIER));
	Handle<BatchClassification> batchclassification;
	
	//Voter
	batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + String::Format(largest_class) + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; LOG.FILENAME = log" + String::Format(iteration) + ".txt; CALIBRATION=F; ROC=F;");
	batchclassifier->SetRules(rules.GetPointer());
	
	// Apply algorithm for classification.
	batchclassification = dynamic_cast(BatchClassification *, test.Apply(*batchclassifier));
	//Failure?
	if (batchclassification.IsNULL()) {
		message.Error("Failed to apply " + IdHolder::GetClassname(batchclassifier->GetId()) + ".");
		return false;
	}
	
	Vector(ICPair) tmp(batchclassification->GetClassificationDetails());
	for (i = 0; i < tmp.size(); i++) {
		classification_details.push_back(tmp[i]);
	}
	
	return true;
}

//-------------------------------------------------------------------
// Method........: PruneRules
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: removed formal parameter Structure &reducts
//===================================================================

Handle<Rules>
CVClassifier::PruneRules(DecisionTable &table, const Rules &rules) const {
	
	bool masked = true;
	
	int no_rules = rules.GetNoRules();
	int no_objects = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(false);
	
	if (no_rules == 0) {
		Message::Warning("Empty rule set.", false);
		return NULL;
	}
	
	Message   message;
	Discerner discerner;
	
	// Initialize discerner.
	if (GetIDG()) {
		if (!discerner.LoadIDGs(GetIDGFilename(), *const_cast(DecisionTable *, &table), masked)) {
			Message::Error("Failed to load IDGs.");
			return NULL;
		}
	}
	
	int i, j, k;
	
	Vector(Vector(int)) tracked(no_objects,Vector(int)());
	
	Handle<InformationVector> inf = Creator::InformationVector();
	
	float fraction = 0.0;
	
	// Build map from objects to rules.
	for (i = 0; i < no_objects; i++) {
		if (!inf->Create(*const_cast(DecisionTable *, &table), i, true))
			return NULL;
		for (j = 0; j < no_rules; j++) {
			if (rules.GetRule(j)->Matches(*inf, discerner, fraction))
				tracked[i].push_back(j);
		}
	}
	
	// PRUNE
	
	Vector(int) decision_values;
	Vector(int) decision_cardinalities;
	table.GetValueSet(decision_values, decision_cardinalities, table.GetDecisionAttribute(masked), masked);
	int largest_class; int largest_class_size = 0;
	for (i = 0; i < decision_values.size(); i++) {
		if (decision_cardinalities[i] > largest_class_size) {
			largest_class_size = decision_cardinalities[i];
			largest_class = decision_values[i];
		}
	}
	
	// Compute scores
	
	Vector(Vector(FIPair)) scores;
	scores.erase(scores.begin(), scores.end());
	scores.reserve(no_objects);
	
	// For each object ... 
	for (i = 0; i < no_objects; i++) {
		scores.push_back(Vector(FIPair)());
		
		int decision = table.GetEntry(i, table.GetDecisionAttribute(masked), masked);
		
		// ... and each rule for this object
		for (j = 0; j < tracked[i].size(); j++) {
			
			Handle<Rule> rule = rules.GetRule(tracked[i][j]);
			
			int no_decisions = rule->GetNoDecisionValues();
			
			bool found = 0;
			for (k = 0; k < no_decisions; k++) {
				
				int sub_decision = rule->GetDecisionValue(k);
				
				if (decision != sub_decision)
					continue;
				
				float coverage = rule->GetCoverage(k, decision_values, decision_cardinalities);
				if (coverage < GetRulePruningCoverage())
					continue;
				float accuracy = rule->GetAccuracy(k);
				if (accuracy < GetRulePruningAccuracy())
					continue;
				
				//float w1 = 0.5f + 0.25f * accuracy;
				//float w2 = 0.5f - 0.25f * accuracy;
				
				//float torgo = (w1 * accuracy) + (w2 * coverage);
				float torgo = coverage;
				
				FIPair pair;
				pair.first = torgo;
				pair.second = tracked[i][j];
				
				scores[i].push_back(pair);
				
				break;
			}
		}
		
		FIPairCompareFirst comparator;
		std::sort(scores[i].begin(), scores[i].end(), comparator);
	}
	
	// New rule set
	Handle<Rules> new_rules_max;
	Handle<Rules> new_rules = Creator::Rules();
	
	// Compute "generalized" decisions
	Vector(Vector(int)) gd;
	gd.erase(gd.begin(), gd.end());
	gd.reserve(no_objects);
	
	for (i = 0; i < no_objects; i++) {
		gd.push_back(Vector(int)());
		for (j = no_objects - 1; j >= 0 ; j--) {
			if (table.GetEntry(i,0,false) == table.GetEntry(j,0,false))
				gd[i].push_back(table.GetEntry(j, no_attributes - 1, false));
		}
	}
	
	// Add rules iteratively
	
	// Keep track of correctly classified objects
	Vector(bool) correct(no_objects,false);
	
	// Keep track of no. of rules added so far for each object
	Vector(int) norules(no_objects,0);
	
	//RA: moved out from loop
	Vector(ICPair) classification_details;
	classification_details.erase(classification_details.begin(),classification_details.end());
	classification_details.reserve(no_objects);
	
	int itr = 0;
	float max_auc = 0;
	while (true) {
		
		if (!message.Progress("Pruning rule " + String::Format(itr + 1) + "..."))
			return NULL;
		
		// For each object add new rule if object was misclassified
		for (i = 0; i < no_objects; i++) {
			
			if (correct[i])
				continue;
			
			// Any rules left?
			if (norules[i] == scores[i].size())
				continue;
			
			Handle<Rule> rule = rules.GetRule(scores[i][norules[i]].second);
			
			bool is_unique = true;
			
			// Avoid duplicates.
			for (j = new_rules->GetNoRules() - 1; j >= 0; j--) {
				if (*rule == *new_rules->GetRule(j)) {
					is_unique = false;
					break;
				}
			}
			
			if (is_unique && !new_rules->AppendStructure(rule.GetPointer())) {
				Message::Error("Failed to add new rule to rule set.");
				return NULL;
			}
			
			norules[i]++;
		}
		
		//- Classify
		Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *,ObjectManager::GetIdentifiedAlgorithm(BATCHCLASSIFIER));
		
		//Voter
		batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + String::Format(largest_class) + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; LOG.FILENAME = log" + String::Format(itr) + ".txt; CALIBRATION=F; ROC=F;");
		batchclassifier->SetRules(new_rules.GetPointer());
		
		// Apply algorithm for classification.
		Handle<BatchClassification> batchclassification = dynamic_cast(BatchClassification *, table.Apply(*batchclassifier));
		//Failure?
		if (batchclassification.IsNULL()) {
			message.Error("Failed to apply " + IdHolder::GetClassname(batchclassifier->GetId()) + ".");
			return NULL;
		}
		
		classification_details.erase(classification_details.begin(), classification_details.end());
		classification_details = batchclassification->GetClassificationDetails();
		
		// Update correct
		
		float auc = 0;
		float sens = 0;
		float spec = 0;
		
		Vector(float) thr; thr.erase(thr.begin(),thr.end()); 
		thr.reserve(1000);
		
		for (i = 0; i < decision_values.size(); i++) {
			int roc_class = decision_values[i];
			
			for (j = 0; j < classification_details.size(); j++) {
				if (classification_details[j].first != roc_class) {
					for (k = 0; k < gd[j].size(); k++) {
						if (gd[j][k] == roc_class) {
							classification_details[j].first = roc_class;
							break;
						}
					}
				}
			}
			
			ROCCurve roc;
			Vector(int)   roc_targets;
			Vector(float) roc_outputs;
			
			roc.SetWeight(GetWeight());
			
			int  roc_resolution = 251;
			bool roc_progress   = true;
			if (!roc.Create(classification_details, roc_class, roc_resolution, roc_progress, &roc_targets, &roc_outputs)) {
				Message::Warning("Generation of ROC curve failed.");
				return NULL;
			} else {
				auc += roc.GetArea();
				thr[roc_class] = roc.GetNWThreshold();
				float sensitivity_nw	= Undefined::Float();
				float specificity_nw	= Undefined::Float();
				roc.GetNWAccuracy(sensitivity_nw,specificity_nw);
				sens += sensitivity_nw;
				spec += specificity_nw;
			}
		}
		
		float error_rate = 0;
		for (i = 0; i < classification_details.size(); i++) {
			int decision = table.GetEntry(i, table.GetDecisionAttribute(masked), masked);
			bool cor = true;
			for (k = 0; k < classification_details[i].second->GetNoDecisionValues(); k++) {
				if (classification_details[i].second->GetDecisionValue(k) == decision
					&& classification_details[i].second->GetCertaintyCoefficient(k) < thr[decision]) {
					cor = false;
					break;
				} else if (classification_details[i].second->GetDecisionValue(k) != decision
					&& classification_details[i].second->GetCertaintyCoefficient(k) >= thr[decision]) {
					cor = false;
					break;
				}
			}
			if (cor) {
				correct[i] = true;
			} else {
				correct[i] = false;
				error_rate++;
			}
		}
		
		auc = auc/decision_values.size();
		sens = sens/decision_values.size();
		spec = spec/decision_values.size();
		error_rate = error_rate/decision_values.size();
		
		int no_rules = new_rules->GetNoRules();
		
		message.Notify("AUC " + String::Format(itr) + ": " + String::Format(auc) + "(" + String::Format(sens) + "," + String::Format(spec) + "," + String::Format(error_rate) + "), No. Rules: " + String::Format(no_rules));
		
		if (auc > max_auc) {
			max_auc = auc;
			new_rules_max = dynamic_cast(Rules*, const_cast(Structure*, new_rules->Duplicate()));
		} else {
			return new_rules_max;
		}
		
		itr++;
	}
	
}

//-------------------------------------------------------------------
// Methods copied from CVSerialExecutor
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Method........: InitializeSamplingScheme
// Author........: Aleksander ¢hrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Torgeir R. Hvidsten
//===================================================================

bool
CVClassifier::InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng) {
	
	int no_objects = table.GetNoObjects(masked);
	
	// Clear and initialize vector of partition indices.
	indices_.erase(indices_.begin(), indices_.end());
	
	// Reserve space to avoid unneeded allocations.
	indices_.reserve(no_objects);
	
	int i, j, k;
	
	for (i = 0; i < no_objects; i++)
		indices_.push_back(Undefined::Integer());
	
	// Create vector for sampling indices.
	Vector(int) remaining;
	remaining.erase(remaining.begin(), remaining.end());
	
	// Reserve space to avoid unneeded allocations.
	remaining.reserve(no_objects);
	
	for (i = 0; i < no_objects; i++)
		remaining.push_back(i);
	
	// Compute partitionsize, rounded to nearest integer.
	int partitionsize = MathKit::Round(static_cast(float,no_objects) / GetCV());
	
	bool back_comp = BackwardCompatible(*const_cast(DecisionTable *, &table));
	
	// Compute partitions. The last partition may be non-ideal in size.
	for (i = 0; i < GetCV(); i++) {
		for (j = 0; j < partitionsize; j++) {
			if (remaining.empty())
				break;
			int index = rng.DrawInteger(0, remaining.size() - 1);
			indices_[remaining[index]] = i;
			int obj = remaining[index];
			remaining[index] = remaining[remaining.size() - 1];
			remaining.pop_back();
			
			// Also add all objects with the same origin
			if (back_comp) {
				for (k = remaining.size() - 1; k >= 0; k--) {
					if (table.GetEntry(remaining[k],0,false) == table.GetEntry(obj,0,false)) {
						indices_[remaining[k]] = i;
						remaining[k] = remaining[remaining.size() - 1];
						remaining.pop_back();
						j++;
					}
				}
			}
		}
		if (i == GetCV() - 1 && !remaining.empty()) {
			for (k = 0; k < remaining.size(); k++)
				indices_[remaining[k]] = GetCV() - 1;
		}
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: SampleTables
// Author........: Aleksander ¢hrn
// Date..........:
// Revisions.....:
//===================================================================

bool
CVClassifier::SampleTables(int i, const RNG &/*rng*/, DecisionTable &training, DecisionTable &testing, bool masked) const {
	
	// Verify dimensions.
	if (indices_.size() != training.GetNoObjects(masked))
		return false;
	
	if (indices_.size() != testing.GetNoObjects(masked))
		return false;
	
	// Remove children, if any.
	training.RemoveAllChildren();
	testing.RemoveAllChildren();
	
	int j;
	
	// Sample tables (by removing objects)
	for (j = indices_.size() - 1; j >= 0; j--) {
		
		// Does the current object belong to the training table?
		bool is_training = (indices_[j] != i);
		
		// Remove object.
		if (is_training) {
			if (!testing.RemoveObject(j, masked))
				return false;
		}
		else {
			if (!training.RemoveObject(j, masked))
				return false;
		}
		
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Methods for backward compatibility
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Method........: BackwardCompatible
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

bool
CVClassifier::BackwardCompatible(const DecisionTable &table) const {
	
	RNG rng(GetSeed());
	
	bool masked = true;
	
	int i, j, k;
	
	Message message;
	
	int no_objects = table.GetNoObjects(masked);
	int no_attrib = table.GetNoAttributes(masked);
	
	Handle<DecisionTable> org_table = dynamic_cast(DecisionTable *, table.Duplicate());
	Normalize(*org_table);
	
	float J = GetLearnability(*org_table);
	
	float max = 0;
	int larger = 0;
	
	message.Notify("J = " + String::Format(J));
	
	//if ((J > 0.0575 && J < 0.0577) || larger < 5) {
	if ((J > 0.0575 && J < 0.0577) || (J > 0.00014 && J < 0.00016)) {
		return false;
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: Normalize
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

bool
CVClassifier::Normalize(DecisionTable &table) const {
	
	int masked = false;
	
	int i, j, k;
	
	int no_objects = table.GetNoObjects(masked);
	int no_attrib = table.GetNoAttributes(masked);
	
	Vector(float) mean(no_attrib,0);
	
	Vector(float) var(no_attrib,0); 
	
	// Compute mean vector
	for (i = 0; i < no_objects; i++) {
		
		for (j = 1; j < no_attrib-1; j++) {
			mean[j-1] += table.GetEntry(i,j,true,masked).GetFloat();
		}
	}
	for (j = 1; j < no_attrib-1; j++) {
		mean[j-1] = mean[j-1]/no_objects;
	}
	
	// Compute variance vector
	for (i = 0; i < no_objects; i++) {
		
		for (j = 1; j < no_attrib-1; j++) {
			var[j-1] += (table.GetEntry(i,j,true,masked).GetFloat() - mean[j-1])*(table.GetEntry(i,j,true,masked).GetFloat() - mean[j-1]);
		}
	}
	for (j = 1; j < no_attrib-1; j++) {
		var[j-1] = sqrt(var[j-1]/(no_objects-1));
	}
	
	// Update table
	for (i = 0; i < no_objects; i++) {
		for (j = 1; j < no_attrib-1; j++) {
			
			if (var[j-1] == 0)
				continue;
			
			float z = (table.GetEntry(i,j,true,masked).GetFloat() - mean[j-1])/var[j-1];
			
			// Insert values
			int dictonary_value = table.SuggestDictionaryEntry(j, String::Format(z), masked);
			if (!table.SetEntry(i, j, dictonary_value, masked)) {
				Message::Error("Error setting table entry (" + String::Format(i) + ", " + String::Format(j) + ").");
				return false;
			}
			if (!table.SetDictionaryEntry(j, dictonary_value, String::Format(z), masked)) {
				Message::Error("Error setting dictionary entry (" + String::Format(j) + ", " + String::Format(dictonary_value) + ", \"" + String::Format(z) + "\").");
				return false;
			}
		}
	}
	
	return true;
}

//-------------------------------------------------------------------
// Method........: GetLearnability
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

float
CVClassifier::GetLearnability(const DecisionTable &table) const {
	
	Message message;
	
	bool masked = true;
	
	int i, ii, j, k;
	
	int no_objects = table.GetNoObjects(masked);
	int no_attrib = table.GetNoAttributes(masked);
	
	Vector(int) decision_values;
	Vector(int) decision_cardinalities;
	table.GetValueSet(decision_values, decision_cardinalities, table.GetDecisionAttribute(masked), masked);
	
	Vector(Vector(int)) track(decision_values.size(),Vector(int)()); 
	
	for (i = 0; i < no_objects; i++) {
		for (k = 0; k < decision_values.size(); k++) {
			if (decision_values[k] == table.GetEntry(i, table.GetDecisionAttribute(masked), masked)) {
				track[k].push_back(i);
				break;
			}
		}
	}
	
	Vector(float) diff(decision_values.size(),0.0f);
	//diff.erase(diff.begin(),diff.end()); 
	//diff.reserve(decision_values.size());
	
	Vector(int) norm(decision_values.size(),0); 
	//norm.erase(norm.begin(),norm.end());
	//norm.reserve(decision_values.size());
	
	for (k = 0; k < decision_values.size(); k++) {
		//diff.push_back(0);
		//norm.push_back(0);
#ifdef X86_64	
		if (!message.Progress("Computing  " + String::Format((int)k) + " of " + String::Format((int)decision_values.size()) + "...", k, decision_values.size()))
#else
		if (!message.Progress("Computing  " + String::Format(k) + " of " + String::Format(decision_values.size()) + "...", k, decision_values.size()))
#endif
			break;
		
		for (i = 0; i < track[k].size(); i++) {
			
			for (ii = i+1; ii < track[k].size(); ii++) {
				
				//message.Notify(String::Format(k) + " (" + String::Format(track[k].size()) + ") " + String::Format(i) + " - " + String::Format(ii) + " " + table.GetEntry(track[k][i],0,true,masked) + " - " + table.GetEntry(track[k][ii],0,true,masked));
				
				float d1 = 0;
				for (j = 1; j < no_attrib-1; j++) {
					d1 += fabs(table.GetEntry(track[k][i],j,true,masked).GetFloat() - table.GetEntry(track[k][ii],j,true,masked).GetFloat());
					//d1 += fabs(table.GetEntry(track[k][i],j+1,true,masked).GetFloat() - table.GetEntry(track[k][i],j,true,masked).GetFloat() - table.GetEntry(track[k][ii],j+1,true,masked).GetFloat() - table.GetEntry(track[k][ii],j,true,masked).GetFloat());
				}
				float d2 = 0;
				for (j = 1; j < no_attrib-1; j++) {
					d2 += fabs(table.GetEntry(track[k][i],j,true,masked).GetFloat() + table.GetEntry(track[k][ii],j,true,masked).GetFloat());
					//d2 += fabs(table.GetEntry(track[k][i],j+1,true,masked).GetFloat() - table.GetEntry(track[k][i],j,true,masked).GetFloat() + table.GetEntry(track[k][ii],j+1,true,masked).GetFloat() - table.GetEntry(track[k][ii],j,true,masked).GetFloat());
				}
				float d = d1;
				if (d2 < d1) d = d2;
				if (d/(no_attrib-2) < 0.3)
					diff[k]++;
				norm[k]++;
			}
		}
	}
	
	float J = 0;
	for (k = 0; k < decision_values.size(); k++) {
		//message.Notify(String::Format(diff[k]) + "/" + String::Format(norm[k]) + "=" + String::Format(diff[k]/norm[k]));
		J += diff[k]/norm[k];
	}
	J = J/decision_values.size();
	
	return J;
}

 
CVClassifier *
CVClassifier::Clone() {
	return new CVClassifier;
} 
