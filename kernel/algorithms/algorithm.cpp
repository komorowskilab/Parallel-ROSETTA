//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/algorithm.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/structure.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Algorithm::Algorithm() {
}

Algorithm::~Algorithm() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Algorithm, ALGORITHM, Identifier)

//-------------------------------------------------------------------
// Parameter manipulation.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns a parsable string describing the algorithm
//                 parameters.
//
//                 Default implementation, should be overloaded where
//                 appropriate.
// Comments......:
// Revisions.....:
//===================================================================

String
Algorithm::GetParameters() const {
	return String("");
}

//-------------------------------------------------------------------
// Method........: GetOutputFilenames
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns in-place all names of files that the
//                 algorithm has produced as side-effects if
//                 application went well.
// Comments......: Used for creating file wrappers in project tree,
// Revisions.....:
//===================================================================

bool
Algorithm::GetOutputFilenames(Vector(String) &filenames) const {
	filenames.erase(filenames.begin(), filenames.end());
	return true;
}

//-------------------------------------------------------------------
// Method........: SetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...: Parses the input string and sets the algorithm
//                 parameters.
// Comments......: Cannot use nested calls to String::GetToken.
// Revisions.....: Aš 971011: Added warn flag.
//===================================================================

bool
Algorithm::SetParameters(const String &parameters, bool warn) {

	String unhandled;
	String parameter;
	String separator  = Keyword::Separator();
	String assignment = Keyword::Assignment();

	
	// Remove space as an interdelimiting symbol.
	while (separator.Contains(' '))
		separator.Delete(' ');

	const char *interdelimiter = separator.GetBuffer();  // Delimiter between parameters.
	const char *intradelimiter = assignment.GetBuffer(); // Delimiter within a parameter.
	#pragma omp critical(GetToken)
	{
	bool first = true;
	bool done  = false;
	while (!done) {
	
		done = !parameters.GetToken(parameter, interdelimiter);
	
		// It could be that the interdelimiter was not found, i.e. there is one parameter only.
		if (done) {
			if (first)
				parameter = parameters;
			else
				break;
		}

		first = false;

		// Verify that the parameter token contains the intradelimiter character.
		// If it is simply a sequence of spaces, skip it.
		if (!parameter.Contains(*intradelimiter)) {
			parameter.Trim(' ');
			if (!parameter.IsEmpty()) {
				if (!unhandled.IsEmpty())
					unhandled += "\n";
				unhandled += parameter;
			}
			continue;
		}

		// Get index of intradelimiter character.
		int index = parameter.GetIndex(*intradelimiter);

		// Extract keyword and value from parameter token.
		String keyword = parameter.Before(index);
		String value   = parameter.After(index);
		// Strip leading/trailing spaces.
		keyword.Trim(' ');
		value.Trim(' ');

		// Allow the user's keyword to be in lower-case.  All keywords are (or should be) in upper-case.
		keyword.ToUppercase();

//		#pragma omp critical(parameters)
//		{ 		
			// Set the parameter.
			if (!SetParameter(keyword, value)) {
				if (!unhandled.IsEmpty())
					unhandled += "\n";
				unhandled += parameter;
			}
//		}
	}
	}
	// Any unhandled parameters?
	if (unhandled.IsEmpty())
		return true;

	// Don't issue a warning?
	if (!warn)
		return false;
	
	// Get the name of this algorithm.
	String name = IdHolder::GetClassname(GetId());
	
	// Give an error message.
	Message::Error("The following parameters were not handled by " + name + ":\n\t" + unhandled);
	
	return false;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a keyword and a value, sets the parameter
//                 specified by the keyword to the given value.
//
//                 Default implementation, should be overloaded where
//                 appropriate.
// Comments......:
// Revisions.....:
//===================================================================

bool
Algorithm::SetParameter(const String &/*keyword*/, const String &/*value*/) {
	return false;
}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Enables querying of the algorithm as to whether or
//                 not it is applicable to the given structure.
//
//                 Default implementation is conservative, just to be
//                 safe.  This method should (must) be overloaded.
// Comments......:
// Revisions.....:
//===================================================================

bool
Algorithm::IsApplicable(const Structure &/*structure*/, bool /*warn*/) const {
	return false;
}
