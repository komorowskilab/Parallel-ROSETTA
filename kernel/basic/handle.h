//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: General template class for handles.  Smart pointer
//                 to a class object.
// Comments......:
// Revisions.....:
//===================================================================

#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <copyright.h>

//#include <stdafx>
#include <kernel/basic/types.h>
#include <kernel/basic/macros.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Class.........: Handle
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Provides a smart pointer to class objects.
//                 The advantage of using handles instead of standard
//                 C++ pointers is that the memory management of (large)
//                 objects is significantly simplified and the safety of
//                 memory operations is increased.
//
//                 Note that the current scheme fails if the structure
//                 formed by the objects in memory is not a directed
//                 acyclic graph (DAG)! Make therefore sure that you
//                 do not use the present handle template if you introduce
//                 circular references. Or, perhaps even better, redesign.
//
// Comments......: Can probably be improved upon a little, Take a look at
//                 Meyers' book ("More Effective C++: 35 New Ways to...")
//                 for a discussion.
//
//                 For some reason MSVC++ 6.0 seems to accept member templates
//                 only if they are declared inside the class body...
//
//                 It seems that MSVC++ 6.0 is somewhat unstable in its
//                 support for member templates, since I encounter mysterious
//                 crashes elsewhere if member templates are used. If they are
//                 not used, everything works just fine...
//
// Revisions.....: AÛ 000211: Added (optional) member templates, to make it
//                            behave more like "dumb" pointers wrt. type
//                            conversion up the class hierarchy.
//===================================================================

template <class T>
class Handle {
protected:

	//- The embedded "dumb" C++ pointer................................
	T          *pointer_;

protected:

	//- Methods for binding to new objects.............................
  void        Bind(const T *in);

	//- Methods for verification.......................................
  bool        IsNULL(const char *message) const;

	//15/05/13 -N
	//- Replacement for Bind and Rebind................................
	void	CopyObjectFromPointer(const T *in);

public:

	//- Access methods.................................................
  T	         *GetPointer();
	const T    *GetPointer() const;

	//- Constructors...................................................
  Handle();
  Handle(T *in);

#if defined(_MEMTMPL)
	template <class S>
	Handle(const Handle<S> &in) {Bind(in.GetPointer());}
#else
	Handle(const Handle<T> &in) {Bind(in.GetPointer());}
#endif

	//- Destructor.....................................................
	~Handle();

	//- Methods for verification.......................................
  bool        IsNULL() const;

	//- Methods for rebinding to new objects...........................
  void        Rebind(const T *in);

	//- Assignment operators...........................................
  Handle<T>  &operator=(const T *in);

#if defined(_MEMTMPL)
	template <class S>
	Handle<T>  &operator=(const Handle<S> &in) {return operator=(in.GetPointer());}
#else
	Handle<T>  &operator=(const Handle<T> &in) {return operator=(in.GetPointer());}
#endif

	//- Equality operators.............................................
  bool        operator==(const T *in) const {return (in == GetPointer());}
  bool        operator!=(const T *in) const {return (in != GetPointer());}

#if defined(_MEMTMPL)
	template <class S>
	bool        operator==(const Handle<S> &in) const {return (in.GetPointer() == GetPointer());}
#else
	bool        operator==(const Handle<T> &in) const {return (in.GetPointer() == GetPointer());}
#endif

#if defined(_MEMTMPL)
	template <class S>
	bool        operator!=(const Handle<S> &in) const {return (in.GetPointer() != GetPointer());}
#else
	bool        operator!=(const Handle<T> &in) const {return (in.GetPointer() != GetPointer());}
#endif

	//- Access operators...............................................
  T          *operator->();
  const T    *operator->() const;

  T          &operator*();
  const T    &operator*() const;

	//- Access methods.................................................
  T          &GetReference();
	const T    &GetReference() const;

  T         **GetPointerAddress() const;

	//- Ownership relinquishment.......................................
	T          *Release();

};

//-------------------------------------------------------------------
// Methods for class Handle (inlined).
//===================================================================

//-------------------------------------------------------------------
// Method........: Bind
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: Takes a T pointer as argument and sets the
//                 pointer in the handle to this value. Assumes that
//                 the embedded T pointer prior to this call is
//                 uninitialized.
// Revisions.....:
//===================================================================

template <class T>
inline void Handle<T>::Bind(const T *in) {
	#pragma omp critical(handle)
	{
	pointer_ = const_cast(T *, in);

// Increment the reference count.
	if (pointer_ != NULL)
		pointer_->Increment();
	/*
	if (in != NULL)
		CopyObjectFromPointer(in);
	*/

	}
}

//-------------------------------------------------------------------
// Method........: IsNULL
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Is true if the pointer in the handle is a NULL
//                 pointer, i.e., the handle does not point to any
//                 object.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline bool Handle<T>::IsNULL() const {

	// Is the embedded "dumb" pointer NULL?
	return (pointer_ == NULL);
}

template <class T>
inline bool Handle<T>::IsNULL(const char *message) const {

	// Display a message if verification fails.
	if (pointer_ == NULL)
		Message::Warning(message);

	return (pointer_ == NULL);
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
inline Handle<T>::Handle() {
	Bind(NULL);
}

template <class T>
inline Handle<T>::Handle(T *in) {
	Bind(in);
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
inline Handle<T>::~Handle() {
	bool deletepointer = false;
	#pragma omp critical(handle)
	{
	// Is the pointer NULL?
	if (pointer_ != NULL) {
		// Decrement the reference count.
		pointer_->Decrement();
	
		// Is the object still referenced?
		if (!pointer_->IsReferenced()) {
			// Is the object dynamic or static?
			if (pointer_->IsHeapObject())
				deletepointer = true;
		}
	}
	}
	if (deletepointer)
		delete pointer_;
	
	return;
/*	// Is the pointer NULL?
	if (pointer_ == NULL)
			return;

	// Decrement the reference count.
	pointer_->Decrement();

	// Is the object still referenced?
	if (pointer_->IsReferenced())
		return;

	// Is the object dynamic or static?
	//if (pointer_->IsHeapObject())
		delete pointer_;
	}
*/
}




//-------------------------------------------------------------------
// Method........: Rebind
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Lets the pointer in the handle point to a (new) object.
//                 The function will automatically count down the number
//                 of references to its present object and delete the object
//                 if there are no more references.  The Rebind function
//                 is optimized such that rebinding to its present object
//                 is detected and no actions are performed.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline void Handle<T>::Rebind(const T *in) {
	// Is rebinding necessary?
	if (pointer_ != in) {


		bool deletepointer = false;
		// Cast away constness.
		T *pointer = const_cast(T *, in);

		// Increment the reference count of the object to bind to.
		
		#pragma omp critical(handle)
		{
		if (pointer != NULL) {
			
			pointer->Increment();
			
		}
		// Is there a currently bound object.
		if (pointer_ != NULL) {
			
			// Decrement the reference count of the currently bound object.
			pointer_->Decrement();
				
		
			// Is the currently bound object dynamic and unreferenced?
			if (pointer_->IsReferenced() == false) {
				if (pointer_->IsHeapObject()) {
					deletepointer = true;
				}
			}
		}
		}
		if (deletepointer)
			delete pointer_;
		
		// Rebind.
		pointer_ = pointer;
	
	}
/*
	if (pointer_ == in)
		return;

	// Cast away constness.
	T *pointer = const_cast(T *, in);

	// Increment the reference count of the object to bind to.
	if (pointer != NULL)
		pointer->Increment();

//	CopyObjectFromPointer(in);


	// Is there a currently bound object.
	if (pointer_ != NULL) {

		// Decrement the reference count of the currently bound object.
		pointer_->Decrement();
		

			// Is the currently bound object dynamic and unreferenced?
		if (!pointer_->IsReferenced()) {
			if (pointer_->IsHeapObject()) {
				delete pointer_;
			}
		}
		
	}

	// Rebind.
	pointer_ = pointer;
*/
/*
	if (pointer_ == in)
		return;
	if ( pointer_ != NULL) {
		pointer_->Decrement();
		if ( !pointer_->IsReferenced()) {
			if ( pointer_->IsHeapObject()) {
				delete pointer_;
			}
		}
	}
	CopyObjectFromPointer(in);
*/
	
}

//-------------------------------------------------------------------
// Method........: operator ->
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Gives the normal pointer syntax for the handle,
//                 i.e., a pointer to the class object is returned.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline const T *Handle<T>::operator->() const {
#if defined(_DEBUG)
  IsNULL("operator->");
#endif
  return pointer_;
}

template <class T>
inline T *Handle<T>::operator->() {
#if defined(_DEBUG)
  IsNULL("operator->");
#endif
  return pointer_;
}

//-------------------------------------------------------------------
// Method........: operator *
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns a reference to the object that is handled
//                 by the handle.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline const T &Handle<T>::operator*() const {
#if defined(_DEBUG)
	IsNULL("operator*");
#endif
  return *pointer_;
}

template <class T>
inline T &Handle<T>::operator*() {
#if defined(_DEBUG)
  IsNULL("operator*");
#endif
  return *pointer_;
}

//-------------------------------------------------------------------
// Method........: GetPointer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns a pointer, similar to the "->" operator.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline const T *Handle<T>::GetPointer() const {
  return pointer_;
}

template <class T>
inline T *Handle<T>::GetPointer() {
  return pointer_;
}
	
//-------------------------------------------------------------------
// Method........: GetReference
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns a reference, similar to the "*" operator.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline const T &Handle<T>::GetReference() const {
#if defined(_DEBUG)
  IsNULL("GetReference()");
#endif
  return *pointer_;
}

template <class T>
inline T &Handle<T>::GetReference () {
#if defined(_DEBUG)
  IsNULL("GetReference()");
#endif
  return *pointer_;
}

//-------------------------------------------------------------------
// Method........: GetPointerAddress
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the address of the pointer.  Enables other
//                 objects to have a pointer that "follows" the handle
//                 pointer.  Typical applications are the following:
//                 Assume that a base class X has a subclass Y that
//                 contains a handle to a class with a name that is
//                 unknown to X.  Let the handle in Y be Handle<S>,
//                 where S has a base R.  Then one can have a R** in
//                 X that is set to Handle<S>::GetPointerAddress() in
//                 the constructor. The R** will follow the Handle<S>
//                 without knowing the exact handle type. However, R**
//                 can be used to invoke virtual functions of the S
//                 hierarchy!
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline T **Handle<T>::GetPointerAddress() const {
  return &pointer_;
}

//-------------------------------------------------------------------
// Method........: Release
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Relinquishes ownership of current dumb pointer and
//                 returns its value.  No deletion is done.  The
//                 inherent dumb pointer is set to NULL afterwards.
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline T *Handle<T>::Release() {
	// Do we have (possibly joint) ownership of an object in the first place?
	if (pointer_ == NULL)
		return pointer_;
	T *dummy;
	#pragma omp critical(handle)
	{

	// Relinquish responsibility for the object.
	pointer_->Decrement();
	// Rebind the pointer and return.
	dummy = pointer_;
	pointer_ = NULL;
	}
	return dummy;
	
}

//-------------------------------------------------------------------
// Method........: operator =
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

template <class T>
inline Handle<T> &Handle<T>::operator=(const T *in) {
  Rebind(in);
	return *this;
}

//-------------------------------------------------------------------
// Method........: CopyObjectFromPointer
// Author........: Nicholas Baltzer
// Date..........: 15/05/13
// Description...: Takes a pointer to a type T and copies the T object
//		   into the memory location pointed to by pointer_. 
//		   This will hopefully
//		   prevent referencing to existing objects, leading
//		   to less parallellization issues. Memory takes a
//		   hit, but pointer hell might be narrowly averted.
//		   This function effectively deprecates Increment()
//		   and Decrement() functions from the referent
//		   base class for handles.
// Comments......: Does *NOT* increment or decrement, only copies
//		   a memory space from the in pointer and then 
//		   assigns pointer_ to the new memory allocation.
// Revisions.....:
//===================================================================
template <class T>
inline void Handle<T>::CopyObjectFromPointer(const T *in) {

	//Writing to an unitialized pointer is a nono. -N
/*	if (pointer_ == NULL) {
	
		T *allocation;
		if (const_cast<T*>(in) != NULL) {
		
//			allocation = in->Duplicate();
		}
		pointer_ = allocation;
	}


		memcpy( pointer_, in, sizeof(T));
*/
	//T allocation = new T(*in);
	//T allocation = in->Duplicate();
	//pointer_ = &allocation;
	T *pointer = const_cast(T *, in);
	pointer_ = pointer->Duplicate();
}


#endif

 
