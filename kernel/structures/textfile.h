//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __TEXTFILE_H__
#define __TEXTFILE_H__

#include <copyright.h>

#include <kernel/structures/annotatedstructure.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: TextFile
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Enables the GUI to represent text files in the
//                 project tree.
// Revisions.....:
//===================================================================

class TextFile : public AnnotatedStructure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<TextFile>) Handles;

private:

  //- Member variables..............................................
	String                filename_;

protected:

	//- Constructors...................................................
  TextFile(const TextFile &in);

public:

  //- Constructor and destructor....................................
	TextFile();
  virtual ~TextFile();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool          Load(ifstream &stream);
  virtual bool          Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure    *Duplicate() const;
  virtual void          Clear();

  //- Local methods.................................................
	const String         &GetFilename() const {return filename_;}
	bool                  SetFilename(const String &filename) {filename_ = filename; return true;}

};

#endif 
