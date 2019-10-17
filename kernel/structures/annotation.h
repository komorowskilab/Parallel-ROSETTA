//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/history.h>

#include <kernel/basic/string.h>
#include <kernel/basic/handle.h>
#include <kernel/system/iostream.h>


//-------------------------------------------------------------------
// Class.........: Annotation
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Contains various standard or nonstandard (symbolic)
//                 information about a structure.
// Revisions.....:
//===================================================================

class Annotation : public Structure {
private:

  //- Annotation fields..............................................
  String          name_;       // The name of the object.
  String          filename_;   // The physical location of the object.
	String          comments_;   // General comments attached to the object.
  Handle<History> history_;    // History log of actions performed on the object.

protected:

	//- Constructors...................................................
  Annotation(const Annotation &in);

public:

  //- Constructor and destructor....................................
	Annotation();
  virtual ~Annotation();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool          Load(ifstream &stream);
  virtual bool          Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure    *Duplicate() const;
  virtual void          Clear();

  virtual const String &GetName() const;
  virtual bool          SetName(const String &name);

  //- Assignment operator...........................................
	Annotation           &operator=(const Annotation &in);

  //- Annotation field manipulation.................................
  bool                  SetFilename(const String &filename);
  const String         &GetFilename() const;

	bool                  AddComment(const String &comment);
	bool                  SetComments(const String &comments);
	const String         &GetComments() const;

  const History        *GetHistory() const;
	bool                  ClearHistory();
	bool                  Touch(const String &action);

};

#endif 
