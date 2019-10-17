//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <copyright.h>

#include <kernel/basic/types.h>
#include <kernel/basic/string.h>

#include <kernel/system/stdlib.h>

//-------------------------------------------------------------------
// Class.........: Array
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Templatized array/vector that can grow dynamically.
//
// Comments......: Vectors/arrays are in use "everywhere", and STL is
//                 conveniently used for such containers. This
//                 class was developed in order to have complete control
//                 of memory allocation, for two reasons:
//
//                 1) STL sometimes allocates memory in
//                    larger chunks than desired.
//
//                 2) It simplifies a later introduction of
//                    techniques for "memory pooling", which can
//                    significantly reduce memory fragmentation.
//
//                 For reasons of backwards compatibility and
//                 component interchangeability, this class should be
//                 interchangeable with STL vectors under most
//                 circumstances.
//
//                 Currently, this template can only be used for primitive
//                 types or types that provide an empty constructor
//                 (are "default constructable") and an assignment
//                 operator (are "assignable").
//
// Revisions.....:
//===================================================================

template <class T>
class Array {
private:

	//- Internal representation........................................
  T              *elements_; // Buffer of elements.
	int             capacity_; // Physical length of element buffer.
	int             size_;     // Logical length of element buffer.

private:

	//- Helper methods.................................................
	int             GetIndex(const T *position) const;

public:

	//- Constructors/destructor........................................
  Array();
  Array(int size, const T &element);
  Array(const Array<T> &in);
	Array(int size, T *elements, bool copy);
  ~Array();

	//- Dimensional methods............................................
  int             GetSize() const {return size_;}
  bool            IsEmpty() const {return (size_ == 0);}

	//- Common mutation methods........................................
	bool            Clear();

	bool            Insert(int i, const T &element);
	bool            Insert(int i, int n, const T &element);
	bool            Insert(int i, const Array<T> &in);

	bool            Append(const T &element);
	bool            Append(int n, const T &element);
	bool            Append(const Array<T> &in);

	bool            Remove(int i);
	bool            Remove(int i, int j);

	//- Operators......................................................
  const T        &operator[](int i) const {return elements_[i];}
  T              &operator[](int i) {return elements_[i];}

  Array<T>       &operator=(const Array<T> &in);

  friend bool     operator==(const Array<T> &x, const Array<T> &y);
  friend bool     operator!=(const Array<T> &x, const Array<T> &y);

  friend bool     operator<(const Array<T> &x, const Array<T> &y);

	//- Storage methods................................................
	bool            Reserve(int capacity, bool preserve = true);

	//- Creation methods...............................................
	bool            Create(int size, T *elements, bool copy, bool destroy = true);

	//- Types and methods for crude STL compatibility..................
	typedef         T *       iterator;
	typedef         const T * const_iterator;

	iterator        begin() {return elements_;}
	const_iterator  begin() const {return elements_;}
	iterator        end() {return elements_ + size_;}
	const_iterator  end() const {return elements_ + size_;}
	bool            empty() const {return IsEmpty();}
	int             size() const {return size_;}
	void            reserve(int size) {Reserve(size, true);}
	void            push_back(const T &element) {Append(element);}
	void            pop_back() {Remove(size_ - 1, size_);}
	iterator        insert(iterator position, const T &element) {
										int i = GetIndex(position);
										Insert(i, element);
										return elements_ + i;
									}
	void            insert(iterator position, int n, const T &element) {Insert(GetIndex(position), n, element);}
	void            insert(iterator position, iterator first, iterator last) {
										int i = GetIndex(position);
										Array<T> tmp;
										tmp.Create(last - first, first, false, false);
										Insert(i, tmp);
										tmp.Create(0, NULL, false, false);
									}
	void            erase(iterator position) {Remove(GetIndex(position));}
	void            erase(iterator first, iterator last) {Remove(GetIndex(first), GetIndex(last));}

	//- Miscellaneous..................................................
	String          Format() const;

};

//-------------------------------------------------------------------
// Methods for class Vector.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetIndex
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Used for STL compatibility. Given an "iterator",
//                 returns the corresponding index using pointer
//                 arithmetic.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline int Array<T>::GetIndex(const T *position) const {
	return (position - elements_);
}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: Consider more optimal reallocation.
// Revisions.....:
//===================================================================

template <class T>
bool Array<T>::Reserve(int capacity, bool preserve) {

	int i;

	// Already enough space reserved?
	if (capacity <= capacity_)
		return true;

	// Allocate new buffer.
	T *elements = new T[capacity];

	// Copy contents of old buffer into new buffer, if specified and possible.
	if (preserve && elements_ != NULL) {
		for (i = 0; i < capacity_; i++) {
			elements[i] = elements_[i];
		}
	}

	// Delete old buffer.
	if (elements_ != NULL)
		delete [] elements_;

	// Reassign internal representation.
	elements_ = elements;
	capacity_ = capacity;

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
bool Array<T>::Create(int size, T *elements, bool copy, bool destroy) {

	int i;

	if (destroy && elements_ != NULL)
		delete [] elements_;

	size_     = size;
	capacity_ = size;

	if (!copy) {
		elements_ = elements;
	}
	else {
		if (elements != NULL) {
			elements_ = new T[size];
			for (i = 0; i < size; i++) {
				elements_[i] = elements[i];
			}
		}
		else {
			elements_ = NULL;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
Array<T>::Array() {

  elements_ = NULL;
	capacity_ = 0;
	size_     = 0;

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
Array<T>::Array(int size, const T &element) {

#if defined(_DEBUG)
	if (size < 0)
		size = -size;
#endif

	// Initialize before allocation.
	elements_ = NULL;
	capacity_ = 0;
	size_     = 0;

	// Allocate buffer.
	Reserve(size, false);

	size_ = size;

	int i;

	// Initialize contents.
	for (i = 0; i < size; i++)
		elements_[i] = element;

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
Array<T>::Array(const Array<T> &in) {

	int i;

	capacity_ = in.capacity_;
	size_     = in.size_;

	if (in.elements_ != NULL) {
		elements_ = new T[capacity_];
		for (i = 0; i < capacity_; i++) {
			elements_[i] = in.elements_[i];
		}
	}
	else {
		elements_ = NULL;
	}

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
Array<T>::Array(int size, T *elements, bool copy) {

	elements_ = NULL;
	capacity_ = 0;
	size_     = 0;

	if (!Create(size, elements, copy)) {
		elements_ = NULL;
		capacity_ = 0;
		size_     = 0;
	}

}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
Array<T>::~Array() {

	if (elements_ != NULL)
		delete [] elements_;

}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Array<T>::Clear() {

	if (elements_ != NULL)
		delete [] elements_;

	elements_ = NULL;
	capacity_ = 0;
	size_     = 0;

	return true;

}

//-------------------------------------------------------------------
// Method........: Insert
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Inserts an array at position i.
// Comments......: Consider more optimal reallocation.
// Revisions.....:
//===================================================================

template <class T>
bool Array<T>::Insert(int i, const Array<T> &in) {

	int j;

#if defined(_DEBUG)
	if (i < 0 || i > size_)
		return false;
#endif

	int n = in.size_;

	if (n == 0)
		return true;

	if (size_ + n > capacity_) { // Capacity exceeded, reallocate.

		// Compute physical size of new buffer.
		int capacity = size_ + n;

		// Allocate new buffer.
		T *elements = new T[capacity];

		// Copy already existing elements that lie before insertion point.
		if (elements_ != NULL && i > 0) {
			for (j = 0; j < i; j++) {
				elements[j] = elements_[j];
			}
		}

		T *ptr1 = elements + i;
		T *ptr2 = in.elements_;

		// Insert new elements at insertion point.
		for (j = 0; j < n; j++, ptr1++, ptr2++)
			*ptr1 = *ptr2;

		// Copy already existing elements that lie after insertion point.
		if (elements_ != NULL && i < size_) {
			for (j = 0; j < size_ - i; j++) {
				elements[i + n + j] = elements_[i + j];
			}
		}

		// Delete old buffer.
		delete [] elements_;

		// Update internal representation.
		elements_ = elements;
		capacity_ = capacity;

	}
	else {                       // There's still room!

		// Move existing elements that lie after insertion point.
		if (i < size_) {
			for (j = 0; j < size_ - i; j++) {
				elements_[i + n + j] = elements_[i + j];
			}
		}

		T *ptr1 = elements_ + i;
		T *ptr2 = in.elements_;

		int j;

		// Insert new elements at insertion point.
		for (j = 0; j < n; j++, ptr1++, ptr2++)
			*ptr1 = *ptr2;

	}

	size_ += n;

	return true;

}

//-------------------------------------------------------------------
// Method........: Insert
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: At position i, inserts n copies of the element.
// Comments......: Consider more optimal reallocation.
// Revisions.....:
//===================================================================

template <class T>
bool Array<T>::Insert(int i, int n, const T &element) {

	int j;

#if defined(_DEBUG)
	if (i < 0 || i > size_)
		return false;
	if (n < 0)
		return false;
#endif

	if (n == 0)
		return true;

	if (size_ + n > capacity_) { // Capacity exceeded, reallocate.

		// Compute physical size of new buffer.
		// Anticipate future insertions to minimize reallocations.
		int capacity = 2 * capacity_;
		if (capacity == 0)
			capacity = 1;
		if (capacity < size_ + n)
			capacity = size_ + n;

		// Allocate new buffer.
		T *elements = new T[capacity];

		// Copy already existing elements that lie before insertion point.
		if (elements_ != NULL && i > 0) {
			for (j = 0; j < i; j++) {
				elements[j] = elements_[j];
			}
		}

		T *ptr = elements + i;

		// Insert new elements at insertion point.
		for (j = 0; j < n; j++, ptr++)
			*ptr = element;

		// Copy already existing elements that lie after insertion point.
		if (elements_ != NULL && i < size_) {
			for (j = 0; j < size_ - i; j++) {
				elements[i + n + j] = elements_[i + j];
			}
		}

		// Delete old buffer.
		delete [] elements_;

		// Update internal representation.
		elements_ = elements;
		capacity_ = capacity;

	}
	else {                       // There's still room!

		// Move existing elements that lie after insertion point.
		if (i < size_) {
			for (j = 0; j < size_ - i; j++) {
				elements_[i + n + j] = elements_[i + j];
			}
		}

		T *ptr = elements_ + i;

		int j;

		// Insert new elements at insertion point.
		for (j = 0; j < n; j++, ptr++)
			*ptr = element;

	}

	size_++;

	return true;

}

//-------------------------------------------------------------------
// Method........: Insert
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
bool Array<T>::Insert(int i, const T &element) {
	return Insert(i, 1, element);
}

//-------------------------------------------------------------------
// Method........: Append
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Array<T>::Append(const T &element) {
	return Insert(GetSize(), element);
}

//-------------------------------------------------------------------
// Method........: Append
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Array<T>::Append(int n, const T &element) {
	return Insert(GetSize(), n, element);
}

//-------------------------------------------------------------------
// Method........: Append
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Array<T>::Append(const Array<T> &in) {
	return Insert(GetSize(), in);
}

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Removes all entries with indices in [i, j).
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Array<T>::Remove(int i, int j) {

	if (i == 0 && j == size_)
		return Clear();

#if defined(_DEBUG)
	if (i < 0 || i >= size_)
		return false;
	if (j < 0 || j > size_)
		return false;
	if (i >= j)
		return false;
#endif

	int k;

	for (k = 0; k < size_ - j; k++) {
		elements_[i + k] = elements_[j + k];
	}

	size_ -= (j - i);

	return true;

}

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Array<T>::Remove(int i) {
	return Remove(i, i + 1);
}

//-------------------------------------------------------------------
// Method........: Assignment operator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
Array<T> &Array<T>::operator=(const Array<T> &in) {

	int i;

	if (&in == this)
		return *this;

	// Can we reuse the current buffer?
	if (capacity_ >= in.size_) {
	}
	else {
		if (elements_ != NULL)
			delete [] elements_;
		capacity_ = in.size_;
		if (capacity_ > 0)
			elements_ = new T[capacity_];
		else
			elements_ = NULL;
	}

	size_ = in.size_;

	// Copy buffer.
	if (elements_ != NULL && in.elements_ != NULL) {
		for (i = 0; i < size_; i++) {
			elements_[i] = in.elements_[i];
		}
	}

	return *this;

}

//-------------------------------------------------------------------
// Method........: operator ==
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool operator==(const Array<T> &x, const Array<T> &y) {

	if (&x == &y)
		return true;

	if (x.size_ != y.size_)
		return false;

	if (x.size_ == 0)
		return true;
	else
		return (0 == memcmp(x.elements_, y.elements_, x.size_ * sizeof(T)));

}

//-------------------------------------------------------------------
// Method........: operator !=
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool operator!=(const Array<T> &x, const Array<T> &y) {
	return !(x == y);
}

//-------------------------------------------------------------------
// Method........: operator <
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Lexicographical comparison, x < y.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool operator<(const Array<T> &x, const Array<T> &y) {

	if (&x == &y)
		return false;

	if (y.size_ == 0)
		return false;

	if (x.size_ == 0)
		return true;

	int size = (x.size_ < y.size_) ? x.size_ : y.size_;

	return (0 > memcmp(x.elements_, y.elements_, size * sizeof(T)));

}

//-------------------------------------------------------------------
// Method........: Format
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
String Array<T>::Format() const {

	int i;

	String s;

	for (i = 0; i < GetSize(); i++) {
		s += String::Format((*this)[i]);
		if (i < GetSize() - 1)
			s += ", ";
	}

  return s;

}

#endif
 
