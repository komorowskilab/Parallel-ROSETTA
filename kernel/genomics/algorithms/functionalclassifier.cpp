//-------------------------------------------------------------------
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....: Robin Andersson
//                 Added support for randomization and iteration
//                 over parameter settings.
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/system/fstream.h>

#include <kernel/genomics/algorithms/functionalclassifier.h>
#include <kernel/genomics/algorithms/cvclassifier.h>

#include <kernel/structures/project.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/textfile.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/roccurve.h>
#include <kernel/structures/projectmanager.h>

#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/batchclassifier.h>

#include <kernel/sav/algorithms/savgeneticreducer.h>

#include <common/objectmanager.h>

#include <kernel/basic/set.h>
#include <kernel/basic/message.h>
#include <kernel/basic/algorithm.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/discerner.h>


//-------------------------------------------------------------------
// Methods for class TemplateScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

FunctionalClassifier::FunctionalClassifier() {
	
	//Templates
	//--------------------------
	// Templates start values
	template_length_ = 2;
	constant_template_length_ = 3;
	crease_thr_ = 0.6f;
	constant_thr_ = 0.2f;
	start_end_thr_ = 0.1f;
	tolerance_thr_ = 0.2f;
	//Templates end values
	template_length_to_ = template_length_;
	constant_template_length_to_ = constant_template_length_;
	crease_thr_to_ = crease_thr_;
	constant_thr_to_ = constant_thr_;
	start_end_thr_to_ = start_end_thr_;
	tolerance_thr_to_ = tolerance_thr_;
	//Templates iterative step
	template_length_step_ = 1;
	constant_template_length_step_ = 1;
	crease_thr_step_ = 0.1f;
	constant_thr_step_ = 0.1f;
	start_end_thr_step_ = 0.1f;
	tolerance_thr_step_ = 0.1f;
	//Iterate over parameter settings?
	iterate_cv_ = false;
	
	//Randomization
	rands_ = 0;
	
	//Indiscernibility graph support for reducer
	idg_ = true;
	idg_file_ = "idg-not-symmetric.txt";
	
	//Cross validation parameters
	cvlog_ = true;
	cvlog_file_ = "cv.txt";
	cv_ = 10;
	rng_ = 0;
	
	decision_ = "unknown";
	
	log_ = true;
	log_file_ = "classifications.txt";
	rulelog_file_ = "rules.txt";
	
	randlog_file_ = "rand.txt";
	paramslog_file_ = "params.txt";
	weight_ = 0.0f;
	
	//Rule pruning
	prune_ = true;
	prune_coverage_thr_ = 0.0f;
	prune_accuracy_thr_ = 0.0f;
	
	reducer_ = dynamic_cast(Reducer*, ObjectManager::GetIdentifiedAlgorithm(SAVGENETICREDUCER));
	//reducer_->SetParameters("DISCERNIBILITY=Object; MODULO.DECISION=T; PRECOMPUTE=F; BRT=F; BRT.PRECISION=0.1; SELECTION=All; IDG=" + String::Format(GetIDG()) +"; IDG.FILENAME=" + GetIDGFile() + "; APPROXIMATE=T; FRACTION=0.9; SIZE.KEEP=256; KEEP.LEVELS=3;");
}

FunctionalClassifier::~FunctionalClassifier() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(FunctionalClassifier, FUNCTIONALCLASSIFIER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
FunctionalClassifier::GetParameters() const {
	
	String parameters = "";

	// CV
	// --

	parameters += Keyword::CV() + Keyword::Dot() + Keyword::Log();
	parameters += Keyword::Assignment();
	parameters += String::Format(cvlog_);
	parameters += Keyword::Separator();
	
	if (cvlog_) {

		parameters += Keyword::Number();
		parameters += Keyword::Assignment();
		parameters += String::Format(cv_);
		parameters += Keyword::Separator();

		parameters += Keyword::CV() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += cvlog_file_;
		parameters += Keyword::Separator();

		parameters += Keyword::Seed();
		parameters += Keyword::Assignment();
		parameters += String::Format(rng_);
		parameters += Keyword::Separator();
	}
	
	// TemplateScaler
	// --------------
	
	parameters += Keyword::TemplateLength();
	parameters += Keyword::Assignment();
	parameters += String::Format(template_length_);
	parameters += Keyword::Separator();
	
	parameters += Keyword::ConstantTemplateLength();
	parameters += Keyword::Assignment();
	parameters += String::Format(constant_template_length_);
	parameters += Keyword::Separator();
	
	parameters += Keyword::CreaseThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(crease_thr_);
	parameters += Keyword::Separator();
	
	parameters += Keyword::ConstantThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(constant_thr_);
	parameters += Keyword::Separator();
	
	parameters += Keyword::StartEndThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(start_end_thr_);
	parameters += Keyword::Separator();
	
	parameters += Keyword::ToleranceThr();
	parameters += Keyword::Assignment();
	parameters += String::Format(tolerance_thr_);
	parameters += Keyword::Separator();
	
	parameters += Keyword::CV() + Keyword::Dot() + Keyword::Iterate();
	parameters += Keyword::Assignment();
	parameters += String::Format(iterate_cv_);
	parameters += Keyword::Separator();
	
	if (iterate_cv_) {

		parameters += Keyword::CV() + Keyword::Dot() + Keyword::Iterate() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += paramslog_file_;
		parameters += Keyword::Separator();
		
		parameters += Keyword::TemplateLength() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(template_length_to_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(constant_template_length_to_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::CreaseThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(crease_thr_to_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::ConstantThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(constant_thr_to_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::StartEndThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(start_end_thr_to_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::ToleranceThr() + Keyword::Dot() + Keyword::To();
		parameters += Keyword::Assignment();
		parameters += String::Format(tolerance_thr_to_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::TemplateLength() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(template_length_step_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(constant_template_length_step_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::CreaseThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(crease_thr_step_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::ConstantThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(constant_thr_step_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::StartEndThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(start_end_thr_step_);
		parameters += Keyword::Separator();
		
		parameters += Keyword::ToleranceThr() + Keyword::Dot() + Keyword::Step();
		parameters += Keyword::Assignment();
		parameters += String::Format(tolerance_thr_step_);
		parameters += Keyword::Separator();
		
	}
	
	parameters += Keyword::Randomization() + Keyword::Dot() + Keyword::Number();
	parameters += Keyword::Assignment();
	parameters += String::Format(rands_);
	parameters += Keyword::Separator();
	
	if (rands_ > 0) {
		parameters += Keyword::Randomization() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += randlog_file_;
		parameters += Keyword::Separator();
	}
	
	// Rule Pruning
	// ------------
	
	parameters += Keyword::RulePruning();
	parameters += Keyword::Assignment();
	parameters += String::Format(prune_);
	parameters += Keyword::Separator();
	
	if (prune_) {
		
		parameters += Keyword::RulePruning() + Keyword::Dot() + Keyword::Coverage();
		parameters += Keyword::Assignment();
		parameters += String::Format(prune_coverage_thr_);
		parameters += Keyword::Separator();
	
		parameters += Keyword::RulePruning() + Keyword::Dot() + Keyword::Accuracy();
		parameters += Keyword::Assignment();
		parameters += String::Format(prune_accuracy_thr_);
		parameters += Keyword::Separator();
	}
	// ReClassifier
	// -------------------
	
	parameters += Keyword::Log();
	parameters += Keyword::Assignment();
	parameters += String::Format(log_);
	parameters += Keyword::Separator();
	
	if (log_) {

		parameters += Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += log_file_;
		parameters += Keyword::Separator();
	
		parameters += Keyword::Rules() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += rulelog_file_;
		parameters += Keyword::Separator();
	}
	
	parameters += Keyword::Weight();
	parameters += Keyword::Assignment();
	parameters += String::Format(weight_);
	parameters += Keyword::Separator();

	// Reducer
	// -------
	parameters += Keyword::Reducer();
	parameters += Keyword::Assignment();
	
	if (GetReducer() == NULL) {
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
// Revisions.....:
//===================================================================

bool
FunctionalClassifier::SetParameter(const String &keyword, const String &value) {
	
	// CV
	// --
	
	if (keyword == Keyword::Number() && value.IsInteger()) {
		cv_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Log() && value.IsBoolean()) {
		cvlog_ = value.GetBoolean();
		return true;
	}
	
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename()) {
		cvlog_file_ = value;
		return true;
	}
	
	if (keyword == Keyword::Seed() && value.IsInteger()) {
		rng_ = value.GetInteger();
		return true;
	}
	
	// TemplateScaler
	// --------------
	
	if (keyword == Keyword::TemplateLength() && value.IsInteger()) {
		template_length_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::ConstantTemplateLength() && value.IsInteger()) {
		constant_template_length_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::CreaseThr() && value.IsFloat()) {
		crease_thr_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::ConstantThr() && value.IsFloat()) {
		constant_thr_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::StartEndThr() && value.IsFloat()) {
		start_end_thr_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::ToleranceThr() && value.IsFloat()) {
		tolerance_thr_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::TemplateLength() + Keyword::Dot() + Keyword::To() && value.IsInteger()) {
		template_length_to_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::To() && value.IsInteger()) {
		constant_template_length_to_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::CreaseThr() + Keyword::Dot() + Keyword::To() && value.IsFloat()) {
		crease_thr_to_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::ConstantThr() + Keyword::Dot() + Keyword::To() && value.IsFloat()) {
		constant_thr_to_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::StartEndThr() + Keyword::Dot() + Keyword::To() && value.IsFloat()) {
		start_end_thr_to_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::ToleranceThr() + Keyword::Dot() + Keyword::To() && value.IsFloat()) {
		tolerance_thr_to_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::TemplateLength() + Keyword::Dot() + Keyword::Step() && value.IsInteger()) {
		template_length_step_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::ConstantTemplateLength() + Keyword::Dot() + Keyword::Step() && value.IsInteger()) {
		constant_template_length_step_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::CreaseThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat()) {
		crease_thr_step_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::ConstantThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat()) {
		constant_thr_step_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::StartEndThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat()) {
		start_end_thr_step_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::ToleranceThr() + Keyword::Dot() + Keyword::Step() && value.IsFloat()) {
		tolerance_thr_step_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::Randomization() + Keyword::Dot() + Keyword::Number() && value.IsInteger()) {
		rands_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::Log() + Keyword::Dot() + Keyword::Filename()) {
		log_file_ = value;
		return true;
	}
	
	if (keyword == Keyword::Randomization() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename()) {
		randlog_file_ = value;
		return true;
	}
	
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Iterate() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename()) {
		paramslog_file_ = value;
		return true;
	}
	
	if (keyword == Keyword::CV() + Keyword::Dot() + Keyword::Iterate() && value.IsBoolean()) {
		iterate_cv_ = value.GetBoolean();
		return true;
	}
	
	if (keyword == Keyword::RulePruning() && value.IsBoolean()) {
		prune_ = value.GetBoolean();
		return true;
	}
	
	if (keyword == Keyword::RulePruning() + Keyword::Dot() + Keyword::Coverage() && value.IsFloat()) {
		prune_coverage_thr_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::RulePruning() + Keyword::Dot() + Keyword::Accuracy() && value.IsFloat()) {
		prune_accuracy_thr_ = value.GetFloat();
		return true;
	}
	
	if (keyword == Keyword::Decision()) {
		decision_ = value;
		return true;
	}
	
	// ReClassifier
	// -------------------
	
	if (keyword == Keyword::Log() && value.IsBoolean()) {
		log_ = value.GetBoolean();
		return true;
	}
	
	if (keyword == Keyword::Log() + Keyword::Dot() + Keyword::Filename()) {
		log_file_ = value;
		return true;
	}
	
	if (keyword == Keyword::Rules() + Keyword::Dot() + Keyword::Log() + Keyword::Dot() + Keyword::Filename()) {
		rulelog_file_ = value;
		return true;
	}
	
	if (keyword == Keyword::Weight() && value.IsFloat()) {
		weight_ = value.GetFloat();
		return true;
	}

	// Reducer
	// -------
	
	if (keyword == Keyword::Reducer()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (!algorithm.IsNULL() && !algorithm->IsA(REDUCER)) {
			Message::Error("Specified reducer is not a reducer.");
			return false;
		}
		
		reducer_ = dynamic_cast(Reducer*, algorithm.GetPointer());
		return true;
	}

	if (!reducer_.IsNULL())
		return reducer_->SetParameter(keyword,value);
	
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
FunctionalClassifier::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
FunctionalClassifier::Apply(Structure &structure) const {
	
	Message message;
	
	Handle<FunctionalClassifier> self = const_cast(FunctionalClassifier*, this);

	// Check and modify reducer
	if (reducer_.IsNULL() || !reducer_->IsA(REDUCER)) {
		Message::Error("Reducer algortihm is not set or not a proper reducer.");
		return NULL;
	}
	
	// Set important parameters, can override user settings...
	if (!GetReducer()->SetParameters("DISCERNIBILITY=Object; MODULO.DECISION=T; SELECTION=All")) {
		Message::Error("Needed parameter settings not supported by reducer: " + IdHolder::GetClassname(GetReducer()->GetId()) + "\nor by one of it's internal algorithms. Aborting...");
		return NULL;
	}

	// Check if IDG is supported
	if (GetIDG() && !GetReducer()->SetParameters("IDG=" + String::Format(GetIDG()) +"; IDG.FILENAME=" + GetIDGFile(), false)) {
		Message::Debug("Use of IDG not supported...");
		self->SetIDG(false);
	}

	// Temporary project
	Handle<Project> project = Creator::Project();
	if (!ProjectManager::InsertProject(project.GetPointer()))
		return NULL;
	if (!project->AppendChild(&structure)) {
		ProjectManager::RemoveProject(project.GetPointer());
		return NULL;
	}
	
	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;
	
	// Cast to verified type.
	Handle<DecisionTable> input = dynamic_cast(DecisionTable *, &structure);
	
	// If possible, do the thing
	if (input->GetNoAttributes(true) > 1) {
		message.Notify("Functional classification on  \"" + input->GetName() + "\"...");
		if (!DoSteps(*input)) 
			return NULL;
	} else {
		Message::Error("Table has no condition attributes.", false);
		return NULL;
	}
	
	// Clean up the project. Only keep textfiles.
	Identifier::Handles identifiers;
	if (!project->GetAllIdentifiers(TEXTFILE, identifiers))
		return NULL;
	
	input->RemoveAllChildren();
	
	int i;
	for (i = 0; i < identifiers.size(); i++) {
		
		input->AppendChild(dynamic_cast(Structure *, identifiers[i].GetPointer()));
		
	}
	
	return input.Release();
}

//-------------------------------------------------------------------
// New methods
//===================================================================

//-------------------------------------------------------------------
// Method........: DoSteps
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
FunctionalClassifier::DoSteps(DecisionTable &table) const {
	
	RNG rng(rng_);

	Message message;
	
	Handle<DecisionTable> labelled = dynamic_cast(DecisionTable *, table.Duplicate());
	Handle<DecisionTable> labelled_test = dynamic_cast(DecisionTable *, table.Duplicate());
	Handle<DecisionTable> unlabelled_test = dynamic_cast(DecisionTable *, table.Duplicate());
	
	if (!const_cast(FunctionalClassifier *, this)->Divide(table,*labelled,*labelled_test,*unlabelled_test))
		return false;
	
	// ----------------------------------------
	// Run CVClassifier
	// ----------------------------------------
	if (cvlog_) {
		message.Notify("CROSS VALIDATION");

		Handle<CVClassifier> algorithm = dynamic_cast(CVClassifier*, ObjectManager::GetIdentifiedAlgorithm(CVCLASSIFIER));
		
		//algorithm = ObjectManager::GetIdentifiedAlgorithm(CVCLASSIFIER);			
		algorithm->SetParameters("; NUMBER = " + String::Format(cv_) + 
			"; IDG = " + String::Format(idg_) + 
			"; IDG.FILENAME = " + idg_file_ + 
			"; LOG.FILENAME = " + cvlog_file_ + 
			"; TEMPLATELENGTH =" + String::Format(template_length_) + 
			"; CONSTANTTEMPLATELENGTH = " + String::Format(constant_template_length_) + 
			"; CREASETHR =" + String::Format(crease_thr_) + 
			"; CONSTANTTHR = " + String::Format(constant_thr_) + 
			"; STARTENDTHR = " + String::Format(start_end_thr_) + 
			"; TOLERANCETHR = " + String::Format(tolerance_thr_) + 
			"; TEMPLATELENGTH.TO =" + String::Format(template_length_to_) + 
			"; CONSTANTTEMPLATELENGTH.TO = " + String::Format(constant_template_length_to_) + 
			"; CREASETHR.TO =" + String::Format(crease_thr_to_) + 
			"; CONSTANTTHR.TO = " + String::Format(constant_thr_to_) + 
			"; STARTENDTHR.TO = " + String::Format(start_end_thr_to_) + 
			"; TOLERANCETHR.TO = " + String::Format(tolerance_thr_to_) + 
			"; TEMPLATELENGTH.STEP =" + String::Format(template_length_step_) + 
			"; CONSTANTTEMPLATELENGTH.STEP = " + String::Format(constant_template_length_step_) + 
			"; CREASETHR.STEP =" + String::Format(crease_thr_step_) + 
			"; CONSTANTTHR.STEP = " + String::Format(constant_thr_step_) + 
			"; STARTENDTHR.STEP = " + String::Format(start_end_thr_step_) + 
			"; TOLERANCETHR.STEP = " + String::Format(tolerance_thr_step_) + 
			"; CV.ITERATE = " + String::Format(iterate_cv_) + 
			"; CV.ITERATE.LOG.FILENAME = " + paramslog_file_ + 
			"; RANDOMIZATION.NUMBER = " + String::Format(rands_) + 
			"; RANDOMIZATION.LOG.FILENAME = " + randlog_file_ + 
			"; RULEPRUNING = " + String::Format(prune_) +  
			"; RULEPRUNING.COVERAGE = " + String::Format(prune_coverage_thr_) + 
			"; RULEPRUNING.ACCURACY = " + String::Format(prune_accuracy_thr_) + 
			"; WEIGHT = " + String::Format(weight_) + 
			"; SEED = " + String::Format(rng_) + ";");
		// Set reducer
		algorithm->SetReducer(reducer_);
		
		Handle<Structure> cv_table = dynamic_cast(DecisionTable *, labelled.GetPointer())->Apply(*algorithm);
		if (cv_table == NULL) {
			message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
			return false;
		}
	}
	
	// ----------------------------------------
	// ReClassify
	// ----------------------------------------
	
	if (log_) {
		message.Notify("RE-CLASSIFICATION AND CLASSIFICATION OF UNKNOWN GENES");
		
		if (!ReClassify(*labelled,*labelled_test,*unlabelled_test))
			return false;
	}
	
	return true;
	
}

//-------------------------------------------------------------------
// Method........: Divide
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
bool
FunctionalClassifier::Divide(DecisionTable &table, DecisionTable &labelled, DecisionTable &labelled_test, DecisionTable &unlabelled_test) {
	
	bool masked = true;
	
	int no_objects = table.GetNoObjects(masked);
	int decision_attribute = table.GetDecisionAttribute(masked);
	int i,j,k;
	
	for (i = no_objects - 1; i >= 0; i--) {
		if (table.GetEntry(i,decision_attribute,true,masked) == decision_) {
			labelled.RemoveObject(i,masked);
			labelled_test.RemoveObject(i,masked);
		} else {
			unlabelled_test.RemoveObject(i,masked);
		}
	}
	
	int no_attributes = labelled_test.GetNoAttributes(false);
	
	generalized_decision_.erase(generalized_decision_.begin(), generalized_decision_.end());
	generalized_decision_.reserve(no_objects);
	
	for (i = 0; i < labelled_test.GetNoObjects(masked); i++) {
		generalized_decision_.push_back(Vector(int)());
		generalized_decision_[i].push_back(labelled_test.GetEntry(i, decision_attribute, masked));
		for (j = labelled_test.GetNoObjects(masked) - 1; j > i; j--) {
			if (labelled_test.GetEntry(i, 0, false) == labelled_test.GetEntry(j, 0, false)) {
				bool found = false;
				for (k = 1; k < no_attributes - 1; k++) {
					if (labelled_test.GetEntry(i, k, false) != labelled_test.GetEntry(j, k, false)) {
						found = true;
						break;
					}
				}
				if (!found) {
					generalized_decision_[i].push_back(labelled_test.GetEntry(j, decision_attribute, masked));
					labelled_test.RemoveObject(j,masked);
				}
			}
		}
	}
	
	table.AppendChild(dynamic_cast(DecisionTable *, &labelled));
	table.AppendChild(dynamic_cast(DecisionTable *, &labelled_test));
	table.AppendChild(dynamic_cast(DecisionTable *, &unlabelled_test));
	
	return true;
}

//-------------------------------------------------------------------
// Method........: ReClassify
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================
bool
FunctionalClassifier::ReClassify(const DecisionTable &labelled, const DecisionTable &labelled_test, const DecisionTable &unlabelled_test) const {
	
	bool masked = true;
	int i;
	
	Message message;
	Handle<Algorithm> algorithm;
	
	// Run TemplateScaler - labelled
	algorithm = ObjectManager::GetIdentifiedAlgorithm(TEMPLATESCALER);
	algorithm->SetParameters("TEMPLATELENGTH =" + String::Format(template_length_) + "; CONSTANTTEMPLATELENGTH = " + String::Format(constant_template_length_) + "; CREASETHR =" + String::Format(crease_thr_) + "; CONSTANTTHR = " + String::Format(constant_thr_) + "; STARTENDTHR = " + String::Format(start_end_thr_) + "; TOLERANCETHR = " + String::Format(tolerance_thr_) + ";");
	Handle<Structure> templatized_labelled = const_cast(DecisionTable *, &labelled)->Apply(*algorithm);
	if (templatized_labelled == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	const_cast(DecisionTable *, &labelled)->AppendChild(templatized_labelled.GetPointer());
	
	// Remove empty objects in traiing set
	int empty = 0;
	for (i = dynamic_cast(DecisionTable *, templatized_labelled.GetPointer())->GetNoObjects(false)-1; i >= 0 ; i--) {
		bool found = false;
		for (int j = 1; j < dynamic_cast(DecisionTable *, templatized_labelled.GetPointer())->GetNoAttributes(false)-1; j++) {
			if (dynamic_cast(DecisionTable *, templatized_labelled.GetPointer())->GetEntry(i, j,true, false) != "Undefined") {
				found = true;
				break;
			}
		}
		if (!found) {
			dynamic_cast(DecisionTable *, templatized_labelled.GetPointer())->RemoveObject(i,false);
			empty++;
		}
	}
	message.Notify("Empty: " + String::Format(empty));
	
	// Print IDG
	if (idg_) {
		ofstream idg;
		if (!IOKit::Open(idg,idg_file_)) {
			Message::Error("Unable to open output file.");
			return false;
		}
		for (int j = 0; j < dynamic_cast(DecisionTable *, templatized_labelled.GetPointer())->GetNoAttributes(true)-1; j++) {
			
			String name = dynamic_cast(DecisionTable *, templatized_labelled.GetPointer())->GetAttributeName(j, true);
			
			idg<< "begin " + name + "\n";
			idg<< "\tnodes Undefined *\n";
			idg<< "\tUndefined -> *\n";
			idg<< "\tmake-reflexive\n";
			idg<< "end " + name + "\n";
		}
		idg.close();
	}
	
	// Run TemplateScaler - labelled test thr
	algorithm = ObjectManager::GetIdentifiedAlgorithm(TEMPLATESCALER);
	algorithm->SetParameters("TEMPLATELENGTH =" + String::Format(template_length_) + "; CONSTANTTEMPLATELENGTH = " + String::Format(constant_template_length_) + "; CREASETHR =" + String::Format(crease_thr_) + "; CONSTANTTHR = " + String::Format(constant_thr_) + "; STARTENDTHR = " + String::Format(start_end_thr_) + "; TOLERANCETHR = " + String::Format(tolerance_thr_) + "; SUPERVISED = F;");
	Handle<Structure> templatized_labelled_test_thr = const_cast(DecisionTable *, &labelled)->Apply(*algorithm);
	if (templatized_labelled_test_thr == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	const_cast(DecisionTable *, &labelled)->AppendChild(templatized_labelled_test_thr.GetPointer());
	
	// Run TemplateScaler - labelled test
	algorithm = ObjectManager::GetIdentifiedAlgorithm(TEMPLATESCALER);			
	algorithm->SetParameters("TEMPLATELENGTH =" + String::Format(template_length_) + "; CONSTANTTEMPLATELENGTH = " + String::Format(constant_template_length_) + "; CREASETHR =" + String::Format(crease_thr_) + "; CONSTANTTHR = " + String::Format(constant_thr_) + "; STARTENDTHR = " + String::Format(start_end_thr_) + "; TOLERANCETHR = " + String::Format(tolerance_thr_) + "; SUPERVISED = F;");
	Handle<Structure> templatized_labelled_test = const_cast(DecisionTable *, &labelled_test)->Apply(*algorithm);
	if (templatized_labelled_test == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	const_cast(DecisionTable *, &labelled)->AppendChild(templatized_labelled_test.GetPointer());
	
	// Run TemplateScaler - unlabelled
	algorithm = ObjectManager::GetIdentifiedAlgorithm(TEMPLATESCALER);			
	algorithm->SetParameters("TEMPLATELENGTH =" + String::Format(template_length_) + "; CREASETHR =" + String::Format(crease_thr_) + "; CONSTANTTHR = " + String::Format(constant_thr_) + "; STARTENDTHR = " + String::Format(start_end_thr_) + "; TOLERANCETHR = " + String::Format(tolerance_thr_) + "; SUPERVISED = F;");
	Handle<Structure> templatized_unlabelled_test = const_cast(DecisionTable *, &unlabelled_test)->Apply(*algorithm);
	if (templatized_unlabelled_test == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	const_cast(DecisionTable *, &unlabelled_test)->AppendChild(templatized_unlabelled_test.GetPointer());
	
	//- Find reducts
	if (reducer_.IsNULL()) {
		Message::Error("Reducer not initialized");
		return false;
	}
	
	Handle<Structure> reducts = templatized_labelled->Apply(*reducer_);
	if (reducts == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(reducer_->GetId()) + ".");
		return false;
	}
	
	templatized_labelled->AppendChild(reducts.GetPointer());
	
	// Find Rules
	algorithm = ObjectManager::GetIdentifiedAlgorithm(RULEGENERATOR);
	algorithm->SetParameters("");
	
	Handle<Rules> rules = dynamic_cast(Rules *,const_cast(Structure *, reducts->Apply(*algorithm)));
	if (rules == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	
	reducts->AppendChild(rules.GetPointer());
	
	if (prune_) {
		rules = const_cast(FunctionalClassifier *, this)->PruneRules(*dynamic_cast(DecisionTable *, templatized_labelled.GetPointer()), *rules, *reducts);
		if (rules == NULL)
			return false;
	}
	
	reducts->AppendChild(rules.GetPointer());
	
	// Sort Rules
	rules->Sort(5, 0, dynamic_cast(DecisionTable *, templatized_labelled.GetPointer()));
	
	//Export Rules
	algorithm = ObjectManager::GetIdentifiedAlgorithm(MYRULEEXPORTER);
	algorithm->SetParameters("FILENAME = " + rulelog_file_);
	
	rules->Apply(*algorithm);
	if (rules == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(algorithm->GetId()) + ".");
		return false;
	}
	
	ifstream stream;
	IOKit::Open(stream, rulelog_file_);
	
	Handle<Structure> textfile = dynamic_cast(Structure *, new TextFile);
	dynamic_cast(TextFile *, const_cast(Structure *, textfile.GetPointer()))->Load(stream);
	dynamic_cast(TextFile *, const_cast(Structure *, textfile.GetPointer()))->SetFilename(rulelog_file_);
	textfile->SetName(rulelog_file_);
	const_cast(DecisionTable *, &labelled)->AppendChild(textfile.GetPointer());
	
	//- Reclassify labelled objects to aquire thresholds
	//--------------------------------------------------
	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *,ObjectManager::GetIdentifiedAlgorithm(BATCHCLASSIFIER));
	Handle<BatchClassification> batchclassification;
	
	Handle<DecisionTable> test1 = dynamic_cast(DecisionTable *, const_cast(Structure *, templatized_labelled_test_thr.GetPointer()));
	
	// Initialize parameter
	Vector(int) decision_values;
	Vector(int) decision_cardinalities;
	test1->GetValueSet(decision_values, decision_cardinalities, test1->GetDecisionAttribute(masked), masked);
	
	//Vector(String) decision_names; decision_names.reserve(decision_values.size());
	
	int largest_class = 0;
	int largest_class_size = decision_cardinalities[0];
	for (i = 1; i < decision_cardinalities.size(); i++) {
		if (decision_cardinalities[i] > largest_class_size) {
			largest_class = i;
			largest_class_size = decision_cardinalities[i];
		}
	}
	String largest_class_string = String::Format(decision_values[largest_class]);
	
	int no_objects = test1->GetNoObjects(masked);
	
	Vector(double) thresholds;
	thresholds.erase(thresholds.begin(), thresholds.end());
	thresholds.reserve(decision_values.size());
	
	message.Notify("Re-classify labelled data 1");
	
	//batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + largest_class_string + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; CALIBRATION=F; ROC=T; ROC.CLASS=" + largest_class_string + "; ROC.FILENAME = tmp; IDG = " + idg_ + "; IDG.FILENAME = " + idg_file_);
	batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + largest_class_string + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; CALIBRATION=F; ROC=T; ROC.CLASS=" + largest_class_string + "; ROC.FILENAME = roc.txt;");
	batchclassifier->SetRules(rules.GetPointer());
	batchclassifier->SetWeight(weight_);
	batchclassifier->ROCData();
	
	// Apply algorithm for classification.
	batchclassification = dynamic_cast(BatchClassification *, test1->Apply(*batchclassifier));
	//Failure?
	if (batchclassification == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(batchclassifier->GetId()) + ".");
		return false;
	}
	
	if (batchclassification->GetROCNWThreshold() != Undefined::Float()) {
		
		Vector(Vector(float)) roc_data = batchclassification->GetROCData();
		
		for (int i = 0; i < decision_values.size(); i++) {
			thresholds.push_back(roc_data[i][8]);
		}
	} else {
		message.Notify("WARNING: Undefined threshold!");	
	}
	
	//- Reclassify labelled objects
	//-----------------------------
	batchclassifier = dynamic_cast(BatchClassifier *,ObjectManager::GetIdentifiedAlgorithm(BATCHCLASSIFIER));
	batchclassification;
	
	Handle<DecisionTable> test2 = dynamic_cast(DecisionTable *, const_cast(Structure *, templatized_labelled_test.GetPointer()));
	
	//batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + largest_class_string + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; CALIBRATION=F; ROC=F; IDG = " + idg_ + "; IDG.FILENAME = " + idg_file_);
	batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + largest_class_string + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; LOG.FILENAME=labelled_log.txt; CALIBRATION=F; ROC=F;");
	batchclassifier->SetRules(rules.GetPointer());
	
	message.Notify("Re-classify labelled data 2");
	
	// Apply algorithm for classification.
	batchclassification = dynamic_cast(BatchClassification *, test2->Apply(*batchclassifier));
	//Failure?
	if (batchclassification == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(batchclassifier->GetId()) + ".");
		return false;
	}
		  
	no_objects = test2->GetNoObjects(masked);
	
	Vector(Vector(int)) predictions_labelled;
	predictions_labelled.erase(predictions_labelled.begin(), predictions_labelled.end());
	predictions_labelled.reserve(no_objects);
	for (i = 0; i < no_objects; i++) {
		predictions_labelled.push_back(Vector(int)());
	}
	
	Vector(ICPair) classification_details = batchclassification->GetClassificationDetails();
	
	int test = classification_details.size();
	
	for (i = 0; i < decision_values.size(); i++) {
		for (int j = 0; j < classification_details.size(); j++) {
			for (int k = 0; k < classification_details[j].second->GetNoDecisionValues(); k++) {
				if (classification_details[j].second->GetDecisionValue(k) == decision_values[i]) {
					if (classification_details[j].second->GetCertaintyCoefficient(k) >= thresholds[i]) {
						predictions_labelled[j].push_back(classification_details[j].second->GetDecisionValue(k));
					}
				}
			}
		}
	}
	
	//- Classify unlabelled data
	//--------------------------
	
	batchclassifier = dynamic_cast(BatchClassifier *,ObjectManager::GetIdentifiedAlgorithm(BATCHCLASSIFIER));
	batchclassification;
	
	Handle<DecisionTable> test3 = dynamic_cast(DecisionTable *, const_cast(Structure *, templatized_unlabelled_test.GetPointer()));
	
	//batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + largest_class_string + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; CALIBRATION=F; ROC=F; IDG = " + idg_ + "; IDG.FILENAME = " + idg_file_);
	batchclassifier->SetParameters("CLASSIFIER=StandardVoter; SPECIFIC=F; VOTING=Support;NORMALIZATION=Firing; FALLBACK=T; FALLBACK.CLASS=" + largest_class_string + "; FALLBACK.CERTAINTY=1.0; MULTIPLE=Best; LOG=F; LOG.FILENAME=unlabelled_log.txt; CALIBRATION=F; ROC=F;");
	batchclassifier->SetRules(rules.GetPointer());
	
	message.Notify("Classify unlabelled data");
	
	// Apply algorithm for classification.
	batchclassification = dynamic_cast(BatchClassification *, test3->Apply(*batchclassifier));
	//Failure?
	if (batchclassification == NULL) {
		message.Error("Failed to apply " + IdHolder::GetClassname(batchclassifier->GetId()) + ".");
		return false;
	}
		  
	no_objects = test3->GetNoObjects(masked);
	
	Vector(Vector(int)) predictions_unlabelled;
	predictions_unlabelled.erase(predictions_unlabelled.begin(), predictions_unlabelled.end());
	predictions_unlabelled.reserve(no_objects);
	for (i = 0; i < no_objects; i++) {
		predictions_unlabelled.push_back(Vector(int)());
	}
	
	classification_details = batchclassification->GetClassificationDetails();
	
	for (i = 0; i < decision_values.size(); i++) {
		for (int j = 0; j < classification_details.size(); j++) {
			for (int k = 0; k < classification_details[j].second->GetNoDecisionValues(); k++) {
				if (classification_details[j].second->GetDecisionValue(k) == decision_values[i]) {
					if (classification_details[j].second->GetCertaintyCoefficient(k) >= thresholds[i]*0.9) { //TODO: USE THR FROM CROSS VALIDATION!
						predictions_unlabelled[j].push_back(classification_details[j].second->GetDecisionValue(k));
					}
				}
			}
		}
	}
	
	
	//- Write predictions of labelled data to file
	float no_correct_predictions = 0;
	float no_predictions = 0;
	float no_annotations = labelled.GetNoObjects(masked);
	
	ofstream log;
	if (!IOKit::Open(log,log_file_)) {
		Message::Error("Unable to open output file.");
		return false;
	}
	
	log<<"NOTE: Annotations and predictions are separated with three blanks if annotations take up more than 75 characters\n";
	log<<"      Annotations/predictions are separated from other annotations/predictions by semi-colon\n\n";
	
	log<<"PREDICTIONS OF LABELLED DATA\n\n";
	log<<"GENE           ANNOTATION                                                                 PREDICTIONS\n\n";
	
	for (i = 0; i < predictions_labelled.size(); i++) {
		String object = labelled_test.GetEntry(i, 0, true, false);
		log<<object;
		
		int j,k;
		for (j = 0; j < 15 - object.GetLength(); j++)
			log<< " ";
		
		int l = 0;
		for (j = 0; j < generalized_decision_[i].size(); j++) {
			String annot = labelled_test.GetDictionaryEntry(labelled_test.GetDecisionAttribute(masked), generalized_decision_[i][j], masked);
			log<<annot + ";";
			l = l + annot.GetLength() + 1;
		}
		
		for (j = 0; j < 75 - l; j++)
			log<< " ";
		if (75 - l < 0)
			log<< "   ";
		
		no_predictions = no_predictions + predictions_labelled[i].size();
		
		for (j = 0; j < predictions_labelled[i].size(); j++) {
			String pred = labelled_test.GetDictionaryEntry(labelled_test.GetDecisionAttribute(masked), predictions_labelled[i][j], masked);
			log<<pred + ";";
			for (k = 0; k < generalized_decision_[i].size(); k++) {
				if (generalized_decision_[i][k] == predictions_labelled[i][j]) {
					no_correct_predictions++;
					break;
				}
			}
		}
		
		log<<"\n";
		
	}
	
	float accuracy = no_correct_predictions/no_predictions;
	String accuracy_string = String::Format(accuracy);
	
	log<<"No. of annotations: " + String::Format(no_annotations) + "\n";
	log<<"No. of predictions: " + String::Format(no_predictions) + "\n";
	log<<"No. of correct predictions: " + String::Format(no_correct_predictions) + "\n";
	log<<"Accuracy: " + accuracy_string + "\n";
	
	message.Notify("No. of annotations: " + String::Format(no_annotations));
	message.Notify("No. of predictions: " + String::Format(no_predictions));
	message.Notify("No. of correct predictions: " + String::Format(no_correct_predictions));
	message.Notify("Accuracy: " + accuracy_string);
	
	//- Write predictions of unlabelled data to file
	
	log<<"\n\nPREDICTIONS OF UNLABELLED DATA\n\n";
	log<<"GENE           ANNOTATION        PREDICTIONS\n\n";
	
	for (i = 0; i < predictions_unlabelled.size(); i++) {
		String object = test3->GetEntry(i, 0, true, false);
		log<<object;
		
		int j,k;
		for (j = 0; j < 15 - object.GetLength(); j++)
			log<< " ";
		
		log<<decision_ + "   ";
		
		for (j = 0; j < 15 - decision_.GetLength(); j++)
			log<< " ";
		
		for (j = 0; j < predictions_unlabelled[i].size(); j++) {
			String pred = labelled_test.GetDictionaryEntry(labelled_test.GetDecisionAttribute(masked), predictions_unlabelled[i][j], masked);
			log<<pred + ";";
		}
		
		log<<"\n";
		
	}
	
	log.close();
	
	IOKit::Open(stream, log_file_);
	
	textfile = dynamic_cast(Structure *, new TextFile);
	dynamic_cast(TextFile *, const_cast(Structure *, textfile.GetPointer()))->Load(stream);
	dynamic_cast(TextFile *, const_cast(Structure *, textfile.GetPointer()))->SetFilename(log_file_);
	textfile->SetName(log_file_);
	const_cast(DecisionTable *, &labelled)->AppendChild(textfile.GetPointer());
	
	return true;
	
}


// ======================================================================

//-------------------------------------------------------------------
// Method........: PruneRules
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

Handle<Rules>
FunctionalClassifier::PruneRules(DecisionTable &table, const Rules &rules, Structure &reducts) const {
	
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
	if (idg_) {
		if (!discerner.LoadIDGs(idg_file_, *const_cast(DecisionTable *, &table), masked)) {
			Message::Error("Failed to load IDGs.");
			return NULL;
		}
	}
	
	Vector(Vector(int)) tracked;
	tracked.erase(tracked.begin(), tracked.end());
	tracked.reserve(no_objects);
	
	int i, j, k;
	
	// Initialize mapping.
	for (i = 0; i < no_objects; i++)
		tracked.push_back(Vector(int)());
	
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
	//scores.erase(scores.begin(), scores.end());
	scores.reserve(no_objects);
	
	// For each object ... 
	for (i = 0; i < no_objects; i++) {
		
		scores.push_back(Vector(FIPair)());
		
		int decision = table.GetEntry(i, table.GetDecisionAttribute(masked), masked);
		
		// ... and each rule for this object
		for (j = 0; j < tracked[i].size(); j++) {
			
			Handle<Rule> rule = rules.GetRule(tracked[i][j]);
			
			int no_decisions = rule->GetNoDecisionValues();
			//if (no_decisions > prune_accuracy_thr_.GetFloat()*decision_values.size())
			//	continue;
			
			bool found = 0;
			for (k = 0; k < no_decisions; k++) {
				
				int sub_decision = rule->GetDecisionValue(k);
				
				if (decision != sub_decision)
					continue;
				
				// Skip the rules with measures below the specified thresholds
				
				float coverage = rule->GetCoverage(k, decision_values, decision_cardinalities);
				if (coverage < prune_coverage_thr_)
					continue;
				float accuracy = rule->GetAccuracy(k);
				if (accuracy < prune_accuracy_thr_)
					continue;
				
				float w1 = 0.5f + 0.25f * accuracy;
				float w2 = 0.5f - 0.25f * accuracy;
				
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
	Vector(Vector(int)) gd; gd.erase(gd.begin(), gd.end());
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
	Vector(bool) correct;
	correct.erase(correct.begin(), correct.end());
	correct.reserve(no_objects);
	for (i = 0; i < no_objects; i++)
		correct.push_back(false);
	
	// Keep track of no. of rules added so far for each object
	Vector(int) norules; norules.erase(norules.begin(), norules.end());
	norules.reserve(no_objects);
	for (i = 0; i < no_objects; i++)
		norules.push_back(0);
	
	int itr = 0;
	float max_auc = 0;
	while (true) {
		
		if (!message.Progress("Pruning rule " + String::Format(itr + 1) +"..."))
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
		if (batchclassification == NULL) {
			message.Error("Failed to apply " + IdHolder::GetClassname(batchclassifier->GetId()) + ".");
			return NULL;
		}
		
		Vector(ICPair) classification_details;
		classification_details.erase(classification_details.begin(), classification_details.end());
		classification_details.reserve(no_objects);
		classification_details = batchclassification->GetClassificationDetails();
		
		// Updata correct
		
		float auc = 0;
		float sens = 0;
		float spec = 0;
		
		Vector(float) thr; thr.erase(thr.begin(), thr.end());
		//thr.reserve(decision_values.size());
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
			
			roc.SetWeight(weight_);
			
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

 
FunctionalClassifier *
FunctionalClassifier::Clone() {
	return new FunctionalClassifier;
} 
