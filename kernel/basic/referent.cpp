//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/referent.h>
#include <kernel/basic/macros.h>
#include <kernel/basic/message.h>

#include <kernel/system/stdlib.h>
#include <kernel/system/malloc.h>

#include "omp.h"

//-------------------------------------------------------------------
// Local static variables.
//===================================================================

static bool static_is_heapobject = false;

//-------------------------------------------------------------------
// Methods for class Referent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: The "new" operator should have set the local static
//                 indicator variable if dynamic creation took place.
//                 Resetting of the indicator variable takes place here.
// Revisions.....:
//===================================================================

Referent::Referent() {
	reference_count_     = 0;
	is_heapobject_       = static_is_heapobject;
	static_is_heapobject = false;
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Invoking the destructor for objects that are
//                 dynamically created and still referenced elsewhere
//                 leads to trouble.
//
//                 The most common source of this error is that a base
//                 base class handle deletes an object and the
//                 base class has a non-virtual destructor.  Make the
//                 destructor virtual!
// Revisions.....:
//===================================================================

Referent::~Referent() {
	// Check for anomalies.
	if (IsReferenced() && IsHeapObject())
		Message::Error("Referencing error, check for virtual destructors.");
}

//-------------------------------------------------------------------
// Method........: operator new
// Author........: Aleksander šhrn
// Date..........:
// Description...: Written to determine if the object was created by
//                 the "new" operator.
//
// Comments......: Use malloc or calloc or ::new or..?
// Revisions.....:
//===================================================================

void *
Referent::operator new(size_t size) {
void *memblock;

//#pragma omp critical(calloc)
//{
	// Allocate memory block.
	memblock = calloc(1, size);

	// Set static flag checked in object constructor.
	static_is_heapobject = true;
//}
	return memblock;

}

//-------------------------------------------------------------------
// Method........: operator new
// Author........: Aleksander šhrn
// Date..........:
// Description...: Written to determine if the object was created by
//                 the "new" operator.
//
//                 For debugging purposes.
//
// Comments......: Use malloc or calloc or ::new or..?
// Revisions.....:
//===================================================================

#if defined(_DEBUG)
void *
Referent::operator new(size_t size, char *file, int line) {

	// Allocate memory block.
	void *memblock = calloc(1, size);

	// Give detailed debug information if allocation request failed.
	if (memblock == NULL) {
		String message("Out of memory, allocation request failed.\n\n");
		message += String("Memory allocation attempted in file ") + file + String(" at line ") + line + String(".");
		Message::FatalError(message);
	}

	// Set static flag checked in object constructor.
	static_is_heapobject = true;

	return memblock;

}
#endif

//-------------------------------------------------------------------
// Method........: operator delete
// Author........: Aleksander šhrn
// Date..........:
// Description...: Counterpart to "new".
// Comments......:
// Revisions.....:
//===================================================================

void
Referent::operator delete(void *memblock) {
//#pragma omp critical(calloc)
//{
	free(memblock);
//}
}
 
