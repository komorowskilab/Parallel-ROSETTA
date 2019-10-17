//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __STRUCTURECREATOR_H__
#define __STRUCTURECREATOR_H__

#include <copyright.h>

#include <kernel/algorithms/scriptalgorithm.h>

//-------------------------------------------------------------------
// Class.........: StructureCreator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Simply calls the Creator::Create method and outputs
//                 the result, ignores the input.
//
//                 For use by command scripts, thus allowing creation
//                 of structural objects.
//
// Revisions.....: AÛ 000211: Inherits from ScriptAlgorithm instead
//                            of from Exporter.
//===================================================================

class StructureCreator : public ScriptAlgorithm {
protected:

  //- Parameters.....................................................
	Id                  output_;

public:

  //- Constructors/destructor........................................
  StructureCreator();
  virtual ~StructureCreator();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual bool        IsApplicable(const Structure &structure, bool warn = true) const;
  virtual Structure  *Apply(Structure &structure) const;
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual StructureCreator	*Clone();

	//- Local methods..................................................
	Id                  GetOutputType() const {return output_;}
	bool                SetOutputType(Id output) {output_ = output; return true;}

};

#endif 
