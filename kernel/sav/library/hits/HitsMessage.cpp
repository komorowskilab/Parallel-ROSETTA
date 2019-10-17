// HitsMessage.cpp: implementation of the HitsMessage class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "HitsMessage.h"

#include <kernel/basic/message.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HitsMessage::HitsMessage()
{

}

HitsMessage::~HitsMessage()
{

}

void HitsMessage::warn(String &s)
{
	//cerr << s;
	Message::Debug(s);
}

void HitsMessage::print(String & s)
{
	//cout << s;
	Message().Notify(s); // Added 990526 by Aleksander šhrn.
	//Message::Progress(s);
}

void HitsMessage::warn(const char * s)
{
	//cerr << s;
	Message::Debug(s);
}

void HitsMessage::print(const char * s)
{
	//cout << s;
	Message().Notify(s); // Added 990526 by Aleksander šhrn.
	//Message::Progress(s);
}
 
