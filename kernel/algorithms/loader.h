//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __LOADER_H__
#define __LOADER_H__

#include <copyright.h>

#include <kernel/algorithms/scriptalgorithm.h>

//-------------------------------------------------------------------
// Class.........: Loader
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simply calls the input structure's Load method.
//
//                 For use by command scripts, thus allowing loading
//                 in "native" format.
//
// Revisions.....: AÛ 000211: Inherits from ScriptAlgorithm instead
//                            of from Importer. Also, changed
//                            specifications.
//===================================================================

class Loader : public ScriptAlgorithm {
protected:

  //- Parameters.....................................................
	String              filename_;

public:

  //- Constructors/destructor........................................
  Loader();
  virtual ~Loader();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool        IsApplicable(const Structure &structure, bool warn = true) const;
  virtual Structure  *Apply(Structure &structure) const;
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual Loader		*Clone();

	//- Local methods..................................................
	const String       &GetFilename() const {return filename_;}
	bool                SetFilename(const String &filename) {filename_ = filename; return true;}

};

#endif 
