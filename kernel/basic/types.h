//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....: After upgrading the MSVC++ compiler from version
//                 4.0 to 4.2, the definitions of bool/true/false
//                 had to be altered, and the header file yvals.h
//                 included.
//
//                 AÛ 990323: Commented away the bool stuff, since
//                            it didn't quite work when compiling
//                            under UNIX using egcs 1.1.2... Assume
//                            that the compiler is current enough to
//                            recognize the bool type.
//===================================================================

#ifndef __TYPES_H__
#define __TYPES_H__

#include <copyright.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class String;

//-------------------------------------------------------------------
// Type declarations.
//===================================================================

typedef int Id;

//-------------------------------------------------------------------
// Boolean.
//===================================================================

/*
#if defined(_MSC_VER) && (_MSC_VER > 1000)   // Microsoft Visual C++, version higher than 4.0.
	#include <kernel/system/yvals.h>
#else
	#if !defined(bool)                         // Avoid conflict with STL.
		typedef int bool;
	#endif
	#if !defined(false)                        // Avoid conflict with STL.
		const int false = 0;
	#endif
	#if !defined(true)                         // Avoid conflict with STL.
		const int true  = 1;
	#endif
#endif
*/

#endif
 
