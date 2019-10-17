//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PERMUTER_H__
#define __PERMUTER_H__

#include <copyright.h>

#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>

//-------------------------------------------------------------------
// Class.........: Permuter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class for creating a permutation of the
//                 index set {0, ..., n - 1} such that the
//                 permutation is sorted according to a given vector
//                 of sorting keys.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
class Permuter {
private:

	//- STL helper class...............................................
	struct IndexComparator {
	private:

		//- Comparison keys..............................................
		const Vector(T) *keys_;

		//- Constructor..................................................
		IndexComparator() {keys_ = NULL;}

	public:

		//- Comparison operator..........................................
		bool operator()(int i, int j) const {
			return (*keys_)[i] < (*keys_)[j];
		}

		//- Constructor..................................................
		IndexComparator(const Vector(T) *keys) {keys_ = keys;}

	};

public:

	//- Constructors/destructor........................................
	Permuter() {};
	~Permuter() {};

	//- Methods........................................................
	bool Permute(const Vector(T) &keys, Vector(int) &permutation) const;

};

//-------------------------------------------------------------------
// Method........: Permute
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Permuter<T>::Permute(const Vector(T) &keys, Vector(int) &permutation) const {

	if (!permutation.empty())
		permutation.erase(permutation.begin(), permutation.end());

	permutation.reserve(keys.size());

	int i;

	for (i = 0; i < keys.size(); i++)
		permutation.push_back(i);

	IndexComparator compare(&keys);

	std::sort(permutation.begin(), permutation.end(), compare);

	return true;

}

#endif 
