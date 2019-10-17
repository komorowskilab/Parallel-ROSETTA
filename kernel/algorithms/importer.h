//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class.........: Importer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for algorithms dealing with importing
//                 structural object in alien formats.
// Revisions.....:
//===================================================================

class Importer : public Algorithm {
private:

  //- Private member variables.......................................
	String filename_;

public:

  //- Constructors/destructor........................................
  Importer();
  virtual ~Importer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String        GetParameters() const;
	virtual bool          SetParameter(const String &keyword, const String &value);
	virtual Importer		*Clone() = 0;

	//- Parameter management...........................................
	virtual bool          SetFilename(const String &filename);
	virtual const String &GetFilename() const;

};

#endif 
