//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SERIALEXECUTORLOOP_H__
#define __SERIALEXECUTORLOOP_H__

#include <copyright.h>

#include <kernel/algorithms/serialexecutor.h>

#include <kernel/structures/rules.h>

#include <kernel/utilities/rng.h>


// -N 17/05/13
#include "omp.h"
#include <utility>
#include <string>
//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class BatchClassification;
class DecisionTable;

//-------------------------------------------------------------------
// Class.........: SerialExecutorLoop
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for repeatedly executing two pipelines
//                 with some sort of systmatic table sampling.
// Revisions.....:
//===================================================================

class SerialExecutorLoop : public SerialExecutor {
protected:

	//- Parameters.....................................................
	int			n_;     // Do loop n times.
	int			seed_;  // Seed to RNG.
	int			length_;// Length of training pipeline.
	Handle<Rules>		rules_; // For temporary, internal use. Output from training pipeline.
	Vector(String)		rulestring;
//	bool			append_;// For setting appending rules to SaveClassifier.
	//05/06/13 -N
//	std::string	log_;	// For logging SerialExecutor events. -N
	#ifdef _OPENMP
//		typedef Map2(int, std::string) ISMap;
		typedef Map2(int, Handle<Rules>) IRMap;
		IRMap	rulesmap_;  //For temporary, internal use. Output from training pipeline.	
//		ISMap	logmap_;	//For keeping track of the threaded logs. -N
	#endif

protected:

  //- Methods inherited from Executor................................
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const;

  //- Methods inherited from SerialExecutor..........................
	virtual bool        SetSpecialParameters(Algorithm &algorithm, const String &parameters) const;
	//Added prototype for passing SE rules. -N
//	virtual bool        SetSpecialParameters(Algorithm &algorithm, const String &parameters, Handle<Rules> rules) const;


  //- New virtual methods............................................
	virtual bool        SplitCommands(int index, const Algorithm::Handles &algorithms, const Vector(String) &parameters, Algorithm::Handles &algorithms1, Vector(String) &parameters1, Algorithm::Handles &algorithms2, Vector(String) &parameters2) const;

	virtual bool        InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng);
	virtual bool        SampleTables(int i, const RNG &rng, DecisionTable &training, DecisionTable &testing, bool masked) const = 0;

	virtual bool        SaveLogEntry(ofstream &stream, const BatchClassification &matrix) const;
	virtual bool        SaveLogStatistics(ofstream &stream, const Vector(float) &statistics, const String &name) const;

	//- Write methods for threads -N...............................
	virtual bool	SaveThreadedEntry(std::string &log, const BatchClassification &matrix, SerialExecutor &pipeline_training, SerialExecutor &pipeline_testing) const;
	virtual bool	SaveThreadedStatistics(std::string &log, const Vector(float) &statistics, const String &name) const;
	virtual bool	PrintThreadLog(Vector(std::string) &logVector, ofstream &stream) const;
//	bool		RemoveExistingClassifierFile() const;
//	bool		FileExists(String file) const;
public:
//- Local methods (should be protected)............................
	#ifdef _OPENMP		
		bool		SetRules(Handle<Rules> rules) { SetRules2(rules, omp_get_thread_num());}
		Handle<Rules>	GetRules() const {return rulesmap_.find(omp_get_thread_num())->second;}
		bool		SetRules2(Handle<Rules> rules, int i) {
					std::pair<int, Handle<Rules> > p (i, rules);
					rulesmap_.insert(p); 
					return true;}
		void 		InitRules() const {IRMap rulesmap_;}
/*
		//-N 06/05/13 Log access and management.....................
		bool		AppendLog(std::string input) { AppendLog2(input, omp_get_thread_num());}
		std::string	GetLog() const {return logmap_.find(omp_get_thread_num())->second;}
		bool		AppendLog2(std::string input, int i) {
					std::pair<int, std::string> p (i, input);
					logmap_.insert(p); 
					return true;}
		void		InitLog() {logmap_.erase(omp_get_thread_num());}
		//void		InitLog2(int i) {logmap_.erase(i);}
*/
	#else

		Handle<Rules>	GetRules() const {return rules_;}
		bool		SetRules(Handle<Rules> rules) {rules_ = rules; return true;}
		void		InitRules() const {Handle<Rules> rules_(NULL);}
	#endif


  //- Constructors/destructor........................................
  SerialExecutorLoop();
  virtual ~SerialExecutorLoop();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm..............................
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual bool        IsApplicable(const Structure &structure, bool warn = true) const;
	virtual SerialExecutorLoop 	*Clone() = 0;

	//- Local methods..................................................
	int                 GetN() const {return n_;}
	bool                SetN(int n) {n_ = n; return n > 0;}

	int                 GetSeed() const {return seed_;}
	bool                SetSeed(int seed) {seed_ = seed; return true;}

	int                 GetLength() const {return length_;}
	bool                SetLength(int length) {length_ = length; return true;}
	//Non-static method of getting the ruleset pointer. -N
//	Handle<Rules>	GetRulesFromTable(const DecisionTable &table) const;
	// -N 05/08/13 Method for loading classifiers from a sourcefile.
//	Handle<Rules>	LoadClassifier(const String &sourcefile, Handle<DecisionTable> table, bool masked) const;
	//-N 02/08/13
	bool		GetSaveClassifier() const {return Executor::GetSaveClassifier();}
//	bool		SaveClassifier(const Handle<DecisionTable> table, bool masked) const;
	// -N 08/08/13 Sets append mode on classifier_ write. Used with threaded computations and CLASSIFIER.SAVE
//	bool		SetAppend(bool* status, bool set) const { 
//				#pragma omp critical(appendmode)
//				{ 
//				*status = set; 
//				}
//				return true;
//				
//			}
//	bool		GetAppend() const {return append_;}
};

#endif
 
