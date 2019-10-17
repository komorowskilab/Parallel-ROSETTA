//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SERIALEXECUTOR_H__
#define __SERIALEXECUTOR_H__

#include <copyright.h>

#include <kernel/algorithms/executor.h>
//#include <kernel/algorithms/serialexecutorloop.h>
#include <kernel/structures/rules.h>


// -N 12/06/13
//#include <kernel/algorithms/batchclassifier.h>


//-------------------------------------------------------------------
// Class.........: SerialExecutor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Executes a pipeline sequence of commands.
// Revisions.....:
//===================================================================

class SerialExecutor : public Executor {
protected:

	//- Parameters.....................................................
	String			logfile_; // Location of log file.
	
	Handle<Rules>		SErules_; //For rulebased classifications. -N
	// -N 04/06/13
	std::string		log_; //For passing the log details back to SerialExecutorLoop and writing it in sequence. -N

protected:

  //- Methods inherited from Executor................................
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const;

  //- New virtual methods............................................
	virtual bool        SetSpecialParameters(Algorithm &algorithm, const String &parameters) const;
	//New virtual for the SE rules system. -N
//	virtual bool        SetSpecialParameters(Algorithm &algorithm, const String &parameters, Handle<Rules> rules) const;
	
	//virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream, bool remove) const;
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream, bool remove, bool execute=true) const;
	//Copies an algorithm and returns a handle to the copy. -N
//	Handle<Algorithm>	CopyAlgorithm(Algorithm &algorithm) const;
	//Copy of the SerialExecutorLoop SetSpecialParameters. Used for inserting SErules. -N
	bool			SetSERules(Algorithm &algorithm, const String &parameters, Handle<Rules> ruleset) const;
	//- Access methods............................................
	//-N 04/06/13
	//std::string*	GetLog() const;
//	bool		AppendLog(std::string &log, std::string text);

	//virtual bool WritePreStatusToLog(const Algorithm &algorithm);
protected:

	//- Friend declaration (ugly hack).................................
	friend class SerialExecutorLoop;
	// and even uglier for consistency 
	friend class LeaveOneOutCVSerialExecutor;
	friend class PermLeaveOneOutCVSerialExecutor;
	friend class PermCVSerialExecutor;

public:

  //- Constructors/destructor........................................
  SerialExecutor();
	SerialExecutor(const Executor *in);
  virtual ~SerialExecutor();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm..............................
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual SerialExecutor 	*Clone();

	// -N 29/05/13
	//- For member instantiated rules.............................
	Handle<Rules>	GetRules() const {return SErules_;}
	bool	SetRules(Handle<Rules> rules) {SErules_ = rules; return true;}

};

#endif
 
