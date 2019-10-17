//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __HISTORYENTRY_H__
#define __HISTORYENTRY_H__

#include <copyright.h>

#include <kernel/structures/structure.h>

#include <kernel/utilities/systemkit.h>

#include <kernel/basic/string.h>
#include <kernel/system/iostream.h>


//-------------------------------------------------------------------
// Class.........: HistoryEntry
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents an entry in a history log.
// Revisions.....:
//===================================================================

class HistoryEntry : public Structure {
public:

  //- Type definitions...............................................
	typedef Vector(Handle<HistoryEntry>) Handles;

private:

	//- Entry fields...................................................
  String timestamp_; // When was the action performed?
	String user_;      // Who performed the action?
  String action_;    // Description of the action performed.

protected:

	//- Constructors...................................................
	HistoryEntry(const HistoryEntry &in);

public:

	//- Constructors/destructors.......................................
	HistoryEntry();
	virtual ~HistoryEntry();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
  virtual bool       Load(ifstream &stream);
  virtual bool       Save(ofstream &stream) const;

  //- Method inherited from Structure................................
  virtual Structure *Duplicate() const;
  virtual void       Clear();

	//- Assignment operator............................................
	HistoryEntry      &operator=(const HistoryEntry &in);

	//- Entry field manipulation.......................................
	bool               Touch(const String &action);

	const String      &GetTimestamp() const;
	const String      &GetUser() const;
	const String      &GetAction() const;

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

inline bool
HistoryEntry::Touch(const String &action) {

	timestamp_ = SystemKit::GetTimestamp();
	user_      = SystemKit::GetUser();
	action_    = action;

	return true;

}

inline const String &
HistoryEntry::GetTimestamp() const {
	return timestamp_;
}

inline const String &
HistoryEntry::GetUser() const {
	return user_;
}

inline const String &
HistoryEntry::GetAction() const {
	return action_;
}

#endif 
