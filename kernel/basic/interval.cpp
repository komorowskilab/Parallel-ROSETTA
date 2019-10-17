//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/interval.h>
#include <kernel/basic/string.h>
#include <kernel/basic/undefined.h>
#include <kernel/basic/message.h>

#include <kernel/system/float.h>
#include <kernel/system/math.h>

//-------------------------------------------------------------------
// Methods for class Interval.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Interval::Interval() {
	MakeInterval(FromInclude(), Undefined::Float(), Undefined::Float(), To());
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Interval::Interval(const Interval &in) {
	MakeInterval(in.GetLowerDelimiter(), in.GetLowerValue(), in.GetUpperValue(), in.GetUpperDelimiter());
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Interval::Interval(char ldelimiter, float lvalue, float uvalue, char udelimiter) {
	MakeInterval(ldelimiter, lvalue, uvalue, udelimiter);
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: If the string does not define a valid interval,
//                 the interval defaults to an invalid range.
// Revisions.....:
//===================================================================

Interval::Interval(const String &interval) {

	String copy = interval;

	// Allow surrounding whitespace.
	copy.Trim(" \t\n");

	// Extract delimiters.
	char ldelimiter = copy.First();
	char udelimiter = copy.Last();

	// Initialize values to define an invalid range.
	float lvalue = 1.0;
	float uvalue = 0.0;

	int index = copy.Search(1, Separator());

	// Extract values, if possible.
	if (index != -1) {

		String lstring = copy.GetSubstring(1, index - 1);
		String ustring = copy.GetSubstring(index + 1, copy.GetLength() - index - 2);

		// Allow surrounding whitespace.
		lstring.Trim(" \t\n");
		ustring.Trim(" \t\n");

		if (lstring == Infinity())
			lvalue = -FLT_MAX;
		else if (lstring.IsFloat())
			lvalue = lstring.GetFloat();

		if (ustring == Infinity())
			uvalue = FLT_MAX;
		else if (ustring.IsFloat())
			uvalue = ustring.GetFloat();

	}

	MakeInterval(ldelimiter, lvalue, uvalue, udelimiter);

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Interval::~Interval() {
}

//-------------------------------------------------------------------
// Mutators.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetLowerDelimiter/SetUpperDelimiter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Interval::SetLowerDelimiter(char delimiter) {
	ldelimiter_ = delimiter;
	return (delimiter == From() || delimiter == FromInclude());
}

bool
Interval::SetUpperDelimiter(char delimiter) {
	udelimiter_ = delimiter;
	return (delimiter == To() || delimiter == ToInclude());
}

//-------------------------------------------------------------------
// Method........: SetLowerValue/SetUpperValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Undefined::Float() is interpreted as +/- infinity.
// Revisions.....:
//===================================================================

bool
Interval::SetLowerValue(float value) {
	if (value == Undefined::Float())
		value = -FLT_MAX;
	lvalue_ = value;
	return true;
}

bool
Interval::SetUpperValue(float value) {
	if (value == Undefined::Float())
		value = FLT_MAX;
	uvalue_ = value;
	return true;
}

//-------------------------------------------------------------------
// Method........: Merge
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Interval validity assumed. Not thoroughly debugged.
// Revisions.....:
//===================================================================

bool
Interval::Merge(const Interval &interval) {

	// Determine new range.
	float lvalue = (GetLowerValue() < interval.GetLowerValue()) ? GetLowerValue() : interval.GetLowerValue();
	float uvalue = (GetUpperValue() > interval.GetUpperValue()) ? GetUpperValue() : interval.GetUpperValue();

	char ldelimiter = GetLowerDelimiter();
	char udelimiter = GetUpperDelimiter();

	// New delimiters?
	if (lvalue == interval.GetLowerValue() && interval.GetLowerDelimiter() == FromInclude())
		ldelimiter = FromInclude();

	if (uvalue == interval.GetUpperValue() && interval.GetUpperDelimiter() == ToInclude())
		udelimiter = ToInclude();

	return MakeInterval(ldelimiter, lvalue, uvalue, udelimiter);

}

//-------------------------------------------------------------------
// Property querying methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsValid
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the interval defines a valid range.
// Comments......:
// Revisions.....:
//===================================================================

bool
Interval::IsValid() const {

	// Is the lvalue delimiter valid?
	if (GetLowerDelimiter() != From() && GetLowerDelimiter() != FromInclude())
		return false;

	// Is the uvalue delimiter valid?
	if (GetUpperDelimiter() != To() && GetUpperDelimiter() != ToInclude())
		return false;

	// Is the range clearly invalid?
	if (GetLowerValue() > GetUpperValue())
		return false;

	// The range may have degenerated into a point.
	if (GetLowerValue() == GetUpperValue()) {
		if (GetLowerDelimiter() != FromInclude() || GetUpperDelimiter() != ToInclude())
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsFinite/IsInfinite
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the interval is finite/infinite.
// Comments......: Interval validity assumed.
// Revisions.....:
//===================================================================

bool
Interval::IsFinite() const {
	return (GetLength() != FLT_MAX);
}

bool
Interval::IsInfinite() const {
	return !IsFinite();
}

//-------------------------------------------------------------------
// Method........: IsAbove
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this interval is "above" the input
//                 interval.
// Comments......: Interval validity assumed. Not thoroughly debugged.
// Revisions.....:
//===================================================================

bool
Interval::IsAbove(const Interval &interval) const {
	return ((GetLowerValue() > interval.GetUpperValue()) ||
		      (GetLowerValue() == interval.GetUpperValue() && !(GetLowerDelimiter() == FromInclude() && interval.GetUpperDelimiter() == ToInclude())));
}

//-------------------------------------------------------------------
// Method........: IsBelow
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this interval is "below" the input
//                 interval.
// Comments......: Interval validity assumed. Not thoroughly debugged.
// Revisions.....:
//===================================================================

bool
Interval::IsBelow(const Interval &interval) const {
	return ((GetUpperValue() < interval.GetLowerValue()) ||
		      (GetUpperValue() == interval.GetLowerValue() && !(GetUpperDelimiter() == ToInclude() && interval.GetLowerDelimiter() == FromInclude())));
}

//-------------------------------------------------------------------
// Method........: Overlaps
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this interval overlaps with the
//                 other interval.
// Comments......: Interval validity assumed. Not thoroughly debugged.
// Revisions.....:
//===================================================================

bool
Interval::Overlaps(const Interval &interval) const {
	return (!IsBelow(interval) && !IsAbove(interval));
}

//-------------------------------------------------------------------
// Method........: Meets
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this interval is adjacent to the
//                 input interval.
// Comments......: Interval validity assumed. Not thoroughly debugged.
// Revisions.....:
//===================================================================

bool
Interval::Meets(const Interval &interval) const {
	return ((IsBelow(interval) && GetUpperValue() == interval.GetLowerValue()) ||
		      (IsAbove(interval) && GetLowerValue() == interval.GetUpperValue()));
}

//-------------------------------------------------------------------
// Method........: Contains
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if this interval fully covers the
//                 input interval.
// Comments......: Interval validity assumed.
// Revisions.....:
//===================================================================

bool
Interval::Contains(const Interval &interval) const {
	return Contains(interval.GetLowerValue()) && Contains(interval.GetUpperValue());
}

//-------------------------------------------------------------------
// Method........: Contains
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the interval contains (covers) the
//                 specified value.
// Comments......: Interval validity assumed.
// Revisions.....:
//===================================================================

bool
Interval::Contains(float value) const {

	if ((GetLowerDelimiter() == From()) && (GetUpperDelimiter() == To()))
		return ((GetLowerValue() < value) && (value < GetUpperValue()));

	if ((GetLowerDelimiter() == FromInclude()) && (GetUpperDelimiter() == To()))
		return ((GetLowerValue() <= value) && (value < GetUpperValue()));

	if ((GetLowerDelimiter() == From()) && (GetUpperDelimiter() == ToInclude()))
		return ((GetLowerValue() < value) && (value <= GetUpperValue()));

	if ((GetLowerDelimiter() == FromInclude()) && (GetUpperDelimiter() == ToInclude()))
		return ((GetLowerValue() <= value) && (value <= GetUpperValue()));

	return false;

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
Interval::Contains(const String &value) const {
	return (value.IsFloat() && Contains(value.GetFloat()));
}

//-------------------------------------------------------------------
// Quantitative querying methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetLength
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Interval validity assumed.
// Revisions.....:
//===================================================================

float
Interval::GetLength() const {

	// Infinite length?
	if (GetLowerValue() == -FLT_MAX || GetUpperValue() == FLT_MAX)
		return FLT_MAX;

	return (GetUpperValue() - GetLowerValue());

}

//-------------------------------------------------------------------
// Method........: GetMidpoint
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Interval validity assumed.
// Revisions.....:
//===================================================================

float
Interval::GetMidpoint() const {

	// Infinite length?
	if (GetLowerValue() == -FLT_MAX && GetUpperValue() == FLT_MAX)
		return 0.0;
	else if (GetLowerValue() == -FLT_MAX)
		return -FLT_MAX;
	else if (GetUpperValue() == FLT_MAX)
		return FLT_MAX;

	// Finite length.
	return 0.5f * (GetUpperValue() + GetLowerValue());

}

//-------------------------------------------------------------------
// Method........: GetDistance
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the distance between the interval and a
//                 given value.
// Comments......: Interval validity assumed.
// Revisions.....:
//===================================================================

float
Interval::GetDistance(float value) const {

	if (Contains(value))
		return 0.0;

	float ldist = (GetLowerValue() == -FLT_MAX) ? FLT_MAX : fabs(value - GetLowerValue());
	float udist = (GetUpperValue() ==  FLT_MAX) ? FLT_MAX : fabs(value - GetUpperValue());

	return (ldist < udist) ? ldist : udist;

}

//-------------------------------------------------------------------
// Formatting methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Interval::Format(String &formatted) const {

	formatted = "";

	String lower = String::Format(GetLowerValue());
	String upper = String::Format(GetUpperValue());

	if (lower.IsInteger())
		lower = String::Format(lower.GetInteger());
	if (upper.IsInteger())
		upper = String::Format(upper.GetInteger());

	// Format range.
	if (GetLowerValue() == -FLT_MAX)
		formatted += Infinity();
	else
		formatted += lower;

	formatted += Separator();
	formatted += ' ';

	if (GetUpperValue() == FLT_MAX)
		formatted += Infinity();
	else
		formatted += upper;

	// Wrap delimiters around range.
	formatted = GetLowerDelimiter() + formatted + GetUpperDelimiter();

	return true;

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
Interval::Format() const {

	String formatted;

	if (!Format(formatted))
		formatted = Undefined::String();

	return formatted;

}

//-------------------------------------------------------------------
// Miscellanous methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: MakeInterval
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the interval. Returns true if everything
//                 is OK.
// Comments......:
// Revisions.....:
//===================================================================

bool
Interval::MakeInterval(char ldelimiter, float lvalue, float uvalue, char udelimiter) {

	bool ok = true;

	// Set delimiters.
	if (!SetLowerDelimiter(ldelimiter))
		ok = false;
	if (!SetUpperDelimiter(udelimiter))
		ok = false;

	// Set values.
	if (!SetLowerValue(lvalue))
		ok = false;
	if (!SetUpperValue(uvalue))
		ok = false;

	// Check validity of interval.
	if (!IsValid())
		ok = false;

	return ok;

}
 
