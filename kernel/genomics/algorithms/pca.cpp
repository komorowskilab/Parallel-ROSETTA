//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/genomics/algorithms/pca.h>
#include <kernel/algorithms/keyword.h>
#include <math.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/genomics/algorithms/featureselector.h>
#include <kernel/utilities/creator.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/basic/message.h>


//-------------------------------------------------------------------
// Methods for class PrincipalComponentAnalysis.
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

PrincipalComponentAnalysis::PrincipalComponentAnalysis() {
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

PrincipalComponentAnalysis::~PrincipalComponentAnalysis() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

// IMPLEMENTIDMETHODS(PrincipalComponentAnalysis, MANUALFEATURESELECTOR, Reducer)
IMPLEMENTIDMETHODS(PrincipalComponentAnalysis, PRINCIPALCOMPONENTANALYSIS, FeatureSelector)

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
PrincipalComponentAnalysis::GetParameters() const {

	String parameters;
	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAttributes());

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
PrincipalComponentAnalysis::SetParameter(const String &keyword, const String &value) {

	

	// Set attribute descriptions.
	if (keyword == Keyword::Attributes() && value.IsInteger())
		return SetAttributes(value.GetInteger());

	// For backwards compatibility.
	//if (keyword == Keyword::Reduct())
	//	return SetAttributes(value);

	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs a search for principal components 
//								 for the specified decisiontable, 
//								 the decision attribute(s) are not included
//								 when the analysis is performed. 
//
//								 The output is two decision tables, the first 
//								 one is the reduced original data set, but with
//								 the selected number of components as attributes,
//								 the second one includes the principal components, 
//								 and their eigenvalues.
//
//
// Revisions.....:
//===================================================================

Structure *
PrincipalComponentAnalysis::Apply(Structure &structure) const {

	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;
	
	String attributes = GetAttributes();

	

	Message message;
	//	message.Warning("Maximum Components :" + String::Format(GetAttributes()));
	// Cast to correct type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	bool masked = true;

	int dcolumn= table->GetDecisionAttribute(masked);
	int no_attributes = table->GetNoAttributes(masked);


	int no_objects    = table->GetNoObjects(masked);

	if (table->HasMissingEntries(masked))
	{
		message.Error("The table contains undefined values, PCA is not possible");
		return false;
	}
	for (int a=0;a<no_attributes-1;a++)
	{

		if (!((table->GetAttributeType(a,masked))== (Attribute::TYPE_FLOAT) ||table->GetAttributeType(a,masked)==Attribute::TYPE_INTEGER))
		{
			message.Error("The table contains non-decision attribute(s) that is not of float or integer type \n Either remove or mask these attribute(s)");
			return table.Release();
		}

	}
	Handle<DecisionTable>	subtable1 = Creator::DecisionTable();

	
		
	Handle<DecisionTable> featuretable = Creator::DecisionTable();

	
    MakeAnalysis(*table, *featuretable, table->GetNoAttributes(masked)-1);	
	Transform(*featuretable, *table, *subtable1, GetAttributes());

	Vector(bool) removes; removes.reserve(no_attributes);


	
	// fill the vector 
	int tkeep=0;
	for (int i = 0; i < (no_attributes); i++) {
		if (attributes[i]=='1')	{
			removes.push_back(0);	
			tkeep++;
		}
		else  {
			removes.push_back(1);	
		}


	}
	// always keep class column
	if (removes[dcolumn]) {
		tkeep++;
		removes[dcolumn]=0;
	}



	int j;
	int skipped=0;

	subtable1->SetName(table->GetName() + " (after PCA)");
	featuretable->SetName("Principal Components of " + table->GetName());


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

// This functions performs the pca, using the functions in StatKit.
//

bool PrincipalComponentAnalysis:: MakeAnalysis(DecisionTable &hej, DecisionTable &pcatable, int size) const
{
	bool masked =true;

	int num_pc=GetAttributes();
	Message message;
	int i,j;
	Matrix <float> calcmatrix(size,size);

	Vector (float) d; d.reserve(size);
	Vector (float) e; e.reserve(size);

	for (i=0;i<size;i++) {
		d.push_back(0.0f);
		e.push_back(0.0f);
	}

	StatKit myKit;
	
	myKit.CoVarianceMatrix(hej, calcmatrix);
	myKit.tred2(calcmatrix, size, d, e);
	myKit.tqli(d, e, size, calcmatrix);
	myKit.Matrix2DecisionTable(calcmatrix, size , pcatable);		

	for (i=0;i<size;i++) {
		pcatable.SetEntry(i,size,d[i]*100000,masked);

	}
	pcatable.SortObjects(size, masked);
	int mya=size-num_pc;
	for (i=0;i<size;i++) {
		// message.Warning("i:" + String::Format(i) + "\nmya:" + String::Format(mya));
		if (i>=mya) {
			pcatable.SetEntry(i, size+1,1,masked);
		}
		else {
			pcatable.SetEntry(i,size+1, 0, masked);
		}
		
	}

	// free..
	return true;
}

//
// This function performs the transformation to the new value space, 
// from the original data table and the pcaTable(which contains the new 
// new principal components) to new decisiontable where the (int component) first 
// components are used.

bool PrincipalComponentAnalysis::Transform(DecisionTable &pcaTable, DecisionTable &dataTable, DecisionTable &newTable, int components) const
{

	Message message;
	bool masked=true;
	int i,j,k;
	float sum;
// resiza decision täjbeln

	Vector(Attribute::Type) types;
	Vector(String) names;
	Vector(int) scales;
	types.reserve(components+1);
	names.reserve(components+1);
	scales.reserve(components+1);

	for(i=0;i<components;i++) {
		types.push_back(Attribute::TYPE_FLOAT);
		names.push_back("Component (" + String::Format(i+1) + ")");
		scales.push_back(5);
	}
	
	// Kopiera klasskolumnen rent av.


	int new_no_attributes = components +1;
	int no_objects = dataTable.GetNoObjects(masked);
	int no_attributes = dataTable.GetNoAttributes(masked);


// message.Warning ("here 1");
	// Empty the input table. Let the table have one initial object.  (If, e.g., the
	// decision table is a RSES decision table, attributes can only be inserted/appended
	// if at least one object is present.)
	if (!newTable.Resize(1, 0, masked))
	 	return false;

	// Create an empty dictionary.
	Handle<Dictionary> dictionary = Creator::Dictionary();

	// Assign the empty dictionary to the table.
	if (!newTable.SetDictionary(dictionary.GetPointer()))
		return false;

	
	// message.Warning ("here 3");

// Add attributes to the table (and dictionary).
	for (i = 0; i < new_no_attributes-1; i++) {

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
		if (!newTable.AppendAttribute(attribute.GetPointer(), masked))
		{	message.Error("3");
			return false;
			
		}

			
	}
	
	if (!newTable.AppendAttribute(dataTable.GetDictionary()->GetAttribute(dataTable.GetUnmaskedAttribute(dataTable.GetDecisionAttribute(masked))),masked))  { 
		message.Error("Unable to append class attribute");
		return false;
	
	}
	
	if (!newTable.Resize(dataTable.GetNoObjects(masked), components+1, masked))
	{	message.Error("Unable to resize table");
		return false;
			
	}


// vektor med medel f•r alla 

	Vector (float) means;
	Vector (float) tempEntries;
	means.reserve(no_attributes);
	tempEntries.reserve(no_objects);

	for (j=0;j<(no_attributes-1); j++){
		dataTable.GetEntries(tempEntries, j, masked, true, true);
		means.push_back(MathKit::Mean(tempEntries));
	}
//
	for (j=0;j<no_objects;j++) {
		for (i=0;i<components;i++) {
			sum=0.0f;						
			for (k=0;k<dataTable.GetNoAttributes(masked)-1;k++) {
								
				sum += ((MathKit::Power(10.0, -dataTable.GetAttributeScalingExponent(k,masked)) * dataTable.GetEntry(j,k,masked))-means[k] )*pcaTable.GetEntry(pcaTable.GetNoObjects(masked)-1-i,k,masked);	
								
			}
			newTable.SetEntry(j, i, sum, masked);
		}
		// setta here med en column..whatever.
	}

		
	for (j=0;j<dataTable.GetNoObjects(masked);j++) {
		newTable.SetEntry(j, components , dataTable.GetEntry(j, dataTable.GetDecisionAttribute(masked) ,masked), masked);
		
	}
	
	
	return true;

}

//
// This function performs sort of a pre-calculation of the components,
// and makes most sense used in the Windows version, where its output 
// is plotted as a graph

bool PrincipalComponentAnalysis::PreCalculate(Structure &structure, Vector (float) &eigenvalues) 
{
	Message message;
	int i;
	Handle<DecisionTable> hej = dynamic_cast(DecisionTable *, &structure);

	bool masked=true;

	
	
	int size= hej->GetNoAttributes(masked)-1;
	
	if (hej->HasMissingEntries(masked))
	{
		message.Error("The table contains undefined values, PCA is not possible");
		return false;
	}

	for (int a=0;a<size;a++)
	{

		if (!((hej->GetAttributeType(a,masked))== (Attribute::TYPE_FLOAT) ||hej->GetAttributeType(a,masked)==Attribute::TYPE_INTEGER))
		{
			message.Error("The table contains non-decision attribute(s) that is not of float or integer type \n Either remove or mask these attribute(s)");
			return false;
		}

	}





	Matrix <float> calcmatrix(size,size);

	Vector (float) e; e.reserve(size);

	for (i=0;i<size;i++) {
		e.push_back(0.0f);
	}

	StatKit myKit;

    myKit.CoVarianceMatrix(*hej, calcmatrix);

	myKit.tred2(calcmatrix, size, eigenvalues, e);

    myKit.tqli(eigenvalues, e, size, calcmatrix);;
	
 	std::sort(eigenvalues.begin(), eigenvalues.end());
 
	return true;
}
 
PrincipalComponentAnalysis *
PrincipalComponentAnalysis::Clone() {
	return new PrincipalComponentAnalysis;
} 
