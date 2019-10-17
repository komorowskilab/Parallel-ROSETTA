//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <common/messagehelper.h>

#if defined(_ROSGUI)
#include <frontend/mainfrm.h>
#include <frontend/dialogs/algorithmdialogs/dialogprogress.h>
#include <frontend/dialogs/algorithmdialogs/dialogcontinue.h>
#endif

//-------------------------------------------------------------------
// Methods for class MessageHelper.
//===================================================================

#if defined(_ROSGUI)
CDialogProgress *MessageHelper::progress_dialog_ = NULL;
CDialogContinue *MessageHelper::continue_dialog_ = NULL;
#endif

//-------------------------------------------------------------------
// Method........: Progress
// Author........: Aleksander šhrn
// Date..........: 960417
//
// Description...: If queried by an algorithm, enables the user to
//                 terminate the ongoing calculation if the Boolean
//                 argument is true.
//
//                 If the percentage of work done is known or can be
//                 estimated, use the appropriate Progress method
//                 instead.
//
// Comments......: The dialog must be popped down from elsewhere.
//
// Revisions.....: Aš 000301: Added IsEmpty test.
//===================================================================

bool
MessageHelper::Progress(const String &text) {

#if defined(_ROSGUI)

	// If the dialog does not exist, create it.
	if (continue_dialog_ == NULL) {
		continue_dialog_ = new CDialogContinue;
		continue_dialog_->Create();
	}

	// Update dialog text, avoiding flickering.
	if (!text.IsEmpty()) {
		if (text != continue_dialog_->GetStatus())
			continue_dialog_->SetStatus(text.GetBuffer());
	}

	// Check for cancellation.
	if (continue_dialog_->CheckCancelButton()) {
		if (Query("Are you sure you want to cancel?", "Cancel", BUTTONS_YESNO) == RESPONSE_YES) {
			delete continue_dialog_;
			continue_dialog_ = NULL;
			return false;
		}
	}

#elif PARALLEL

	MessageSink::CollectText(0, text);

#else

	// Dump text to stdout.
	cout << text << endl;

#endif

	return true;

}

//-------------------------------------------------------------------
// Method........: Progress
// Author........: Aleksander šhrn
// Date..........: 960417
//
// Description...: Displays a progress bar indicating percentage
//                 of progress.
//
// Comments......: The dialog must be popped down from elsewhere.
// Revisions.....: 
//===================================================================

bool
MessageHelper::Progress(const String &text, int current, int total) {

#if defined(_ROSGUI)

	// If the dialog does not exist, create it.
  if (progress_dialog_ == NULL) {
    progress_dialog_ = new CDialogProgress;
    progress_dialog_->Create();
    progress_dialog_->SetRange(0, total);
    progress_dialog_->SetStep(total);
  }

	// Update progress dialog.
	progress_dialog_->SetPos(current);
	progress_dialog_->SetStatus(text.GetBuffer());

	// Check for cancellation.
  if (progress_dialog_->CheckCancelButton()) {
		if (Query("Are you sure you want to cancel?", "Cancel", BUTTONS_YESNO) == RESPONSE_YES) {
			delete progress_dialog_;
			progress_dialog_ = NULL;
      return false;
    }
  }

#else

	/*
	// Compute percentage, avoiding overflow.
	int percent = static_cast(int, 100 * (static_cast(float, current) / total));

	cout << text << " (" << percent << "%)" << endl;
	*/

	/*
	// What message was previously issued?
	static String previous;

	if (text != previous) {
		cout << text << endl;
		previous = text;
	}
	*/

#endif

  return true;

}

//-------------------------------------------------------------------
// Method........: Notify
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
MessageHelper::Notify(const String &text) {
#if defined(_ROSGUI)
	CMainFrame *mainframe = (CMainFrame *) AfxGetMainWnd();
	if (mainframe != NULL) {
		CString ctext(text.GetBuffer());
		mainframe->ProgressAddString(ctext);
		mainframe->StatusBarSetText(ctext);
	}

#elif PARALLEL

	MessageSink::CollectText(0, text);

#else
	
	cout << text << endl;
#endif
}

//-------------------------------------------------------------------
// Method........: Debug
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: For calls like Debug("Hello!"), this method is
//                 invoked. Hence, if _DEBUG is not defined,
//                 then there is no need to create a temporary String
//                 object.
// Revisions.....:
//===================================================================

void
MessageHelper::Debug(const char *text) {
#if defined(_DEBUG)
	if (text != NULL)
		Debug(String(text));
#endif
}

//-------------------------------------------------------------------
// Method........: Debug
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
MessageHelper::Debug(const String &text) {
#if defined(_DEBUG)
	#if defined(_ROSGUI) || defined(_MSGGUI)
		Warning("DEBUG: " + text, false);

	#elif PARALLEL

		MessageSink::CollectText(0, "DEBUG: " + text);

	#else

		cout << "DEBUG: " << text << endl;

	#endif
#endif
}

//-------------------------------------------------------------------
// Method........: Warning
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
MessageHelper::Warning(const String &text, bool important) {
#if defined(_ROSGUI) || defined(_MSGGUI)
	if (important) {
		MessageBeep(MB_ICONEXCLAMATION);
		Query(text, "Warning", BUTTONS_OK, ICON_EXCLAMATION);
	}
#if defined(_ROSGUI)
	CMainFrame *mainframe = (CMainFrame *) AfxGetMainWnd();
	if (mainframe != NULL) {
		CString ctext(text.GetBuffer());
		mainframe->WarningAddString(ctext);
		mainframe->StatusBarSetText(ctext);
	}
#endif

#elif PARALLEL

	MessageSink::CollectText(0, "WARNING: " + text);

#else
	cerr << "WARNING: " << text << endl;
#endif
}

//-------------------------------------------------------------------
// Method........: Error
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void

MessageHelper::Error(const String &text, bool important) {
#if defined(_ROSGUI) || defined(_MSGGUI)
	if (important) {
		MessageBeep(MB_ICONASTERISK);
		Query(text, "Error", BUTTONS_OK, ICON_STOP);
	}
#if defined(_ROSGUI)
	CMainFrame *mainframe = (CMainFrame *) AfxGetMainWnd();
	if (mainframe != NULL) {
		CString ctext(text.GetBuffer());
		mainframe->WarningAddString(ctext);
		mainframe->StatusBarSetText(ctext);
	}
#endif

#elif PARALLEL

	MessageSink::CollectText(0, "ERROR: " + text);

#else
	cerr << "ERROR: " << text << endl;
#endif
}

//-------------------------------------------------------------------
// Method........: FatalError
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
MessageHelper::FatalError(const String &text) {
#if defined(_ROSGUI) || defined(_MSGGUI)
	MessageBeep(MB_ICONASTERISK);
	Query(text + "\n\nPlanet Earth will self-destruct in seven minutes.", "Fatal error", BUTTONS_OK, ICON_STOP);

#elif PARALLEL

	MessageSink::CollectText(0, "FATAL ERROR: " + text);
	MessageSink::Synchronize();
	MessageSink::Flush();
	MessageSink::End();

#else
	cerr << "FATAL ERROR: " << text << endl;
#endif

	exit(1);
}

//-------------------------------------------------------------------
// Method........: Reset
// Author........: Aleksander šhrn
// Date..........:
// Description...: Ensures that everything is reset: All message
//                 dialogs popped down, etc.
// Comments......:
// Revisions.....:
//===================================================================

void
MessageHelper::Reset() {
#if defined(_ROSGUI)

	// Pop down dialogs.
	if (progress_dialog_ != NULL) {
		delete progress_dialog_;
		progress_dialog_ = NULL;
	}

	if (continue_dialog_ != NULL) {
		delete continue_dialog_;
		continue_dialog_ = NULL;
	}

#endif
}

//-------------------------------------------------------------------
// Method........: Information
// Author........: Aleksander šhrn
// Date..........: 960417
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI) || defined(_MSGGUI)
void
MessageHelper::Information(const String &text) {
	MessageBeep(MB_OK);
	Query(text, "Information", BUTTONS_OK, ICON_INFORMATION);
}
#endif

//-------------------------------------------------------------------
// Method........: Query
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI) || defined(_MSGGUI)
MessageHelper::Response
MessageHelper::Query(const String &text, const String &caption, MessageHelper::Buttons buttons, MessageHelper::Icon icon) {

	int          response;
	unsigned int style = 0;

	// Convert from class enums to MFC specific button flags.
	switch (buttons) {
		case BUTTONS_ABORTRETRYIGNORE: style |= MB_ABORTRETRYIGNORE; break;
		case BUTTONS_OK:               style |= MB_OK;               break;
		case BUTTONS_OKCANCEL:         style |= MB_OKCANCEL;         break;
		case BUTTONS_RETRYCANCEL:      style |= MB_RETRYCANCEL;      break;
		case BUTTONS_YESNO:            style |= MB_YESNO;            break;
		case BUTTONS_YESNOCANCEL:      style |= MB_YESNOCANCEL;      break;
		default:                       style |= MB_OK;
	}

	// Convert from class enums to MFC specific icon flags.
	switch (icon) {
		case ICON_EXCLAMATION: style |= MB_ICONEXCLAMATION; break;
		case ICON_INFORMATION: style |= MB_ICONINFORMATION; break;
		case ICON_QUESTION:    style |= MB_ICONQUESTION;    break;
		case ICON_STOP:        style |= MB_ICONSTOP;        break;
		default:               style |= MB_ICONINFORMATION;
	}

	// Define message modality flag.
	style |= MB_APPLMODAL;

	// Display the message box and wait for user response.
	response = MessageBox(NULL, text.GetBuffer(), caption.GetBuffer(), style);

	// Convert from MFC specific response flags to class enums.
	switch (response) {
		case IDABORT:  return RESPONSE_ABORT;
		case IDCANCEL: return RESPONSE_CANCEL;
		case IDIGNORE: return RESPONSE_IGNORE;
		case IDNO:     return RESPONSE_NO;
    case IDOK:     return RESPONSE_OK;
		case IDRETRY:  return RESPONSE_RETRY;
		case IDYES:    return RESPONSE_YES;
	}

	// This code should never be reached.
	return RESPONSE_CANCEL;

}
#endif
 
