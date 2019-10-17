//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __HASHSET_H__
#define __HASHSET_H__

#include <copyright.h>

#include <kernel/basic/macros.h>

#if GCC_STL
	#include <kernel/system/stl/ext/hash_set>
// ext/hash_set
#else
	#include <kernel/system/stl/hash_set>
#endif

#endif
 
