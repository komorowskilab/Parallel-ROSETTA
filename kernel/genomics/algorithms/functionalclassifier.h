//-------------------------------------------------------------------
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __FUNCTIONALCLASSIFIER_H__
#define __FUNCTIONALCLASSIFIER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/basic/pair.h>
#include <kernel/utilities/rng.h>
#include <kernel/structures/classification.h>

#include <kernel/structures/rules.h>
#include <kernel/algorithms/reducer.h>

//-------------------------------------------------------------------
// Class.........: FunctionalClassifier
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class FunctionalClassifier : public Algorithm {

	// Templates start values
	int template_length_;
	int constant_template_length_;
	float crease_thr_;
	float constant_thr_;
	float start_end_thr_;
	float tolerance_thr_;
	//Templates end values
	int template_length_to_;
	int constant_template_length_to_;
	float crease_thr_to_;
	float constant_thr_to_;
	float start_end_thr_to_;
	float tolerance_thr_to_;
	//Templates iterative step
	int template_length_step_;
	int constant_template_length_step_;
	float crease_thr_step_;
	float constant_thr_step_;
	float start_end_thr_step_;
	float tolerance_thr_step_;
	//Iterate over parameter settings?
	bool iterate_cv_;

	//Randomization
	bool randomize_;
	int rands_;
	
	bool idg_;
	String idg_file_;

	//Cross validation parameters
	bool cvlog_;
	String cvlog_file_;
	int cv_;
	int rng_;

	String decision_;

	bool log_;
	String log_file_;
	String rulelog_file_;
	String randlog_file_;
	String paramslog_file_;
	float weight_;

	//Rule pruning
	bool prune_;
	float prune_coverage_thr_;
	float prune_accuracy_thr_;

	//Algorithm for reduct computation
	Handle<Reducer> reducer_;

protected:

	//- For internal use...............................................
	typedef Pair(int, Handle<Classification>) ICPair;
	typedef Pair(float, int) FIPair;

	struct FIPairCompareFirst {
		bool operator () (const FIPair &x, const FIPair &y) const {
			return x.first > y.first;
		}
	};

protected:

	Vector(Vector(int)) generalized_decision_;

public:

  //- Constructors/destructor........................................
  FunctionalClassifier();
  virtual ~FunctionalClassifier();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String         GetParameters() const;
	virtual bool           SetParameter(const String &keyword, const String &value);
	virtual Structure      *Apply(Structure &structure) const;
	virtual bool		   IsApplicable(const Structure &structure, bool warn = true) const;
	virtual FunctionalClassifier	*Clone();

	//- New methods for FunctionalClassifier...........................
	bool				DoSteps(DecisionTable &table) const;
	bool				Divide(DecisionTable &table, DecisionTable &labelled, DecisionTable &labelled_test, DecisionTable &unlabelled_test);
	bool				ReClassify(const DecisionTable &labelled, const DecisionTable &labelled_test, const DecisionTable &unlabelled_test) const;
	Handle<Rules>		PruneRules(DecisionTable &table, const Rules &rules, Structure &reducts) const;

	//- Local methods..................................................
	bool                SetWeight(float weight) {weight_ = weight; return true;}
	float               GetWeight() const {return weight_;}
	bool				SetLogFile(String log_file) {log_file_ = log_file; return true;} 
	String				GetLogFile() const {return log_file_;} 
	bool				SetRuleLogFile(String rulelog_file) {rulelog_file_ = rulelog_file; return true;} 
	String				GetRuleLogFile() const {return rulelog_file_;} 
	bool				SetRandLogFile(String randlog_file) {randlog_file_ = randlog_file; return true;} 
	String				GetRandLogFile() const {return randlog_file_;} 
	bool				SetParamsLogFile(String paramslog_file) {paramslog_file_ = paramslog_file; return true;} 
	String				GetParamsLogFile() const {return paramslog_file_;} 

	bool				SetTemplateLength(int template_length, int template_length_to, int template_length_step) {
							template_length_ = template_length; 
							template_length_to_ = template_length_to;
							template_length_step_ = template_length_step;
							return true;
						}

	void				GetTemplateLength(int &template_length, int &template_length_to, int &template_length_step) const {
							template_length = template_length_; 
							template_length_to = template_length_to_;
							template_length_step = template_length_step_;
						}

	bool				SetConstantTemplateLength(int constant_template_length, int constant_template_length_to, int constant_template_length_step) {
							constant_template_length_ = constant_template_length;
							constant_template_length_to_ = constant_template_length_to;
							constant_template_length_step_ = constant_template_length_step;
							return true;
						} 

	void				GetConstantTemplateLength(int &constant_template_length, int &constant_template_length_to, int &constant_template_length_step) const {
							constant_template_length = constant_template_length_;
							constant_template_length_to = constant_template_length_to_;
							constant_template_length_step = constant_template_length_step_;
						} 

	bool				SetCreaseThr(float crease_thr, float crease_thr_to, float crease_thr_step) {
							crease_thr_ = crease_thr;
							crease_thr_to_ = crease_thr_to;
							crease_thr_step_ = crease_thr_step;
							return true;
						} 

	void				GetCreaseThr(float &crease_thr, float &crease_thr_to, float &crease_thr_step) const{
							crease_thr = crease_thr_;
							crease_thr_to = crease_thr_to_;
							crease_thr_step = crease_thr_step_;
						} 

	bool				SetConstantThr(float constant_thr, float constant_thr_to, float constant_thr_step) {
							constant_thr_ = constant_thr;
							constant_thr_to_ = constant_thr_to;
							constant_thr_step_ = constant_thr_step;
							return true;
						} 

	void				GetConstantThr(float &constant_thr, float &constant_thr_to, float &constant_thr_step) const{
							constant_thr = constant_thr_;
							constant_thr_to = constant_thr_to_;
							constant_thr_step = constant_thr_step_;
						} 

	bool				SetStartEndThr(float start_end_thr, float start_end_thr_to, float start_end_thr_step) {
							start_end_thr_ = start_end_thr;
							start_end_thr_to_ = start_end_thr_to;
							start_end_thr_step_ = start_end_thr_step;
							return true;
						} 

	void				GetStartEndThr(float &start_end_thr, float &start_end_thr_to, float &start_end_thr_step) const{
							start_end_thr = start_end_thr_;
							start_end_thr_to = start_end_thr_to_;
							start_end_thr_step = start_end_thr_step_;
						} 
	
	bool				SetToleranceThr(float tolerance_thr, float tolerance_thr_to, float tolerance_thr_step) {
							tolerance_thr_ = tolerance_thr;
							tolerance_thr_to_ = tolerance_thr_to;
							tolerance_thr_step_ = tolerance_thr_step;
							return true;
						}


	void				GetToleranceThr(float &tolerance_thr, float &tolerance_thr_to, float &tolerance_thr_step) {
							tolerance_thr = tolerance_thr_;
							tolerance_thr_to = tolerance_thr_to_;
							tolerance_thr_step = tolerance_thr_step_;
						}
	
	bool                SetIterateCV(bool iterate) {iterate_cv_ = iterate; return true;}
	bool                GetIterateCV() const {return iterate_cv_;}
	bool				SetClassify(bool classify) {log_ = classify; return true;}
	bool				GetClassify() const {return log_;}
	bool                SetRandomizations(int rands) {rands_ = rands; 
													  randomize_ = (rands > 0);
													  return true;
						}

	int                 GetRandomizations() const {return rands_;}

	bool				SetIDG(bool idg) {idg_ = idg; return true;}
	bool				GetIDG() const {return idg_;}
	bool				SetIDGFile(String idg_file) {idg_file_ = idg_file; return true;}
	String				GetIDGFile() const {return idg_file_;}

	bool				SetCVLog(bool cvlog) {cvlog_ = cvlog; return true;}
	bool				GetCVLog() const {return cvlog_;}
	bool				SetCVLogFile(String cvlog_file) {cvlog_file_ = cvlog_file; return true;}
	String				GetCVLogFile() const {return cvlog_file_;}
	bool				SetCV(int cv) {cv_ = cv; return true;}
	int 				GetCV() const {return cv_;}
	bool				SetRNG(int rng) {rng_ = rng; return true;}
	int 				GetRNG() const {return rng_;}

	bool				SetRulePruning(bool prune) {prune_ = prune; return true;}
	bool				GetRulePruning() const {return prune_;}
	bool				SetRulePruningAcc(float prune_accuracy_thr) {
							prune_accuracy_thr_ = prune_accuracy_thr;
							return true;
						}
	float				GetRulePruningAcc() const {return prune_accuracy_thr_;}
	bool				SetRulePruningCov(float prune_coverage_thr) {
							prune_coverage_thr_ = prune_coverage_thr;
							return true;
						}
	float				GetRulePruningCov() const {return prune_coverage_thr_;}

	Handle<Reducer>     GetReducer() const {return reducer_;}
	bool                SetReducer(const Handle<Reducer> reducer) {reducer_ = reducer; return true;}
};

#endif 
