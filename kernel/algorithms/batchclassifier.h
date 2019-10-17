//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960619
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BATCHCLASSIFIER_H__
#define __BATCHCLASSIFIER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/algorithms/classifier.h>

#include <kernel/basic/pair.h>
#include <kernel/system/iostream.h>

// -N 13/06/13
#include "omp.h"
//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class BatchClassification;
class Classification;
class ConfusionMatrix;
class DecisionTable;
class Rules;

//-------------------------------------------------------------------
// Class.........: BatchClassifier
// Author........: Aleksander Ûhrn
// Date..........: 960619
// Description...: Base class for batch classifiers.  As default, a
//                 batch classifier can be seen as an algorithm that
//                 makes repeated use of a (single object) classifier
//                 in order to produce a batch classification
//                 structure containing a confusion matrix.
//
// Comments......: Class becoming complex...
// Revisions.....:
//===================================================================

class BatchClassifier : public Algorithm {
public:

	//- Enumerated types...............................................
	enum MultipleStrategy {MULTIPLE_BEST, MULTIPLE_PRIORITIZE, MULTIPLE_REFRAIN};

	//- Textual representations........................................
	static String      GetString(BatchClassifier::MultipleStrategy strategy);

protected:

	//- For internal use...............................................
	typedef Pair(int, Handle<Classification>) ICPair;

protected:

	//- Parameters.....................................................
	Handle<Classifier> classifier_;            // The classifier algorithm.

	bool               fallback_;              // Assign a fallback (default) decision class on classifier failure?
  String             fallback_class_;        // Fallback decision class.
	float              fallback_certainty_;    // Certainty factor (abstract) associated with fallback class.

  MultipleStrategy   strategy_multiple_;     // How should multiple possible classifications be dealt with?
	String             priority_class_;        // Class to prioritize.
	float              priority_threshold_;    // Certainty threshold for prioritization.

	bool               log_;                   // Log individual classifications to file?
	String             log_filename_;          // Log filename.
	bool               log_verbose_;           // Verbose log?

	bool               roc_;                   // Generate ROC curve?
	String             roc_class_;             // Class to generate ROC curve for.
	String             roc_filename_;          // ROC filename.

	bool               roc_details_;           // For internal use: Include ROC details in the returned BatchClassification structure?

	bool               calibration_;           // Generate calibration plot file?
	String             calibration_class_;     // Class to generate calibration plot for.
	String             calibration_filename_;  // Calibration plot filename.
	int                calibration_no_groups_; // Number of groups (data points).
	//Torgeir 07.06.01 ------------------------
	float		weight_;
	bool		roc_data_;
	//Torgeir 07.06.01 ------------------------

	//-N 13/06/13 --- For thread safe classifiers.
/*	#ifdef _OPENMP
		typedef Map2(int, Classifier*) ICMap;
		ICMap	classifierMap_;
	#endif
*/
protected:

	//- Decision making................................................
	virtual int        ProcessClassification(Handle<Classification> classification, int priority_class) const;

	//- Log file formatting............................................
	virtual bool       SaveLogHeader(ofstream &stream, const DecisionTable &table) const;
	virtual bool       SaveLogEntry(ofstream &stream, int object_no, int actual, int predicted, const Handle<Classification> result, const Handle<DecisionTable> table, int attribute_no, bool masked) const;
	virtual bool       SaveLogFooter(ofstream &stream, const ConfusionMatrix &matrix) const;

public:

  //- Constructors/destructor........................................
  BatchClassifier();
  virtual ~BatchClassifier();

	//- Copy Constructor -N 20/05/13.............................
	BatchClassifier(const BatchClassifier& other);

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String     	GetParameters() const;
	virtual bool       	GetOutputFilenames(Vector(String) &filenames) const;
	virtual bool       	SetParameter(const String &keyword, const String &value);
	virtual bool       	IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure 	*Apply(Structure &structure) const;
	virtual BatchClassifier	*Clone();

  //- Local methods..................................................
	bool               SetRules(const Rules *rules);


	//- Parameter management methods...................................
	// -N 13/06/13
	// Removing threadshared classifiers. -N
/*	#ifdef _OPENMP
		Classifier	*GetClassifier() const {
					return const_cast(Classifier *, classifierMap_.find(omp_get_thread_num())->second);
				}
		bool		SetClassifier(Classifier *classifier) {
					if (classifierMap_.count(omp_get_thread_num()) == 1) {
						classifierMap_.erase(omp_get_thread_num());
					}
					std::pair<int, Classifier*> p(omp_get_thread_num(), classifier);
					classifierMap_.insert(p);
					return (classifier != NULL);
				}

	#else
*/		Classifier	*GetClassifier() const {return const_cast(Classifier *, classifier_.GetPointer());}
		bool		SetClassifier(Classifier *classifier) {classifier_ = classifier; return (classifier_ != NULL);}
//	#endif
	bool               AssignFallback() const {return fallback_;}
	bool               AssignFallback(bool fallback) {fallback_ = fallback; return true;}

	const String      &GetFallbackClass() const {return fallback_class_;}
	bool               SetFallbackClass(const String &fallback_class) {fallback_class_ = fallback_class; return true;}

	float              GetFallbackCertainty() const {return fallback_certainty_;}
	bool               SetFallbackCertainty(float fallback_certainty) {fallback_certainty_ = fallback_certainty; return true;}

	MultipleStrategy   GetMultipleStrategy() const {return strategy_multiple_;}
	bool               SetMultipleStrategy(MultipleStrategy strategy) {strategy_multiple_ = strategy; return true;}

	const String      &GetPriorityClass() const {return priority_class_;}
	bool               SetPriorityClass(const String &priority_class) {priority_class_ = priority_class; return true;}

	float              GetPriorityThreshold() const {return priority_threshold_;}
	bool               SetPriorityThreshold(float priority_threshold) {priority_threshold_ = priority_threshold; return true;}

	bool               LogDetails() const {return log_;}
	bool               LogDetails(bool log) {log_ = log; return true;}

	const String      &GetLogFilename() const {return log_filename_;}
	bool               SetLogFilename(const String &filename) {log_filename_ = filename; return true;}

	bool               VerboseLog() const {return log_verbose_;}
	bool               VerboseLog(bool verbose) {log_verbose_ = verbose; return true;}

	bool               GenerateROC() const {return roc_;}
	bool               GenerateROC(bool roc) {roc_ = roc; return true;}

	const String      &GetROCClass() const {return roc_class_;}
	bool               SetROCClass(const String &roc_class) {roc_class_ = roc_class; return true;}

	const String      &GetROCFilename() const {return roc_filename_;}
	bool               SetROCFilename(const String &filename) {roc_filename_ = filename; return true;}

	bool               ReturnROCDetails() const {return roc_details_;}
	bool               ReturnROCDetails(bool roc_details) {roc_details_ = roc_details; return true;}

	bool               GenerateCalibration() const {return calibration_;}
	bool               GenerateCalibration(bool calibration) {calibration_ = calibration; return true;}

	const String      &GetCalibrationClass() const {return calibration_class_;}
	bool               SetCalibrationClass(const String &calibration_class) {calibration_class_ = calibration_class; return true;}

	const String      &GetCalibrationFilename() const {return calibration_filename_;}
	bool               SetCalibrationFilename(const String &filename) {calibration_filename_ = filename; return true;}

	int                GetNoCalibrationGroups() const {return calibration_no_groups_;}
	bool               SetNoCalibrationGroups(int no_groups) {calibration_no_groups_ = no_groups; return no_groups > 0;}
	//Torgeir 07.06.01 ------------------------
	bool							 SetWeight(float weight) {weight_ = weight; return true;}
	bool							 ROCData() { roc_data_ = true; return true;}
	//Torgeir 07.06.01 ------------------------

};

#endif 
