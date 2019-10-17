//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTPERFORMANCEFILTER_H__
#define __REDUCTPERFORMANCEFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductfilter.h>
#include <kernel/algorithms/rulegenerator.h>
#include <kernel/algorithms/batchclassifier.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class.........: ReductPerformanceFilter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: Each reduct (its rules) is used separately to classify
//                 all objects in a decision table. This produces a
//                 confusion matrix (CM) for each reduct, which is used to
//                 score them. Filters away those reducts that do not perform
//                 well according to some performance criterion.
//
// Revisions.....: AÛ 990204: For some obscure reason, the ú$@$$ú$ú
//                            compiler suddenly complains about using
//                            an STL (int, String) pair, even though
//                            the code has always compiled OK before...
//                            So I define a helper struct ISPair
//                            instead as a workaround.
//===================================================================

class ReductPerformanceFilter : public ReductFilter {
public:

	//- Enumerated types...............................................
	enum Criterion {CRITERION_ROW, CRITERION_COLUMN, CRITERION_DIAGONAL};

	//- Textual representations........................................
	static String             GetString(ReductPerformanceFilter::Criterion criterion);

protected:

	//- Helper stuff for internal use..................................
	struct ReductScore {
		float  score_;                             // Performance score.
		float  auc_;                               // Area under ROC curve, if any.
		String reduct_;                            // String representation of reduct.
		int    index_;                             // Index of reduct.
	};

	struct ReductScoreComparator1 {
		bool operator () (const ReductScore &x, const ReductScore &y) const {
			return x.score_ > y.score_;
		}
	};

	struct ReductScoreComparator2 {
		bool operator () (const ReductScore &x, const ReductScore &y) const {
			return x.auc_ > y.auc_;
		}
	};

protected:

  //- Parameters.....................................................
	Handle<RuleGenerator>     rulegenerator_;    // For generating rules from reducts and from...
	Handle<DecisionTable>     decisiontable_rg_; // ...this decision table (if set).
	Handle<BatchClassifier>   batchclassifier_;  // For applying rules to classify...
	Handle<DecisionTable>     decisiontable_bc_; // ...this decision table (if set).
	Criterion                 criterion_;        // Performance criterion, contents of CM of to examine.
	int                       index_;            // Index of row or column in CM.
	float                     threshold_;        // Remove reducts with a score below this threshold.
	String                    filename_;         // Location of log file.

  //- Mutable stuff for bookkeeping..................................
	Vector(ReductScore)       scores_;           // Performance scores with extra information, for summary statistics.
	Vector(float)             only_scores_;      // Only the performance scores, for summary statistics.

protected:

	//- Methods inherited from Filter..................................
	virtual bool              Remove(const Structures &structures, int i) const;

	//- Local methods..................................................
	void                      CalculateStatistics() const;

public:

  //- Constructors/destructor........................................
  ReductPerformanceFilter();
  virtual ~ReductPerformanceFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String            GetParameters() const;
	virtual bool              SetParameter(const String &keyword, const String &value);
	virtual bool              GetOutputFilenames(Vector(String) &filenames) const;
	virtual Structure        *Apply(Structure &structure) const;
	virtual ReductPerformanceFilter	*Clone();

	//- Parameter management...........................................
	Handle<RuleGenerator>     GetRuleGenerator() const {return rulegenerator_;}
	bool                      SetRuleGenerator(Handle<RuleGenerator> rulegenerator) {rulegenerator_ = rulegenerator; return rulegenerator != NULL;}

	Handle<BatchClassifier>   GetBatchClassifier() const {return batchclassifier_;}
	bool                      SetBatchClassifier(Handle<BatchClassifier> batchclassifier) {batchclassifier_ = batchclassifier; return batchclassifier != NULL;}

	Handle<DecisionTable>     GetRGDecisionTable() const {return decisiontable_rg_;}
	bool                      SetRGDecisionTable(Handle<DecisionTable> decisiontable) {decisiontable_rg_ = decisiontable; return true;}

	Handle<DecisionTable>     GetBCDecisionTable() const {return decisiontable_bc_;}
	bool                      SetBCDecisionTable(Handle<DecisionTable> decisiontable) {decisiontable_bc_ = decisiontable; return true;}

	Criterion                 GetCriterion() const {return criterion_;}
	bool                      SetCriterion(Criterion criterion) {criterion_ = criterion; return true;}

	int                       GetIndex() const {return index_;}
	bool                      SetIndex(int index) {index_ = index; return index >= 0;}

	float                     GetThreshold() const {return threshold_;}
	bool                      SetThreshold(float threshold) {threshold_ = threshold; return true;}

	const String             &GetFilename() const {return filename_;}
	bool                      SetFilename(const String &filename) {filename_ = filename; return true;}

};

#endif 
