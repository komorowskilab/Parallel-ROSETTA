//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

#include <copyright.h>

#include <kernel/basic/persistent.h>
#include <kernel/basic/handle.h>
#include <kernel/basic/types.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Algorithm;
class Annotation;
class ParentStructure;

//-------------------------------------------------------------------
// Class.........: Structure
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for structures.
// Revisions.....:
//===================================================================

class Structure : public Persistent {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<Structure>) Handles;

protected:

	//- Constructors...................................................
	Structure(const Structure &in);

public:

  //- Constructors/destructor........................................
  Structure();
  virtual ~Structure();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
	virtual bool          Load(ifstream &stream);
	virtual bool          Save(ofstream &stream) const;
	virtual bool          Load(const String &filename);
	virtual bool          Save(const String &filename) const;

  //- Algorithm application..........................................
  	virtual Structure    	*Apply(const Algorithm &algorithm);
//	virtual Structure	*Apply(const Algorithm &algorithm, bool usethreads);
	//- Cloning........................................................
  virtual Structure    *Duplicate() const = 0;


	//- Total reset....................................................
  virtual void          Clear() = 0;

	//- Methods for interface uniformity, naming.......................
	virtual const String &GetName() const;
	virtual bool          SetName(const String &name);

	//- Methods for interface uniformity, child management.............
	virtual int           GetNoChildren() const;
	virtual Structure    *GetChild(int i) const;

  virtual bool          InsertChild(Structure *child, int i);
  virtual bool          AppendChild(Structure *child);
  virtual bool          RemoveChild(int i);

	virtual bool          RemoveAllChildren();

	virtual bool          GetAllChildren(Id id, Identifier::Handles &children, bool recursive) const;

  virtual int           FindChild(const Structure *child) const;
  virtual bool          IsParent(const Structure *child, bool recursive) const;

	virtual bool          HasChildren() const;
	virtual bool          HasChild(Id id, bool recursive) const;

	//- Methods for interface uniformity, member management............
  virtual int           GetNoStructures() const;
  virtual Structure    *GetStructure(int i) const;

  virtual bool          InsertStructure(Structure *member, int i);
  virtual bool          AppendStructure(Structure *member);
  virtual bool          RemoveStructure(int i);
	virtual bool          RemoveAllStructures();

  virtual int           FindMember(const Structure *member, bool physical = true) const;
  virtual bool          IsMember(const Structure *member, bool physical = true) const;
	virtual bool          HasMembers() const;

	virtual bool          Merge(const Structure &structure, bool verify = true);

	//- Methods for interface uniformity, annotation...................
	virtual bool          IsAnnotated() const;

	virtual Annotation   *GetAnnotation() const;
	virtual bool          SetAnnotation(Annotation *annotation);

	virtual bool          Touch(const String &action);

	//- Parent search methods..........................................
	virtual Structure    *FindParent() const;
	virtual Structure    *FindParent(Id id) const;
	virtual Structure    *FindParent(const Structure &ancestor) const;
	virtual Structure    *FindParent(Id id, const Structure &ancestor) const;

};

#endif 
