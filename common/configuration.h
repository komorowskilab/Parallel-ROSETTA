//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: Configuration
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Static scope around system configuration variables.
// Revisions.....:
//===================================================================

class Configuration {
public:

	//- Common methods.................................................
	static String GetCompilationTime();
	static String GetCompilationDate();

  //- Kernel configuration...........................................
	class Kernel {
	private:

		//- Kernel state variables.......................................
		static String version_;           // Version of kernel.
		static String stl_version_;       // Version of STL used.
		static bool   is_verbose_;        // Verbose message level?

	public:

		//- Kernel methods...............................................
		static String GetVersion() {return version_;}
		static bool   SetVersion(const String &version) {version_ = version; return true;}
		static String GetSTLVersion() {return stl_version_;}
		static bool   SetSTLVersion(const String &version) {stl_version_ = version; return true;}
		static bool   IsVerbose() {return is_verbose_;}
		static bool   IsVerbose(bool is_verbose) {is_verbose_ = is_verbose; return true;}

		//- Kernel methods, I/O..........................................
		static bool   Load(const String &filename);

};

#if defined(_ROSGUI)
  //- GUI front-end configuration....................................
	class GUI {
	private:

		//- GUI state variables..........................................
		static String version_;      // Version of GUI front-end.
		static String gx_version_;   // Version of Grid Extension used in GUI front-end.
		static int    gx_font_size_; // Standard font size for GUI views.
		static bool   show_types_;   // Show attribute types in decision table GUI views?
		static bool   show_units_;   // Show attribute units in decision table GUI views?
		static bool   append_files_; // Append output file icons in the GUI?

	public:

		//- GUI methods....................................................
		static String GetVersion() {return version_;}
		static bool   SetVersion(const String &version) {version_ = version; return true;}
		static String GetGXVersion() {return gx_version_;}
		static bool   SetGXVersion(const String &version) {gx_version_ = version; return true;}
		static int    GetGXFontSize() {return gx_font_size_;}
		static bool   SetGXFontSize(int gx_font_size) {if (gx_font_size <= 0 || gx_font_size > 50) return false; gx_font_size_ = gx_font_size; return true;}
		static bool   ShowAttributeTypes() {return show_types_;}
		static bool   ShowAttributeTypes(bool show_types) {show_types_ = show_types; return true;}
		static bool   ShowAttributeUnits() {return show_units_;}
		static bool   ShowAttributeUnits(bool show_units) {show_units_ = show_units; return true;}
		static bool   AppendOutputFiles() {return append_files_;}
		static bool   AppendOutputFiles(bool append_files) {append_files_ = append_files; return true;}

	  //- GUI methods, I/O...............................................
		static bool   Load(const String &filename);

	};
#endif

};

#endif 
