//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SPLITTER_H__
#define __SPLITTER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

//-------------------------------------------------------------------
// Class.........: Splitter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Splits a decision table into subtables.
// Revisions.....:
//===================================================================

class Splitter : public Algorithm {
protected:

	//- Parameters.....................................................
	bool           append_; // Append resulting structures as children of parent structure?

public:

  //- Constructors/destructor........................................
  Splitter();
  virtual ~Splitter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String GetParameters() const;
	virtual bool   SetParameter(const String &keyword, const String &value);
	virtual bool   IsApplicable(const Structure &structure, bool warn = true) const;

	//- Parameter management...........................................
	bool           AppendAsChildren() const {return append_;}
	bool           AppendAsChildren(bool append) {append_ = append; return true;}

};

#endif 
