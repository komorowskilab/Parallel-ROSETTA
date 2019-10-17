//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 
// Description...: Include file for standard system include files,
//                 or project specific include files that are used
//                 frequently, but are changed infrequently.
// Revisions.....:
//===================================================================

#define VC_EXTRALEAN		                   // Exclude rarely-used stuff from Windows headers

#if defined(_MSC_VER)
	#pragma warning(disable: 4786)           // Disable warning about truncated debug information in templates.
	#pragma warning(disable: 4097)           // Disable warning about typedef stuff deep down in STL.
	#if defined(_ROSGUI)
		#pragma message("Compiling with the ROSETTA GUI.")
	#else
		#pragma message("Compiling without the ROSETTA GUI.")
	#endif
#endif

#if defined(_MSC_VER) && defined(_WINDOWS)
	#include <afxwin.h>                      // MFC core and standard components
	#include <afxext.h>                      // MFC extensions
	#include <afxdisp.h>                     // MFC OLE automation classes
	#if !defined(_AFX_NO_AFXCMN_SUPPORT)
		#include <afxcmn.h>			               // MFC support for Windows 95 Common Controls
	#endif
#endif

#if defined(_ROSGUI)
	#include <config/gxstndrd.h>             // Objective Grid (Lite)
	#include <gxall.h>                       // Objective Grid (Lite)
#endif





 
