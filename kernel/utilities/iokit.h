//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __IOKIT_H__
#define __IOKIT_H__

#include <copyright.h>

#include <kernel/system/stdlib.h>

#include <kernel/basic/types.h>
#include <kernel/system/iostream.h>
#include <kernel/system/fstream.h>
//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class String;

//-------------------------------------------------------------------
// Class.........: IOKit
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class IOKit {
protected:

	//- Static variables...............................................
	static unsigned int  buffersize_; // The size of the buffer.
	static char         *buffer_;     // The buffer itself.

protected:

	//- Buffer methods.................................................
	static bool          AllocateBuffer(unsigned int buffersize);

public:

	//- Buffer methods.................................................
	static unsigned int  GetBuffersize() {return buffersize_;}
	static bool          SetBuffersize(unsigned int buffersize) {return AllocateBuffer(buffersize);}

	//- Initialization.................................................
	static bool          Open(ifstream &stream, const String &filename);
	static bool          Open(ofstream &stream, const String &filename, bool append = false);

	//- Cleaning up....................................................
	static bool          Close(ifstream &stream);
	static bool          Close(ofstream &stream);

  //- Load from stream...............................................
  static bool          Load(ifstream &stream, String &string, bool delimiters = true);
  static bool          Load(ifstream &stream, int &i, bool delimiters = true);
#if 1
  static bool          Load(ifstream &stream, bool &b, bool delimiters = true);
#endif
  static bool          Load(ifstream &stream, float &f, bool delimiters = true);
  static bool          Load(ifstream &stream, char &c, bool delimiters = true);

	static bool          LoadLine(ifstream &stream, String &line, int *counter = NULL, char symbol = '%', bool trim = true);

  //- Save to stream.................................................
  static bool          Save(ofstream &stream, const String &string, bool delimiters = true);
  static bool          Save(ofstream &stream, int i, bool delimiters = true);
  static bool          Save(ofstream &stream, unsigned int i, bool delimiters = true);
#if 1
  static bool          Save(ofstream &stream, bool b, bool delimiters = true);
#endif
  static bool          Save(ofstream &stream, float f, bool delimiters = true);
  static bool          Save(ofstream &stream, char c, bool delimiters = true);

	//- Querying.......................................................
	static bool          Exists(const String &filename);
	static bool          IsEOF(ifstream &stream);
	static bool          IsOpen(fstream &stream);

};

#endif 
