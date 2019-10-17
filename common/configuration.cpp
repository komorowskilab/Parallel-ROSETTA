//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Make sure this file is compiled as part of any
//                 released builds. That way, the build timestamp
//                 is sure to be valid.
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <common/configuration.h>

#include <kernel/basic/message.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/systemkit.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Initialization.
//===================================================================

//-------------------------------------------------------------------
// GUI.
//===================================================================

#if defined(_ROSGUI)
String Configuration::GUI::version_               = "1.5.1";
String Configuration::GUI::gx_version_            = "9.04";
int    Configuration::GUI::gx_font_size_          = 8;
bool   Configuration::GUI::show_types_            = false;
bool   Configuration::GUI::show_units_            = true;
bool   Configuration::GUI::append_files_          = true;
#endif

//-------------------------------------------------------------------
// Kernel.
//===================================================================

String Configuration::Kernel::version_            = "1.3";
//1.2 - Genomic additions.
//1.3 - Threading and Random Reducts.
String Configuration::Kernel::stl_version_        = "STLport-5.0RC2";
bool   Configuration::Kernel::is_verbose_         = true;

//-------------------------------------------------------------------
// Static helpers, file scope.
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticLoadPair
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticLoadPair(ifstream &stream, String &line, String &keyword, String &value) {

	line = "";

	while (!line.Contains('=')) {
		if (!IOKit::LoadLine(stream, line))
			return false;
	}

	keyword = line.Before('=');
	value   = line.After('=');

	keyword.Trim(" \t");
	value.Trim(" \t");

	keyword.ToUppercase();

	return true;

}

//-------------------------------------------------------------------
// Configuration methods.
//===================================================================

String
Configuration::GetCompilationTime() {
	return String(__TIME__);
}

String
Configuration::GetCompilationDate() {
	return String(__DATE__);
}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...: GUI initialization from file.
// Comments......:
// Revisions.....:
//===================================================================

#if defined(_ROSGUI)
bool
Configuration::GUI::Load(const String &filename) {

	// Does the specified file exist?
	if (!IOKit::Exists(filename))
		return false;

	ifstream stream;

	// Open stream.
	if (!IOKit::Open(stream, filename))
		return false;

	String line, keyword, value;

	// Load and dispatch.
	while (StaticLoadPair(stream, line, keyword, value)) {

		// Skip keywords irrelevant to the GUI.
		if (!keyword.StartsWith("GUI::"))
			continue;

		bool processed = false;

		// Font size in grid views.
		if (keyword == "GUI::FONTSIZE" && value.IsInteger()) {
			processed = SetGXFontSize(value.GetInteger());
		}

		// Fate of side-effect text files wrt the project tree.
		else if (keyword == "GUI::APPENDFILES" && value.IsBoolean()) {
			processed = AppendOutputFiles(value.GetBoolean());
		}

		// Show attribute types in decision table view columns headers?
		else if (keyword == "GUI::SHOWTYPES" && value.IsBoolean()) {
			processed = ShowAttributeTypes(value.GetBoolean());
		}

		// Show attribute units in decision table view columns headers?
		else if (keyword == "GUI::SHOWUNITS" && value.IsBoolean()) {
			processed = ShowAttributeUnits(value.GetBoolean());
		}

		// Warning?
		if (!processed) {
			Message::Warning("Could not process line:\n\n" + line);
		}

	}

	return true;

}
#endif

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...: Kernel initialization from file.
// Comments......:
// Revisions.....:
//===================================================================

bool
Configuration::Kernel::Load(const String &filename) {

	// Does the specified file exist?
	if (!IOKit::Exists(filename))
		return false;

	ifstream stream;

	// Open stream.
	if (!IOKit::Open(stream, filename))
		return false;

	String line, keyword, value;

	// Load and dispatch.
	while (StaticLoadPair(stream, line, keyword, value)) {

		// Skip keywords irrelevant to the kernel.
		if (!keyword.StartsWith("KERNEL::"))
			continue;

		bool processed = false;

		// What should the verbosity level of messages be?
		if (keyword == "KERNEL::VERBOSEMESSAGES" && value.IsBoolean()) {
			Configuration::Kernel::IsVerbose(value.GetBoolean());
			processed = true;
		}

		// What is the size of the static buffer used by IOKit?
		if (keyword == "KERNEL::IOKIT::BUFFERSIZE" && value.IsInteger()) {
			IOKit::SetBuffersize(value.GetInteger());
			processed = true;
		}

		// Warning?
		if (!processed) {
			Message::Warning("Could not process line:\n\n" + line);
		}

	}

	return true;

}


 
