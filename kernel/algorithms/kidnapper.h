//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KIDNAPPER_H__
#define __KIDNAPPER_H__

#include <copyright.h>

#include <kernel/algorithms/scriptalgorithm.h>

//-------------------------------------------------------------------
// Class.........: Kidnapper
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simply returns the result of calling the input's 
//                 GetChild method.
//
//                 For use by command scripts.
// Revisions.....: 
//===================================================================

class Kidnapper : public ScriptAlgorithm {
protected:

  //- Parameters.....................................................
	int                 index_;

public:

  //- Constructors/destructor........................................
  Kidnapper();
  virtual ~Kidnapper();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool        IsApplicable(const Structure &structure, bool warn = true) const;
  virtual Structure  *Apply(Structure &structure) const;
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual Kidnapper	*Clone();

	//- Local methods..................................................
	int                 GetChildIndex() const {return index_;}
	bool                SetChildIndex(int index) {index_ = index; return true;}

};

#endif 
