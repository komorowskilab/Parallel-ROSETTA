//-------------------------------------------------------------------
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CVCLASSIFIER_H__
#define __CVCLASSIFIER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/algorithms/reducer.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/basic/pair.h>
#include <kernel/utilities/rng.h>
#include <kernel/structures/classification.h>
#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Class.........: CVClassifier
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class CVClassifier : public Algorithm {

private:
	//- Type definitions...............................................
	typedef Map(const String,Vector(float))   AnnotationData;
	typedef Map(const String, int)             RandomizationData;

protected:

	//TEMPLATE PARAMETERS
	//parameters "from"
	int template_length_;
	int constant_template_length_;
	float crease_thr_;
	float constant_thr_;
	float start_end_thr_;
	float tolerance_thr_;

	//parameters "to"
	int template_length_to_;
	int constant_template_length_to_;
	float crease_thr_to_;
	float constant_thr_to_;
	float start_end_thr_to_;
	float tolerance_thr_to_;

	//parameters "step"
	int template_length_step_;
	int constant_template_length_step_;
	float crease_thr_step_;
	float constant_thr_step_;
	float start_end_thr_step_;
	float tolerance_thr_step_;
	
	// parameter iteration
	bool iterate_cv_;
	String paramslog_file_;
	int iterations_;

	// randomization
	int rands_;
	String randlog_file_;
	
	// approximative reducts
	bool approximate_;
	float approximate_fraction_;
	
	// indiscernibility graph usage
	bool idg_;
	String idg_file_;

	// cross validation
	bool   cvlog_;
	String cvlog_file_;
	int    cv_;

	// seed to random number generator
	int rng_;

	// weight on false positives
	float weight_;

	// Rule pruning
	bool prune_;
	float prune_coverage_thr_;
	float prune_accuracy_thr_;

	//Algorithm for reduct computation
	Handle<Reducer> reducer_;

	//- Feature selection
	int coverage_thr_;
	float accuracy_thr_;
	int max_features_thr_;
	int min_features_thr_;

	// obsolete?
	bool supervised_;

	//internal use
	int both_;
	Vector(int) indices_;
	Vector(Vector(int)) gd_;


	//- For internal use...............................................
	typedef Pair(int, Handle<Classification>) ICPair;
	typedef Pair(float, int) FIPair;

	struct FIPairCompareFirst {
		bool operator () (const FIPair &x, const FIPair &y) const {
			return x.first > y.first;
		}
	};

public:

  //- Constructors/destructor........................................
  CVClassifier();
  virtual ~CVClassifier();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String          GetParameters() const;
	virtual bool            SetParameter(const String &keyword, const String &value);
	virtual Structure       *Apply(Structure &structure) const;
	virtual bool			IsApplicable(const Structure &structure, bool warn = true) const;
	virtual CVClassifier	*Clone();
	
  //- Parameter settings methods
	Handle<Reducer>     GetReducer() const {return reducer_;}
	bool                SetReducer(const Handle<Reducer> reducer) {reducer_ = reducer; return true;}
	//template length
	bool                    SetTemplateLength(int template_length) {template_length_ = template_length; return true;};
	int                     GetTemplateLength() const {return template_length_;};
	bool                    SetTemplateLengthTo(int template_length_to) {template_length_to_ = template_length_to; return true;};
	int                     GetTemplateLengthTo() const {return template_length_to_;};
	bool                    SetTemplateLengthStep(int template_length_step) {template_length_step_ = template_length_step; return true;};
	int                     GetTemplateLengthStep() const {return template_length_step_;};
	// constant template length
	bool                    SetConstantTemplateLength(int constant_template_length) {constant_template_length_ = constant_template_length; return true;};
	int                     GetConstantTemplateLength() const {return constant_template_length_;};
	bool                    SetConstantTemplateLengthTo(int constant_template_length_to) {constant_template_length_to_ = constant_template_length_to; return true;};
	int                     GetConstantTemplateLengthTo() const {return constant_template_length_to_;};
	bool                    SetConstantTemplateLengthStep(int constant_template_length_step) {constant_template_length_step_ = constant_template_length_step; return true;};
	int                     GetConstantTemplateLengthStep() const {return constant_template_length_step_;};
	// required increase/decrease
	bool                    SetCreaseThr(float crease_thr) {crease_thr_ = crease_thr; return true;};
	float                   GetCreaseThr() const {return crease_thr_;};
	bool                    SetCreaseThrTo(float crease_thr_to) {crease_thr_to_ = crease_thr_to; return true;};
	float                   GetCreaseThrTo() const {return crease_thr_to_;};
	bool                    SetCreaseThrStep(float crease_thr_step) {crease_thr_step_ = crease_thr_step; return true;};
	float                   GetCreaseThrStep() const {return crease_thr_step_;};
	// constant threshold
	bool                    SetConstantThr(float constant_thr) {constant_thr_ = constant_thr; return true;};
	float                   GetConstantThr() const {return constant_thr_;};
	bool                    SetConstantThrTo(float constant_thr_to) {constant_thr_to_ = constant_thr_to; return true;};
	float                   GetConstantThrTo() const {return constant_thr_to_;};
	bool                    SetConstantThrStep(float constant_thr_step) {constant_thr_step_ = constant_thr_step; return true;};
	float                   GetConstantThrStep() const {return constant_thr_step_;};
	// start end threshold
	bool                    SetStartEndThr(float start_end_thr) {start_end_thr_ = start_end_thr; return true;};
	float                   GetStartEndThr() const {return start_end_thr_;};
	bool                    SetStartEndThrTo(float start_end_thr_to) {start_end_thr_to_ = start_end_thr_to; return true;};
	float                   GetStartEndThrTo() const {return start_end_thr_to_;};
	bool                    SetStartEndThrStep(float start_end_thr_step) {start_end_thr_step_ = start_end_thr_step; return true;};
	float                   GetStartEndThrStep() const {return start_end_thr_step_;};
	// tolerance threshold
	bool                    SetToleranceThr(float tolerance_thr) {tolerance_thr_ = tolerance_thr; return true;};
	float                   GetToleranceThr() const {return tolerance_thr_;};
	bool                    SetToleranceThrTo(float tolerance_thr_to) {tolerance_thr_to_ = tolerance_thr_to; return true;};
	float                   GetToleranceThrTo() const {return tolerance_thr_to_;};
	bool                    SetToleranceThrStep(float tolerance_thr_step) {tolerance_thr_step_ = tolerance_thr_step; return true;};
	float                   GetToleranceThrStep() const {return tolerance_thr_step_;};
	// Reducer
	bool                    SetApproximate(bool approximate) {approximate_ = approximate; return true;};
	bool                    GetApproximate() const {return approximate_;};
	bool                 	SetApproximateFraction(float approximate_fraction) {approximate_fraction_ = approximate_fraction; return true;};
	float                	GetApproximateFraction() const {return approximate_fraction_;};
	bool                    SetIDG(bool idg) {idg_ = idg; return true;};
	bool                    GetIDG() const {return idg_;};
	bool                 	SetIDGFilename(String idg_file) {idg_file_ = idg_file; return true;};
	String                 	GetIDGFilename() const {return idg_file_;};
	// CV
	bool                    SetCVLog(bool cvlog) {cvlog_ = cvlog; return true;};
	bool                    GetCVLog() const {return cvlog_;};
	bool                    SetLogFilename(String cvlog_file) {cvlog_file_ = cvlog_file; return true;};
	String                  GetLogFilename() const {return cvlog_file_;};
	bool                    SetCV(int cv) {cv_ = cv; return true;};
	int                     GetCV() const {return cv_;};
	bool                    SetIterateCV(bool iterate_cv) {iterate_cv_ = iterate_cv; return true;};
	bool                    GetIterateCV() const {return iterate_cv_;};
	bool                    SetIterateCVLogFilename(String paramslog_file) {paramslog_file_ = paramslog_file; return true;};
	String                  GetIterateCVLogFilename() const {return paramslog_file_;};
	// seed
	bool                    SetSeed(int rng) {rng_ = rng; return true;};
	int                     GetSeed() const {return rng_;};
	// weight on false positives
	bool                    SetWeight(float weight) {weight_ = weight; return true;};
	float                   GetWeight() const {return weight_;};
	//Ranomizations
	bool                    SetNoRandomization(int rands) {rands_ = rands; return true;};
	int                     GetNoRandomization() const {return rands_;};
	bool                	SetRandomizationLogFilename(String randlog_file) {randlog_file_ = randlog_file; return true;};
	String                	GetRandomizationLogFilename() const {return randlog_file_;};
	//- Rule pruning
	bool                    SetRulePruning(bool prune) {prune_ = prune; return true;};
	bool                    GetRulePruning() const {return prune_;};
	bool                	SetRulePruningAccuracy(float prune_accuracy_thr) {prune_accuracy_thr_ = prune_accuracy_thr; return true;};
	float                	GetRulePruningAccuracy() const {return prune_accuracy_thr_;};
	bool                    SetRulePruningCoverage(float prune_coverage_thr) {prune_coverage_thr_ = prune_coverage_thr; return true;};
	float                   GetRulePruningCoverage() const {return prune_coverage_thr_;};
	
protected:

	//- New methods for IterativeClassifier
	bool					CV(DecisionTable &table, bool output, AnnotationData &annotData, RandomizationData &randCalc, int iter = 0);
	bool					CVOnce(DecisionTable &train, DecisionTable &test, Vector(ICPair) &classification_details, const int iteration, const bool masked) const;
	bool					CVIter(DecisionTable &table, bool output, AnnotationData &annotData, RandomizationData &randCalc);

	bool					CalculateStatistics(const DecisionTable &data, Vector(ICPair) &classification_details, Vector(int) &decision_values, Vector(Vector(float)) &results, float &auc, int &no_classes);
	bool					UpdateAnnotData(const DecisionTable &data, Vector(int) &decision_values, int no_classes, Vector(Vector(float)) &results, AnnotationData &annotData);
	void					UpdateRandData(const DecisionTable &data, Vector(int) &decision_values, int no_classes, Vector(Vector(float)) &results, AnnotationData &annotData, RandomizationData &randCalc);
	void					AddTextFile(DecisionTable &table, String filename) const;

	void					WriteParamsResult(AnnotationData &annotData) const;
	void					WriteRandResults(AnnotationData &annotData, RandomizationData &randCalc) const;
	bool					WriteResultsToFile(const DecisionTable &data, Vector(int) &decision_values, Vector(Vector(float)) &results, float auc, int no_classes, int iter = 0) const;

	bool					FeatureSelection(DecisionTable &train, DecisionTable &test, int iteration, bool masked) const;
	bool					PreprocessData(DecisionTable &data, DecisionTable &train, DecisionTable &test) const;
	bool					Discretize(DecisionTable &train, DecisionTable &test, bool masked) const;
	bool					SetGeneralizedDecision(const DecisionTable &train, DecisionTable &test);
	Handle<Rules>			PruneRules(DecisionTable &table, const Rules &rules) const;

	bool					BackwardCompatible(const DecisionTable &table) const;
	bool					Normalize(DecisionTable &table) const;
	bool					Randomize(DecisionTable &table, int seed) const;
	float					GetLearnability(const DecisionTable &table) const;

	void                    CleanUp();

	//- Methods copied from CVSerialExecutor
	bool					InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng);
	bool					SampleTables(int i, const RNG &/*rng*/, DecisionTable &training, DecisionTable &testing, bool masked) const;
	
};

#endif
 
