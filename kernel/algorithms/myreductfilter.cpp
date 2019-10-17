//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/myreductfilter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>

//-------------------------------------------------------------------
// Methods for class MyReductFilter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

MyReductFilter::MyReductFilter() {
	SetFilteringStrategy(0);
	SetFilteringConnective(CONNECTIVE_OR);
	SetLowerLengthThreshold(10);
	SetUpperLengthThreshold(1000);
	SetLowerSupportThreshold(0);
	SetUpperSupportThreshold(40);
	SetAttribute(Undefined::Integer());
}

MyReductFilter::~MyReductFilter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(MyReductFilter, MYREDUCTFILTER, ReductFilter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Allow compound filtering.
//===================================================================

String
MyReductFilter::GetParameters() const {

	String parameters;

	// Filtering strategy (compound).
	parameters += Keyword::Filtering();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetFilteringStrategy());

	parameters += Keyword::Separator();

	// Filtering connective.
	parameters += Keyword::Connective();
	parameters += Keyword::Assignment();
	parameters += GetString(GetFilteringConnective());


	// Length parameters.
	if (GetFilteringStrategy() & FILTERING_LENGTH) {

	  parameters += Keyword::Separator();

		parameters += Keyword::Length() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerLengthThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Length() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperLengthThreshold());

	}

	// Support parameters.
	if (GetFilteringStrategy() & FILTERING_SUPPORT) {

		parameters += Keyword::Separator();

		parameters += Keyword::Support() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetLowerSupportThreshold());

		parameters += Keyword::Separator();

		parameters += Keyword::Support() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetUpperSupportThreshold());

	}

	// Attribute parameters.
	if (GetFilteringStrategy() & FILTERING_ATTRIBUTE) {

		parameters += Keyword::Separator();

		parameters += Keyword::Attribute();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetAttribute());

	}

	parameters += Keyword::Separator();

	return parameters + ReductFilter::GetParameters();

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 0797 - Allow compound filtering.
//===================================================================

bool
MyReductFilter::SetParameter(const String &keyword, const String &value) {

	// Filtering strategy.
	if (keyword == Keyword::Filtering() && value.IsInteger())
		return SetFilteringStrategy(value.GetInteger());

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_LENGTH))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_LENGTH);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_SUPPORT))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_SUPPORT);

	if (keyword == Keyword::Filtering() && value == GetString(FILTERING_ATTRIBUTE))
		return SetFilteringStrategy(GetFilteringStrategy() | FILTERING_ATTRIBUTE);

	// Filtering connective.
	if (keyword == Keyword::Connective() && value == GetString(CONNECTIVE_OR))
		return SetFilteringConnective(CONNECTIVE_OR);

	if (keyword == Keyword::Connective() && value == GetString(CONNECTIVE_AND))
		return SetFilteringConnective(CONNECTIVE_AND);

	// Lower length threshold.
	if ((keyword == Keyword::Length() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerLengthThreshold(value.GetInteger());

	// Lower length threshold (backwards compatibility).
	if ((keyword == Keyword::Threshold() + Keyword::Dot() + Keyword::Length() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerLengthThreshold(value.GetInteger());

	// Lower length threshold (backwards compatibility).
	if (keyword == Keyword::LLThreshold() && value.IsInteger())
		return SetLowerLengthThreshold(value.GetInteger());

	// Upper length threshold.
	if ((keyword == Keyword::Length() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperLengthThreshold(value.GetInteger());

	// Upper length threshold (backwards compatibility).
	if ((keyword == Keyword::Threshold() + Keyword::Dot() + Keyword::Length() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperLengthThreshold(value.GetInteger());

	// Upper length threshold (backwards compatibility).
	if (keyword == Keyword::ULThreshold() && value.IsInteger())
		return SetUpperLengthThreshold(value.GetInteger());

	// Lower support threshold.
	if ((keyword == Keyword::Support() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerSupportThreshold(value.GetInteger());

	// Lower support threshold (backwards compatibility).
	if ((keyword == Keyword::Threshold() + Keyword::Dot() + Keyword::Support() + Keyword::Dot() + Keyword::Lower()) && value.IsInteger())
		return SetLowerSupportThreshold(value.GetInteger());

	// Lower support threshold (backwards compatibility).
	if (keyword == Keyword::LSThreshold() && value.IsInteger())
		return SetLowerSupportThreshold(value.GetInteger());

	// Upper support threshold.
	if ((keyword == Keyword::Support() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperSupportThreshold(value.GetInteger());

	// Upper support threshold (backwards compatibility).
	if ((keyword == Keyword::Threshold() + Keyword::Dot() + Keyword::Support() + Keyword::Dot() + Keyword::Upper()) && value.IsInteger())
		return SetUpperSupportThreshold(value.GetInteger());

	// Upper support threshold (backwards compatibility).
	if (keyword == Keyword::USThreshold() && value.IsInteger())
		return SetUpperSupportThreshold(value.GetInteger());

	// Attribute.
	if (keyword == Keyword::Attribute() && value.IsInteger())
		return SetAttribute(value.GetInteger());

	return ReductFilter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Methods inherited from Filter.
//===================================================================

//-------------------------------------------------------------------
// Method........: Remove
// Author........: Aleksander šhrn
// Date..........:
// Description...: Return true if the specified reduct should be removed
//                 from the reduct set.
// Comments......:
// Revisions.....: Aš 0797 - Allow compound filtering.
//                 Aš 0198 - Connective.
//===================================================================

bool
MyReductFilter::Remove(const Structures &structures, int i) const {

	if (!structures.IsA(REDUCTS))
		return false;

	Handle<Reducts> reducts = dynamic_cast(Reducts *, const_cast(Structures *, &structures));
	Handle<Reduct>  reduct  = reducts->GetReduct(i);

	bool is_or  = (GetFilteringConnective() == CONNECTIVE_OR);
	bool is_and = (GetFilteringConnective() == CONNECTIVE_AND);

	bool prune = false;

	// Prune according to reduct length?
	if (GetFilteringStrategy() & FILTERING_LENGTH) {

		int length = reduct->GetNoAttributes();
		int upper  = GetUpperLengthThreshold();
		int lower  = GetLowerLengthThreshold();

		// Remove?
		prune = ((length <= upper) && (length >= lower));

		// Short-circuit evaluation.
		if (is_or && prune)
			return true;
		if (is_and && !prune)
			return false;

	}


	// Prune according to reduct support?
	if (GetFilteringStrategy() & FILTERING_SUPPORT) {

		int support = reduct->GetSupport();
		int upper   = GetUpperSupportThreshold();
		int lower   = GetLowerSupportThreshold();

		// Remove?
		prune = ((support <= upper) && (support >= lower));

		// Short-circuit evaluation.
		if (is_or && prune)
			return true;
		if (is_and && !prune)
			return false;

	}

	// Prune according to reduct contents?
	if (GetFilteringStrategy() & FILTERING_ATTRIBUTE) {

		// Remove?
		prune = reduct->IsMember(GetAttribute());

		// Short-circuit evaluation.
		if (is_or && prune)
			return true;
		if (is_and && !prune)
			return false;

	}

	return prune;

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
MyReductFilter::GetString(Filtering filtering) {

	switch (filtering) {
		case FILTERING_LENGTH:         return "Length";;
		case FILTERING_SUPPORT:        return "Support";
		case FILTERING_ATTRIBUTE:      return "Attribute";;
		default:                       return Undefined::String();
	}

}

String
MyReductFilter::GetString(Connective connective) {

	switch (connective) {
		case CONNECTIVE_OR:            return "Or";
		case CONNECTIVE_AND:           return "And";
		default:                       return Undefined::String();
	}

}
 
MyReductFilter *
MyReductFilter::Clone() {
	return new MyReductFilter;
}
