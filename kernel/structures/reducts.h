//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTS_H__
#define __REDUCTS_H__

#include <copyright.h>

#include <kernel/structures/structures.h>
#include <kernel/structures/reduct.h>

#include <kernel/basic/map.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Discerner;

//-------------------------------------------------------------------
// Class.........: Reducts
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...: A set of reducts.
// Revisions.....:
//===================================================================

class Reducts : public Structures {
private:

	//- STL comparator.................................................
	struct Compare {
		bool operator()(const Handle<Structure> &a, const Handle<Structure> &b) const;
	};

protected:

  //- Constructors...................................................
  Reducts(const Reducts &in);

public:

	//- Type definitions...............................................
	typedef Map(int, Vector(int)) IVMap;

public:

  //- Constructors/destructor........................................
  Reducts();
  virtual ~Reducts();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure...............................
  virtual Structure *Duplicate() const;
	virtual int        FindMember(const Structure *member, bool physical) const;

	//- Methods inherited from Structures..............................
	virtual bool       InsertStructure(Structure *structure, int i);

	//- New virtual methods............................................
	virtual int        GetNoReducts() const;
	virtual Reduct    *GetReduct(int i) const;
	virtual bool       Reindex(const Vector(int) &indices);
	virtual bool       Sort(int property, bool ascending);
	virtual bool       PostCreation(const DecisionTable &table, const Discerner &discerner, bool modulo, bool lock=false);
	virtual bool       AppendReduct(Reduct *reduct, const DecisionTable &table, int object_no, bool masked, bool modulo, bool verify, bool lock=false);

	//- Local methods..................................................
	bool               GetAttributeSupports(IVMap &attributes) const;

};

#endif 
