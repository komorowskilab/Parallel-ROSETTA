//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __VALUESPLITTER_H__
#define __VALUESPLITTER_H__

#include <copyright.h>

#include <kernel/algorithms/splitter.h>

//-------------------------------------------------------------------
// Class.........: ValueSplitter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Splits a decision table into subtables, each subtable
//                 having different table entries for some attribute.
// Revisions.....:
//===================================================================

class ValueSplitter : public Splitter {
protected:

  //- Parameters.....................................................
	int                attribute_; // Attribute index.

public:

  //- Constructors/destructor........................................
  ValueSplitter();
  virtual ~ValueSplitter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual ValueSplitter	*Clone();

	//- Parameter management...........................................
	int                GetAttribute() const {return attribute_;}
	bool               SetAttribute(int attribute) {attribute_ = attribute; return true;}

};

#endif 
