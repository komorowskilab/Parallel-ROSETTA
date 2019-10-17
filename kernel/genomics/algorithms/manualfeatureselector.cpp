//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/manualfeatureselector.h>
#include <kernel/genomics/algorithms/featureselector.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>

#include <kernel/system/fstream.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>


//-------------------------------------------------------------------
// Methods for class ManualFeatureSelector.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Sets default parameters.
// Revisions.....:
//===================================================================

ManualFeatureSelector::ManualFeatureSelector() {
	//SetAttributes("");
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ManualFeatureSelector::~ManualFeatureSelector() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

// IMPLEMENTIDMETHODS(ManualFeatureSelector, MANUALFEATURESELECTOR, Reducer)
IMPLEMENTIDMETHODS(ManualFeatureSelector, MANUALFEATURESELECTOR, FeatureSelector)

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
ManualFeatureSelector::GetParameters() const {
	
	String parameters;
	
	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += GetAttributes();
	
	parameters += Keyword::Separator();
	
	parameters += FeatureSelector::GetParameters();
	
	
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
ManualFeatureSelector::SetParameter(const String &keyword, const String &value) {
	
	
	
	// Set attribute descriptions.
	if (keyword == Keyword::Attributes())
		return SetAttributes(value);
	
	// For backwards compatibility.
	//if (keyword == Keyword::Reduct())
	//	return SetAttributes(value);
	
	return false;
	
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Performs a manual feature selection, 
//								 according to the parameter attributes.
//								 if attributes="1011", attributes
//								 1, 3 and 4 are kept in the new table.
//	
//
//
//===================================================================

Structure *
ManualFeatureSelector::Apply(Structure &structure) const {
	
	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;
	
	String attributes = GetAttributes();
	
	Message message;
	// Cast to correct type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	
	bool masked = true;
	
	int dcolumn= table->GetDecisionAttribute(masked);
	int no_attributes = table->GetNoAttributes(masked);
	
	int no_objects    = table->GetNoObjects(masked);
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	Handle<DecisionTable> featuretable = Creator::DecisionTable();
	
	
	
	Vector(bool) removes; removes.reserve(no_attributes);
	
	
	
	// fill the vector 
	int tkeep=0;
	int i;
	for (i = 0; i < (no_attributes); i++) {
		if (attributes[i]=='1')
		{
			removes.push_back(0);	
			tkeep++;
		}
		else 
		{
			removes.push_back(1);	
		}
		
		
	}
	// always keep class column
	if (removes[dcolumn])
	{
		tkeep++;
		removes[dcolumn]=0;
		message.Warning("You tried to exclude the class attribute from the feature set. It was added.");
	}
	
	if (!subtable1->Resize(no_objects, tkeep, masked) )
		return NULL;
	
	if (!featuretable->Resize(no_attributes, 2, masked) )
		return NULL;
	
	
	int j;
	int skipped=0;
	for (i = 0; i < no_objects; i++) {
		
		//  This is where we select only the selected attributes.
		
		skipped=0;
		
		for (j = 0; j < no_attributes; j++) {			
			
			// should it be included	
			
			if (removes[j]==1) {
				skipped++;						
			}
			else if (!subtable1->SetEntry(i, j-skipped, table->GetEntry(i, j, masked), masked))	{	
				return NULL;
			}
			
		}
	}
	
	if (table->HasDictionary()) {
		
		subtable1->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
		
		// we have to throw away the attributes not needed anymore.
		
		for (j=no_attributes-1;j>=0;j--){
			if (removes[j]){
				subtable1->GetDictionary()->RemoveAttribute(j);
			}
		}
		
	}
	else {
		subtable1->SetDictionary(NULL);
		
	}
	
	Vector(Attribute::Type) types;
	Vector(String) names;
	Vector(int) scales;
	types.reserve(3);
	names.reserve(3);
	scales.reserve(3);
	types.push_back(Attribute::TYPE_STRING);
	types.push_back(Attribute::TYPE_INTEGER);
	types.push_back(Attribute::TYPE_INTEGER);
	
	names.push_back("Attribute");
	names.push_back("Index");
	names.push_back("Selected");
	
	scales.push_back(1);
	scales.push_back(1);
	scales.push_back(1);
	
	if(!(InitFeatureSelectionTable(*table, *featuretable, removes, names, types, scales))) 
		return NULL;
	
	masked = false;
	
	subtable1->SetName(table->GetName() + " (with feature selection)");
	featuretable->SetName("Feature Selection Table of " + table->GetName());
	
	if (GetSaveFeatureSelection())
	{
		ofstream stream;
		
		if (!IOKit::Open(stream, GetSaveFeatureSelectionFilename()) ){
			Message::Error("Error opening" + GetSaveFeatureSelectionFilename() + " for input.");
			return NULL;
		}
		
		
		featuretable->Save(stream);
		IOKit::Close(stream);
	}
	
	
	if (AppendAsChildren()) {
		// Append the subtables as children to the input table.
		if (!table->AppendChild(subtable1.GetPointer()) ||!table->AppendChild(featuretable.GetPointer()) ) {
			Message::Error("Error appending the subtable to the parent table.");
			return NULL;
		}
		// Return the input table itself.
		return subtable1.Release();
		
	}
	else {
		// Create a decision table set.
		Handle<DecisionTables> subtables = Creator::DecisionTables();
		if (!subtables->AppendStructure(subtable1.GetPointer()) ) {
			Message::Error("Error appending the subtable to the subtable set.");
			return NULL;
		}
		// Return the set of subtables.
		return subtables.Release();
	}
	
	
}


 
ManualFeatureSelector *
ManualFeatureSelector::Clone() {
	return new ManualFeatureSelector;
} 
