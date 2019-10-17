//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <copyright.h>

#include <kernel/basic/persistent.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Structure;

//-------------------------------------------------------------------
// Class.........: Algorithm
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...: Base class for algorithm objects.
// Revisions.....:
//===================================================================

class Algorithm : public Persistent {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<Algorithm>) Handles;

	//int parameters_set;

public:

  //- Constructors/destructor........................................
  Algorithm();
  virtual ~Algorithm();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Parameter manipulation.........................................
	virtual String     GetParameters() const;
	virtual bool       GetOutputFilenames(Vector(String) &filenames) const;
	virtual bool       SetParameters(const String &parameters, bool warn = true);
	virtual bool       SetParameter(const String &keyword, const String &value);

	//- Algorithm application..........................................
	virtual bool       IsApplicable(const Structure &structure, bool warn = true) const;
  	virtual Structure *Apply(Structure &structure) const = 0;
//	virtual Structure *Apply(Structure &structure, bool usethreads) const;

  virtual Algorithm *Clone() = 0;
  virtual Algorithm *Duplicate() { return Clone(); };
};

#endif
 
