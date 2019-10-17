//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ANNOTATEDSTRUCTURE_H__
#define __ANNOTATEDSTRUCTURE_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/annotation.h>

#include <kernel/basic/handle.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: AnnotatedStructure
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A structure with an attached annotation object.
// Revisions.....:
//===================================================================

class AnnotatedStructure : public Structure {
protected:

	//- Member variables...............................................
 	Handle<Annotation> annotation_;

protected:

  //- Constructors...................................................
  AnnotatedStructure(const AnnotatedStructure &in);

public:

  //- Constructors/destructor........................................
  AnnotatedStructure();
  virtual ~AnnotatedStructure();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool          Load(ifstream &stream);
  virtual bool          Save(ofstream &stream) const;
	virtual bool          Load(const String &filename);
	virtual bool          Save(const String &filename) const;
	virtual bool          Load();
	virtual bool          Save() const;

  //- Methods inherited from Structure...............................
  virtual Structure    *Duplicate() const;
  virtual void          Clear();

	virtual const String &GetName() const;
	virtual bool          SetName(const String &name);

	virtual Annotation   *GetAnnotation() const;
	virtual bool          SetAnnotation(Annotation *annotation);

	virtual bool          Touch(const String &action);

};

#endif 
