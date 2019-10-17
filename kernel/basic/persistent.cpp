//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960328
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/persistent.h>
#include <kernel/basic/message.h>

#include <kernel/utilities/iokit.h>

#include <kernel/system/fstream.h>

//-N 08/05/13
#include <string> 


using namespace std;
//-------------------------------------------------------------------
// Methods for class Persistent.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Persistent::Persistent() {
}

Persistent::~Persistent() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Persistent, PERSISTENT, Identifier)

//-------------------------------------------------------------------
// Default load/save methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...: Loads stuff used for file type identification
//                 purposes.
// Comments......:
// Revisions.....: Aš 990209: Removed type verification since this
//                            was overly restrictive when different
//                            (mutually exclusive) installed components
//                            share the same file format.
//
//		   -N  130518: Changed comparison to string == string
//				rather than String == string.
//				(Note capital S).
//
//
//===================================================================

bool
Persistent::Load(ifstream &stream) {

	String header;
	String type;

	// Load and verify application name header.
	if (!IOKit::Load(stream, header))
		return false;

	header.ToUppercase();

	string header_match = header.GetBuffer(); // -N

//	if (header != "ROSETTA")
//		return false;

	//After String class modernization, some changes are needed. -N
	if (header_match != "ROSETTA")
		return false;

	

	// Load and verify type of object.
	if (!IOKit::Load(stream, type))
		return false;

	/*
	if (IdHolder::GetId(type) == Undefined::Id())
		return false;
	*/

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...: Saves stuff used for file type identification
//                 purposes.
// Comments......:
// Revisions.....:
//===================================================================

bool
Persistent::Save(ofstream &stream) const {

	const String header = "Rosetta";
	const String type   = IdHolder::GetClassname(GetId());

//	cout << "Type: |" << type << "|"<< endl;
	// Save application name header.
	if (!IOKit::Save(stream, header) || !IOKit::Save(stream, '\n'))
		return false;

	// Save type of object.
	if (!IOKit::Save(stream, type) || !IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Persistent::Load(const String &filename) {

	ifstream stream;

	if (!IOKit::Open(stream, filename)) {
		Message::Error("Unable to open " + filename + " for input.");
		return false;
	}

	return Load(stream);

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Persistent::Save(const String &filename) const {

	ofstream stream;

	if (!IOKit::Open(stream, filename)) {
		Message::Error("Unable to open " + filename + " for output.");
		return false;
	}

	return Save(stream);

}

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...:
// Comments......: Should be overloaded by objects with default
//                 filenames.
// Revisions.....:
//===================================================================

bool
Persistent::Load() {
	return false;
}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Comments......: Should be overloaded by objects with default
//                 filenames.
// Revisions.....:
//===================================================================

bool
Persistent::Save() const {
	return false;
}

 
