//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <copyright.h>

#include <kernel/basic/string.h>
#include <kernel/basic/map.h>	// sus
#include <string>
//-------------------------------------------------------------------
// Class.........: Message
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simple class that relays progress messages to the
//                 user.
//
//                 Intended use, example:
//
//                    // Declare a local message object.
//                    Message message;
//
//                    // The Notify method is used for general progress messages.
//                    message.Notify("A long operation is about to be executed...");
//
//                    // A long operation might, e.g., take place in a loop.
//                    for (int i = 0; i < 1000; i++) {
//
//                      // The Progress method is used to provide a cancellation facility.
//                      if (!message.Progress("Executing...", i, 1000))
//                         break;
//
//                    }
//
//                    // For warnings and errors the static methods may be used.
//                    Message::Warning("This is a warning message.");
//                    Message::Warning("This is a less important warning message.", false);
//
// Comments......:
// Revisions.....: AÛ 990528: Added stack stuff.
//===================================================================

class Message {
private:

	//- Static variables...............................................
	static int                  static_counter_;   // "Global" message level counter.
	static bool                 static_available_; // "Global" flag indicating if cancellation flag is currently available for taking.

	//- Local variables................................................
	int                         level_;            // Message level for this object.
	mutable bool                cancel_;           // Does this object hold the cancellation flag?
	String                      blanks_;           // String of blanks, for indentation.

protected:

  //- Constructors...................................................
  Message(const Message &in);

public:

  //- Constructors/destructor........................................
  Message();
  virtual ~Message();

  //- Messaging methods..............................................
  virtual void                Notify(const String &text) const;
	//Support for std::string. -N 21/05/13
	virtual void	Notify(std::string &text) const;

  virtual bool                Progress() const;
  virtual bool                Progress(const String &text) const;
	virtual bool                Progress(const String &text, int current, int total) const;

  static void                 Debug(const char *text);
  static void                 Debug(const String &text);
  static void                 Warning(const String &text, bool important = true);
  static void                 Error(const String &text, bool important = true);
  static void                 FatalError(const String &text);

};

//-------------------------------------------------------------------
// Class.........: MessageProxy
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simple trick for resolving name conflict with RSES.
// Comments......:
// Revisions.....:
//===================================================================

class MessageProxy : public Message {
protected:

  //- Constructors...................................................
  MessageProxy(const MessageProxy &in) {}

public:

  //- Constructors/destructor........................................
  MessageProxy() {}
  virtual ~MessageProxy() {}

};

#endif 
