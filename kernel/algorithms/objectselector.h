//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __OBJECTSELECTOR_H__
#define __OBJECTSELECTOR_H__

#include <copyright.h>

#include <kernel/basic/macros.h>
#include <kernel/basic/string.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: ObjectSelector
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class.
// Revisions.....:
//===================================================================

class ObjectSelector {
public:

	//- Enumerated types...............................................
	enum Selection {SELECTION_ALL, SELECTION_VALUE, SELECTION_FILE, SELECTION_INDEX};

	//- Textual representations........................................
	static String            GetString(Selection selection);

protected:

	//- Algorithm parameters...........................................
	Selection                selection_; // Which objects indices are to be selected?
	String                   attribute_; // For selection by attribute value: Attribute index.
	String                   value_;     // For selection by attribute value: Attribute value.
	String                   filename_;  // For selection by file contents: Location of file.
	int                      index_;     // For selection by index: Object index (masked).

protected:

  //- Constructors/destructor........................................
  ObjectSelector(const ObjectSelector &in);

	//- Object selection...............................................
	bool                     SelectAll(Vector(int) &indices, const DecisionTable &table, bool masked) const;
	bool                     SelectByValue(Vector(int) &indices, const DecisionTable &table, bool masked) const;
	bool                     SelectFromFile(Vector(int) &indices, const DecisionTable &table, bool masked) const;
	bool                     SelectByIndex(Vector(int) &indices, const DecisionTable &table, bool masked) const;

public:

  //- Constructors/destructor........................................
  ObjectSelector();
  virtual ~ObjectSelector();

	//- Object selection...............................................
	bool                     Select(Vector(int) &indices, const DecisionTable &table, bool masked) const;

  //- Parameter string methods.......................................
	String                   GetParameters() const;
	bool                     SetParameter(const String &keyword, const String &value);

	//- Local parameter methods........................................
	Selection                GetSelection() const {return selection_;}
	bool                     SetSelection(Selection selection) {selection_ = selection; return true;}

	const String            &GetAttributeName() const {return attribute_;}
	bool                     SetAttributeName(const String &attribute) {attribute_ = attribute; return true;}

	const String            &GetValueName() const {return value_;}
	bool                     SetValueName(const String &value) {value_ = value; return true;}

	const String            &GetFilename() const {return filename_;}
	bool                     SetFilename(const String &filename) {filename_ = filename; return true;}

	int                      GetIndex() const {return index_;}
	bool                     SetIndex(int index) {index_ = index; return true;}

	//- Operators......................................................
	ObjectSelector          &operator=(const ObjectSelector &in);

};

#endif 
