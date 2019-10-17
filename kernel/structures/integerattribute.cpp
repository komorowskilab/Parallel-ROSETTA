//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/integerattribute.h>

#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class IntegerAttribute.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

IntegerAttribute::IntegerAttribute(const IntegerAttribute &in) : Attribute(in) {
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

IntegerAttribute::IntegerAttribute() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

IntegerAttribute::~IntegerAttribute() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(IntegerAttribute, INTEGERATTRIBUTE, Attribute)

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
IntegerAttribute::Duplicate() const {
  return new IntegerAttribute(*this);
}

//-------------------------------------------------------------------
// Methods inherited from Attribute.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetType
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Attribute::Type
IntegerAttribute::GetType() const {
	return TYPE_INTEGER;
}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a real-world value, returns the internal coded
//                 value.
// Comments......:
// Revisions.....: Aš 970723 - Use IsFloat, GetFloat and Round.
//===================================================================

int
IntegerAttribute::GetEntry(const String &text) const {

	if (text.IsEmpty() || (text == Undefined::String()))
		return Undefined::Integer();

	// Check type.
	if (!text.IsFloat())
		return Undefined::Integer();

	return MathKit::Round(text.GetFloat());

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an internal coded value, returns the
//                 corresponding real-world value.
// Comments......:
// Revisions.....: 182011 - N 
// 			No more static, it's causing problems with
// 			memory deletions. Passing a reference
// 			doesn't make sense here any longer.
//===================================================================

//const String &
const String
IntegerAttribute::GetEntry(int value) const {

	// So we can return a reference.
	//static String entry;
	String entry;
	// Set textual repr. of integral value.
	if (value == Undefined::Integer())
		entry = Undefined::String();
	else
		entry = String::Format(value);

	return entry;

}

//-------------------------------------------------------------------
// Method........: SuggestEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
IntegerAttribute::SuggestEntry(const String &text) const {
	return GetEntry(text);
}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Ignored, returns true.
// Comments......:
// Revisions.....:
//===================================================================

bool
IntegerAttribute::SetEntry(int /*value*/, const String &/*text*/) {
	return true;
}

 
