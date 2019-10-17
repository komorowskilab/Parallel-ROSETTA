//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Parent class for feature selection algorithms
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/utilities/creator.h>

#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>

//-------------------------------------------------------------------
// Methods for class FeatureSelector.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

FeatureSelector::FeatureSelector() {
	AppendAsChildren(true);
	SetSaveFeatureSelection(false);
}

FeatureSelector::~FeatureSelector() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(FeatureSelector, FEATURESELECTOR, Algorithm)

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
FeatureSelector::GetParameters() const {
	
	String parameters;
	
	parameters += Keyword::Append();
	parameters += Keyword::Assignment();
	parameters += String::Format(AppendAsChildren());
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::SaveFeatureSelection();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSaveFeatureSelection());
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::SaveFeatureSelectionFilename();
	parameters += Keyword::Assignment();
	parameters += GetSaveFeatureSelectionFilename();
	
	
	parameters += Keyword::Separator();
	
	return parameters;
	
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
FeatureSelector::SetParameter(const String &keyword, const String &value) {
	
	if (keyword == Keyword::Append() && value.IsBoolean())
		return AppendAsChildren(value.GetBoolean());
	
	
	if (keyword == Keyword::SaveFeatureSelection() && value.IsBoolean())
		return SetSaveFeatureSelection(value.GetBoolean());
	
	if (keyword == Keyword::SaveFeatureSelectionFilename())
		return SetSaveFeatureSelectionFilename(value);
	
	return false;
	
}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
FeatureSelector::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}




bool
FeatureSelector:: InitFeatureSelectionTable(DecisionTable &table, DecisionTable &featuretable, Vector(bool) &removes, Vector(String) &names, Vector(Attribute::Type) &types, Vector(int) &scales) const
{
	Message message;
	
	// dimension checks....
	
	if(!((names.size() == types.size()) && (names.size() == scales.size())))
	{
		message.Error("1");
		return false;
	}
	int new_no_attributes = names.size();
	bool masked = false;
	int no_attributes = table.GetNoAttributes(masked);
	
	int i,j;
	// Empty the input table. Let the table have one initial object.  (If, e.g., the
	// decision table is a RSES decision table, attributes can only be inserted/appended
	// if at least one object is present.)
	if (!featuretable.Resize(1, 0, masked))
		return false;
	
	// Create an empty dictionary.
	Handle<Dictionary> dictionary = Creator::Dictionary();
	
	// Assign the empty dictionary to the table.
	if (!featuretable.SetDictionary(dictionary.GetPointer()))
		return false;
	
	// Add attributes to the table (and dictionary).
	for (i = 0; i < new_no_attributes; i++) {
		
		// Skip non-selected columns.
		//if (removes[i])
		// continue;
		
		Handle<Attribute> attribute;
		
		switch (types[i]) {
		case Attribute::TYPE_STRING:  attribute = Creator::StringAttribute();                      break;
		case Attribute::TYPE_INTEGER: attribute = Creator::IntegerAttribute();                     break;
		case Attribute::TYPE_FLOAT:   attribute = Creator::FloatAttribute(scales[i], NULL, false); break;
		default:                      attribute = Creator::StringAttribute();                      break;
		}
		
		if (!attribute->SetName(names[i]))
		{	message.Error("2");
		return false;
		
		}
		if (!featuretable.AppendAttribute(attribute.GetPointer(), masked))
		{	message.Error("3");
		return false;
		
		}
		
	}
	
	
	if (!featuretable.Resize(no_attributes, new_no_attributes, masked))
	 		{	message.Error("4");
	return false;
	
	}
	
	
	
	
	for (j = 0; j < no_attributes; j++) {			
		
		
		if (!featuretable.SetEntry(j, new_no_attributes-1, !removes[j], masked))
		{	
			message.Error("5");
			return false;
		}
		if (!featuretable.SetEntry(j, 1, j,masked))
		{	
			message.Error("6");
			return false;
		}
		
		
		if (!featuretable.SetEntry(j, "Attribute", table.GetAttributeName(j,masked), masked))
		{	
			message.Error("7");
			return false;
		}
		
		
		
	}
	
	return true;
} 
