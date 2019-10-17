//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/string.h>
#include <kernel/basic/macros.h>

#include <kernel/system/stdlib.h>
#include <kernel/system/stdio.h>
#include <kernel/system/math.h>
#include <kernel/system/iostream.h>
#include <kernel/system/ctype.h>
#include <sstream>
#include <string> //Attempting to replace the local string variant, and use it as a wrapper instead. -N
#include <stdexcept> //Exceptions that should be caught. -N

using namespace std;

//-------------------------------------------------------------------
// Static stuff (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticCopy
// Author........: Aleksander šhrn
// Date..........:
// Description...: Copies n bytes and null-terminates.
// Comments......:
// Revisions.....:
//===================================================================

static void
StaticCopy(const char *source, char *destination, int n) {

	if (source == destination) {
    destination[n] = '\0';
		return;
	}

	while (--n >= 0)
		*destination++ = *source++;
	*destination = '\0';

}

//-------------------------------------------------------------------
// Methods for class String.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String::String() {
//  representation_ = new Representation();
	string representation_;
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String::String(const char *buffer) {

	// Create a new representation object.
  //representation_ = new Representation();
//	representation_ = ""; //Replaced with empty string. -N

	// Check to avoid trouble.
//  if (buffer == NULL)
//    buffer = "";

	// Allocate buffer.
//	representation_->size_   = strlen(buffer) + 1;
//	representation_->buffer_ = new char[representation_->size_];

	// Copy the buffer.
//  strcpy(representation_->buffer_, buffer);
	representation_ = buffer; //Removed all but strcpy. -N
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String::String(const String &in) {

	// Share the input string's representation object. This makes assignments cheap!
//	in.representation_->reference_count_++;
//	representation_ = in.representation_;

	representation_ = in.representation_; //Sharing of representation objects is a big no no in threaded environments. Change to copy. -N
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String::String(char c, int length) {
/*
	if (length < 0)
		length = 0;

	// Create a buffer of the specified length.
  representation_          = new Representation();
  representation_->size_   = length + 1;
  representation_->buffer_ = new char[representation_->size_];

	int i;

	// Fill the buffer with the specified character.
  for (i = 0; i < length; i++)
    representation_->buffer_[i] = c;
  representation_->buffer_[length] = '\0';
*/
	string buffer(length, c);
	representation_ = buffer; // Need none of this for a dynamic allocation. -N
	//representation_.resize(length, c); //Resize the empty string and fill with c. -N
//	cout << ":|"<<representation_<<"|"<<endl;
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Nicholas Baltzer
// Date..........: 29-05-15
// Description...: Adds a direct std::string->String constructor.
// Comments......: Added for fixing an issue with GetConditionValue().
// Revisions.....:
//===================================================================

String::String(const std::string& sIn) {
	representation_ = sIn;

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String::~String() {
	// Is this the last string using this representation object?
//	if (representation_ && --representation_->reference_count_ == 0) {
//		delete [] representation_->buffer_;
//		delete representation_;
//	}
//No need as there is no more sharing being done. -N
}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........: 980703
// Description...: Reserves space so that allocations can be avoided.
// Comments......: Intended for expert use only.
//
//                 Note that the following won't work as intended:
//
//                    String s;
//                    s.Reserve(1000);
//                    s =  "f = ";
//                    s += "(w + x)";
//                    s += "(x + y)";
//                    s += "(y + z)";
//                    ...
//
//                 What happens is that the reserved buffer gets
//                 scrapped in the third line by the assignment
//                 operator '='! To achieve the intended effect of
//                 Reserve, do the following instead:
//
//                    String s;
//                    s.Reserve(1000);
//                    s += "f = ";
//                    s += "(w + x)";
//                    s += "(x + y)";
//                    s += "(y + z)";
//                    ...
//
//                 Consider more optimal reallocation.
// Revisions.....:
//===================================================================

bool
String::Reserve(int length) {
/*
	if (length < 0)
		length = 0;

	if (representation_ == NULL)
		representation_ = new Representation();

	// Already space enough?
	if (representation_->size_ >= length + 1)
		return true;

	// Check current reference count, disconnect if needed.
  if (representation_->reference_count_ > 1) {
  	#pragma omp atomic
    representation_->reference_count_--;
		char *buffer             = representation_->buffer_;
    representation_          = new Representation();
		representation_->size_   = length + 1;
		representation_->buffer_ = new char[representation_->size_];
		strcpy(representation_->buffer_, buffer);
  }
  else {
		representation_->size_   = length + 1;
		char *newbuffer          = new char[representation_->size_];
		if (representation_->buffer_ != NULL) {
			strcpy(newbuffer, representation_->buffer_);
			delete [] representation_->buffer_;
		}
		else {
			strcpy(newbuffer, "");
		}
		representation_->buffer_ = newbuffer;
	}
*/ //Don't bother. Use the inbuilt reserve function instead. -N
	representation_.reserve(length);
	return true;

}

//-------------------------------------------------------------------
// Method........: Shrink
// Author........: Aleksander šhrn
// Date..........: 980703
// Description...:
// Comments......: Intended for expert use only, in conjunction with
//                 the Reserve method.
//
//                 Consider more optimal reallocation.
// Revisions.....:
//===================================================================

bool
String::Shrink() {
/*
	int length = GetLength();

	// Already shrunk to a minimum?
	if (representation_ == NULL || representation_->buffer_ == NULL || representation_->size_ == length + 1)
		return true;

	// Check current reference count, disconnect if needed.
  if (representation_->reference_count_ > 1) {
  	#pragma omp atomic
    representation_->reference_count_--;
		char *buffer             = representation_->buffer_;
    representation_          = new Representation();
		representation_->size_   = length + 1;
		representation_->buffer_ = new char[representation_->size_];
		strcpy(representation_->buffer_, buffer);
  }
  else {
		representation_->size_   = length + 1;
		char *newbuffer          = new char[representation_->size_];
		strcpy(newbuffer, representation_->buffer_);
		delete [] representation_->buffer_;
		representation_->buffer_ = newbuffer;
	}
*/ //No need for this. Reduce capacity to size with inbuilt function if C++11 is used. -N
//	representation_.shrink_to_fit(); //Unfortunately, there are no flags to check for the version that works yet. -N
	return true;

}

//-------------------------------------------------------------------
// Method........: operator =
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 9807: Rewrote.
//===================================================================

String &
String::operator=(const char *buffer) {
/*
	// Avoid trouble.
  if (buffer == NULL)
		buffer = "";

	// Check current reference count, disconnect if needed.
  if (representation_->reference_count_ > 1) {
    representation_->reference_count_--;
    representation_          = new Representation();
		representation_->size_   = strlen(buffer) + 1;
		representation_->buffer_ = new char[representation_->size_];
		strcpy(representation_->buffer_, buffer);
  }
  else {
		int size = strlen(buffer) + 1;
		if (size <= representation_->size_) {
			strcpy(representation_->buffer_, buffer);
		}
		else {
			delete [] representation_->buffer_;
			representation_->size_   = size;
			representation_->buffer_ = new char[representation_->size_];
			strcpy(representation_->buffer_, buffer);
		}
	}
*/
	representation_ = buffer; //All we need. -N

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator =
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: With reference counting, assignments are cheap!
// Revisions.....:
//===================================================================

String &
String::operator=(const String &in){
/*
	// Protect against self-assignment.
	in.representation_->reference_count_++;

	// Check current reference count.
	if (--representation_->reference_count_ == 0) {
		delete [] representation_->buffer_;
    	delete representation_;
  	}
  	representation_ = in.representation_;
*/
	representation_ = in.representation_; // -N
  	return *this;

}

//-------------------------------------------------------------------
// Method........: operator []
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Since a reference is returned, we have to do some
//                 checking to see if any other strings share the same
//                 physical representation.  If so, we allocate a
//                 duplicate representation before returning the
//                 reference so that we do not inadvertently alter
//                 the other strings when/if the returned value is
//                 assigned to something.
//
//                 Note that this may be costly, so make sure that
//                 the corresponding const method gets called instead
//                 if possible!
//
// Revisions.....: Aš 002102: Sanity check on index too time-consuming.
//===================================================================

char &
String::operator[](int i) {
/*
	static char undefined = ' ';

	// Is the string empty?
  if (representation_->buffer_ == NULL)
		return undefined;

#if 0
	// Check input index.
	if ((i < 0) || (i >= GetLength()))
		return undefined;
#endif

	// Check current reference count.
  if (representation_->reference_count_ > 1) {
		char *buffer = representation_->buffer_;
	#pragma omp atomic
    representation_->reference_count_--;
    representation_          = new Representation();
    representation_->size_   = strlen(buffer) + 1;
    representation_->buffer_ = new char[representation_->size_];
		strcpy(representation_->buffer_, buffer);
  }

  return representation_->buffer_[i];
*/
	return representation_[i]; // -N

}

//-------------------------------------------------------------------
// Method........: operator []
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 002102: Sanity check on index too time-consuming.
//===================================================================

char
String::operator[](int i) const {
/*
	static char undefined = ' ';

	// Is the string empty?
  if (representation_->buffer_ == NULL)
		return undefined;

#if 0
	// Check input index.
	if ((i < 0) || (i >= GetLength()))
		return undefined;
#endif

  return representation_->buffer_[i];
*/
	return representation_[i]; // -N
}

//-------------------------------------------------------------------
// Method........: IsEmpty
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: -N 08/05/13 Changed basic design to account for
//			the modern string class. Furthermore,
//			changed "" comparison to find_first_not_of to
//			account for non-empty strings composed of
//			only spaces and/or tabs.
//===================================================================

bool
String::IsEmpty() const {
/*
  if (representation_->buffer_ == NULL)
		return true;

	return ((representation_->buffer_)[0] == '\0');
*/
	//Easier this way. -N
//	if (representation_.find_first_not_of(" \t") == representation_.npos)
//		return true;
	if (representation_.empty())
		return true;
	if (representation_ == "")
		return true;
	return (representation_ == "\0");
}

//-------------------------------------------------------------------
// Method........: GetBuffer
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Potentially dangerous, may cause undesirable side-
//                 effects if someone casts away the constness of the
//                 returns buffer and alters it while multiple strings
//                 share the same physical representation...
// Revisions.....:
//===================================================================

const char *
String::GetBuffer() const {
//  return representation_->buffer_;
	//No need to get the buffer. -N
	return representation_.c_str();
}

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Nicholas Baltzer
// Date..........: 03/06/13
// Description...: Returns the representation buffer as a string.
//		   This should alleviate some of the issues
//		   encountered when attempting to move away from
//		   the local String class.
// Comments......: 
// Revisions.....:
//===================================================================

string
String::GetString() const {
	return representation_;
}


//-------------------------------------------------------------------
// Method........: GetLength
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 29-05-15 -N
// 			Switched to std::string style.
//===================================================================

int
String::GetLength() const {
/*
  if (representation_->buffer_ == NULL)
    return 0;

  return strlen(representation_->buffer_);
*/
	//string has inbuilt length. -N
	return representation_.length();
}

//-------------------------------------------------------------------
// Method........: First
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 29-05-15 -N
// 			Switched to std::string style.
//===================================================================

char
String::First() const {
//	return (*this)[0];
	return representation_[0];
}

//-------------------------------------------------------------------
// Method........: Last
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: 
// Revisions.....: 29-05-15 -N
// 			Switched to std::string style.
//===================================================================

char
String::Last() const {
//	return (*this)[GetLength() - 1];
	return representation_[representation_.length() - 1];
}

//-------------------------------------------------------------------
// Method........: IsInteger
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: See system documentation for strtol for a
//                 description of which strings that can be
//                 interpreted as integer values.
//
//                 An extra modification is made so that e.g. the
//                 string "3.14" is not interpreted as the integer 3.
//                 The string "3.00" is accepted as the integer 3,
//                 however.
// Revisions.....:
//===================================================================

bool
String::IsInteger() const {

	// Integers can also be interpreted as floating point values.
	if (!IsFloat())
		return false;

	// Get the floating point value...
	float value = GetFloat();

	double integer;

	// ..and examine it. We don't want "3.14" to be interpreted as 3, but as 3.14.
	if (modf(value, &integer) != 0.0)
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: IsFloat
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: See system documentation for strtod and atof for a
//                 description of which strings that can be
//                 interpreted as floating point values.
//
//                 Note that integers are also interpreted as floating
//                 point values.
//
//                 Note that strings such as e.g. "3.14ghjgk" will
//                 return false, even though the strtod method would
//                 accept this as a float (namely 3.14 with the
//                 stopstring "ghjgk").
// Revisions.....: 29/11/13 -N Fixed for string.
//===================================================================

bool
String::IsFloat() const {

	// Is the repr. buffer instantiated?
//	if (representation_->buffer_ == NULL)
	if (representation_.empty())
		return false;

	// Pointer to character that stops scan.
  char *stopstring;

	// Attempt conversion.
 // 	strtod(representation_->buffer_, &stopstring);
  	strtod(representation_.c_str(), &stopstring);

	// Does the stopstring point to the null-terminating character of the repr. buffer?
	// Check for the special case of the repr. buffer having length 0.
//	if ((stopstring == representation_->buffer_) || (*stopstring != '\0'))
	if ((stopstring == representation_.c_str()) || (*stopstring != '\0'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: IsBoolean
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Synchronize with GetBoolean method.
// Revisions.....: Aš 9807: Added "no"/"yes".
//===================================================================

bool
String::IsBoolean() const {

/*
	if ((representation_ == NULL) || (representation_->buffer_ == NULL))
		return false;

	// Accept the strings below as false.
  if (strcmp("0",     representation_->buffer_) == 0)
		return true;
  if (strcmp("f",     representation_->buffer_) == 0)
		return true;
  if (strcmp("F",     representation_->buffer_) == 0)
		return true;
  if (strcmp("false", representation_->buffer_) == 0)
		return true;
  if (strcmp("False", representation_->buffer_) == 0)
		return true;
  if (strcmp("FALSE", representation_->buffer_) == 0)
		return true;
  if (strcmp("n",     representation_->buffer_) == 0)
		return true;
  if (strcmp("N",     representation_->buffer_) == 0)
		return true;
  if (strcmp("No",    representation_->buffer_) == 0)
		return true;
  if (strcmp("NO",    representation_->buffer_) == 0)
		return true;

	// Accept the strings below as true.
  if (strcmp("1",     representation_->buffer_) == 0)
		return true;
  if (strcmp("t",     representation_->buffer_) == 0)
		return true;
  if (strcmp("T",     representation_->buffer_) == 0)
		return true;
  if (strcmp("true",  representation_->buffer_) == 0)
		return true;
  if (strcmp("True",  representation_->buffer_) == 0)
		return true;
  if (strcmp("TRUE",  representation_->buffer_) == 0)
		return true;
  if (strcmp("y",     representation_->buffer_) == 0)
		return true;
  if (strcmp("Y",     representation_->buffer_) == 0)
		return true;
  if (strcmp("yes",   representation_->buffer_) == 0)
		return true;
  if (strcmp("Yes",   representation_->buffer_) == 0)
		return true;
  if (strcmp("YES",   representation_->buffer_) == 0)
		return true;
*/

	//Repeating the above sequence, minus the references. -N
	if (representation_ == "")
		return false;

	// Accept the strings below as false.
  if ("0" == representation_)
		return true;
  if ("f" == representation_)
		return true;
  if ("F" == representation_)
		return true;
  if ("false" == representation_)
		return true;
  if ("False" == representation_)
		return true;
  if ("FALSE" == representation_)
		return true;
  if ("n" == representation_)
		return true;
  if ("N" == representation_)
		return true;
  if ("No" == representation_)
		return true;
  if ("NO" == representation_)
		return true;

	// Accept the strings below as true.
  if ("1" == representation_)
		return true;
  if ("t" == representation_)
		return true;
  if ("T" == representation_)
		return true;
  if ("true" == representation_)
		return true;
  if ("True" == representation_)
		return true;
  if ("TRUE" == representation_)
		return true;
  if ("y" == representation_)
		return true;
  if ("Y" == representation_)
		return true;
  if ("yes" == representation_)
		return true;
  if ("Yes" == representation_)
		return true;
  if ("YES" == representation_)
		return true;


	return false;

}

//-------------------------------------------------------------------
// Method........: GetInteger
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes the string represents a valid integer.
// Revisions.....: 29-05-15 -N
// 			Switched to c++11 standard.
//===================================================================

int
String::GetInteger() const {
//	return atoi(representation_->buffer_);
//	return atoi(representation_.c_str()); //Remove buffer. -N
	return std::stoi(representation_);
}

//-------------------------------------------------------------------
// Method........: GetFloat
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes the string represents a valid float.
// Revisions.....: 29-05-15 -N
// 			Switched to c++11 standard.
//===================================================================

float
String::GetFloat() const {
	//return static_cast(float, atof(representation_->buffer_));
	//return static_cast(float, atof(representation_.c_str())); //-N
	return std::stof(representation_); // -N
}



//-------------------------------------------------------------------
// Method........: GetBoolean
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Synchronize with IsBoolean method.
// Revisions.....: Aš 9807: Added "no"/"yes".
//===================================================================

bool
String::GetBoolean() const {
/*
	if ((representation_ == NULL) || (representation_->buffer_ == NULL))
		return false;

	// Accept the strings below as false.
  if (strcmp("0",     representation_->buffer_) == 0)
		return false;
  if (strcmp("f",     representation_->buffer_) == 0)
		return false;
  if (strcmp("F",     representation_->buffer_) == 0)
		return false;
  if (strcmp("false", representation_->buffer_) == 0)
		return false;
  if (strcmp("False", representation_->buffer_) == 0)
		return false;
  if (strcmp("FALSE", representation_->buffer_) == 0)
		return false;
  if (strcmp("n",     representation_->buffer_) == 0)
		return false;
  if (strcmp("N",     representation_->buffer_) == 0)
		return false;
  if (strcmp("no",    representation_->buffer_) == 0)
		return false;
  if (strcmp("No",    representation_->buffer_) == 0)
		return false;
  if (strcmp("NO",    representation_->buffer_) == 0)
		return false;

	// Accept the strings below as true.
  if (strcmp("1",     representation_->buffer_) == 0)
		return true;
  if (strcmp("t",     representation_->buffer_) == 0)
		return true;
  if (strcmp("T",     representation_->buffer_) == 0)
		return true;
  if (strcmp("true",  representation_->buffer_) == 0)
		return true;
  if (strcmp("True",  representation_->buffer_) == 0)
		return true;
  if (strcmp("TRUE",  representation_->buffer_) == 0)
		return true;
  if (strcmp("y",     representation_->buffer_) == 0)
		return true;
  if (strcmp("Y",     representation_->buffer_) == 0)
		return true;
  if (strcmp("yes",   representation_->buffer_) == 0)
		return true;
  if (strcmp("Yes",   representation_->buffer_) == 0)
		return true;
  if (strcmp("YES",   representation_->buffer_) == 0)
		return true;
*/
	//Same as above, but altered for strings. -N
	if (representation_ == "")
		return false;

	// Accept the strings below as false.
  if ("0" == representation_)
		return false;
  if ("f" == representation_)
		return false;
  if ("F" == representation_)
		return false;
  if ("false" == representation_)
		return false;
  if ("False" == representation_)
		return false;
  if ("FALSE" == representation_)
		return false;
  if ("n" == representation_)
		return false;
  if ("N" == representation_)
		return false;
  if ("no" == representation_)
		return false;
  if ("No" == representation_)
		return false;
  if ("NO" == representation_)
		return false;

	// Accept the strings below as true.
  if ("1" == representation_)
		return true;
  if ("t" == representation_)
		return true;
  if ("T" == representation_)
		return true;
  if ("true" == representation_)
		return true;
  if ("True" == representation_)
		return true;
  if ("TRUE" == representation_)
		return true;
  if ("y" == representation_)
		return true;
  if ("Y" == representation_)
		return true;
  if ("yes" == representation_)
		return true;
  if ("Yes" == representation_)
		return true;
  if ("YES" == representation_)
		return true;
	return false;

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Format(int i) {
/*
//	static char buffer[20];
	char buffer[20];

	// Convert input from integer to characters.
	sprintf(buffer, "%d", i);

	return String(buffer);
*/
	//Moved to string base. -N
	return String(std::to_string(i));
}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Format(unsigned int i) {
	return Format(static_cast(int, i));
}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Format(long l) {
	return Format(static_cast(int, l));
}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Spurious trailing post-decimalpoint zeros are
//                 discarded, e.g.:
//
//                    2.74000 -> "2.74"
//                    2.00000 -> "2.0"
//                    2.74020 -> "2.7402"
//
//                 If the number is real big, it is stored in
//                 stored in exponent format, otherwise it is stored
//                 in full.
// Revisions.....:
//===================================================================

String
String::Format(float f) {

	const int   bufsize = 100;
	const float limit   = 1E10;
//	static char buffer[bufsize];
	char buffer[bufsize];

	bool f_format = (f < limit);

	// Convert input from float to characters.
	if (f_format)
		sprintf(buffer, "%f", f);
	else
		sprintf(buffer, "%e", f);

	// Discard spurious trailing zeros if appropriate.
	if (f_format) {
		int n = strlen(buffer);
		while ((n > 1) && (buffer[n - 1] == '0') && (buffer[n - 2] != '.')) {
			buffer[n - 1] = '\0';
			n--;
		}
	}

	return String(buffer);

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Format(double d) {

	const int   bufsize = 100;
//	static char buffer[bufsize];
	char buffer[bufsize];

	// Convert input from float to characters.
	sprintf(buffer, "%f", d);

	int n = strlen(buffer);

	// Discard spurious trailing zeros if appropriate.
	while ((n > 1) && (buffer[n - 1] == '0') && (buffer[n - 2] != '.')) {
		buffer[n - 1] = '\0';
		n--;
	}

	return String(buffer);

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Format(bool b) {
	return String(b ? 'T' : 'F', 1);
}

//-------------------------------------------------------------------
// Method........: GetNoTokens
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of tokens in a string.
// Comments......:
// Revisions.....: NB 08/05/13 Added init to *char to avoid those
//			the allocation issues.
//===================================================================

int
String::GetNoTokens(const char *delimiters) const {

  if (delimiters == NULL)
		return 0;
/*
	// Duplicate the current buffer.
  char *tokenstr = new char[GetLength() + 1];
  strcpy(tokenstr, representation_->buffer_);

	// Get the first token.
  char *tokenptr = strtok(tokenstr, delimiters);

	// Initialize token counter.
  int no_tokens = 0;

	// Get all subsequent tokens.
  while (tokenptr != NULL) {
		no_tokens++;
    tokenptr = strtok(NULL, delimiters);
  }

	// Clean up.
  delete [] tokenstr;
*/
	//Not so fond of the char*, but it's only local to this scope. -N
	char* token = new char[representation_.length() + 1];

	strcpy(token, representation_.c_str()); 
	int token_count;

	char *current_token = strtok(token, delimiters);

	token_count = 0;
	
	while (current_token != NULL) {
		token_count++;
		current_token = strtok(NULL, delimiters);
	}

  return token_count;

}

//-------------------------------------------------------------------
// Method........: GetToken
// Author........: Aleksander šhrn
// Date..........:
// Description...: This method must be used until false is returned,
//                 otherwise the next call to GetToken will be
//                 erroneous.
// Comments......:
// Revisions.....:
//===================================================================

bool
String::GetToken(String &token, const char *delimiters) const {

  static char *tokenptr = NULL;
  static char *tokenstr = NULL;
/*
  if (delimiters == NULL || representation_ == NULL || representation_->buffer_ == NULL)
		return false;

  if (tokenptr == NULL) {
    // New token search, take copy of string since strtok destroys the character array.
		tokenstr = new char[GetLength() + 1];
    strcpy(tokenstr, representation_->buffer_);
    tokenptr = strtok(tokenstr, delimiters);
  }
  else {
		// We have already started reading tokens.
		tokenptr = strtok(NULL, delimiters);
  }

	// Any more tokens?
  if (tokenptr == NULL) {
    delete [] tokenstr;
    token = "";
    return false;
  }
  else {
    token = tokenptr;
    return true;
  }
*/
//	cout << "Token: " << token << endl;	
	//This could be altered to work with strings instead. -N
	if (delimiters == NULL || representation_ == "")
		return false;

	if (tokenptr == NULL) {
		tokenstr = new char[GetLength() + 1];
		strcpy(tokenstr, representation_.c_str());
		tokenptr = strtok(tokenstr, delimiters);
	}
	else {
		tokenptr = strtok(NULL, delimiters);
	}

	if (tokenptr == NULL) {
//		cout << "Parsed: "<< token << endl;
//		delete [] tokenstr;
		token = "";
		return false;
	}
	else {
		token = tokenptr;
		return true;
	}
}

//-------------------------------------------------------------------
// Method........: GetToken
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
String::GetToken(String &token, int token_no, const char *delimiters) const {

	char *tokenstr = NULL;
	char *tokenptr = NULL;
/*
  if (representation_ == NULL || representation_->buffer_ == NULL || delimiters == NULL)
		return false;
*/
	//No buffer to consider. -N
  if (representation_ == "" || delimiters == NULL)
		return false;

	// Duplicate the current buffer.
  tokenstr = new char[GetLength() + 1];
//  strcpy(tokenstr, representation_->buffer_);

	//Slightly different with a string. -N
	strcpy(tokenstr, representation_.c_str());

	// Get the first token.
  tokenptr = strtok(tokenstr, delimiters);

  int counter = 0;

	
	// Iterate until the given token number is found.
  while (tokenptr != NULL) {
	if (counter == token_no) {
		token = tokenptr;
		delete [] tokenstr;
		return true;
	  }
//	cout << "Token: "<<token<<endl;
  	tokenptr = strtok(NULL, delimiters);
	counter++;
  }

  // Token not found.
  delete [] tokenstr;
  return false;

}

//-------------------------------------------------------------------
// Method........: Reverse
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Optimize implementation, if needed.
// Revisions.....:
//===================================================================

void
String::Reverse() {

	int length = GetLength();
	int half   = length / 2;
	int last   = length - 1;
	int i;

	// Swap characters around middle.
	for (i = 0; i < half; i++) {
		char tmp = (*this)[i];
		(*this)[i] = (*this)[last - i];
		(*this)[last - i] = tmp;
	}

}

//-------------------------------------------------------------------
// Method........: Search
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Returns -1 if not found.
// Revisions.....:
//===================================================================

int
String::Search(int start, char c) const {
/*
  const char *buffer = GetBuffer();
  int         length = GetLength();

  if (length > 0) {
		if (start >= 0) {
			const char *p = &(buffer[start]);
	    const char *q = &(buffer[length]);
	    while (p < q)
				if (*p++ == c)
					return --p - buffer;
		}
    else {
			const char *p = &(buffer[length + start + 1]);
			while (--p >= buffer)
				if (*p == c)
					return p - buffer;
		}
  }

  return -1;
*/

	//faster and easier. - N
	if (GetString().find(c, start) == string::npos)
		return -1;

	return GetString().find(c, start);

	

}

//-------------------------------------------------------------------
// Method........: Search
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Returns -1 if not found.
// Revisions.....:
//===================================================================

int
String::Search(int start, const String &substring) const {
/*
  const char *buffer1 = GetBuffer();
  const char *buffer2 = substring.GetBuffer();

  int length1 = GetLength();
  int length2 = substring.GetLength();

  if ((length1 > 0) && (length2 > 0)) {
		if (start >= 0) {
			const char *last1 = &(buffer1[length1 - length2]);
			const char *last2 = &(buffer2[length2]);
			const char *p     = &(buffer1[start]);
			while (p <= last1) {
	      const char *x = p++;
	      const char *y = buffer2;
	      while (*x++ == *y++)
					if (y >= last2)
						return --p - buffer1;
	    }
		}
		else {
			const char *first1 = &(buffer1[length2 - 1]);
			const char *last2  = &(buffer2[length2 - 1]);
			const char *p      = &(buffer1[length1 + start + 1]);
			while (--p >= first1) {
	      const char *x = p;
	      const char *y = last2;
	      while (*x-- == *y--)
					if (y < buffer2)
						return ++x - buffer1;
	    }
		}
  }

  return -1;
*/	
	//Easier and faster this way. -N
	
	if (GetString().find(substring.GetString(), start) == string::npos)
		return -1;

	return GetString().find(substring.GetString(), start);
	
}

//-------------------------------------------------------------------
// Method........: Replace
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Optimize implementation, if needed.
// Revisions.....:
//===================================================================

void
String::Replace(char search, char replace) {

  int i, length = GetLength();

	// Search and replace.
  for (i = 0; i < length; i++)
    if ((*this)[i] == search)
			(*this)[i] = replace;

}

//-------------------------------------------------------------------
// Method........: Replace
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
String::Replace(const String &search, const String &replace) {
	/*
	String processed;

	int index = 0, start = 0;

	// Build a new string.
	do {
		index = GetIndex(search, start);
		if (index != -1) {
			processed += GetSubstring(start, index - start) + replace;
			start = index + search.GetLength();
		}
		else {
			processed += GetSubstring(start, GetLength() - start);
		}
	} while (index != -1);

	// Replace this string with new string.
	*this = processed;
	*/


	//Easier and faster. -N
	int index = representation_.find(search.GetString());
	while(index != std::string::npos) {

		representation_.replace(index, search.GetString().length(), replace.GetString());
		index = representation_.find(search.GetString());
	}

	
	//Check it1
	if (representation_ == replace.GetString())	
		exit(400);
	
}

//-------------------------------------------------------------------
// Method........: Match
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Returns -1 if no match.
// Revisions.....:
//===================================================================

int
String::Match(int start, bool exact, const String &substring) const {

  int length1 = GetLength();
  int length2 = substring.GetLength();

  const char *buffer2 = substring.GetBuffer();

  if (start < 0) {
    start = length1 + start - length2 + 1;
    if ((start < 0) || (exact && (start != 0)))
      return -1;
  }
  else {
		if (exact && (length1 - start != length2))
			return -1;
	}

  if ((length1 == 0) || (length2 == 0) || (length1 - start < length2) || (start >= length1))
    return -1;

  int n = length2;

//  const char *buffer1 = &(representation_->buffer_[start]);

	//Replace with something a little more elegant. -N
	const char *buffer1 = &representation_.at(start);

  while (--n >= 0)
		if (*buffer1++ != *buffer2++)
			return -1;

	return length2;

}

//-------------------------------------------------------------------
// Method........: Matches
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
String::Matches(const String &substring, int start) const {
  return (Match(start, true, substring) >= 0);
}

//-------------------------------------------------------------------
// Method........: GetSubstring
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::GetSubstring(int start, int length) const {
/*
  if (representation_->buffer_ == NULL)
		return String("");
*/
	//Reformat. -N
//	if (representation_ == "")
//		return String("");

//  String substring(' ', length);

//	int i;
/*
	for (i = 0; i < length; i++)
		substring[i] = representation_->buffer_[start + i];
*/
	//Reformat. -N
//	for (i = 0; i < length; i++)
//		substring[i] = representation_[start + i];
	String substring;
	try {
	substring.representation_ = representation_.substr(start, start + length);
	}
	catch (const std::out_of_range& oor) {
		return String("");
	}
  return substring;

}

//-------------------------------------------------------------------
// Method........: Delete
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
String::Delete(int start, int length) {

/*
  if ((start < 0) || (length <= 0) || ((start + length) > GetLength()))
    return;


	// Check current reference count.  Make sure the deletion has a local effect only.
  if (representation_->reference_count_ > 1)
		(*this)[0] = (*this)[0];
*/
	//There is no reference count.

//  int first = start + length;

/*
	// Copy buffer to eliminate the deleted portion.
  StaticCopy(&(representation_->buffer_[first]), &(representation_->buffer_[start]), GetLength() - first);
*/

	//So much simpler. Let the standard functions do the work. -N
	try {
	representation_.erase(start, length);
	}
	catch (const std::out_of_range& oor) {
		return;
	}
}

void
String::Delete(const String &substring, int start) {
//  Delete(Search(start, substring), substring.GetLength());
	//Delete a substring within a string. -N
	representation_.erase(Search(start, substring), substring.GetLength());
}

void
String::Delete(char c, int start) {
//  Delete(Search(start, c), 1);
	//Delete the first character c within a string, starting at position start. -N
	representation_.erase(Search(start, c), 1);

}

//-------------------------------------------------------------------
// Method........: Trim
// Author........: Aleksander šhrn
// Date..........:
// Description...: Trims the string for a specified character at the
//                 beginning and end of the string.
//
//                 Example:    s1 = "   Welcome to Jamaica, man!     "
//                             s1.Trim(' ');
//                             s1 = "Welcome to Jamaica, man!"
//
//                             s2 = "***Have a nice day, dude...  ****"
//                             s2.Trim('*');
//                             s2 = "Have a nice day, dude...  "
// Comments......:
// Revisions.....: -N 08/05/13 No to all of that. Function now
//			uses find_first_not_of instead, and takes
//			the remainder as a substring copy.
//===================================================================

void
String::Trim(char c) {

//	Trim(""+c);
	// Nothing to trim?
//	if (IsEmpty())
//		return;

//	int start = 0;
//  int stop  = GetLength() - 1;


/*
	// Check current reference count.  Make sure the deletion has a local effect only.
  if (representation_->reference_count_ > 1)
		(*this)[0] = (*this)[0];

  while (representation_->buffer_[start] == c)
		start++;
	while ((representation_->buffer_[stop] == c) && (stop > start))
		stop--;

	int i, length = stop - start;

	if (start > 0) {
		for (i = 0; i <= length; i++)
			representation_->buffer_[i] = representation_->buffer_[start + i];
	}

	representation_->buffer_[length + 1] = '\0';
*/

	//Reformatted. -N
//	cout << "|"<< representation_ << "| -- ";	
	representation_.erase(representation_.find_last_not_of(c) + 1);
	representation_.erase(0, representation_.find_first_not_of(c));
/*	int start = representation_.find_first_not_of(c);

	if (start != std::string::npos)
		representation_ = representation_.substr(start);

	int stop = representation_.find_last_not_of(c);

	if (stop != std::string::npos)
		representation_ = representation_.substr(0, stop + 1);
	if (representation_ == c+"")
		representation_ = "";

//	cout << "|"<< representation_ << "|" << endl;	
*/
}

//-------------------------------------------------------------------
// Method........: Trim
// Author........: Aleksander šhrn
// Date..........:
// Description...: Like Trim(char), but handles all characters in
//                 the input string.
// Comments......: -N 11/05/13 Trim will not work if the input string
//			chars contains multiple entires of the same
//			character while the length of the string that 
//			is being trimmed is 1. 
//			This is not a big issue, as one would not 
//			expect this type of trimming to happen. Thus,
//			there is no sanity check.
// Revisions.....:
//===================================================================

void
String::Trim(const String &chars) {

	// Nothing to trim?
//	if (IsEmpty())
//		return;

//	int start = 0;
//  int stop  = GetLength() - 1;

/*
	// Check current reference count.  Make sure the deletion has a local effect only.
  if (representation_->reference_count_ > 1)
		(*this)[0] = (*this)[0];

  while (chars.Contains(representation_->buffer_[start]))
		start++;
	while ((chars.Contains(representation_->buffer_[stop])) && (stop > start))
		stop--;

	int i, length = stop - start;

	if (start > 0) {
		for (i = 0; i <= length; i++)
			representation_->buffer_[i] = representation_->buffer_[start + i];
	}

	representation_->buffer_[length + 1] = '\0';
*/

	//Reformatted. -N
//	cout << "|"<< representation_ << "| -- ";	
/*	int start = representation_.find_first_not_of(chars.representation_);

	if (start != representation_.npos)
		representation_ = representation_.substr(start);
 
	int stop = representation_.find_last_not_of(chars.representation_);

	if (stop != representation_.npos)
		representation_ = representation_.substr(0, stop + 1);

	if (representation_ == chars.representation_)
		representation_ = "";
*/
//	cout << "|"<< representation_ << "|" << endl;	

	representation_.erase(representation_.find_last_not_of(chars.representation_) + 1);
	representation_.erase(0, representation_.find_first_not_of(chars.representation_));

}

//-------------------------------------------------------------------
// Method........: Count
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns how many occurrences of the given character
//                 the string contains.
// Comments......:
// Revisions.....:
//===================================================================

int
String::Count(char c) const {

//  const char *buffer = representation_->buffer_;

	//Reformatted. -N
	const char *buffer = representation_.c_str();

	if (buffer == NULL)
		return 0;

	int count = 0;

	while (*buffer != '\0') {
		if (*buffer == c)
			count++;
		buffer++;
	}

  return count;

}

//-------------------------------------------------------------------
// Method........: Contains
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
String::Contains(char c) const {

//  const char *buffer = representation_->buffer_;

	//Reformatted. -N
	const char *buffer = representation_.c_str();

	if (buffer == NULL)
		return false;

	while (*buffer != '\0') {
		if (*buffer == c)
			return true;
		buffer++;
	}

	return false;

}

bool
String::Contains(const String &substring, int start) const {
  return (Match(start, false, substring) >= 0);
}

bool
String::Contains(const String &substring) const {
  return (Search(0, substring) >= 0);
}

//-------------------------------------------------------------------
// Method........: StartsWith
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
String::StartsWith(char c) const {

	if (IsEmpty())
		return false;

	return First() == c;

}

bool
String::StartsWith(const String &substring) const {

	int length = substring.GetLength();

	if (length > GetLength())
		return false;

	return substring == GetSubstring(0, length);

}

//-------------------------------------------------------------------
// Method........: EndsWith
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
String::EndsWith(char c) const {

	if (IsEmpty())
		return false;

	return Last() == c;

}

bool
String::EndsWith(const String &substring) const {

	int length1 = substring.GetLength();
	int length2 = GetLength();

	if (length1 > length2)
		return false;

	return substring == GetSubstring(length2 - length1, length1);

}

//-------------------------------------------------------------------
// Method........: GetIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
String::GetIndex(char c, int start) const {
  return Search(start, c);
}

int
String::GetIndex(const String &substring, int start) const {
  return Search(start, substring);
}

//-------------------------------------------------------------------
// Method........: At
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::At(int start, int length) const {
  return GetSubstring(start, length);
}

String
String::At(const String &substring, int start) const {
  return GetSubstring(Search(start, substring), substring.GetLength());
}

String
String::At(char c, int start) const {
  return GetSubstring(Search(start, c), 1);
}

//-------------------------------------------------------------------
// Method........: Before
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Before(int position) const {
  return GetSubstring(0, position);
}

String
String::Before(const String &substring, int start) const {
  return GetSubstring(0, Search(start, substring));
}

String
String::Before(char c, int start) const {
  return GetSubstring(0, Search(start, c));
}

//-------------------------------------------------------------------
// Method........: Through
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::Through(int position) const {
  return GetSubstring(0, position + 1);
}

String
String::Through(const String &substring, int start) const {
  int last = Search(start, substring);
  if (last >= 0)
		last += substring.GetLength();
  return GetSubstring(0, last);
}

String
String::Through(char c, int start) const {
  int last = Search(start, c);
  if (last >= 0)
		last += 1;
  return GetSubstring(0, last);
}

//-------------------------------------------------------------------
// Method........: After
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::After(int position) const {
  return GetSubstring(position + 1, GetLength() - (position + 1));
	//Less problems this way. -N
//	String s;
//	s.representation_ = representation_.substr(position + 1);
//	return s;
}

String
String::After(const String &substring, int start) const {
  int first = Search(start, substring);
  if (first >= 0)
		first += substring.GetLength();
  return GetSubstring(first, GetLength() - first);

	//Easy way.
//	String s;
//	s.representation_ = representation_.substr(first);
//	return s;


}

String
String::After(char c, int start) const {
  int first = Search(start, c);
  if (first >= 0)
		first += 1;
  return GetSubstring(first, GetLength() - first);

//	String s;
//	s.representation_ = representation_.substr(first);
//	return s;

}

//-------------------------------------------------------------------
// Method........: From
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
String::From(int position) const {
  return GetSubstring(position, GetLength() - position);
}

String
String::From(const String &substring, int start) const {
  int first = Search(start, substring);
  return GetSubstring(first, GetLength() - first);
}

String
String::From(char c, int start) const {
  int first = Search(start, c);
  return GetSubstring(first, GetLength() - first);
}

//-------------------------------------------------------------------
// Method........: Pad
// Author........: Aleksander šhrn
// Date..........:
// Description...: Pads the string (to the left or right) with a
//                 given character, such that the padded string has
//                 a given length.
//
//                 The string is truncated if it is longer than the
//                 specified length.
//
// Comments......: Optimize implementation, if needed.
// Revisions.....:
//===================================================================

void
String::Pad(char c, int length, bool leftjustify) {

	// Truncate?
	if (length < GetLength()) {
		*this = GetSubstring(0, length);
		return;
	}

	String pad = String(c, length - GetLength());

	if (leftjustify)
		*this += pad;
	else
		*this = pad + *this;

}

//-------------------------------------------------------------------
// Method........: ToLowercase
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Optimize implementation, if needed.
// Revisions.....:
//===================================================================

void
String::ToLowercase() {

	int i, length = GetLength();

	for (i = 0; i < length; i++) {
		char c = (*this)[i];
		if (isupper(c))
			(*this)[i] = tolower(c);
	}

}

//-------------------------------------------------------------------
// Method........: ToUppercase
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Optimize implementation, if needed.
// Revisions.....:
//===================================================================

void
String::ToUppercase() {

	int i, length = GetLength();

	for (i = 0; i < length; i++) {
		char c = (*this)[i];
		if (islower(c))
			(*this)[i] = toupper(c);
	}

}

//-------------------------------------------------------------------
// Method........: Capitalize
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Optimize implementation, if needed.
// Revisions.....:
//===================================================================

void
String::Capitalize() {

	if (IsEmpty())
		return;

	char c = First();

	if (islower(c))
		(*this)[0] = toupper(c);

}


//-------------------------------------------------------------------
// Method........: Append
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from concatenation operators.
// Revisions.....:
//===================================================================

void
String::Append(const char *buffer, int length) {
/*
	if (buffer == NULL)
		return;

	if (buffer[0] == '\0')
		return;

	int addlength = (length >= 0) ? length : strlen(buffer);

	if (addlength == 0)
		return;

	if (representation_ == NULL)
    representation_ = new Representation();

	if (representation_->buffer_ == NULL) {
		representation_->size_        = addlength + 1;
		representation_->buffer_      = new char[representation_->size_];
		(representation_->buffer_)[0] = '\0';
	}

	int newsize = strlen(representation_->buffer_) + addlength + 1;

  if (representation_->reference_count_ > 1) { // Disconnect, allocate new representation and buffer.
		char *newbuffer          = new char[newsize];
		strcpy(newbuffer, representation_->buffer_);
		strcat(newbuffer, buffer);
		#pragma omp atomic
    representation_->reference_count_--;
    representation_          = new Representation();
		representation_->size_   = newsize;
		representation_->buffer_ = newbuffer;
	}
	else {                                       // Keep representation object.
		if (newsize <= representation_->size_) {   // Reuse existing buffer, it's large enough.
			strcat(representation_->buffer_, buffer);
		}
		else {                                     // Allocate new buffer, existing one too small.
			char *newbuffer          = new char[newsize];
			strcpy(newbuffer, representation_->buffer_);
			strcat(newbuffer, buffer);
			delete [] representation_->buffer_;
			representation_->size_   = newsize;
			representation_->buffer_ = newbuffer;
		}
	}
*/

	//Using standard functions, this is easier. variable length must now be greater than 0, not greater or equal. -N
	if (length > 0) {
		representation_.append(buffer, length);
	}
	else {
		representation_.append(buffer);
	} 

}

//-------------------------------------------------------------------
// Method........: Concatenation operators
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The low-level string operations strcat and strcpy
//                 don't seem to handle NULL cases very well...
//
// Revisions.....: Aš 9807: Rewrote += operators.
//===================================================================

void
String::operator+=(const char *buffer) {
	Append(buffer);
}

void
String::operator+=(const String &s) {
	Append(s.GetBuffer());
}

void
String::operator+=(char c) {
//	char buffer[] = {c, '\0'};
//	Append(buffer, 1);
//	Append(c, 0);
	representation_ = representation_ + c;
}

//-------------------------------------------------------------------
// Method........: Stream operators
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Currently, reads in max 4K.
// Revisions.....:
//===================================================================

istream &
operator>>(istream &stream, String &s) {
	static char buffer[4096];
	stream >> buffer;
	s = buffer;
	return stream;
}

ostream &
operator<<(ostream &stream, const String &s) {
//	if (s.GetBuffer() != NULL)
		//stream << s.GetBuffer();
		stream << s.representation_;
  return stream;
}


//-------------------------------------------------------------------
// Method........: Comparison operators
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Could handle NULL cases a lot better.
//
//                 The current implementation reuses code by checking
//                 complementary operators, but would be more efficient
//                 if "expanded" instead.
// Revisions.....:
//===================================================================

bool
String::operator==(const char *buffer) const {

/*  if (buffer == NULL)
		return (representation_->buffer_ == NULL);

	if (representation_->buffer_ == NULL)
		return (strcmp("", buffer) == 0);

	return (strcmp(representation_->buffer_, buffer) == 0);
*/
	//So much easier. -N
	return (representation_.c_str() == buffer);

}

bool
String::operator==(const String &s) const {
//	return ((*this) == s.representation_->buffer_);
	//About the same. -N
	return (representation_ == s.representation_);
}

bool
String::operator==(const std::string &s) const {
	return (representation_ == s);
}

bool
String::operator!=(const char *buffer) const {
	return !((*this) == buffer);
}

bool
String::operator!=(const String &s) const {
//	return !((*this) == s.representation_->buffer_);
	//Reformatted. -N
	return (representation_ != s.representation_);
}

bool
String::operator<(const char *buffer) const {
/*  if ((representation_->buffer_ == NULL) || (buffer == NULL))
		return false;
  return (strcmp(representation_->buffer_, buffer) < 0);
*/
	//Again, simpler. -N
	return (representation_.compare(buffer) < 0);
}

bool
String::operator<(const String &s) const {
//	return ((*this) < s.representation_->buffer_);

	//Same as above. -N
	return (representation_ < s.representation_.c_str());
}

bool
String::operator<=(const char *buffer) const {
	return (((*this) < buffer) || ((*this) == buffer));
}

bool
String::operator<=(const String &s) const {
//	return ((*this) <= s.representation_->buffer_);
	//These operators already exist for std::string. -N
	return (representation_ <= s.representation_);
}

bool
String::operator>(const char *buffer) const {
	return !((*this) <= buffer);
}

bool
String::operator>(const String &s) const {
	return !((*this) <= s);
}

bool
String::operator>=(const char *buffer) const {
	return (((*this) > buffer) || ((*this) == buffer));
}

bool
String::operator>=(const String &s) const {
	return (((*this) > s) || ((*this) == s));
}






 
