//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960619
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/batchclassifier.h>
#include <kernel/algorithms/classifier.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/classification.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/roccurve.h>
#include <kernel/structures/calibrationcurve.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/iokit.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>

//-N 11/06/13 debugging purposes
#include "omp.h"

//-N 14/06/13
//#include <kernel/algorithms/standardvoter.h>

//-------------------------------------------------------------------
// Static variables (file scope).
//===================================================================

static Handle<Classification> static_fallback_classification_ = NULL;


//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticInitializeFallbackClassification etc.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticInitializeFallbackClassification(const BatchClassifier &batchclassifier, const DecisionTable &table, bool masked) {

	// Instantiate if needed.
	if (static_fallback_classification_ == NULL)
		static_fallback_classification_ = Creator::Classification();

	// Clear everything.
	static_fallback_classification_->Clear();

	// Get decision attribute.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	// Set classification decision attribute.
	static_fallback_classification_->SetDecisionAttribute(decision_attribute);

	// Determine fallback class.
	int fallback = table.GetDictionaryEntry(decision_attribute, batchclassifier.GetFallbackClass(), masked);

	if (fallback == Undefined::Integer() && batchclassifier.GetFallbackClass().IsInteger())
		fallback = batchclassifier.GetFallbackClass().GetInteger();

	// Determine fallback certainty.
	float certainty = batchclassifier.GetFallbackCertainty();

	// Fill fallback classification structure.
	static_fallback_classification_->InsertDecisionValue(fallback, certainty, 0);

	return true;

}

static Handle<Classification>
StaticGetFallbackClassification() {
	return static_fallback_classification_;
}

static void
StaticSetFallbackClassification(Handle<Classification> classification) {
	static_fallback_classification_ = classification;
}

//static
int
StaticGetDecisionClass(const String &name, const DecisionTable &table, bool masked) {

	if (name == Undefined::String())
		return Undefined::Integer();

	// Get decision attribute index.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return Undefined::Integer();

	// Determine decision class.
	int decision_class = table.GetDictionaryEntry(decision_attribute, name, masked);

	if (decision_class == Undefined::Integer() && name.IsInteger())
		decision_class = name.GetInteger();

	return decision_class;

}

//-------------------------------------------------------------------
// Methods for class BatchClassifier.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

BatchClassifier::BatchClassifier() {

	AssignFallback(false);
	SetFallbackClass(Undefined::String());
	SetFallbackCertainty(1.0);

	SetMultipleStrategy(MULTIPLE_BEST);
	SetPriorityClass(Undefined::String());
	SetPriorityThreshold(0.0);

	LogDetails(false);
	SetLogFilename(Undefined::String());
	VerboseLog(true);

	GenerateROC(false);
	SetROCClass(Undefined::String());
	SetROCFilename(Undefined::String());

	// Torgeir 06.03.01 ----------------
	//ReturnROCDetails(false);
	ReturnROCDetails(true);
	roc_data_ = false;
	// Torgeir 06.03.01 ----------------

	GenerateCalibration(false);
	SetCalibrationClass(Undefined::String());
	SetCalibrationFilename(Undefined::String());
	SetNoCalibrationGroups(10);

}

BatchClassifier::~BatchClassifier() {
}

//-------------------------------------------------------------------
// Copy Constructor. -N 20/05/13
//===================================================================

BatchClassifier::BatchClassifier(const BatchClassifier& other) {

	classifier_ = other.classifier_; 

	AssignFallback(other.AssignFallback());
	SetFallbackClass(other.GetFallbackClass());
	SetFallbackCertainty(other.GetFallbackCertainty());

	SetMultipleStrategy(other.GetMultipleStrategy());
	SetPriorityClass(other.GetPriorityClass());
	SetPriorityThreshold(other.GetPriorityThreshold());

	LogDetails(other.LogDetails());
	VerboseLog(other.VerboseLog());

	GenerateROC(other.GenerateROC());
	SetROCClass(other.GetROCClass());

	ReturnROCDetails(other.ReturnROCDetails());
	roc_data_ = other.roc_data_;

	GenerateCalibration(other.GenerateCalibration());
	SetCalibrationClass(other.GetCalibrationClass());
	SetNoCalibrationGroups(other.GetNoCalibrationGroups());

	//Should copies inherit the filenames? -N
//	SetCalibrationFilename(other.GetCalibrationFilename());
	SetCalibrationFilename(Undefined::String());
//	SetLogFilename(other.GetLogFilename);
	SetLogFilename(Undefined::String());
//	SetROCFilename(other.GetROCFilename());
	SetROCFilename(Undefined::String());
}




//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BatchClassifier, BATCHCLASSIFIER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 971124: Resolved overlapping keywords using
//                            dot-notation.
//                 Aš 980107: Modified fallback stuff.
//                 Aš 980128: Added calibration stuff.
//===================================================================

String
BatchClassifier::GetParameters() const {

	String parameters;

	// Classifier.
	parameters += Keyword::Classifier();
	parameters += Keyword::Assignment();

	if (GetClassifier() == NULL) {
		parameters += "NULL";
	}
	else {
		parameters += IdHolder::GetClassname(GetClassifier()->GetId());
		parameters += Keyword::Separator();
		parameters += GetClassifier()->GetParameters();
	}

	parameters += Keyword::Separator();

	// Fallback.
	parameters += Keyword::Fallback();
	parameters += Keyword::Assignment();
	parameters += String::Format(AssignFallback());

	if (AssignFallback()) {

		parameters += Keyword::Separator();

		// Fallback class.
		parameters += Keyword::Fallback() + Keyword::Dot() + Keyword::Class();
		parameters += Keyword::Assignment();
		parameters += GetFallbackClass();

		parameters += Keyword::Separator();

		// Fallback certainty.
		parameters += Keyword::Fallback() + Keyword::Dot() + Keyword::Certainty();
		parameters += Keyword::Assignment();

		if (GetFallbackCertainty() == Undefined::Float())
			parameters += Undefined::String();
		else
			parameters += String::Format(GetFallbackCertainty());

	}

	parameters += Keyword::Separator();

	// Multiple.
	parameters += Keyword::Multiple();
	parameters += Keyword::Assignment();
	parameters += GetString(GetMultipleStrategy());

	if (GetMultipleStrategy() == MULTIPLE_PRIORITIZE) {

		parameters += Keyword::Separator();

		// Priority class.
		parameters += Keyword::Multiple() + Keyword::Dot() + Keyword::Class();
		parameters += Keyword::Assignment();
		parameters += GetPriorityClass();

		parameters += Keyword::Separator();

		// Priority threshold.
		parameters += Keyword::Multiple() + Keyword::Dot() + Keyword::Threshold();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetPriorityThreshold());

	}

	parameters += Keyword::Separator();

	// Log.
	parameters += Keyword::Log();
	parameters += Keyword::Assignment();
	parameters += String::Format(LogDetails());

	if (LogDetails()) {

		parameters += Keyword::Separator();

		// Filename, log.
		parameters += Keyword::Log() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetLogFilename();

		parameters += Keyword::Separator();

		// Verbose flag.
		parameters += Keyword::Log() + Keyword::Dot() + Keyword::Verbose();
		parameters += Keyword::Assignment();
		parameters += String::Format(VerboseLog());

	}

	parameters += Keyword::Separator();

	// Calibration.
	parameters += Keyword::Calibration();
	parameters += Keyword::Assignment();
	parameters += String::Format(GenerateCalibration());

	if (GenerateCalibration()) {

		parameters += Keyword::Separator();

		// Calibration class.
		parameters += Keyword::Calibration() + Keyword::Dot() + Keyword::Class();
		parameters += Keyword::Assignment();
		parameters += GetCalibrationClass();

		parameters += Keyword::Separator();

		// Filename, calibration.
		parameters += Keyword::Calibration() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetCalibrationFilename();

		parameters += Keyword::Separator();

		// Number of groups, calibration.
		parameters += Keyword::Calibration() + Keyword::Dot() + Keyword::Groups();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetNoCalibrationGroups());

	}

	parameters += Keyword::Separator();

	// ROC.
	parameters += Keyword::ROC();
	parameters += Keyword::Assignment();
	parameters += String::Format(GenerateROC());

	if (GenerateROC()) {

		parameters += Keyword::Separator();

		// ROC class.
		parameters += Keyword::ROC() + Keyword::Dot() + Keyword::Class();
		parameters += Keyword::Assignment();
		parameters += GetROCClass();

		parameters += Keyword::Separator();

		// Filename, ROC.
		parameters += Keyword::ROC() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetROCFilename();

	}

	return parameters;

}

//-------------------------------------------------------------------
// Method........: GetOutputFilenames
// Author........: Aleksander šhrn
// Date..........:
// Description...: See Algorithm::GetOutputFilenames method.
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassifier::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	//stl containers are not thread safe, so critical section might be needed here. -N 02/05/13
	
		if (LogDetails())
			filenames.push_back(GetLogFilename());
	
		if (GenerateCalibration())
			filenames.push_back(GetCalibrationFilename());
	
		if (GenerateROC())
			filenames.push_back(GetROCFilename());
	
	return true;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overlapping keywords, resolve later. All ROC stuff
//                 currently not handled.
// Revisions.....: Aš 980107: Modified fallback stuff.
//                 Aš 980128: Added calibration stuff.
//===================================================================

bool
BatchClassifier::SetParameter(const String &keyword, const String &value) {

	// Fallback.
	if (keyword == Keyword::Fallback() && value.IsBoolean())
		return AssignFallback(value.GetBoolean());

	// Fallback class.
	if (keyword == Keyword::Fallback() + Keyword::Dot() + Keyword::Class())
		return SetFallbackClass(value);

	// Fallback certainty.
	if ((keyword == Keyword::Fallback() + Keyword::Dot() + Keyword::Certainty()) && value.IsFloat())
		return SetFallbackCertainty(value.GetFloat());

	// Classifier.
	if (keyword == Keyword::Classifier()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (algorithm == NULL || !algorithm->IsA(CLASSIFIER))
			return false;
		return SetClassifier(dynamic_cast(Classifier *, algorithm.GetPointer()));
	}

	// Log.
	if (keyword == Keyword::Log() && value.IsBoolean())
		return LogDetails(value.GetBoolean());

	// Log filename.
	if (keyword == Keyword::Log() + Keyword::Dot() + Keyword::Filename())
		return SetLogFilename(value);

	// Log filename (backwards compatibility).
	if (keyword == Keyword::Filename())
		return SetLogFilename(value);

	// Verbose log (backwards compatibility).
	if ((keyword == Keyword::Log() + Keyword::Dot() + Keyword::Verbose()) && value.IsBoolean())
		return VerboseLog(value.GetBoolean());

	// Multiple.
	if (keyword == Keyword::Multiple()) {
		if (value == GetString(MULTIPLE_BEST))
			return SetMultipleStrategy(MULTIPLE_BEST);
		if (value == GetString(MULTIPLE_PRIORITIZE))
			return SetMultipleStrategy(MULTIPLE_PRIORITIZE);
		if (value == GetString(MULTIPLE_REFRAIN))
			return SetMultipleStrategy(MULTIPLE_REFRAIN);
		return false;
	}

	// Multiple class, prioritization.
	if (keyword == Keyword::Multiple() + Keyword::Dot() + Keyword::Class())
		return SetPriorityClass(value);

	// Multiple class, prioritization (backwards compatibility).
	if (keyword == Keyword::Class() && value.IsInteger())
		return SetPriorityClass(String::Format(value.GetInteger()));

	// Multiple threshold, prioritization.
	if ((keyword == Keyword::Multiple() + Keyword::Dot() + Keyword::Threshold()) && value.IsFloat())
		return SetPriorityThreshold(value.GetFloat());

	// Multiple threshold, prioritization (backwards compatibility).
	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetPriorityThreshold(value.GetFloat());

	// ROC.
	if (keyword == Keyword::ROC() && value.IsBoolean())
		return GenerateROC(value.GetBoolean());

	// ROC class.
	if (keyword == Keyword::ROC() + Keyword::Dot() + Keyword::Class())
		return SetROCClass(value);

	// ROC filename.
	if (keyword == Keyword::ROC() + Keyword::Dot() + Keyword::Filename())
		return SetROCFilename(value);

	// Calibration.
	if (keyword == Keyword::Calibration() && value.IsBoolean())
		return GenerateCalibration(value.GetBoolean());

	// Calibration class.
	if (keyword == Keyword::Calibration() + Keyword::Dot() + Keyword::Class())
		return SetCalibrationClass(value);

	// Calibration filename.
	if (keyword == Keyword::Calibration() + Keyword::Dot() + Keyword::Filename())
		return SetCalibrationFilename(value);

	// Calibration groups.
	if ((keyword == Keyword::Calibration() + Keyword::Dot() + Keyword::Groups()) && value.IsInteger())
		return SetNoCalibrationGroups(value.GetInteger());

	// Could the parameters concern the inherent single-object classifier?
	if (GetClassifier() == NULL)
		return false;
	
	return GetClassifier()->SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassifier::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Classifies all information vectors in the input
//                 decision table with the set classifier.
//
//                 Produces a batch classification structure (with a
//                 confusion matrix) and possibly a log file and a
//                 ROC file.
//
//                 If the classifier fails, a fallback class may be
//                 specified.
//
//                 The classifier may also be overridden by giving
//                 priority to a certain decision class. This means
//                 that is the classifier indicates that a given class
//                 is possible with a measure of certainty above some
//                 threshold, then classify it as such -- even if the
//                 class is not the most "likely" one according to the
//                 classifier. Not all classifiers may support this
//                 feature.
//
// Comments......: Assumes that library clients use handles.
//
//                 This method is currently way too long and complex.
//                 Modularize!
//
// Revisions.....: Aš 970616: Introduced fallback.
//                 Aš 971002: Introduced verbose log, restructured
//                            log code.
//                 Aš 971103: Introduced prioritization.
//                 Aš 971106: Introduced explicit best/prioritize/refrain.
//                 Aš 971123: Introduced ROC.
//                 Aš 980107: Refined fallback option.
//                 Aš 980128: Introduced calibration.
//                 Aš 981218: Introduced ROC target/output details in
//                            BatchClassification structure.
//		   -N 30/05/13: Removed Rule resets. Changed SetRules
//				to get the set from SE objects in order
//				to thread the SerialExecutorLoop
//				execution process.
//		   -N 14/16/13: Swapped fallback and fallback validation.
//		   		Now only executes when needed.
//===================================================================

Structure *
BatchClassifier::Apply(Structure &structure) const {

	Message message;

	

	// Check input structure.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Has the classifier been set?
	if (GetClassifier() == NULL) {
		Message::Error("No classifier algorithm has been specified.");
		return NULL;
	}

	// Reset the classifier.
	if (!GetClassifier()->Reset()) {
		Message::Error("The classifier could not be reset.");
		return NULL;
	}

	bool masked = true;

	// Is the decision table compatible with the classifier?
	//if (!GetClassifier()->IsCompatible(*table, masked)) {
	//	Message::Error("The classifier and the decision table are incompatible.");
	//	return NULL;
	//}

	// Initialize the classifier, if needed.
	if (!GetClassifier()->Initialize(*table, masked)) {
		Message::Error("The classifier could not be initialized.");
		return NULL;
	}



	if (!AssignFallback())
		Message::Warning("No fallback classification selected.", false);
	else
		// Initialize fallback classification structure.
		if (!StaticInitializeFallbackClassification(*this, *table, masked)) {
			Message::Error("Failed to initialize fallback classification.");
			return NULL;
		}

	int no_objects    = table->GetNoObjects(masked);

	// Determine decision attribute.
	int decision_attribute = table->GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer()) {
		Message::Error("Unable to determine index of decision attribute.");
		return NULL;
	}

	ofstream logfile;

	// Prepare log file if needed.
	if (LogDetails()) {

		// Open log file.
		if (!IOKit::Open(logfile, GetLogFilename())) {
			Message::Error("Failed to open file " + GetLogFilename() + ".");
			return NULL;
		}

		// Save header.
		if (!SaveLogHeader(logfile, *table))
			return NULL;

	}

	Vector(int)    predicted;  predicted.reserve(no_objects);
	Vector(int)    actual;     actual.reserve(no_objects);
	Vector(int)    decisions;  decisions.reserve(2 * no_objects);
	Vector(int)    unique;     unique.reserve(10);
	Vector(ICPair) results;    results.reserve(no_objects);

	Handle<InformationVector> inf = Creator::InformationVector();

	// Get priority class.
	int priority_class = StaticGetDecisionClass(GetPriorityClass(), *table, masked);
	int i;

	
	// Classify all objects.
	for (i = 0; i < no_objects; i++) {

		// Notify user of progress.
		if (!message.Progress("Classifying objects...", i, no_objects))
			break;

		// Extract an inf. vector.
		if (!inf->Create(*table, i, masked)) {
			return NULL;
		}

		// Acquire actual (target) value.
		int act = inf->GetEntry(decision_attribute);

		// Determine predicted value.
		Handle<Classification> result = dynamic_cast(Classification *, inf->Apply(*GetClassifier()));
		// Was a classification really made?
		if (!result.IsNULL() && result->GetNoDecisionValues() == 0) {
			result = NULL;
		}

		if (!result.IsNULL() && result->GetNoDecisionValues() == 1 && result->GetDecisionValue(0) == Undefined::Integer()) {
			result = NULL;
		}
		// Resort to fallback?
		if (result.IsNULL() && AssignFallback()) {
			result = StaticGetFallbackClassification();
		}
		// Make sure we're not mixing apples and oranges.
		if (!result.IsNULL() && result->GetDecisionAttribute() != decision_attribute) {
			return NULL;
		}
		// Save for possible calibration/resolution use.
		// if (GenerateCalibration() || GenerateROC())
		// Might need a critical here at some point. -N 02/05/13	
		results.push_back(std::make_pair(act, result));
		// Process classification to arrive at a single (predicted) decision value.
		int prd = ProcessClassification(result, priority_class);

		// Bookkeeping.
		// Might need a critical section here. -N 02/05/13
		actual.push_back(act);
		predicted.push_back(prd);
		decisions.push_back(act);
		decisions.push_back(prd);

		// Append result to file if specified.
		if (LogDetails() && !SaveLogEntry(logfile, i, act, prd, result, table, decision_attribute, masked)) {
			return NULL;
		}
	}
	
	int calibration_class = Undefined::Integer();

	// Determine index of calibration class.
	if (GenerateCalibration())
		calibration_class = StaticGetDecisionClass(GetCalibrationClass(), *table, masked);

	CalibrationCurve calibration;

	// Save calibration plot, if specified.
	if (GenerateCalibration()) {
		if (!calibration.Create(results, calibration_class, GetNoCalibrationGroups())) {
			Message::Warning("Generation of calibration curve failed.");
		}
		else {
			if (!calibration.Save(GetCalibrationFilename(), GetId(), GetParameters()))
				Message::Warning("Saving of calibration curve failed.");
		}
	}

	int roc_class = Undefined::Integer();

	// Determine index of ROC class.
	if (GenerateROC())
		roc_class = StaticGetDecisionClass(GetROCClass(), *table, masked);

	float roc_area          = Undefined::Float();
	float roc_stderr        = Undefined::Float();
	float roc_threshold_nw  = Undefined::Float();
	float roc_threshold_acc = Undefined::Float();
	//Torgeir 27.02.01 ------------------------
	float accuracy_nw		= Undefined::Float();
	float tp = Undefined::Float(), fp = Undefined::Float(), tn = Undefined::Float(), fn = Undefined::Float();
	float sensitivity_nw	= Undefined::Float();
	float specificity_nw	= Undefined::Float();
	Vector(Vector(float)) roc_data;
	//Torgeir 27.02.01 ------------------------

	ROCCurve roc;

	Vector(int)   roc_targets;
	Vector(float) roc_outputs;
	//Torgeir 07.06.01 ------------------------
	roc.SetWeight(weight_);
	//Torgeir 07.06.01 ------------------------
	// Save ROC curve, if specified.
	if (GenerateROC()) {

		int  roc_resolution = 251;
		bool roc_progress   = true;
		if (!roc.Create(results, roc_class, roc_resolution, roc_progress, &roc_targets, &roc_outputs)) {
			Message::Warning("Generation of ROC curve failed.");
		}
		else {
			roc_area          = roc.GetArea();
			roc_stderr        = roc.GetStandardError();
			roc_threshold_nw  = roc.GetNWThreshold();
			//Torgeir 27.02.01 ------------------------
			accuracy_nw = roc.GetNWAccuracy(sensitivity_nw,specificity_nw);
			roc.GetTPFPTNFN(tp, fp, tn, fn);
			//Torgeir 27.02.01 ------------------------
			roc_threshold_acc = roc.GetAccuracyThreshold();

			if (!roc.Save(GetROCFilename(), GetId(), GetParameters()))
				Message::Warning("Saving of ROC curve failed.");
		}
		//Torgeir 29.08.01 ------------------------
		if (roc_data_) {
			Vector(int) roc_classes;
			Vector(int) dummy;
			table->GetValueSet(roc_classes, dummy, table->GetDecisionAttribute(true), true);

			if (roc_classes.size() > 0 && true) {
				roc_data.erase(roc_data.begin(), roc_data.end());
				for (i = 0; i < roc_classes.size(); i++) {
					roc_data.push_back(Vector(float)());
				}
				for (i = 0; i < roc_classes.size(); i++) {
					for (int j = 0; j < 9; j++)
						roc_data[i].push_back(0);
				}

				for (i = 0; i < roc_classes.size(); i++) {

					ROCCurve roc2;
					Vector(int)   roc_targets2;
					Vector(float) roc_outputs2;
					roc2.SetWeight(weight_);

					if (!roc2.Create(results, roc_classes[i], roc_resolution, roc_progress, &roc_targets2, &roc_outputs2)) {
						Message::Warning("Generation of ROC curve failed.");
					} else {
						roc_data[i][0]		 = roc2.GetArea();
						roc_data[i][1]		 = roc2.GetStandardError();
						roc_data[i][8]		 = roc2.GetNWThreshold();
						//roc_data[i][8]		 = roc2.GetAccuracyThreshold();
						float sensitivity_nw2 = Undefined::Float(), specificity_nw2 = Undefined::Float();
						accuracy_nw = roc2.GetNWAccuracy(sensitivity_nw2,specificity_nw2);
						roc_data[i][2]		 = sensitivity_nw2;
						roc_data[i][3]		 = specificity_nw2;
						float tp2 = Undefined::Float(), fp2 = Undefined::Float(), tn2 = Undefined::Float(), fn2 = Undefined::Float();
						roc2.GetTPFPTNFN(tp2, fp2, tn2, fn2);
						roc_data[i][4]		 = tp2;
						roc_data[i][5]		 = fp2;
						roc_data[i][6]		 = tn2;
						roc_data[i][7]		 = fn2;
					}
				}
			}
		}
		//Torgeir 29.08.01 ------------------------		
	}

	// Determine unique decisions.
	for (i = 0; i < decisions.size(); i++) {
		if (std::find(unique.begin(), unique.end(), decisions[i]) == unique.end())
			unique.push_back(decisions[i]);
	}

	// Sort the unique decisions.
	std::sort(unique.begin(), unique.end());

	// Create a batch classification structure.
	Handle<BatchClassification> batchclassification = Creator::BatchClassification();

	// Set the decision attribute.
	batchclassification->SetDecisionAttribute(decision_attribute);

	// Set the confusion matrix' dimension.
	batchclassification->GetConfusionMatrix().SetDimension(unique.size());

	// Set the confusion matrix' map.
	for (i = 0; i < unique.size(); i++)
		batchclassification->GetConfusionMatrix().SetMap(i, unique[i]);

	// Fill the confusion matrix.
	for (i = 0; i < predicted.size(); i++) {
		int actual_class    = actual[i];
		int predicted_class = predicted[i];
		int actual_index    = batchclassification->GetConfusionMatrix().GetIndex(actual_class);
		int predicted_index = (predicted_class == actual_class) ? actual_index : batchclassification->GetConfusionMatrix().GetIndex(predicted_class);
		batchclassification->GetConfusionMatrix().IncrementEntry(actual_index, predicted_index);
	}

	if (GenerateROC()) {
		batchclassification->SetROCClass(roc_class);
		batchclassification->SetROCArea(roc_area);
		batchclassification->SetROCStandardError(roc_stderr);
		batchclassification->SetROCNWThreshold(roc_threshold_nw);
		// Torgeir 27.02.01 ----------------------------
		batchclassification->SetROCNWAccuracy(accuracy_nw,sensitivity_nw,specificity_nw);
		batchclassification->SetROCNWTPFPTNFN(tp,fp,tn,fn);
		// Torgeir 27.02.01 ----------------------------
		batchclassification->SetROCAccuracyThreshold(roc_threshold_acc);
		if (ReturnROCDetails()) {
			batchclassification->SetROCTargetVector(roc_targets);
			batchclassification->SetROCOutputVector(roc_outputs);
			// Torgeir 06.03.01 ----------------
			batchclassification->SetROCData(roc_data);
			// Torgeir 06.03.01 ----------------
		}
	}

	// Torgeir 13.11.01 ----------------
	batchclassification->SetClassificationDetails(results);
	// Torgeir 13.11.01 ----------------

	// Append matrix to file if specified.
	if (LogDetails() && !SaveLogFooter(logfile, batchclassification->GetConfusionMatrix()))
		return NULL;

	// Clean up.
	StaticSetFallbackClassification(Handle<Classification>(NULL));

	// Hack: Reset rules to avoid possible multiple deletes in RSES.
	// This caused a *massive* amount of problems, since classifiers are referenced objects. 
	// They're called hacks for a reason, and should not be used
	// in generalized/highlevel functions. -N
	//GetClassifier()->SetRules(NULL);

	return batchclassification.Release();

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetRules
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassifier::SetRules(const Rules *rules) {

	if (GetClassifier() == NULL) {
		return false;
	}

	return GetClassifier()->SetRules(rules);

}

//-------------------------------------------------------------------
// Method........: SaveLogHeader
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassifier::SaveLogHeader(ofstream &stream, const DecisionTable &table) const {

	stream << "% Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " + IdHolder::GetClassname(GetId()) << endl;
	stream << "% {" + GetParameters() + "}" << endl;
	stream << "%" << endl;
	stream << "% " << table.GetName() << endl;
	stream << "%" << endl;
	stream << "% Note that the object indices below are 0-based." << endl;
	stream << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: SaveLogEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassifier::SaveLogEntry(ofstream &stream, int object_no, int actual, int predicted, const Handle<Classification> result, const Handle<DecisionTable> table, int attribute_no, bool masked) const {

	String astring = table->GetDictionaryEntry(attribute_no, actual, masked);
	String pstring = table->GetDictionaryEntry(attribute_no, predicted, masked);

	String header1("Object " + String::Format(object_no) + ": ");
	String indent1(' ', header1.GetLength() + 6);

	// Save classification details.
	stream << header1;

	if (actual == predicted)
		stream << "ok    ";
	else
		stream << "ERROR ";

	if (actual != Undefined::Integer())
		stream << "Actual    = " << astring << " (" << actual << ")" << endl;
	else
		stream << "Actual    = " << astring << endl;

	if (predicted != Undefined::Integer())
		stream << indent1 << "Predicted = " << pstring << " (" << predicted << ")" << endl;
	else
		stream << indent1 << "Predicted = " << pstring << endl;

	// Save verbose stuff.
	if (VerboseLog()) {

		String header2("Ranking   = ");
		String indent2(' ', indent1.GetLength() + header2.GetLength());

		stream << indent1 + header2;

		// No result structure returned?
		if (result == NULL) {
			stream << "None" << endl;
			return true;
		}

		int no_values = result->GetNoDecisionValues();
		int i;

		// Save ranking of all possible classifications as returned by the classifier.
		for (i = 0; i < no_values; i++) {

			int   dv = result->GetDecisionValue(i);
			float ce = result->GetCertaintyCoefficient(i);

			String dstring = table->GetDictionaryEntry(attribute_no, dv, masked);
			String cstring = (ce == Undefined::Float()) ? Undefined::String() : String::Format(ce);

			stream << ((i == 0) ? "" : indent2) + "(" + cstring + ") " + dstring + " (" + String::Format(dv) + ")";

			// Save info as to how the ranking came about.
			if (result == StaticGetFallbackClassification()) {
				stream << " Fallback";
			}
			else if (result->GetNoRules(i) > 0) {
				stream << " " << result->GetNoRules(i) << " rule(s)";
			}

			stream << endl;

		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SaveLogFooter
// Author........: Aleksander šhrn
// Date..........:
// Description...: Saves formatted confusion matrix to stream.
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassifier::SaveLogFooter(ofstream &stream, const ConfusionMatrix &matrix) const {

	String formatted;

	if (!matrix.Format(formatted))
		return false;

	stream << endl;
	stream << "Confusion matrix:" << endl;
	stream << endl;
	stream << formatted;

	return true;

}

//-------------------------------------------------------------------
// Method........: ProcessClassification
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a classification output from a classifier,
//                 processes it in order to arrive at a predicted
//                 value.
// Comments......: Used to construct the confusion matrix.
// Revisions.....: Aš 980107: Refined fallback option (done externally).
//===================================================================

int
BatchClassifier::ProcessClassification(Handle<Classification> classification, int priority_class) const {

	// Is there a classification to process?
	if (classification == NULL)
		return Undefined::Integer();

	// Sort them according to some measure of "certainty".
	if ((classification->GetNoDecisionValues() > 1) && !classification->Sort()) {
		Message::Error("Failed to sort possible classifications.");
		return Undefined::Integer();
	}

	// Choose the "most certain" or "best" as default.
	int predicted = classification->GetDecisionValue(0);

	// Return "most certain" value?
	if (GetMultipleStrategy() == MULTIPLE_BEST)
		return predicted;

	int j;

	// Prioritize a specified class?
	if (GetMultipleStrategy() == MULTIPLE_PRIORITIZE) {
		for (j = 0; j < classification->GetNoDecisionValues(); j++) {
			if (classification->GetDecisionValue(j) != priority_class)
				continue;
			if (classification->GetCertaintyCoefficient(j) >= GetPriorityThreshold())
				return priority_class;
	  }
	}

	// Refrain from making a classification?
	if (GetMultipleStrategy() == MULTIPLE_REFRAIN)
		return Undefined::Integer();

	return predicted;

}

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
BatchClassifier::GetString(BatchClassifier::MultipleStrategy strategy) {

	switch (strategy) {
		case MULTIPLE_BEST:       return "Best";
		case MULTIPLE_PRIORITIZE: return "Prioritize";
		case MULTIPLE_REFRAIN:    return "Refrain";
		default:                  return Undefined::String();
	}

}
BatchClassifier *
BatchClassifier::Clone() {
	return new BatchClassifier;
}
