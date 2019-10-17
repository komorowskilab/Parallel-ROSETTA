//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MESSAGEHELPER_H__
#define __MESSAGEHELPER_H__

#include <copyright.h>

#if defined(_ROSGUI)
	#include <frontend/dialogs/algorithmdialogs/dialogprogress.h>
	#include <frontend/dialogs/algorithmdialogs/dialogcontinue.h>
#endif

#ifdef PARALLEL
	#include <kernel/parallel/messagesink.h>
#endif

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: MessageHelper
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class for Message.
// Comments......: Should not be called directly from the kernel,
//                 use Message class instead.
// Revisions.....:
//===================================================================

class MessageHelper {
private:

#if defined(_ROSGUI)
	//- MFC specific message routing...................................
  static CDialogProgress  *progress_dialog_; // Dialog with progress bar.
  static CDialogContinue  *continue_dialog_; // Dialog without progress bar.
#endif

public:

  //- Methods for long operations....................................
  static bool              Progress(const String &text);
	static bool              Progress(const String &text, int current, int total);

  //- Methods for warnings, error messages, etc......................
	static void              Notify(const String &text);
  static void              Debug(const char *text);
  static void              Debug(const String &text);
  static void              Warning(const String &text, bool important = true);
  static void              Error(const String &text, bool important = true);
  static void              FatalError(const String &text);

	//- Methods for clean-up...........................................
	static void              Reset();

public:

#if defined(_ROSGUI) || defined(_MSGGUI)
	//- Enumerated types...............................................
	enum Icon     {ICON_EXCLAMATION, ICON_INFORMATION, ICON_QUESTION, ICON_STOP};
	enum Buttons  {BUTTONS_ABORTRETRYIGNORE, BUTTONS_OK, BUTTONS_OKCANCEL,
		             BUTTONS_RETRYCANCEL, BUTTONS_YESNO, BUTTONS_YESNOCANCEL};
	enum Response {RESPONSE_ABORT, RESPONSE_CANCEL, RESPONSE_IGNORE,
		             RESPONSE_NO, RESPONSE_OK, RESPONSE_RETRY, RESPONSE_YES};

  //- Methods that require user response or acknowledgement..........
  static void              Information(const String &text);
	static Response          Query(const String &text, const String &caption, Buttons buttons, Icon icon = ICON_QUESTION);
#endif

};

#endif
 
