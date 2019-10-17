//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/identifier.h>

//-------------------------------------------------------------------
// Methods for class Identifier.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsA/GetId
// Author........: Aleksander šhrn
// Date..........:
// Description...: Provides run-time type identification (RTTI).
// Comments......: Has to be implemented explicitly instead of via
//                 the IMPLEMENTIDMETHODS macro because this is the
//                 top-level identifying class, i.e. it has no
//                 baseclass equipped with an IsA relation.
// Revisions.....:
//===================================================================

bool
Identifier::IsA(Id id) const {
  return (id == IDENTIFIER);
}

Id
Identifier::GetId() const {
  return IDENTIFIER;
}
 
