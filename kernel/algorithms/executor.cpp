//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <cstdlib>

#include <kernel/algorithms/executor.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/systemkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>
#include "omp.h"
#include <sstream>
#include <kernel/utilities/threadcontrol.h>
//-------------------------------------------------------------------
// Methods for class Executor.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Executor::Executor() {
	SetOutputType(Undefined::Id());
	SetCommandFilename(Undefined::String());
	SetLogFilename(Undefined::String());
	load_ = false;
	save_ = false;
}

Executor::~Executor() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Executor, EXECUTOR, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Executor::GetParameters() const {

	String parameters;

	// Output type.
	parameters += Keyword::Output();
	parameters += Keyword::Assignment();
	parameters += IdHolder::GetClassname(GetOutputType());

	parameters += Keyword::Separator();

	// Filename (commands).
	parameters += Keyword::Filename() + Keyword::Dot() + Keyword::Commands();
	parameters += Keyword::Assignment();
	parameters += GetCommandFilename();

	parameters += Keyword::Separator();

	// Filename (log).
	parameters += Keyword::Filename() + Keyword::Dot() + Keyword::Log();
	parameters += Keyword::Assignment();
	parameters += GetLogFilename();

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
Executor::GetOutputFilenames(Vector(String) &filenames) const {

	if (!Algorithm::GetOutputFilenames(filenames))
		return false;

	filenames.push_back(GetLogFilename());

	return true;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Executor::SetParameter(const String &keyword, const String &value) {

	// Output type.
	if (keyword == Keyword::Output())
		return SetOutputType(IdHolder::GetId(value));

	// Filename (commands).
	if (keyword == Keyword::Filename() + Keyword::Dot() + Keyword::Commands())
		return SetCommandFilename(value);

	// Filename (commands). Backwards compatibility.
	if (keyword == Keyword::Commands())
		return SetCommandFilename(value);

	// Filename (log).
	if (keyword == Keyword::Filename() + Keyword::Dot() + Keyword::Log())
		return SetLogFilename(value);

	// Filename (log). Backwards compatibility.
	if (keyword == Keyword::Log())
		return SetLogFilename(value);
	
	//-N 01/08/13 For saving and loading classifiers.
	if (keyword == Keyword::Classifier() + Keyword::Dot() + Keyword::Save())
		return SetSave(value);
	
	if (keyword == Keyword::Classifier() + Keyword::Dot() + Keyword::SaveAndExit())
		return SetSaveAndExit(value);

	if (keyword == Keyword::Classifier() + Keyword::Dot() + Keyword::Load())
		return SetLoad(value);
	// -N 02/09/13 Sets the max number of threads available to the program.
	if (keyword == Keyword::MaxThreads())
		return ThreadControl::SetMaxThreads(value.GetInteger());

	if (keyword == Keyword::DynamicThreads())
		return ThreadControl::SetDynamicThreads(value.GetBoolean());

	if (keyword == Keyword::NestedThreads())
		return ThreadControl::SetNestedThreads(value.GetBoolean());


	return Algorithm::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Executor::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(STRUCTURE);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Executor::Apply(Structure &structure) const {

	// Check applicability.
	if (!IsApplicable(structure))
		return NULL;

	// Command representation.
	Algorithm::Handles algorithms;
	Vector(String)     parameters;


	// Load commands.
	if (!LoadCommands(GetCommandFilename(), algorithms, parameters)) {
		Message::Error("Error reading commands from " + GetCommandFilename() + ".");
		return NULL;
	}

	// Verify that the command set is not empty.
	if (algorithms.empty()) {
		Message::Error("No commands read from " + GetCommandFilename() + ".");
		return NULL;
	}

	// Execute commands.
	return ExecuteCommands(structure, algorithms, parameters);

}

/*
//-------------------------------------------------------------------
// Method........: SetThreadingDefaults
// Author........: Nicholas Baltzer
// Date..........: 05/09/13
// Description...: Sets the threading parameters to their default state.
//                 This is necessary since openmp defaults are implementation
//		   specific, and may cause havoc on different systems.
// Comments......:
// Revisions.....: 
//===================================================================

bool
Executor::SetThreadingDefaults() const {

	if (!SetMaxThreads(omp_get_num_procs()))
		return false;
	if (!SetNestedThreads(false))
		return false;
	if (!SetDynamicThreads(true))
		return false;
	return true;

}
*/
//-------------------------------------------------------------------
// New (virtual) methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: LoadCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the commands (algorithms) and
//                 their parameters.
// Comments......:
// Revisions.....: -N 08/06/13
//			Added checks for spacing and non-empty lines.
//===================================================================

bool
Executor::LoadCommands(const String &filename, Algorithm::Handles &algorithms, Vector(String) &parameters) const {

	ifstream stream;

	if (!IOKit::Open(stream, filename)) {
		Message::Error("Failed to open " + filename + ".");
		return false;
	}

	// Clear inputs (in-place outputs).
	algorithms.erase(algorithms.begin(), algorithms.end());
	parameters.erase(parameters.begin(), parameters.end());

	String line;               // The line we read.
	int    line_no = 0;        // The current line number in the file.
	bool   is_command = true;  // Flag indicating how to interpret the line.


	bool error = false; 
	// Read all lines.
		while (IOKit::LoadLine(stream, line, &line_no) && !error) {
			if (!line.IsEmpty()) { //Checking for \s and \t lines. -N
				// Interpret line as a command or a parameter?
				if (is_command) {
					Id id = IdHolder::GetId(line);
					if (id == Undefined::Id()) {
						Message::Error("Unknown algorithm (" + line + ") on line " + String::Format(line_no) + ".");
						//return false;
						error = true;
					}
					Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(id);
					if (algorithm == NULL) {
						Message::Error("Algorithm " + line + " on line " + String::Format(line_no) + " not installed.");
						//return false;
						error = true;
					}
					algorithms.push_back(algorithm);
				}
				else {
					line.Trim("{}");
					parameters.push_back(line);
				}
	
				// Flip flag.
				is_command = !is_command;
			}
		}
	//return true;
	return !error;

}



/*
//-------------------------------------------------------------------
// Method........: SetMaxThreads
// Author........: Nicholas Baltzer
// Date..........: 02/09/13
// Description...: Sets the max number of threads that the program may use.
// Comments......:
// Revisions.....: 03/09/13 -N Changed from omp_set_num_threads() to setting
//			the environment variable instead. This way it can be 
//			overwritten for specific cases, but remain for general
//			cases. It should also help with nesting. 
//===================================================================
bool
Executor::SetMaxThreads(unsigned int value) {
#ifdef _OPENMP
	//There is no reason to allow more threads than twice the available processor cores.
	//Since dynamic threading can now be turned off, it's even more important with damage control.
	if (value > omp_get_num_procs())
		value = omp_get_num_procs();
//	char *number;
//	sprintf(number, "%d", value);
	std::stringstream ss;
	ss << "OMP_THREAD_LIMIT=" << value;
	std::string threadmax = ss.str();
//	threadmax.append(number);
//	cout << threadmax << endl;
	return !putenv(const_cast<char*>(threadmax.c_str()));

//	omp_set_num_threads(value);
#endif
	return true;
}
*/
/*
//-------------------------------------------------------------------
// Method........: SetDynamicThreads
// Author........: Nicholas Baltzer
// Date..........: 03/09/13
// Description...: Sets dynamic mode for threads. Default is on.
// Comments......:
// Revisions.....: 
//===================================================================
bool
Executor::SetDynamicThreads(bool value) const {
#ifdef _OPENMP
	omp_set_dynamic(value);
#endif
	return true;
}
//-------------------------------------------------------------------
// Method........: SetNestedThreads
// Author........: Nicholas Baltzer
// Date..........: 03/09/13
// Description...: Sets nested mode for threads. Default is off.
// Comments......:
// Revisions.....: 
//===================================================================
bool
Executor::SetNestedThreads(bool value) const {
#ifdef _OPENMP
	omp_set_nested(value);
#endif
	return true;
}
*/
//-------------------------------------------------------------------
// Method........: ExecuteCommands
// Author........: Aleksander šhrn
// Date..........:
// Description...: Opens log file and calls ExecuteCommands.
// Comments......:
// Revisions.....: 21/08/13 -N Added option to load ruleset from file.
//			Given that this loading mechanism is universal (i.e. 
//			does not require knowledge of the internal decision table state)
//			feature names are used to map into the dictionary. This does not work with 
//			non-unique feature names.
//===================================================================

Structure *
Executor::ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters) const {

	ofstream stream;

	if (!IOKit::Open(stream, GetLogFilename())) {
		Message::Error("Failed to open " + GetLogFilename() + " for output.");
		return NULL;
	}

	// Save log header.
	if (!SaveLogHeader(stream)) {
		Message::Error("Failed to save log file header.");
		return NULL;
	}

	Handle<Structure> output; 
	// Execute commands.


	if (GetLoadClassifier()) {
		Message::Warning("Loading ruleset from file. Non-unique feature names will be removed from computations.");
		Handle<DecisionTable> table = dynamic_cast(DecisionTable *, structure.Duplicate());
		Handle<Rules> rules = LoadClassifier(table, true);
		structure.AppendChild(rules.GetPointer());
	}
	if (GetSaveClassifier())
		RemoveExistingClassifierFile();

	output = ExecuteCommands(structure, algorithms, parameters, stream);
	IOKit::Close(stream);

	return output.Release();

}

//-------------------------------------------------------------------
// Method........: SaveLogHeader
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called before execution.
// Revisions.....:
//===================================================================

bool
Executor::SaveLogHeader(ofstream &stream) const {

	stream << "% Output from ROSETTA, " << SystemKit::GetUser() << " " << SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " << IdHolder::GetClassname(GetId()) << endl;
	stream << "% {" << GetParameters() << "}" << endl << endl;

	return true;

}
//-------------------------------------------------------------------
// Method........: WriteHeaderToLog
// Author........: Nicholas Baltzer
// Date..........: 05/06/13
// Description...: Saves the header to the Executor log. Not
//		   necessary right now, but if the threading moves
//		   one step up or the methodology changes...
// Comments......: Called before execution.
// Revisions.....:
//===================================================================

bool
Executor::WriteHeaderToLog() const {

	std::string log = "% Output from ROSETTA, " + SystemKit::GetUser().GetString() + " " \
			+ SystemKit::GetTimestamp().GetString() + "\n" \
			+ "%" + "\n" + "% " + IdHolder::GetClassname(GetId()).GetString() + "\n% {" \
			+ GetParameters().GetString() + "}\n\n";
	AppendLog(log);
	return true;

}

//-------------------------------------------------------------------
// Method........: WritePreStatusToLog
// Author........: Nicholas Baltzer
// Date..........: 05/06/13
// Description...: Saves the pre-execution entry to the Executor
//		   log.
// Comments......: Called before execution step.
// Revisions.....:
//===================================================================

bool
Executor::WritePreStatusToLog(const Algorithm &algorithm) const {


	std::string timestamp = SystemKit::GetTimestamp().GetString();
	std::string indent2(timestamp.length(), ' ');
	std::string indent(indent2);

	AppendLog(timestamp + " Executing " + IdHolder::GetClassname(algorithm.GetId()).GetString() + "..." + "\n" \
		+ indent + " Parameters = {" + algorithm.GetParameters().GetString() + "}" + "\n");

	return true;

}
//-------------------------------------------------------------------
// Method........: WritePostStatusToLog
// Author........: Nicholas Baltzer
// Date..........: 05/06/13
// Description...: Saves the post-execution entry to the Executor
//		   log.
// Comments......: Called after execution step.
// Revisions.....:
//===================================================================

bool
Executor::WritePostStatusToLog(const Structure &structure) const {

	std::string timestamp = SystemKit::GetTimestamp().GetString();
	std::string indent(timestamp.length(), ' ');

	std::string log = timestamp + " Output = " + IdHolder::GetClassname(structure.GetId()).GetString() + "\n";

	if (structure.HasMembers())
		log += indent + " #Members = " + String::Format(structure.GetNoStructures()).GetString() + "\n";

	if (structure.HasChildren())
		log += indent + " #Children = " + String::Format(structure.GetNoChildren()).GetString() + "\n";

	AppendLog(log);

	return true;

}
//-------------------------------------------------------------------
// Method........: SaveLogEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called before execution step.
// Revisions.....:
//===================================================================

bool
Executor::SaveLogEntry(ofstream &stream, const Algorithm &algorithm) const {

	String timestamp = SystemKit::GetTimestamp();
	String indent(' ', timestamp.GetLength());

	stream << timestamp << " Executing " << IdHolder::GetClassname(algorithm.GetId()) << "..." << endl;
	stream << indent    << " Parameters = {" << algorithm.GetParameters() << "}" << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: SaveLogEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called after execution step.
// Revisions.....:
//===================================================================

bool
Executor::SaveLogEntry(ofstream &stream, const Structure &structure) const {

	String timestamp = SystemKit::GetTimestamp();
	String indent(' ', timestamp.GetLength());

	stream << timestamp << " Output = " << IdHolder::GetClassname(structure.GetId()) << endl;

	if (structure.HasMembers())
		stream << indent  << " #Members = "  << structure.GetNoStructures() << endl;

	if (structure.HasChildren())
		stream << indent  << " #Children = " << structure.GetNoChildren() << endl;

	return true;

}






//-------------------------------------------------------------------
// Method........: SaveClassifier
// Author........: Nicholas Baltzer
// Date..........: 06/08/13
// Description...: Saves a ruleset after mapping it. Rules are saved to the file name specified by
//			GetClassifierSource(). Uses the dictionary from the supplied decisiontable.
// Comments......: 
// Revisions.....:
//===================================================================

bool
Executor::SaveClassifier(const Handle<DecisionTable> table, bool masked) const {

        //Should the rules be saved?
//        if (!GetSaveClassifier())
//              return true;

        Message message;


        #pragma omp critical(ruleset)
        {
	        message.Notify("Saving ruleset...");
	        //Grab the rules from the decision table.
	        ofstream stream;
	        //Use the supplied filename for storage.
	
	        if (!IOKit::Open(stream, GetClassifierSource(), true)) {
			Message::Error("Failed to open file " + GetClassifierSource());
		}
		else {
	                Handle<Rules> ruleset = table->GetRuleset();

	        	        for (int i = 0; i < ruleset->GetNoRules(); i++) {
                        //Save the rule to the file. 
	                	        ruleset->GetRule(i)->SaveMappedRule(stream, table, masked);
	               		}
		} 
//      bool append = true;
//      SetAppend(appendClassifierFile, true);
        //append_ = true;
        //And done.
	        IOKit::Close(stream);
        }
        return true;
}

//-------------------------------------------------------------------
// Method........: LoadClassifier
// Author........: Nicholas Baltzer
// Date..........: 06/08/13
// Description...: Loads a ruleset from the supplied classifier file. The rules
//                      are mapped through the decisiontable dictionary to properly
//                      set their index values.
// Comments......: 
// Revisions.....:
//===================================================================

Handle<Rules>
Executor::LoadClassifier(Handle<DecisionTable> table, bool masked) const {

        //Should rules be loaded?
        if (!GetLoadClassifier())
                return NULL;
        //new ruleset.
        Handle<Rules> classifierRules = Creator::Rules();

        //Load them.
        if (!classifierRules->LoadRulesetFromFile(GetClassifierSource(), table, masked))
                Message::Error("SerialExecutorLoop::LoadClassifier -> Failed to build ruleset from file");
        //If we have atleast one rule in the set, it's a success.
        return classifierRules->GetNoRules() > 0 ? classifierRules : NULL;


}



//-------------------------------------------------------------------
// Method........: RemoveExistingClassifierFile
// Author........: Nicholas Baltzer
// Date..........: 12/08/13
// Description...: Helper to SaveClassifier. Checks for an existing file with the name from
//			GetClassifierSource(). If such a file is found, it is deleted. This way,
//			writing to the classifier file can be set to always append, helping out
//			in the case of threaded writes and removing the need for pre-write buffering
//			in both serial and parallel versions.
// Comments......: 
// Revisions.....:
//===================================================================
bool
Executor::RemoveExistingClassifierFile() const {
	if (GetSaveClassifier()) {
		if (FileExists())
			return !remove(GetClassifierSource().GetString().c_str());
		else
			return true;
	}
	return true;
}
/*
bool
Executor::AddThreadToCounter() const {
	static int currentThreads;
	if (omp_get_thread_num() != 0) {
		#pragma omp atomic
		currentThreads++;
	}
	return true;
}
bool
Executor::RemoveThreadFromCounter() {
	static int currentThreads;
	if (omp_get_thread_num() != 0) {
		#pragma omp atomic
		currentThreads--;
	}
	return true;
}


int
Executor::GetNumThreadsActive() {
	static int currentThreads;
	return currentThreads;
}

int
Executor::GetMaxThreads() {
	int x = maxThreads_;
	return x;
}
*/
