//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DISCERNER_H__
#define __DISCERNER_H__

#include <copyright.h>

#include <kernel/structures/graph.h>
#include <kernel/system/iostream.h>

#if defined(_DEBUG)
#include <kernel/basic/message.h>
#endif

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: Discerner
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class for algorithms that need a
//                 discernibility predicate. Support for indiscernibility
//                 definition graphs (IDGs).
// Revisions.....:
//===================================================================

class Discerner {
protected:

	//- Implementation structures......................................
	Graph::Handles    graphs_; // Representation of IDGs.

protected:

  //- Constructors...................................................
  Discerner(const Discerner &in);

public:

  //- Constructors/destructor........................................
  Discerner();
  virtual ~Discerner();

	//- IDG methods....................................................
	bool              LoadIDGs(const String &filename, const DecisionTable &table, bool masked);
	bool              LoadIDGs(ifstream &stream, const DecisionTable &table, bool masked);

	int               GetNoIDGs() const;

	bool              HasIDG(int attribute) const;
	bool              HasIDGs() const;

	Handle<Graph>     GetIDG(int attribute) const;
	bool              SetIDG(Handle<Graph> graph, int attribute);

	bool              AllReflexive() const;
	bool              AllSymmetric() const;
	bool              AllTransitive() const;

	//- Discernibility predicate.......................................
	bool              Discerns(int attribute, int value1, int value2) const;

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoIDGs
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns the number of attributes this Discerner is
//                 dimensioned for.
// Comments......:
// Revisions.....:
//===================================================================

inline int
Discerner::GetNoIDGs() const {
	return graphs_.size();
}

//-------------------------------------------------------------------
// Method........: GetIDG
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if an IDG is present for the given
//                 attribute.
// Comments......:
// Revisions.....:
//===================================================================

inline Handle<Graph>
Discerner::GetIDG(int attribute) const {

#if defined(_DEBUG)
	if (attribute < 0 || attribute >= graphs_.size()) {
		Message::Error("Index out of range in call to Discerner::GetIDG(int).");
		return NULL;
	}
#endif

	return graphs_[attribute];

}

//-------------------------------------------------------------------
// Method........: HasIDG
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if an IDG is present for the given
//                 attribute.
// Comments......:
// Revisions.....:
//===================================================================

inline bool
Discerner::HasIDG(int attribute) const {
	return (attribute >= 0 && attribute < graphs_.size() && !graphs_[attribute].IsNULL());
}

//-------------------------------------------------------------------
// Method........: Discerns
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if the two attribute values can be used to
//                 discern between two objects wrt the given attribute.
//
//                 Does a lookup in the IDG. If no IDG is present for
//                 the given attribute, strict inequality is enforced.
//
// Comments......: It is the user's responsibility that the IDG
//                 is properly defined, i.e., reflexive and
//                 symmetric.
// Revisions.....:
//===================================================================

inline bool
Discerner::Discerns(int attribute, int value1, int value2) const {

	if (!HasIDG(attribute))
		return value1 != value2;

	return !(GetIDG(attribute)->GetEdgeByValue(value1, value2));

}

//-------------------------------------------------------------------
// Method........: HasIDGs
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Returns true if an IDG is present for any attribute.
//
// Comments......: If HasIDGs() returns false, then the discerner defines
//                 an equivalence relation.
// Revisions.....:
//===================================================================

inline bool
Discerner::HasIDGs() const {

	int i;

	for (i = GetNoIDGs() - 1; i >= 0; i--) {
		if (HasIDG(i))
			return true;
	}

	return false;

}

#endif
 
