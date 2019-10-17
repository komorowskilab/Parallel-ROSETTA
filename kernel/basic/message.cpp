//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <omp.h>

#include <kernel/basic/message.h>

#include <common/messagehelper.h>
#include <string>

using namespace std;

//-------------------------------------------------------------------
// Methods for class Message.
//===================================================================

//-------------------------------------------------------------------
// Initialization of static class variables.
//===================================================================

int  Message::static_counter_   = 0;
bool Message::static_available_ = true;

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Pushes this Message onto the stack.
// Revisions.....:
//===================================================================

Message::Message() {

	// Grab the next available level.
	level_ = static_counter_;

	// Increment shared level counter.
	#pragma omp atomic
	static_counter_++;

	// Create a prefix for indentation purposes.
	if (level_ > 0)
		blanks_ = String(' ', 2 * level_);

	// Initially, don't hog the cancellation flag.
	cancel_ = false;

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Implemented as protected to avoid it from being
//                 used.
// Revisions.....:
//===================================================================

Message::Message(const Message &in) {
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Pop this Message off the stack.
// Revisions.....:
//===================================================================

Message::~Message() {

	// Decrement shared level counter.
	#pragma omp atomic
	static_counter_--;

	// Make the shared cancelletaion flag available again, if held.
	if (cancel_)
		static_available_ = true;

	// "Last one out turns off the light."
	if (static_counter_ == 0)
		MessageHelper::Reset();

}

//-------------------------------------------------------------------
// Methods for class Message.
//===================================================================

//-------------------------------------------------------------------
// Method........: Notify
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
Message::Notify(const String &text) const {
#ifdef _OPENMP
	int this_thread = omp_get_thread_num();
	#pragma omp critical(text)
	{
		cout << "#" << this_thread << ": " << text << endl;
	}
#else
	MessageHelper::Notify(blanks_ + text);
#endif
}

//-------------------------------------------------------------------
// Method........: Notify
// Author........: Nicholas Baltzer
// Date..........: 21/05/13
// Description...: Same as above, but takes a std::string instead.
// Comments......:
// Revisions.....:
//===================================================================

void
Message::Notify(string &text) const {
#ifdef _OPENMP
	int this_thread = omp_get_thread_num();
	#pragma omp critical(text)
	{
		cout << "#" << this_thread << ": " << text << endl;
	}
#else
	MessageHelper::Notify(blanks_ + text);
#endif
}
//-------------------------------------------------------------------
// Method........: Progress
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: 
//===================================================================

bool
Message::Progress() const {
	return Progress("");
}

//-------------------------------------------------------------------
// Method........: Progress
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 000301: Added IsEmpty test.
//===================================================================

bool
Message::Progress(const String &text) const {

	// Can a cancellation facility be offered? Grab shared flag, if available.
	if (!cancel_) {
		if (static_available_) {
			cancel_ = true;
			static_available_ = false;
		}
		else {
			if (!text.IsEmpty())
				Notify(text);
			return true;
		}
	}

	return MessageHelper::Progress(text);

}

//-------------------------------------------------------------------
// Method........: Progress
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Message::Progress(const String &text, int current, int total) const {

	// Grab shared flag, if available. If cancellation is not available, simply return.
	if (!cancel_) {
		if (static_available_) {
			cancel_ = true;
			static_available_ = false;
		}
		else {
			return true;
		}
	}

	bool proceed = MessageHelper::Progress(text, current, total);

	// Pop down windows, if any.
	if (!proceed || (current >= total - 1))
		MessageHelper::Reset();

	return proceed;

}

//-------------------------------------------------------------------
// Method........: Debug, Warning, Error, FatalError
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
Message::Debug(const char *text) {
	MessageHelper::Debug(text);
}

void
Message::Debug(const String &text) {
	MessageHelper::Debug(text);
}

void
Message::Warning(const String &text, bool important) {
	MessageHelper::Warning(text, important);
}

void
Message::Error(const String &text, bool important) {
	MessageHelper::Error(text, important);
}

void
Message::FatalError(const String &text) {
	MessageHelper::FatalError(text);
}



 
