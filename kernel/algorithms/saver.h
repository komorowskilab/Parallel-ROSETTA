//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SAVER_H__
#define __SAVER_H__

#include <copyright.h>

#include <kernel/algorithms/scriptalgorithm.h>

//-------------------------------------------------------------------
// Class.........: Saver
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simply calls the input structure's Save method.
//
//                 For use by command scripts, thus allowing saving
//                 in "native" format.
//
// Revisions.....: AÛ 000211: Inherits from ScriptAlgorithm instead
//                            of from Exporter.
//===================================================================

class Saver : public ScriptAlgorithm {
protected:

  //- Parameters.....................................................
	String              filename_;

public:

  //- Constructors/destructor........................................
  Saver();
  virtual ~Saver();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool        IsApplicable(const Structure &structure, bool warn = true) const;
  virtual Structure  *Apply(Structure &structure) const;
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual Saver		*Clone();

	//- Local methods..................................................
	const String       &GetFilename() const {return filename_;}
	bool                SetFilename(const String &filename) {filename_ = filename; return true;}

};

#endif 
