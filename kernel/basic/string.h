//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __STRING_H__
#define __STRING_H__

#include <copyright.h>

#include <kernel/basic/types.h>

#include <kernel/system/string.h>
#include <kernel/system/stdio.h>
#include <kernel/system/stdlib.h>
#include <kernel/system/iostream.h>
#include <string>

//-------------------------------------------------------------------
// Class.........: String
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Implements strings with reference counting.  The
//                 implementation is inspired by Stroustrup's example in
//                 his C++ textbook.  Additional functions are added here.
//
//                 The class was developed as a consequence of the STL
//                 string class causing problems with some compilers.
//
// Revisions.....: AÛ 9807: Added size variable to Representation class,
//                          added Reserve/Shrink methods.
//===================================================================

class String {
private:
/*
	//- String representation structure................................
  struct Representation {

		//- Physical representation......................................
		char *buffer_;           // Pointer to data.
		int   size_;             // Size of allocated buffer. String can be shorter!
    int   reference_count_;  // Reference counter.

		//- Constructor..................................................
    Representation() {reference_count_ = 1; buffer_ = NULL; size_ = 0;}

	};
*/
	//- String representation..........................................
//  Representation *representation_; //Replaced. -N
	std::string representation_;

private:

	//- Concatenation helpers..........................................
	void            Append(const char *buffer, int length = -1);

public:

	//- Constructors/destructor........................................
  String();
  String(const char *buffer);
  String(const String &in);
  String(char c, int length = 1);
  String(const std::string&);
  ~String();

	//- Memory management methods......................................
	bool            Reserve(int length);
	bool            Shrink();

	//- Assignment operators...........................................
  String         &operator=(const char *buffer);
  String         &operator=(const String &in);

	//- Bracket operators..............................................
  char            operator[](int i) const;
  char           &operator[](int i);

	//- Status methods.................................................
  bool            IsEmpty() const;

	//- Access methods.................................................
  const char     *GetBuffer() const;
  int             GetLength() const;
	std::string	GetString() const;

	char            First() const;
  char            Last() const;

	//- Type conversion methods........................................
	bool            IsInteger() const;
	bool            IsFloat() const;
	bool            IsBoolean() const;

  int             GetInteger() const;
  float           GetFloat() const;
	bool            GetBoolean() const;

	static String   Format(int i);
	static String   Format(unsigned int i);
	static String   Format(long l);
	static String   Format(float f);
	static String   Format(double d);
	static String   Format(bool b);

	//- Tokenization methods...........................................
  int             GetNoTokens(const char *delimiters = " \n\t") const;
  bool            GetToken(String &token, const char *delimiters = " \n\t") const;
  bool            GetToken(String &token, int token_no, const char *delimiters = " \n\t") const;

	//- Search/substring methods.......................................
  int             Search(int start, char c) const;
  int             Search(int start, const String &substring) const;

  int             Match(int start, bool exact, const String &substring) const;
  bool            Matches(const String &substring, int start = 0) const;

	String          GetSubstring(int start, int length) const;

	int             Count(char c) const;

  bool            Contains(char c) const;
  bool            Contains(const String &substring) const;
  bool            Contains(const String &substring, int start) const;

  bool            StartsWith(char c) const;
  bool            StartsWith(const String &substring) const;

  bool            EndsWith(char c) const;
  bool            EndsWith(const String &substring) const;

  int             GetIndex(char c, int start = 0) const;
  int             GetIndex(const String &substring, int start = 0) const;

  String          At(int start, int length) const;
  String          At(const String &substring, int start = 0) const;
  String          At(char c, int start = 0) const;

  String          Before(int position) const;
  String          Before(const String &substring, int start = 0) const;
  String          Before(char c, int start = 0) const;

  String          Through(int position) const;
  String          Through(const String &substring, int start = 0) const;
  String          Through(char c, int start = 0) const;

  String          After(int position) const;
  String          After(const String &substring, int start = 0) const;
  String          After(char c, int start = 0) const;

  String          From(int position) const;
  String          From(const String &substring, int start = 0) const;
  String          From(char c, int start = 0) const;

	//- Alteration methods.............................................
  void            Delete(int start, int length);
  void            Delete(const String &substring, int start = 0);
  void            Delete(char c, int start = 0);

	void            Reverse();

	void            Trim(char c);
	void            Trim(const String &chars);

  void            Replace(char search, char replace);
  void            Replace(const String &search, const String &replace);

	void            Pad(char c, int length, bool leftjustify = true);

	//- Case conversion methods........................................
	void            ToUppercase();
	void            ToLowercase();
	void            Capitalize();
	void		IntToString(int n);   //-N 25/04/13
	void		itoa(int n); //-N 25/04/13

	//- Comparison operators...........................................
  bool            operator==(const char *buffer) const;
  bool            operator==(const String &s) const;
	bool	operator==(const std::string &s) const;

  bool            operator!=(const char *buffer) const;
  bool            operator!=(const String &s) const;

  bool            operator<(const char *buffer) const;
  bool            operator<(const String &s) const;

  bool            operator<=(const char *buffer) const;
  bool            operator<=(const String &s) const;

  bool            operator>(const char *buffer) const;
  bool            operator>(const String &s) const;

  bool            operator>=(const char *buffer) const;
  bool            operator>=(const String &s) const;


	//- Concatenation operators........................................
  void            operator+=(const char *buffer);
  void            operator+=(const String &s);
  void            operator+=(char c);

	//- Stream operators...............................................
  friend ostream &operator<<(ostream &stream, const String &s);
  friend istream &operator>>(istream &stream, String &s);

};

//-------------------------------------------------------------------
// Global operators.
//===================================================================

//-------------------------------------------------------------------
// Method........: operator +
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Concatenation
// Comments......:
// Revisions.....:
//===================================================================

inline String
operator+(const String &s1, const String &s2) {
	String s3(s1);
	s3 += s2;
	return s3;
}
inline String
operator+(const String &s1, const char c) {
	String s3(s1);
	s3 += c;
	return s3;
}

#endif
 
