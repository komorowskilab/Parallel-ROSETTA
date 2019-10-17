//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/xmlkit.h>

#include <kernel/basic/macros.h>

//-------------------------------------------------------------------
// Methods for class XMLKit.
//===================================================================

//-------------------------------------------------------------------
// Method........: Escape
// Author........: Aleksander šhrn
// Date..........:
// Description...: Escapes certain characters in a string.
// Comments......:
// Revisions.....:
//===================================================================

String
XMLKit::Escape(const String &unescaped, const String &escapees) {

	int i, length = unescaped.GetLength();

	String escaped;

	// The result will have at least this many characters.
	escaped.Reserve(length);

	for (i = 0; i < length; i++) {
		if (escapees.Contains(unescaped[i]))
			escaped += Escape(unescaped[i]);
		else
			escaped += unescaped[i];
	}

	return escaped;

}

//-------------------------------------------------------------------
// Method........: Escape
// Author........: Aleksander šhrn
// Date..........:
// Description...: Escapes a character.
// Comments......:
// Revisions.....:
//===================================================================

String
XMLKit::Escape(char character) {

	switch (character) {
		case '&': return "&amp;";
		case '<': return "&lt;";
		case '>': return "&gt;";
		case '"': return "&quot;";
		default:  return "&#" + String::Format(static_cast(unsigned char, character)) + ";";
	}

}

 
