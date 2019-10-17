//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/systemkit.h>

#include <kernel/basic/undefined.h>
#include <kernel/basic/message.h>

#include <kernel/system/time.h>
#include <kernel/system/stdio.h>
#include <kernel/system/direct.h>
#include <kernel/system/sys/stat.h>

//-------------------------------------------------------------------
// Method........: StaticBuildTimestamp
// Author........: Aleksander šhrn
// Date..........:
// Description...: Builds a readable string from a timestamp structure.
// Comments......:
// Revisions.....:
//===================================================================

static String StaticBuildTimestamp(const tm *timestamp) {

	// Valid timestamp?
	if (timestamp == NULL)
		return Undefined::String();

	String year, month, day, hour, minute, second;

	// Build YYYY (year).
	if (timestamp->tm_year < 10)
		year = "190" + String::Format(timestamp->tm_year);
	else if (timestamp->tm_year < 100)
		year = "19" + String::Format(timestamp->tm_year);
	else if (timestamp->tm_year == 100)
		year = "2000";
	else if (timestamp->tm_year < 110)
		year = "200" + String::Format(timestamp->tm_year - 100);
	else
		year = "20" + String::Format(timestamp->tm_year - 100);

	// Build MM (month).
	if ((timestamp->tm_mon + 1) < 10)
		month = "0" + String::Format(timestamp->tm_mon + 1);
	else
		month = String::Format(timestamp->tm_mon + 1);

	// Build DD (day).
	if (timestamp->tm_mday < 10)
		day = "0" + String::Format(timestamp->tm_mday);
	else
		day = String::Format(timestamp->tm_mday);

	// Build HH (hour).
	if (timestamp->tm_hour < 10)
		hour = "0" + String::Format(timestamp->tm_hour);
	else
		hour = String::Format(timestamp->tm_hour);

	// Build MM (minute).
	if (timestamp->tm_min < 10)
		minute = "0" + String::Format(timestamp->tm_min);
	else
		minute = String::Format(timestamp->tm_min);

	// Build SS (second).
	if (timestamp->tm_sec < 10)
		second = "0" + String::Format(timestamp->tm_sec);
	else
		second = String::Format(timestamp->tm_sec);

	// Return the composite timestamp.
	return year + '.' + month + '.' + day + ' ' + hour + ':' + minute + ':' + second;

}

//-------------------------------------------------------------------
// Methods for class SystemKit.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetTimestamp
// Author........: Aleksander šhrn
// Date..........:
// Description...: Retrieves a timestamp from the OS.
// Comments......:
// Revisions.....:
//===================================================================

String
SystemKit::GetTimestamp() {

	tm     *timestamp;
  time_t  timer;

	// Get the system time.
  timer     = time(NULL);
	timestamp = localtime(&timer);

	return StaticBuildTimestamp(timestamp);

}

//-------------------------------------------------------------------
// Method........: GetUser
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the current user.
// Comments......:
// Revisions.....:
//===================================================================

String
SystemKit::GetUser() {
#if defined(_ROSGUI)

	DWORD size = 100;
	char  buffer[100];

	// Get username from OS.
	if (!GetUserName(buffer, &size)) {
		Message::Debug("Failed to get user name.");
		return String("");
	}

	return String(buffer);

#else
	Message::Debug("SystemKit::GetUser() not implemented yet for this compiler/platform.");
	return String("");
#endif
}

//-------------------------------------------------------------------
// Method........: ChangeDirectory
// Author........: Aleksander šhrn
// Date..........:
// Description...: Changes the current, standing directory.
// Comments......:
// Revisions.....:
//===================================================================

bool
SystemKit::ChangeDirectory(const String &directory) {
#if defined(_MSC_VER) // Microsoft Visual C++

	// Change directory.
	return (_chdir(directory.GetBuffer()) == 0);

#else
	Message::Error("SystemKit::ChangeDirectory() not implemented yet for this compiler/platform.");
	return false;
#endif
}

//-------------------------------------------------------------------
// Method........: CreateDirectory
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates a new folder/directory.
// Comments......:
// Revisions.....:
//===================================================================

bool
SystemKit::CreateDirectory(const String &directory) {
#ifdef _MSC_VER // Microsoft Visual C++

	// Make directory.
	return (_mkdir(directory.GetBuffer()) == 0);

#else
	Message::Error("SystemKit::CreateDirectory() not implemented yet for this compiler/platform.");
	return false;
#endif
}

//-------------------------------------------------------------------
// Method........: GetTimestamp
// Author........: Aleksander šhrn
// Date..........:
// Description...: Retrieves a timestamp of a given type from the
//                 specified file.
// Comments......:
// Revisions.....:
//===================================================================

bool
SystemKit::GetTimestamp(const String &filename, String &timestamp, int type) {
#ifdef _MSC_VER // Microsoft Visual C++

  struct _stat status;

  // Get data associated with the file.
	int result = _stat(filename.GetBuffer(), &status);

  // Check if statistics are valid.
  if (result != 0)
		return false;

	time_t time;

	// Get the required information.
	switch (type) {
		case 0:  time = status.st_ctime; break; // Time of creation of file.
		case 1:  time = status.st_mtime; break; // Time of last modification of file.
		case 2:  time = status.st_atime; break; // Time of last access of file.
		default: return false;
	}

	timestamp = StaticBuildTimestamp(localtime(&time));

	return (timestamp != Undefined::String());

#else
	Message::Error("SystemKit::GetTimestamp(...) not implemented yet for this compiler/platform.");
	return false;
#endif
}

