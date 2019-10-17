//-------------------------------------------------------------------
// Author........: Thomas ×gotnes/Aleksander šhrn
// Date..........: 981026
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/qualityrulefilterloop.h>
#include <kernel/algorithms/ruleevaluator.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/roccurve.h>
#include <kernel/structures/rulebasedclassification.h>
#include <kernel/structures/informationvector.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/systemkit.h>
#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/set.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>
#include <kernel/system/math.h>

#include <common/objectmanager.h>

//-------------------------------------------------------------------
// Static helpers (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticGetDecisionClass
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static int
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
// Methods for class QualityRuleFilterLoop.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

QualityRuleFilterLoop::QualityRuleFilterLoop() {
	UseLower(false);
	UseUpper(false);
	InvertLogic(false);
	SetFilename(Undefined::String());
	SetROCFocusClass(Undefined::String());
	SetROCFallbackCertainty(0.5);
	SetResolution(RESOLUTION_DYNAMIC);
	SetFixedResolution(10);
	SetDynamicResolution(0.01f);
	SetResolutionLimit(100);
}

QualityRuleFilterLoop::~QualityRuleFilterLoop() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(QualityRuleFilterLoop, QUALITYRULEFILTERLOOP, QualityRuleFilter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......:
// Revisions.....: Aš 981124: Changed.
//===================================================================

String
QualityRuleFilterLoop::GetParameters() const {

	String parameters = QualityRuleFilter::GetParameters();

	// Resolution.
	parameters += Keyword::Resolution();
	parameters += Keyword::Assignment();
	parameters += GetString(GetResolution());

	parameters += Keyword::Separator();

	parameters += Keyword::Resolution() + Keyword::Dot() + Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetResolutionLimit());

	parameters += Keyword::Separator();

	// Resolution parameters.
	if (GetResolution() == RESOLUTION_FIXED) {
		parameters += Keyword::Resolution() + Keyword::Dot() + Keyword::Gap();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetFixedResolution());
		parameters += Keyword::Separator();
	}

	if (GetResolution() == RESOLUTION_DYNAMIC) {
		parameters += Keyword::Resolution() + Keyword::Dot() + Keyword::Fraction();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetDynamicResolution());
		parameters += Keyword::Separator();
	}

	// Filename.
	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();

	parameters += Keyword::Separator();

	// Decision table.
	parameters += Keyword::DecisionTable();
	parameters += Keyword::Assignment();

	if (GetDecisionTable() == NULL)
		parameters += "NULL";
	else
		parameters += GetDecisionTable()->GetName();

	parameters += Keyword::Separator();

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

	// ROC class.
	parameters += Keyword::ROC() + Keyword::Dot() + Keyword::Class();
	parameters += Keyword::Assignment();
	parameters += GetROCFocusClass();

	parameters += Keyword::Separator();

	// ROC fallback certainty.
	parameters += Keyword::Fallback() + Keyword::Dot() + Keyword::Certainty();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetROCFallbackCertainty());

	return parameters;

}

//-------------------------------------------------------------------
// Method........: GetOutputFilenames
// Author........: Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
QualityRuleFilterLoop::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	filenames.push_back(GetFilename());

	return true;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......:
// Revisions.....: Aš 981124: Changed.
//===================================================================

bool
QualityRuleFilterLoop::SetParameter(const String &keyword, const String &value) {

	// Higher up?
	if (QualityRuleFilter::SetParameter(keyword, value))
		return true;

	// ROC class.
	if (keyword == Keyword::ROC() + Keyword::Dot() + Keyword::Class())
		return SetROCFocusClass(value);

	// Fallback certainty.
	if ((keyword == Keyword::Fallback() + Keyword::Dot() + Keyword::Certainty()) && value.IsFloat())
		return SetROCFallbackCertainty(value.GetFloat());

	// Filename.
	if (keyword == Keyword::Filename())
		return SetFilename(value);

	// Decision table.
	if (keyword == Keyword::DecisionTable()) {
		Message::Error("The decision table cannot currently be set this way.");
		return false;
	}

	// Classifier
	if (keyword == Keyword::Classifier()) {
		Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(value));
		if (algorithm == NULL || !algorithm->IsA(VOTER)) {
		  Message::Error("Illegal classifier identifier.");
			return false;
		}
		return SetClassifier(dynamic_cast(Voter *, algorithm.GetPointer()));
	}

	// Pass on to classifier?
	 if (!GetClassifier().IsNULL()) {
		if (GetClassifier()->SetParameter(keyword, value))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn/Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......: We use the second ROCCurve::Create version, due to
//                 unnecessary overhead in the first one.
// Revisions.....: Aš 981124: Simplified.
//                 Aš 990607: Binarized vector of actual values before
//                            passing it to ROCCurve::Create.
//===================================================================

Structure *
QualityRuleFilterLoop::Apply(Structure &structure) const {

	Message message;

	// Check applicability.
	if (!IsApplicable(structure))
		return NULL;

	Handle<Rules> rules = dynamic_cast(Rules *, &structure);

	// This method is conceptually const only.
	Handle<QualityRuleFilterLoop> self = const_cast(QualityRuleFilterLoop *, this);

	if (!self->InitializeClassifier(rules.GetPointer()))
		return NULL;

	if (GetDecisionTable() == NULL) {
		Message::Error("Decision table not set.");
		return NULL;
	}

	bool masked = true;

	// Is the decision table compatible with the classifier?
	if (!GetClassifier()->IsCompatible(*GetDecisionTable(), masked)) {
		Message::Error("The classifier and the decision table are incompatible.");
		return NULL;
	}

	// Get basic table information.
	int decision_attribute = GetDecisionTable()->GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return NULL;

	int no_objects = GetDecisionTable()->GetNoObjects(masked);

	// Determine index of ROC class.
	int rocclass = StaticGetDecisionClass(GetROCFocusClass(), *GetDecisionTable(), masked);

	if (rocclass == Undefined::Integer()) {
		Message::Error("Could not resolve decision class " + GetROCFocusClass() + ".");
		return NULL;
	}

	Vector(Vector(int))	matrix;

	message.Notify("Building firing matrix...");

	// Determine which rules that fire for which objects.
	if (!InitializeFireMatrix(*rules, masked, matrix))
		return NULL;

	// Create the evaluator to use.
	Handle<RuleEvaluator> evaluator = CreateEvaluator(*rules);

	if (evaluator == NULL)
		return NULL;

	Vector(float) qualities;

	message.Notify("Building quality map...");

	// Compute quality measure for each rule.
	if (!(rules->Evaluate(*evaluator, qualities)))
		return NULL;

	QualityMap  map;
	STLSet(int) undefined;

	// Create a map from qualities to rule indices.
	if (!InitializeQualityMap(qualities, map, undefined))
		return NULL;

	if (!undefined.empty())
#ifdef X86_64
		Message::Warning(String::Format((int)undefined.size()) + " rules have undefined qualities.", false);
#else
		Message::Warning(String::Format(undefined.size()) + " rules have undefined qualities.", false);
#endif

	ofstream stream;

	if (!IOKit::Open(stream, GetFilename()))
		return NULL;

	message.Notify("Logging quality measures...");

  // Initialize log file.
	if (!SaveLogHeader(stream, *rules, map, undefined))
		return NULL;

	Handle<InformationVector> object = Creator::InformationVector();
	Handle<Classification>    classification;

	Vector(int)   actual;
	Vector(float) predicted;
	Vector(int)   intersection;
	STLSet(int)   current;

	// Always include rules with undefined quality?
	if (!RemoveUndefinedRules())
		current = undefined;

	// These should be const_iterators, but VC++ 6.0 won't let me...
	QualityMap::iterator  it1 = map.end();
	Vector(int)::iterator it2;

	message.Notify("Filtering and evaluating...");

	// Gradually add rules with increasing quality.
	while (!(it1 == map.begin())) {

		int no_rules = current.size();

		// Add rule indices to current set (include a certain minimum number of rules).
		while ((current.size() - no_rules) < GetRuleStep(current.size())) {
			if (it1 == map.begin())
				break;
			it1--;
			for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++)
				current.insert(*it2);
		}

		// Notify user of progress.
#ifdef X86_64
		if (!message.Progress("Assessing rule set of size " + String::Format((int)current.size()) + "...", current.size(), rules->GetNoRules()))
#else
		if (!message.Progress("Assessing rule set of size " + String::Format(current.size()) + "...", current.size(), rules->GetNoRules()))
#endif
			break;

		actual.erase(actual.begin(), actual.end());          actual.reserve(no_objects);
		predicted.erase(predicted.begin(), predicted.end()); predicted.reserve(no_objects);

		int i;

		// Classify all objects using the current rule set.
		for (i = 0; i < no_objects; i++) {
			if (!ComputeFireIntersection(current, matrix[i], intersection))
				return NULL;
			if (!object->Create(*GetDecisionTable(), i, masked))
				return NULL;
			classification = intersection.empty() ? NULL : dynamic_cast(Classification *, GetClassifier()->Classify(*object, intersection));
			actual.push_back((object->GetEntry(decision_attribute) == rocclass) ? 1 : 0);
			predicted.push_back((classification == NULL) ? GetROCFallbackCertainty() : classification->GetBinaryOutcomeCoefficient(rocclass));
		}

		ROCCurve roccurve;

		int  resolution = 251;
		bool progress   = false;
		bool cindex     = false;

		// Compute area under ROC curve.
		if (!roccurve.Create(actual, predicted, resolution, progress, cindex))
			return NULL;

		Vector(float) entry;
		entry.reserve(4);

		entry.push_back((*it1).first);						    // Quality threshold.
		entry.push_back(current.size());		          // Number of rules.
		entry.push_back(roccurve.GetArea());          // AUC.
		entry.push_back(roccurve.GetStandardError()); // SE.

		if (!SaveLogEntry(stream, entry))
			return NULL;

	}

	// Hack.
	//Message::Reset();

	// Make sure classifier does not "hold" the rules unnecessarily.
	self->InitializeClassifier(NULL);

	IOKit::Close(stream);

	return &structure;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SaveLogHeader
// Author........: Aleksander šhrn/Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
QualityRuleFilterLoop::SaveLogHeader(ofstream &stream, const Rules &rules, const QualityMap &qualitymap, const STLSet(int) &undefined) const {

	stream << "% Output from ROSETTA, " << SystemKit::GetUser() << " " << SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " << IdHolder::GetClassname(GetId()) << endl;
	stream << "% {" << GetParameters() << "}" << endl;
	stream << "%" << endl;
	stream << "% Applied to " << rules.GetName() << endl;
	stream << "% Rule set contains " << rules.GetNoRules() << " rules." << endl;
	stream << "%" << endl;
	stream << "% Note that rule indices below are 0-based." << endl;
	stream << endl;

	int width = 12;

	String headera = "Quality";       headera.Pad(' ', width);
	String headerb = "Rule index";    headerb.Pad(' ', width);

	stream << headera << " " << headerb << endl;

	QualityMap::const_iterator it1 = qualitymap.end();

	// Dump quality map.
	while (true) {
		if (it1 == qualitymap.begin())
			break;
		it1--;
		String quality = String::Format((*it1).first);
		quality.Pad(' ', width);
		for (Vector(int)::const_iterator it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
			stream << quality << " " <<	(*it2) << endl;
		}
	}

	String nada(Undefined::String()); nada.Pad(' ', width);

	// Dump undefined quality information.
	for (STLSet(int)::const_iterator it3 = undefined.begin(); it3 != undefined.end(); it3++) {
		stream << nada << " " << (*it3) << endl;
	}

	stream << endl;

	// Dump header for individual loop results.
	String header1 = "Threshold";     header1.Pad(' ', width);
	String header2 = "Rules";         header2.Pad(' ', width);
	String header3 = "AUC";           header3.Pad(' ', width);
	String header4 = "SE";            header4.Pad(' ', width);

	stream << header1 << " " << header2 << " " << header3 << " " << header4 << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: SaveLogEntry
// Author........: Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
QualityRuleFilterLoop::SaveLogEntry(ofstream &stream, const Vector(float) &entry) const {

	int    i, width = 12;
	String value;

	for (i = 0; i < entry.size(); i++) {
		value = (entry[i] == Undefined::Float()) ? Undefined::String() : String::Format(entry[i]);
		value.Pad(' ', width);
		stream << value << " ";
	}

	stream << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetRuleStep
// Author........: Thomas ×gotnes
// Date..........: 981105
// Description...:
// Comments......:
// Revisions.....: Aš 981124: Changes.
//===================================================================

int
QualityRuleFilterLoop::GetRuleStep(int no_rules) const {

	int step = 1;

	switch (GetResolution()) {
		case RESOLUTION_FIXED:   step = GetFixedResolution();                                       break;
		case RESOLUTION_DYNAMIC: step = static_cast(int, floor(GetDynamicResolution() * no_rules)); break;
	}

	return std::max(1, std::min(step, GetResolutionLimit()));

}

//-------------------------------------------------------------------
// Method........: InitializeClassifier
// Author........: Aleksander šhrn
// Date..........: 981124
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
QualityRuleFilterLoop::InitializeClassifier(const Rules *rules) {

	// Are the parameters set?
	if (GetClassifier() == NULL) {
		Message::Error("Classifier not set.");
		return false;
	}

	// Set up classifier
	if (!GetClassifier()->SetRules(rules))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: InitializeFireMatrix
// Author........: Aleksander šhrn
// Date..........: 981124
// Description...: Determines which rules that fire for which objects.
// Comments......: Rules::Matches must create sorted indices.
// Revisions.....: Aš 990727: Added IDG stuff.
//===================================================================

bool
QualityRuleFilterLoop::InitializeFireMatrix(const Rules &rules, bool masked, Vector(Vector(int)) &matrix) const {

	int no_objects = GetDecisionTable()->GetNoObjects(masked);

	matrix.erase(matrix.begin(), matrix.end());
	matrix.reserve(no_objects);

	Vector(int)               fires;
	Handle<InformationVector> object = Creator::InformationVector();
	Discerner                 discerner;

	// Initialize discerner.
	if (GetClassifier()->UseIDG()) {
		if (!discerner.LoadIDGs(GetClassifier()->GetIDGFilename(), *GetDecisionTable(), masked)) {
			Message::Error("Failed to load IDGs.");
			return false;
		}
	}

	int i;

	// For each object, create a vector of indices of rules that fire.
	for (i = 0; i < no_objects; i++) {
		if (!object->Create(*GetDecisionTable(), i, masked))
			return false;
		if (!rules.Matches(*object, discerner, GetClassifier()->GetFraction(), fires))
			return false;
		matrix.push_back(fires);
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: InitializeQualityMap
// Author........: Aleksander šhrn
// Date..........: 981124
// Description...: Creates a map from quality measures to rule
//                 indices. Also, a sorted vector with indices of
//                 rules with undefined quality measures is created.
// Comments......:
// Revisions.....:
//===================================================================

bool
QualityRuleFilterLoop::InitializeQualityMap(const Vector(float) &qualities, QualityMap &qualitymap, STLSet(int) &undefined) const {

	qualitymap.erase(qualitymap.begin(), qualitymap.end());
	undefined.erase(undefined.begin(), undefined.end());

	Vector(int) singleton;
	singleton.reserve(1);
	singleton.push_back(0);

	int i;

	// Build map.
	for (i = 0; i < qualities.size(); i++) {

		// Undefined quality?
		if (qualities[i] == Undefined::Float()) {
			undefined.insert(i);
			continue;
		}

		QualityMap::iterator it = qualitymap.find(qualities[i]);

		if (it == qualitymap.end()) {
			singleton[0] = i;
			qualitymap.insert(Pair(const float, Vector(int))(qualities[i], singleton));
		}
		else {
			(*it).second.push_back(i);
		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ComputeFireIntersection
// Author........: Aleksander šhrn
// Date..........: 981124
// Description...: Computes the intersection between (i) the set of
//                 rules in the current working set, and (ii) the
//                 set of rules that fire for an object.
// Comments......: The sets are sorted containers of indices.
// Revisions.....:
//===================================================================

bool
QualityRuleFilterLoop::ComputeFireIntersection(const STLSet(int) &current, const Vector(int) &fires, Vector(int) &intersection) const {

	Vector(int) temporary;
	temporary.reserve(current.size());

	// Compute intersection.
	Vector(int)::const_iterator end = std::set_intersection(current.begin(), current.end(), fires.begin(), fires.end(), temporary.begin());

	intersection.erase(intersection.begin(), intersection.end());
	intersection.reserve(end - temporary.begin());

	// Copy. Not very elegant, rewrite later...
	for (Vector(int)::const_iterator it = temporary.begin(); it != end; it++)
		intersection.push_back(*it);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Thomas ×gotnes
// Date..........: 981026
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
QualityRuleFilterLoop::GetString(Resolution resolution) {

	switch (resolution) {
		case RESOLUTION_FIXED:	 return "Fixed";
		case RESOLUTION_DYNAMIC: return "Dynamic";
		default:                 return Undefined::String();
	}

}

 
