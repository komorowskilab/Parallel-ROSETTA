//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __IDENTIFIER_H__
#define __IDENTIFIER_H__

#include <copyright.h>

#include <kernel/basic/macros.h>
#include <kernel/basic/referent.h>
#include <kernel/basic/handle.h>
#include <kernel/basic/ids.h>
#include <kernel/basic/idholder.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: Identifier
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...: Base class for self-identifying objects. IsA is a
//                 transitive relation.
// Revisions.....:
//===================================================================

class Identifier : public Referent {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<Identifier>) Handles;

public:

  //- Virtual methods for self-identification.......................
	DECLAREIDMETHODS()

};

#endif 
