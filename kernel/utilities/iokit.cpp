//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/delimiter.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/string.h>
#include <string>
//-------------------------------------------------------------------
// Static stuff.
//===================================================================

unsigned int  IOKit::buffersize_ = 1024000;
char         *IOKit::buffer_     = NULL;

//-------------------------------------------------------------------
// Methods for class IOKit.
//===================================================================

//-------------------------------------------------------------------
// Method........: AllocateBuffer
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: More optimal reuse of allocated buffer is possible.
// Revisions.....:
//===================================================================

bool
IOKit::AllocateBuffer(unsigned int buffersize) {

	// Reuse current buffer?
	if ((buffer_ != NULL) && (buffersize == buffersize_))
		return true;

	buffersize_ = buffersize;

	// Deallocate current buffer?
	if (buffer_ != NULL)
		delete [] buffer_;

	// Allocate new buffer.
	buffer_ = new char[buffersize_];

	return (buffer_ != NULL);

}

//-------------------------------------------------------------------
// Method........: Open
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Open(ifstream &stream, const String &filename) {

	#pragma omp critical(stl_open)
	{
		stream.open(filename.GetBuffer(), ios::in);
	}
	return (1 == stream.is_open());
	
}

//-------------------------------------------------------------------
// Method........: Open
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Open(ofstream &stream, const String &filename, bool append) {

	#pragma omp critical(stl_open)
	{
		if (!append)
			stream.open(filename.GetBuffer());
		else
			stream.open(filename.GetBuffer(), ios::out | ios::app);
	}
	return (1 == stream.is_open());

}

//-------------------------------------------------------------------
// Method........: Close
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Close(ifstream &stream) {
	if (stream.is_open())	// sus added
		stream.close();

	return true;

}

//-------------------------------------------------------------------
// Method........: Close
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Close(ofstream &stream) {
	if (stream.is_open()) // sus added
		stream.close();

	return true;
}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 970511 Aš: Delete contained delimiter characters.
//===================================================================

bool
IOKit::Save(ofstream &stream, const String &string, bool delimiters) {

	if (!delimiters) {
		stream << string;
		return true;
	}

	String tmp(string);

	// Delete contained delimiter characters, if any.
	while (tmp.Contains(Delimiter::Text()))
		tmp.Delete(Delimiter::Text());

	stream << Delimiter::Text() << tmp << Delimiter::Text() << Delimiter::Field();

  return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Save(ofstream &stream, unsigned int i, bool delimiters) {

	if (delimiters)
		stream << i << Delimiter::Field();
	else
		stream << i;

  return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Save(ofstream &stream, int i, bool delimiters) {

	if (delimiters)
		stream << i << Delimiter::Field();
	else
		stream << i;

  return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

#if 1
bool
IOKit::Save(ofstream &stream, bool b, bool delimiters) {

	if (delimiters)
		stream << b << Delimiter::Field();
	else
		stream << b;

  return true;

}
#endif

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Save(ofstream &stream, float f, bool delimiters) {

	if (delimiters)
		stream << f << Delimiter::Field();
	else
		stream << f;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Save(ofstream &stream, char c, bool /*delimiters*/) {

  stream << c;

	return true;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn, Lukasz Ligowski
// Date..........:
// Description...:
// Comments......: Generalize method to handle lines longer than
//                 the size of the static buffer...
//
// Revisions.....: Aš 000221: Made buffer a static class member, stream.get(buf,buflen,delimiter) freaks out when delimiter is first character in file it reads. Weird. 
//===================================================================

bool
IOKit::Load(ifstream &stream, String &string, bool delimiters) {
/*
	// Allocate buffer?
	if ((buffer_ == NULL) && !AllocateBuffer(buffersize_))
		return false;

	// Have we reached the end-of-file marker?
	if (stream.eof())
		return false;

	// Fill buffer from stream.
	// Looks messy :( but works :)
	if (delimiters) {
		string = "";
		char ch;
		bool sw = true;
		while (sw && stream.good()){
			ch=stream.get();
			if (ch == '"'){
				while(stream.good()){
					ch = stream.get();
					if (ch=='"'){
						sw = false;
						break;
					}
					string+=ch;
				};
			};
		};
	}
	else {
		stream.getline(buffer_, buffersize_);
  		string = buffer_;
	}

	// Copy the buffer into the string object.

	return true;
*/
	//Let's drop the buffer. -N
	
	
	//If we hit the end of file, return false.
	if (stream.eof())
		return false;

	//Need a temp std::string for this.
	std::string sLine;

	//If we're using " as delimiter.
	if (delimiters) {
		//Grab whatever is leading up to a ". Most likely this will only contain the delimiter.
		std::getline(stream, sLine, '"');
		//Now that we are starting right after a ", grab everything until the next ". (---"Grab this"--)
		std::getline(stream, sLine, '"');
	}
	else {
		//No delimiters.Grab everything up until the next \n.
		std::getline(stream, sLine);

	}	
	//Copy the temp string into the provided String.
	string = sLine;
	//We are happy.
	return true;
}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Load(ifstream &stream, int &i, bool /*delimiters*/) {

  stream >> i;

  return true;
}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

#if 1
bool
IOKit::Load(ifstream &stream, bool &b, bool /*delimiters*/) {

	int i;

  	stream >> i;

	if (i == 0)
		b = false;
	else
		b = true;

  return true;

}
#endif

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Load(ifstream &stream, float &f, bool /*delimiters*/) {

  stream >> f;
  stream.ignore();

  return true;

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Load(ifstream &stream, char &c, bool /*delimiters*/) {

	stream >> c;

	return true;

}

//-------------------------------------------------------------------
// Method........: LoadLine
// Author........: Aleksander šhrn
// Date..........:
// Description...: Loads the whole next non-empty line from the stream.
//                 A counter can be incremented, if supplied. Lines
//                 starting with a specified symbol are skipped.
//                 Leading/trailing whitespace is trimmed away, if
//                 specified.
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::LoadLine(ifstream &stream, String &line, int *counter, char symbol, bool trim) {

	bool delimiters = false;

  // Skip blank lines and comment lines.
  do {
    if (!Load(stream, line, delimiters))
      return false;
		if (counter != NULL)
			(*counter)++;
		if (trim)
			line.Trim(" \t");
  }	while (line.IsEmpty() || line.First() == symbol);

  return true;

}

//-------------------------------------------------------------------
// Method........: Exists
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::Exists(const String &filename) {

	ifstream stream;

	return Open(stream, filename);

}

//-------------------------------------------------------------------
// Method........: IsEOF
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::IsEOF(ifstream &stream) {
	return (1 == stream.eof());
}

//-------------------------------------------------------------------
// Method........: IsOpen
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IOKit::IsOpen(fstream &stream) {
	return (1 == stream.is_open());
}
 
