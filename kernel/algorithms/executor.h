//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include <copyright.h>
#include <iostream>
#include <kernel/algorithms/algorithm.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/decisiontable.h>

//-N 05/06/13
#ifdef _OPENMP
	#include "omp.h"
#endif
//-------------------------------------------------------------------
// Class.........: Executor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Executes a set of commands in a command script.
// Comments......:
// Revisions.....:
//===================================================================

class Executor : public Algorithm {
protected:

	//- Parameters.....................................................
	Id		output_;        // Type of output.
	String		commandfile_;   // Location of file with commands.
	String		logfile_;       // Location of log file.
	//-N 05/06/13
	mutable std::string	log_;		// Stores logging data of the execution process. -N
	//-N 01/08/13
	bool			save_;
	bool			exit_;
	bool			load_;
	String			classifierfile_;
//	static volatile int	currentThreads_;
//	static int		maxThreads_;
protected:
	#ifdef _OPENMP
		typedef Map2(int, std::string) ISMap;
		mutable ISMap	logmap_;
	#endif

protected:

	//- Protected methods..............................................
	virtual bool        LoadCommands(const String &filename, Algorithm::Handles &algorithms, Vector(String) &parameters) const;
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters) const;
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const = 0;
	virtual bool        SaveLogHeader(ofstream &stream) const;
	virtual bool        SaveLogEntry(ofstream &stream, const Algorithm &algorithm) const;
	virtual bool        SaveLogEntry(ofstream &stream, const Structure &structure) const;

	//-N 04/06/13
	//- Threaded writing methods. Used with the Executor log and passed back to SerialExecutorLoop.
	virtual bool		WriteHeaderToLog() const;
	virtual bool		WritePreStatusToLog(const Algorithm &algorithm) const; 
	virtual bool		WritePostStatusToLog(const Structure &structure) const;
	virtual bool		SaveClassifier(const Handle<DecisionTable> table, bool masked) const;
	virtual Handle<Rules>	LoadClassifier(const Handle<DecisionTable> table, bool masked) const;
	bool 			FileExists() const {ifstream stream(GetClassifierSource().GetString().c_str()); return stream;}	
	bool			RemoveExistingClassifierFile() const;
public:

  //- Constructors/destructor........................................
  Executor();
  virtual ~Executor();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String      GetParameters() const;
	virtual bool        GetOutputFilenames(Vector(String) &filenames) const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual bool        IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure  *Apply(Structure &structure) const;
	virtual Executor 	*Clone() = 0;

	//- Local methods..................................................
	Id                  GetOutputType() const {return output_;}
	bool                SetOutputType(Id output) {output_ = output; return true;}

	const String       &GetCommandFilename() const {return commandfile_;}
	bool                SetCommandFilename(const String &commandfile) {commandfile_ = commandfile; return true;}

	const String       &GetLogFilename() const {return logfile_;}
	bool                SetLogFilename(const String &logfile) {logfile_ = logfile; return true;}

	bool		SetLoad(String value) {load_ = true; classifierfile_ = value; return true;}
	bool		SetSave(String value) {save_ = true; classifierfile_ = value; return true;}
	bool		SetSaveAndExit(String value) {save_ = value.GetBoolean(); exit_ = value.GetBoolean(); return true;}
	const String 	GetClassifierSource() const {return classifierfile_;}
	bool		GetLoadClassifier() const {return load_;}
	bool		GetSaveClassifier() const {return save_;}
/*
	// -N 02/09/13 Thread management
//	bool		SetThreadingDefaults() const;
	bool		SetMaxThreads(unsigned int value) {maxThreads_ = value; return true;} 
	static int	GetMaxThreads();
	bool		SetDynamicThreads(bool value) const;
	bool		GetDynamicThreads() const {return omp_get_dynamic();}
	bool		SetNestedThreads(bool value) const;
	bool		GetNestedThreads() const {return omp_get_nested();}
	static bool	GetInParallel() {return omp_in_parallel();}
	static bool	GetNested() {return omp_get_nested();}
	static int	GetNumThreadsActive();	
	bool		AddThreadToCounter() const;
	bool		RemoveThreadFromCounter();
*/
	//-N 05/06/13 log management........................................
	#ifdef _OPENMP
//		void		InitLogMap() const {ISMap logmap_;}
		bool		AppendLog(std::string input) const {AppendLog2(input, omp_get_thread_num());return true;}
		bool 		AppendLog2(std::string input, unsigned int i) const {
					//Incase the element already exists, take it out, append the strings,
					//and then proceed like normal. -N
					if (logmap_.count(i) == 1) {
						std::string log = logmap_.find(i)->second;
						logmap_.erase(i);
						input = log + input;
					}
					std::pair<int, std::string> p (i, input);
					logmap_.insert(p);
					//cout << logmap_.find(i)->second;
					return true;}
		std::string	GetLog() const {return logmap_.find(omp_get_thread_num())->second;}
		void		InitLog() const {logmap_.erase(omp_get_thread_num());}
//		void		InitLog() const {log_ = "";}
//		bool		AppendLog(std::string input) const {log_ += input; return true;}
//		std::string	GetLog() const {std::string ret = log_; return ret;}

	#endif

};

#endif
 
