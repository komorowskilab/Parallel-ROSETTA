//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __INTERVAL_H__
#define __INTERVAL_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class.........: Interval
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents an interval over the set of reals.
//                 The interval may be open or closed.
// Revisions.....:
//===================================================================

class Interval {
protected:

	//- Internal representation........................................
	char         ldelimiter_; // Lower delimiter.
	char         udelimiter_; // Upper delimiter.
	float        lvalue_;     // Lower value.
	float        uvalue_;     // Upper value.

protected:

	//- Internal methods...............................................
	bool         MakeInterval(char ldelimiter, float lvalue, float uvalue, char udelimiter);

public:

  //- Tokens.........................................................
	static char  From()        {return '(';}
	static char  To()          {return ')';}
	static char  FromInclude() {return '[';}
	static char  ToInclude()   {return ']';}
	static char  Infinity()    {return '*';}
	static char  Separator()   {return ',';}

public:

	//- Constructors/destructor........................................
	Interval();
	Interval(const Interval &interval);
	Interval(char ldelimiter, float lvalue, float uvalue, char udelimiter);
	Interval(const String &interval);
	~Interval();

	//- Accessors.......................................................
  char         GetLowerDelimiter() const {return ldelimiter_;}
	char         GetUpperDelimiter() const {return udelimiter_;}

	float        GetLowerValue() const {return lvalue_;}
	float        GetUpperValue() const {return uvalue_;}

	//- Mutators........................................................
  bool         SetLowerDelimiter(char delimiter);
  bool         SetUpperDelimiter(char delimiter);

	bool         SetLowerValue(float value);
	bool         SetUpperValue(float value);

	bool         Merge(const Interval &interval);

	//- Property querying...............................................
	bool         IsValid() const;

	bool         IsFinite() const;
	bool         IsInfinite() const;

	bool         IsAbove(const Interval &interval) const;
	bool         IsBelow(const Interval &interval) const;

	bool         Contains(const Interval &interval) const;
	bool         Contains(float value) const;
	bool         Contains(const String &value) const;

	bool         Overlaps(const Interval &interval) const;

	bool         Meets(const Interval &interval) const;

	//- Quantitative querying..........................................
	float        GetLength() const;
	float        GetMidpoint() const;
	float        GetDistance(float value) const;

	//- Formatting.....................................................
	bool         Format(String &formatted) const;
	String       Format() const;

};

#endif 
