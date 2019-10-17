//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: A referent is something that is referenced by others.
//                 Used in conjunction with handles (smart pointers).
//
//                 Partially based on code from the DiffPack library
//                 developed by SINTEF Applied Mathematics, Oslo, Norway,
//                 and University of Oslo, Dept. of Mathematics, Norway.
// Revisions.....:
//===================================================================

#ifndef __REFERENT_H__
#define __REFERENT_H__

#include <copyright.h>

#include <kernel/basic/types.h>

#include <kernel/system/stddef.h>

//-------------------------------------------------------------------
// Class.........: Referent
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Provides the data structure for reference counting
//                 (and debugging of reference counting) when using
//                 handles.
// Comments......:
// Revisions.....:
//===================================================================

class Referent {
private:

	//- Private variables..............................................
	volatile int   reference_count_;  // Current reference count.
	bool  is_heapobject_;    // Dynamically or statically allocated?

public:

	//- Constructors/destructor........................................
  Referent();
  virtual ~Referent();

	//- Reference count methods........................................
	void  Increment();
	void  Decrement();

  int   GetNoReferences() const;

	//- Query methods..................................................
  bool  IsReferenced() const;
	bool  IsShared() const;
	bool  IsHeapObject() const;

	//- Memory management operators....................................
  void *operator new(size_t size);
#if defined (_DEBUG)
	void *operator new(size_t size, char *file, int line);
#endif
  void  operator delete(void *memblock);

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Increment/Decrement
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline void
Referent::Increment() {
	#pragma omp atomic
  reference_count_++;
}

inline void
Referent::Decrement() {
	#pragma omp atomic
  reference_count_--;
}

//-------------------------------------------------------------------
// Method........: GetNoReferences
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the current reference count, i.e. how many
//                 objects are presently referencing this object.
// Comments......:
// Revisions.....:
//===================================================================

inline int
Referent::GetNoReferences() const {
  return reference_count_;
}

//-------------------------------------------------------------------
// Method........: IsReferenced
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if there are pointers pointing to the
//                 present object.  Here present object means the object
//                 of the subclass derived from Referent, i.e. the
//                 object to be handled by a handle.
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Referent::IsReferenced() const {
  return (reference_count_ != 0);
}

//-------------------------------------------------------------------
// Method........: IsShared
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if more than one object has pointers
//                 pointing to the present object.
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Referent::IsShared() const {
  return (reference_count_ > 1);
}

//-------------------------------------------------------------------
// Method........: IsHeapObject
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if the object was created by a call to
//                 "new" rather than being declared as an instance (e.g.
//                 "X x;").  This function is important:  Handles should
//                 only delete objects that were created by "new".
//                 C++ will automatically delete objects that go out of
//                 scope.
// Revisions.....:
//===================================================================

inline bool
Referent::IsHeapObject() const {
  return is_heapobject_;
}

#endif 
