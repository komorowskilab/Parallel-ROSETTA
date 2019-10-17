//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn/Thomas Êgotnes
// Date..........: 981026
// Description...:
// Revisions.....:
//===================================================================

#ifndef __QUALITYRULEFILTERLOOP_H__
#define __QUALITYRULEFILTERLOOP_H__

#include <copyright.h>

#include <kernel/algorithms/qualityrulefilter.h>
#include <kernel/algorithms/voter.h>

#include <kernel/basic/set.h>

//-------------------------------------------------------------------
// Class.........: QualityRuleFilterLoop
// Author........: Aleksander Ûhrn/Thomas Êgotnes
// Date..........:
// Description...: Performs a simple filter-evaluate loop and logs
//                 the performance results at each iteration.
// Revisions.....:
//===================================================================

class QualityRuleFilterLoop : public QualityRuleFilter {
public:

	//- Type definitions...............................................
	typedef Map(float, Vector(int)) QualityMap;

	//- Enumerated types...............................................
	enum Resolution {RESOLUTION_FIXED, RESOLUTION_DYNAMIC};

	//- Textual representations........................................
	static String               GetString(Resolution resolution);

protected:

	//- Parameters.....................................................
	String                      filename_;           // Location of log file.
	Handle<DecisionTable>       table_;              // Decision table to apply rules to.
	Handle<Voter>		            classifier_;	       // The algorithm that actually applies the rules.
	String									    focus_class_;        // Focus class for ROC curve generation.
	float										    certainty_;          // Certainty for fallback.
	Resolution							    resolution_;				 // Resolution type.
	int											    fixed_resolution_;	 // Value for fixed resolution.
	float										    dynamic_resolution_; // Percentage for dynamic resolution.
	int											    resolution_limit_;	 // Maximum rule step.

protected:

	//- Helper methods.................................................
	bool                        SaveLogHeader(ofstream &stream, const Rules &rules, const QualityMap &qualitymap, const STLSet(int) &undefined) const;
	bool                        SaveLogEntry(ofstream &stream, const Vector(float) &entry) const;

	int										 	    GetRuleStep(int no_rules) const;

	bool                        InitializeClassifier(const Rules *rules);
	bool                        InitializeFireMatrix(const Rules &rules, bool masked, Vector(Vector(int)) &matrix) const;
	bool                        InitializeQualityMap(const Vector(float) &qualities, QualityMap &qualitymap, STLSet(int) &undefined) const;

	bool                        ComputeFireIntersection(const STLSet(int) &current, const Vector(int) &fires, Vector(int) &intersection) const;

public:

  //- Constructors/destructor........................................
  QualityRuleFilterLoop();
  virtual ~QualityRuleFilterLoop();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String              GetParameters() const;
	virtual bool                SetParameter(const String &keyword, const String &value);
	virtual bool                GetOutputFilenames(Vector(String) &filenames) const;
  virtual Structure          *Apply(Structure &structure) const;

	//- Parameter methods..............................................
	const Handle<DecisionTable> GetDecisionTable() const {return table_;}
	Handle<DecisionTable>       GetDecisionTable() {return table_;}
  bool                        SetDecisionTable(const Handle<DecisionTable> table) {table_ = table; return true;}

	const Handle<Voter>         GetClassifier() const {return classifier_;}
	Handle<Voter>               GetClassifier() {return classifier_;}
  bool                        SetClassifier(const Handle<Voter> classifier) {classifier_ = classifier; return true;}

	const String               &GetFilename() const {return filename_;}
	bool                        SetFilename(const String &filename) {filename_ = filename; return true;}

	const String							 &GetROCFocusClass() const {return focus_class_;}
	bool                        SetROCFocusClass(const String &focus_class) {focus_class_ = focus_class; return true;}

	float										    GetROCFallbackCertainty() const {return certainty_;}
	bool                        SetROCFallbackCertainty(float certainty) {certainty_ = certainty; return true;}

	Resolution							    GetResolution() const {return resolution_;}
	bool                        SetResolution(Resolution resolution) {resolution_ = resolution; return true;}

	float										    GetDynamicResolution() const {return dynamic_resolution_;}
	bool                        SetDynamicResolution(float resolution) {dynamic_resolution_ = resolution; return true;}

	int											    GetFixedResolution() const {return fixed_resolution_;}
	bool                        SetFixedResolution(int resolution) {fixed_resolution_ = resolution; return true;}

	int											    GetResolutionLimit() const {return resolution_limit_;}
	bool                        SetResolutionLimit(int limit) {resolution_limit_ = limit; return true;}

};

#endif
 
