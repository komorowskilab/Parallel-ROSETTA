// HitsMessage.h: interface for the HitsMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HITSMESSAGE_H__F2ED4F87_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_HITSMESSAGE_H__F2ED4F87_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <kernel/system/iostream.h> // Added 980901 by Aleksander šhrn.

#include <kernel/basic/string.h>

class HitsMessage  
{
public:
	static void print(const char * s);
	static void warn(const char * s);
	static void print(String &s);
	static void warn(String &s);
	static bool Progress(String & /*s*/){ return true; }
	static bool Progress(char * /*s*/){ return true; }
	HitsMessage();
	virtual ~HitsMessage();

};

#endif // !defined(AFX_HITSMESSAGE_H__F2ED4F87_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
