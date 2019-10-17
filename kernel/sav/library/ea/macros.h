/* -*-C-*-
********************************************************************************
*
* File:         macros.h
* RCS:          $Header: /cvsroot/rosetta/rosetta/src/kernel/sav/library/ea/macros.h,v 1.1.1.1 2000/11/01 21:34:04 aleksander Exp $
* Description:
* Author:       Staal Vinterbo
* Created:      Thu Aug 20 13:22:58 1998
* Modified:     Thu Aug 20 13:40:24 1998 (Staal Vinterbo) staalv@or.idi.ntnu.no
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
*
* (c) Copyright 1998, Staal Vinterbo, all rights reserved.
*
********************************************************************************
*/

#ifndef INCLUDEDE_MACROS_H
#define INCLUDEDE_MACROS_H

#if defined(_MSC_VER)
#pragma warning( disable: 4786; disable: 4804)
#endif

// Changed 990721 by Aleksander Ûhrn.
#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/basic/set.h>
#include <kernel/basic/algorithm.h>

#include <kernel/basic/macros.h>

// Removed 980901 by Aleksander šhrn.
#if 0 
#define MultiMap(type1, type2)                                           \
std::multimap<type1, type2, std::less<type1 > >
#endif 


#endif
 
