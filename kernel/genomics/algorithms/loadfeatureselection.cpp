//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/loadfeatureselection.h>
#include <kernel/genomics/algorithms/featureselector.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/system/fstream.h>

#include <kernel/basic/message.h>


//-------------------------------------------------------------------
// Methods for class LoadFeatureSelection.
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

LoadFeatureSelection::LoadFeatureSelection() {
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

LoadFeatureSelection::~LoadFeatureSelection() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(LoadFeatureSelection, LOADFEATURESELECTION, FeatureSelector)

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
LoadFeatureSelection::GetParameters() const {
	
	String parameters;
	
	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();
	
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
LoadFeatureSelection::SetParameter(const String &keyword, const String &value) {
	
	if (keyword == Keyword::Filename())
		return SetFilename(value);
	
	// For backwards compatibility.
	//if (keyword == Keyword::Reduct())
	//	return SetAttributes(value);
	
	return false;
	
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Jacob Vesterlund
// Date..........: 20040320
// Description...: Loads a feature selection from a file,
//								 specified in the parameter filename.
//								
//
//
// Revisions.....:
//===================================================================

Structure *
LoadFeatureSelection::Apply(Structure &structure) const {
	
	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;
	
	
	Message message;
	// Cast to correct type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	
	bool masked = true;
	
	int dcolumn= table->GetDecisionAttribute(masked);
	int no_attributes = table->GetNoAttributes(masked);
	
	int no_objects    = table->GetNoObjects(masked);
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	Handle<DecisionTable> featuretable = Creator::DecisionTable();
	bool noj;
	
	
	
	
	ifstream stream;
	
	if (!IOKit::Open(stream, GetFilename())) {
		Message::Error("Error opening file " + GetFilename() + " for input.");
		return NULL;
	}
	featuretable->Load(stream);
	
	// Check that featuretable and table fit each other...ie
	// has the right dimensions,  and that dcolumn is 1 in featuretable 
	
	
	int fno_attributes = featuretable->GetNoAttributes(masked);
	
	int fno_objects    = featuretable->GetNoObjects(masked);
	
	if (!((fno_objects==no_attributes) && (fno_attributes>1))) {
		message.Error("Wrong dimension of feature selection file");
		return NULL;
	}
	// Extract a string vector with the decision attribute
	Vector (int) fvalues;
	Vector (int) keepindices;
	Vector (int) indices;
	fvalues.reserve(fno_objects);
	keepindices.reserve(fno_objects);
	indices.reserve(fno_objects);
	
	
	featuretable->GetEntries(fvalues, featuretable->GetDecisionAttribute(masked), masked,true);
	featuretable->GetEntries(indices, 1, masked,true);
	
	for (int k=0;k<fno_objects;k++) {
		keepindices[indices[k]] = fvalues[k];
	}		
	
	
	Vector(bool) removes; removes.reserve(no_attributes);
	
	// set removes
	
	// fill the vector 
	int tkeep=0;
	int i;
	for (i = 0; i < (no_attributes); i++) {
		if (keepindices[i]==1) {
			removes.push_back(0);	
			tkeep++;
		}
		else {
			removes.push_back(1);	
		}
		
		
	}
	// always keep class column
	if (removes[dcolumn]) {
		tkeep++;
		removes[dcolumn]=0;
		message.Warning("You tried to exclude the class attribute from the feature set. It was added.");
	}
	
	if (!subtable1->Resize(no_objects, tkeep, masked) )
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
	
	
	
	subtable1->SetName(table->GetName() + " (with feature selection)");
	featuretable->SetName("Feature Selection Table of " + table->GetName());
	
	
	
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
	
	return featuretable.Release();
}

 
LoadFeatureSelection *
LoadFeatureSelection::Clone() {
	return new LoadFeatureSelection(*this);
}
