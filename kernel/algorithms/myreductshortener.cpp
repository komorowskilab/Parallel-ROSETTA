//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/myreductshortener.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>

#include <kernel/utilities/mathkit.h>

#include <kernel/basic/bits.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class MyReductShortener.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyReductShortener::MyReductShortener() {
	SetShorteningStrategy(0);
	SetLowerPercentage(0.0f);
	SetUpperPercentage(90.0f);
	SetAttribute(0);
	SetPrimaryAttribute(0);
	SetSecondaryAttribute(1);
}

MyReductShortener::~MyReductShortener() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyReductShortener, MYREDUCTSHORTENER, ReductShortener)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
MyReductShortener::GetParameters() const {

	String parameters;

	// Shortening strategy (compound).
	parameters += Keyword::Shortening();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetShorteningStrategy());

	parameters += Keyword::Separator();

	// Percentage parameters.
	if (GetShorteningStrategy() & SHORTENING_PERCENTAGE) {

		parameters += Keyword::Percentage() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerPercentage());

		parameters += Keyword::Separator();

		parameters += Keyword::Percentage() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperPercentage());

		parameters += Keyword::Separator();

	}

	// Occurrence parameters.
	if (GetShorteningStrategy() & SHORTENING_OCCURRENCE) {

		parameters += Keyword::Attribute();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetAttribute());

		parameters += Keyword::Separator();

	}

	// Combined occurrence parameters.
	if (GetShorteningStrategy() & SHORTENING_COMBINEDOCCURRENCE) {

		parameters += Keyword::Primary();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetPrimaryAttribute());

		parameters += Keyword::Separator();

		parameters += Keyword::Secondary();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetSecondaryAttribute());

		parameters += Keyword::Separator();

	}

	return parameters + ReductFilter::GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
MyReductShortener::SetParameter(const String &keyword, const String &value) {

	// Shortening strategy.
	if (keyword == Keyword::Shortening() && value.IsInteger())
		return SetShorteningStrategy(value.GetInteger());

	if (keyword == Keyword::Shortening() && value == GetString(SHORTENING_PERCENTAGE))
		return SetShorteningStrategy(GetShorteningStrategy() | SHORTENING_PERCENTAGE);

	if (keyword == Keyword::Shortening() && value == GetString(SHORTENING_OCCURRENCE))
		return SetShorteningStrategy(GetShorteningStrategy() | SHORTENING_OCCURRENCE);

	if (keyword == Keyword::Shortening() && value == GetString(SHORTENING_COMBINEDOCCURRENCE))
		return SetShorteningStrategy(GetShorteningStrategy() | SHORTENING_COMBINEDOCCURRENCE);

	// Lower percentage.
	if ((keyword == Keyword::Percentage() + Keyword::Dot() + Keyword::Lower()) && value.IsFloat())
		return SetLowerPercentage(value.GetFloat());

	// Lower percentage (backwards compatibility).
	if (keyword == Keyword::LPercentage() && value.IsFloat())
		return SetLowerPercentage(value.GetFloat());

	// Upper percentage.
	if ((keyword == Keyword::Percentage() + Keyword::Dot() + Keyword::Upper()) && value.IsFloat())
		return SetUpperPercentage(value.GetFloat());

	// Upper percentage (backwards compatibility).
	if (keyword == Keyword::UPercentage() && value.IsFloat())
		return SetUpperPercentage(value.GetFloat());

  // Attribute, absolute presence.
	if (keyword == Keyword::Attribute() && value.IsInteger())
		return SetAttribute(value.GetInteger());

  // Attribute, primary, relative presence.
	if (keyword == Keyword::Primary() && value.IsInteger())
		return SetPrimaryAttribute(value.GetInteger());

  // Attribute, secondary, relative presence.
	if (keyword == Keyword::Secondary() && value.IsInteger())
		return SetSecondaryAttribute(value.GetInteger());

	return ReductFilter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes a reduct set as input and shortens each reduct
//                 according to some shortening scheme.
//
//                 Attribute indices are virtual, i.e. relative to the
//                 masked table.
//
// Comments......: Assumes that library clients use handles.
//
//                 Compound criterion currently functions as an 'OR',
//                 perhaps it should function as an 'AND'...?
//
// Revisions.....: Aš 0897 - Added compound criterion.
//===================================================================

Structure *
MyReductShortener::Apply(Structure &structure) const {

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	Handle<Reducts> reducts = dynamic_cast(Reducts *, &structure);

	// Is shortening allowed?  (Better safe than sorry, one might @±%” up the children's indices.)
	if (reducts->HasChildren()) {
		Message::Error("Shortening of reducts with children is not allowed.");
		return NULL;
	}

	// Get size of original reduct set.
	int no_reducts = reducts->GetNoReducts();

	Map(int, int) histogram;

	int i, j, k;

	// If percentage shortening is chosen, a histogram has to be built.
	if (GetShorteningStrategy() & SHORTENING_PERCENTAGE) {
		Vector(int) attributes;
		attributes.reserve(no_reducts * 5);
		for (i = 0; i < no_reducts; i++) {                          // Extract all attributes.
			Handle<Reduct> reduct = reducts->GetReduct(i);
			int no_attributes = reduct->GetNoAttributes();
			for (j = 0; j < no_attributes; j++)
				attributes.push_back(reduct->GetAttribute(j));
		}
		if (!MathKit::Distribution(histogram, attributes)) {        // Calculate histogram.
			Message::Error("Could not calculate histogram.");
			return NULL;
		}
	}

	Message message;

	// Remove all attributes that match the shortening criterion.
	for (i = 0; i < no_reducts; i++) {

		if ((i == 0) || ((i + 1) % 100) || (i == no_reducts - 1)) { // Give progress message.
			if (!message.Progress("Shortening reducts...", i, no_reducts))
				break;
		}

		Handle<Reduct> reduct = reducts->GetReduct(i);             // The reduct to shorten.
		int no_attributes = reduct->GetNoAttributes();             // Number of attributes in current reduct.

		Bits positions(no_attributes, false);                      // Array of positions of attributes to remove.
                                                               // As default, do not schedule attribute for removal.
		// Determine positions of attributes to remove.
		for (j = 0; j < no_attributes; j++) {

			int attribute = reduct->GetAttribute(j);                 // Index of attribute is current position in current reduct.

			// Shorten according to occurrence frequency?
			if (GetShorteningStrategy() & SHORTENING_PERCENTAGE) {
				float percentage = static_cast(float, histogram[attribute]) / no_reducts;
				if ((GetUpperPercentage() >= percentage) && (GetLowerPercentage() <= percentage))
					positions[j] = true;
			}

			// Shorten according to absolute presence?
			if (GetShorteningStrategy() & SHORTENING_OCCURRENCE) {
				if (attribute == GetAttribute())
					positions[j] = true;
			}

			// Shorten according to relative presence?
			if (GetShorteningStrategy() & SHORTENING_COMBINEDOCCURRENCE) {
				if (attribute == GetPrimaryAttribute()) {
					for (k = 0; k < no_attributes; k++) {
						if (reduct->GetAttribute(k) == GetSecondaryAttribute())
							break;
					}
					if (k < no_attributes)
						positions[j] = true;
				}
			}
		}

    // Check the removal flags, and do the shortening.
		for (j = no_attributes - 1; j >= 0; j--) {
			if (InvertLogic())                                       // Invert logic?
				positions[j] = !positions[j];
			if (positions[j] == false)                               // Do not remove attribute.
				continue;
			if (!reduct->RemoveAttribute(j)) {                       // Removal went sour.
				Message::Error("An error occurred when trying to remove attribute " + String::Format(j) + " from reduct " + String::Format(i) + ".");
			  //Message::Reset();                                      // Make sure to signal that we're done.
				return NULL;
			}
		}

	}

	// Remove duplicates.
	Message::Warning("Removal of duplicate reducts not implemented yet.");

	return reducts.Release();

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
MyReductShortener::GetString(MyReductShortener::Shortening shortening) {

	switch (shortening) {
		case SHORTENING_PERCENTAGE:         return "Percentage";
		case SHORTENING_OCCURRENCE:         return "Occurrence";
		case SHORTENING_COMBINEDOCCURRENCE: return "Combined";
		default:                            return Undefined::String();
	}

}
 

MyReductShortener *
MyReductShortener::Clone() {
	return new MyReductShortener;
}
