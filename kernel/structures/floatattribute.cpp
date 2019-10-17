//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960630
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/floatattribute.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/system/math.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class FloatAttribute.
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

FloatAttribute::FloatAttribute(const FloatAttribute &in) : Attribute(in) {
	exponent_ = in.exponent_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

FloatAttribute::FloatAttribute() {
	exponent_ = 0;
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

FloatAttribute::~FloatAttribute() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(FloatAttribute, FLOATATTRIBUTE, Attribute)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
FloatAttribute::Load(ifstream &stream) {

	if (!Attribute::Load(stream))
		return false;

	if (!IOKit::Load(stream, exponent_))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
FloatAttribute::Save(ofstream &stream) const {

	if (!Attribute::Save(stream))
		return false;

	if (!IOKit::Save(stream, GetScalingExponent()))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

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
FloatAttribute::Duplicate() const {
  return new FloatAttribute(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
FloatAttribute::Clear() {
	Attribute::Clear();
	SetScalingExponent(0);
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
FloatAttribute::GetType() const {
	return TYPE_FLOAT;
}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a real-world value, returns the internal coded
//                 value.
//
//                 Example (scaling exponent 2):
//
//                   GetEntry("3.14") returns 314
//                   GetEntry("Boat") returns Undefined::Integer();
//
// Comments......:
// Revisions.....:
//===================================================================

int
FloatAttribute::GetEntry(const String &text) const {

	if (text.IsEmpty() || (text == Undefined::String()))
		return Undefined::Integer();

	// Check input.
	if (!text.IsFloat()) {
		return Undefined::Integer();
	}

	int   e = GetScalingExponent();
	float f = text.GetFloat();
	float s = (e == 0) ? 1.0 : MathKit::Power(10.0, e);
	float p = f * s;

	return MathKit::Round(p);

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given an attribute index and an internal coded value,
//                 returns the corresponding real-world value.
//
//                 Example (scaling exponent 2):
//
//                   GetEntry(314) returns "3.14"
//
// Comments......:
// Revisions.....: 181120 N
// 			No more static.
//===================================================================

//const String &
const String 
FloatAttribute::GetEntry(int value) const {

	// So we can return a reference.
	//static String entry;
	String entry;
	// Check for undefined value.
	if (value == Undefined::Integer())
		return Undefined::String();

	// Get the scaling exponent.
	int e = GetScalingExponent();

	// Do we really need to bother with the scaling factor?
	if (e == 0) {
		entry = String::Format(value);
		return entry;
	}

	// Determine the real-world float value.
	float s = MathKit::Power(10.0, -e);
	float p = value * s;

	// Cut off string in case of numerical quirks in the conversion process.
	if (e > 0) {
		char   buffer[20];
		String format = "%." + String::Format(e) + "f";
		sprintf(buffer, format.GetBuffer(), p);
		entry = buffer;
	}
	else {
		entry = String::Format(p);
	}

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
FloatAttribute::SuggestEntry(const String &text) const {
	return GetEntry(text);
}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Ignored, returns true.
// Comments......: Consider checking the scaling factor.
// Revisions.....:
//===================================================================

bool
FloatAttribute::SetEntry(int /*value*/, const String &/*text*/) {
	return true;
}

//-------------------------------------------------------------------
// Method........: GetScalingExponent
// Author........: Aleksander šhrn
// Date..........:
// Description...: The scaling factor is a power of 10, signifying the
//                 floating point number "resolution" of the attribute.
//                 The scaling exponent is the logarithm of the
//                 scaling factor.
// Comments......:
// Revisions.....:
//===================================================================

int
FloatAttribute::GetScalingExponent() const {
	return exponent_;
}

//-------------------------------------------------------------------
// Method........: SetScalingExponent
// Author........: Aleksander šhrn
// Date..........:
// Description...: The input argument is interpreted as the
//                 exponent n of the scaling factor 10^n.  In effect,
//                 the exponent thus signifies the resolution for
//                 the attribute's floating point values.
// Comments......:
// Revisions.....:
//===================================================================

bool
FloatAttribute::SetScalingExponent(int exponent) {
	exponent_ = exponent;
	return true;
}

 
