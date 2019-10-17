//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __GENERALIZEDDECISION_H__
#define __GENERALIZEDDECISION_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: GeneralizedDecision
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents a generalized decision value.
// Comments......:
// Revisions.....:
//===================================================================

class GeneralizedDecision : public Structure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<GeneralizedDecision>) Handles;

protected:

	//- Member variables...............................................
	Vector(int) decisions_;   // The generalized decision values...
	Vector(int) supports_;    // ...and their corresponding cardinalities.

protected:

	//- Constructors...................................................
  GeneralizedDecision(const GeneralizedDecision &in);

public:

	//- Constructors/destructor........................................
  GeneralizedDecision();
  virtual ~GeneralizedDecision();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent.............................
  virtual bool                   Load(ifstream &stream);
  virtual bool                   Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual void                   Clear();
	virtual Structure             *Duplicate() const;

  //- Decision value management methods.............................
  virtual int                    GetNoDecisions() const;

  virtual int                    GetDecision(int position_no) const;
  virtual int                    GetSupport(int position_no) const;

  virtual bool                   InsertDecision(int position_no, int decision, int support);
  virtual bool                   AppendDecision(int decision, int support);
  virtual bool                   RemoveDecision(int position_no);

	//- Miscellaneous methods..........................................
	virtual int                    GetDominatingDecision(int &position_no) const;
	virtual float                  GetProbability(int position_no) const;
	virtual bool                   Reserve(int size);

	virtual bool                   HasEqualDecisions(const GeneralizedDecision &in, float precision) const;
	virtual bool                   HasEqualSupports(const GeneralizedDecision &in) const;

  //- Operators......................................................
  GeneralizedDecision           &operator=(const GeneralizedDecision &in);

  bool                           operator==(const GeneralizedDecision &in) const;
  bool                           operator!=(const GeneralizedDecision &in) const;

};

#endif 
