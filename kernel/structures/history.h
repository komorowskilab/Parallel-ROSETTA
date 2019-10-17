//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __HISTORY_H__
#define __HISTORY_H__

#include <copyright.h>

#include <kernel/structures/structure.h>
#include <kernel/structures/historyentry.h>

#include <kernel/basic/handle.h>
#include <kernel/basic/vector.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class String;

//-------------------------------------------------------------------
// Class.........: History
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: The history log of an annotated structure.
// Revisions.....:
//===================================================================

class History : public Structure {
private:

	//- The individual entries.........................................
	HistoryEntry::Handles entries_;

protected:

   //- Constructors...................................................
  History(const History &in);

public:

  //- Constructors/destructor........................................
  History();
  virtual ~History();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool        Load(ifstream &stream);
  virtual bool        Save(ofstream &stream) const;

  //- Methods inherited from Structure...............................
  virtual Structure  *Duplicate() const;
  virtual void        Clear();

	//- Assignment operator............................................
	History            &operator=(const History &in);

  //- History entry administration...................................
	bool                Touch(const String &action);

	int                 GetNoEntries() const;
	const HistoryEntry *GetEntry(int i) const;

};

#endif 
