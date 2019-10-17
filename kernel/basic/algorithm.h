//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BASIC_ALGORITHM_H__
#define __BASIC_ALGORITHM_H__

#include <copyright.h>

#if 0
	#include <kernel/system/stl/algorithm>
	#include <kernel/system/stl/numeric>
	#ifdef _ROSGUI
		#undef min
		#undef max
	#endif
#else
	#include <algorithm>
	#include <functional>
	#include <numeric>
	#if defined(min)
		#undef min
		namespace std {
			template <class T>
				inline const T &min(const T &a, const T &b) {
					return a < b ? a : b;
				}
		}
	#endif
	#if defined(max)
		#undef max
		namespace std {
			template <class T>
				inline const T &max(const T &a, const T &b) {
					return b < a ? a : b;
				}
		}
	#endif
#endif

#endif
 
