//-------------------------------------------------------------------
// Author........: Daniel Remmem
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <copyright.h>

#include <kernel/structures/parentstructure.h>

//-------------------------------------------------------------------
// Class.........: Project
// Author........: Daniel Remmem
// Date..........:
// Description...: A project consists of structures and algorithms.
// Comments......:
// Revisions.....:
//===================================================================

class Project : public ParentStructure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<Project>) Handles;

protected:

  //- Constructors...................................................
  Project(const Project &in);

public:

  //- Constructors/destructor........................................
  Project();
  virtual ~Project();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Structure...............................
	virtual Structure  *Duplicate() const;

  //- Local methods..................................................
	bool                GetAllIdentifiers(Id id, Identifier::Handles &identifiers) const;
	bool                GetAllIdentifiers(Id id, const String &name, Identifier::Handles &identifiers) const;

	int                 Count(Id id) const;
	int                 Count(Id id, const String &name) const;

};


#endif 
