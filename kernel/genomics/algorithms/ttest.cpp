//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/ttest.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/genomics/utilities/statkit.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>

#include <kernel/system/fstream.h>
#include <kernel/utilities/iokit.h>

#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/utilities/rng.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/creator.h>
#include <kernel/basic/message.h>

#include <time.h>

struct ClassStat {
	int n;
	Vector (float) values;
	int o;
	Vector (float) ovalues;
	float var;
	float ovar;
	float average;
	float oaverage;
	float tvalue;
	float pvalue;
};



//-------------------------------------------------------------------
// Methods for class TTest.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

TTest::TTest() {
	SetAttributes(1);
	SetThreshold(0.5);
}

TTest::~TTest() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(TTest, TTEST, FeatureSelector)

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
TTest::GetParameters() const {

	String parameters;

	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAttributes());

	parameters += Keyword::Separator();

	parameters += Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetThreshold());

	parameters += Keyword::Separator();
	
	parameters += Keyword::Bootstrap();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetBootStrap());

	parameters += Keyword::Separator();

	
	parameters += Keyword::Straps();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetStraps());

	parameters += Keyword::Separator();

	
	parameters += Keyword::PThreshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetPThreshold());

	parameters += Keyword::Separator();



	parameters += Keyword::Generate();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetGenerateSeed());
	parameters += Keyword::Separator();

	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());

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
TTest::SetParameter(const String &keyword, const String &value) {

	if (keyword == Keyword::Attributes() && value.IsInteger())
		return SetAttributes(value.GetInteger());

	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetThreshold(value.GetFloat());

	if (keyword == Keyword::Bootstrap() && value.IsBoolean())
		return SetBootStrap(value.GetBoolean());

	if (keyword == Keyword::Straps() && value.IsInteger())
		return SetStraps(value.GetInteger());

	if (keyword == Keyword::PThreshold() && value.IsFloat())
		return SetPThreshold(value.GetFloat());

	if (keyword == Keyword::Generate() && value.IsBoolean())
		return SetGenerateSeed(value.GetBoolean());


	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());


	return FeatureSelector::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Jacob Vesterlund
// Date..........: 2004
// Description...: Takes a decision table as input and performs 
//	  						 a t-test on it's attributes, filtering out  
//								 a number of the attributes, with the lowest t-values. 
//
//								 The optional bootstrapping feature, performs a
//                 bootstrap test on the attributes, where attributes
//								 are removed if they fail the test, before filtering 
//								 according to t-values.
//
//                 If specified, the resulting subtable is appended
//                 to the input table as a child, and the input table
//                 itself is returned.  Alternatively, the subtable is
//                 not appended as a child, but is instead returned as
//                 a decision table set.
//
//===================================================================

Structure *
TTest::Apply(Structure &structure) const {

	Message message;

	// Check input.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	if ((GetThreshold()>1.0f || GetThreshold()<0.0f) && (!GetBootStrap())) {
		message.Error("Error : The significance level has to be between 0 and 1");
		return table.Release();
	}
	
	if ((GetPThreshold()>1.0f || GetPThreshold()<0.0f ) && (GetBootStrap())) {
		message.Error("Error : The bootstrap significance level has to be between 0 and 1");
		return table.Release();
	}

	bool masked = true;
	int attributes_out= GetAttributes();
// if(GetBootStrap())
// {
	int n_straps=GetStraps();
	int boot_count=0;
	float boot_rvals_var;
	float boot_rovals_var;
	float boot_rvals_avr;
	float boot_rvals_oavr;
	float boot_tval;
	int passed_boot=0;
	float df=0.0f;
	int my_seed=0;

	if (GetGenerateSeed())
	{ RNG  rng3(time(0));
		my_seed= rng3.DrawInteger(0,INT_MAX);
			
			
	}
	else 
	{
		my_seed=GetSeed();
	}
		
	RNG rng2(my_seed);
	message.Notify("Seed:" + String::Format(my_seed));

	
	//	RNG rng2(3);
	// 	message.Warning("Seed:" + String::Format(GetSeed()) + "Bootstrapping: " + String::Format(GetBootStrap()));
	

// }

	bool do_boot=GetBootStrap();

	// Get size of original table.
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);
	int i, j,k;
			
	Vector (float) boot_vals; 
	Vector (float) boot_ovals; 

	Vector (float) boot_rvals; 
	Vector (float) boot_rovals;
	Vector (bool) boot_filter; 

	boot_vals.reserve(no_objects);
	boot_ovals.reserve(no_objects);
	boot_rvals.reserve(no_objects);
	boot_rovals.reserve(no_objects);
	boot_filter.reserve(no_attributes-1);

	// Create two subtables.
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	// Handle<DecisionTable> subtable2 = Creator::DecisionTable();

	message.Notify("Initializing new tables...");

	

	// let's extract the number of classes, and the numbers of objects belonging to each class, 

	
	// find class column.		
	int dcolumn= table->GetDecisionAttribute(masked);

	Vector(int) classes; classes.reserve(no_objects);
	Vector(int) card; card.reserve(no_objects);

	Vector(int) raw_classes; raw_classes.reserve(no_objects);

	// calculate the number of classes.
	table->GetEntries(raw_classes, dcolumn, 1, 1);
	table->GetValueSet(classes, card, dcolumn, masked) ;


// message.Warning("Attributes :" + String::Format(dcolumn) +"\nClasses:" + String::Format(classes.size()));

	int n_classes=classes.size();
	Vector (ClassStat) classesstat; classesstat.reserve(n_classes);
	Vector (float) tvalues; tvalues.reserve(no_attributes-1);
	Vector (float) tpvalues; tpvalues.reserve(no_attributes-1);
	Vector (float) tvalues_old; tvalues_old.reserve(no_attributes-1); //
	Vector (float) pvalues; pvalues.reserve(no_attributes-1);

	for (i = 0; i < (no_attributes-1); i++) {
		tvalues.push_back(0.0f);	
	}
	for (i = 0; i < (no_attributes-1); i++) {
		tpvalues.push_back(1.0f);	
	}

	for (i = 0; i < (no_attributes-1); i++) {
		pvalues.push_back(1.0f);	
	}

	ClassStat tempStat;


// go through all attributes
	Vector (float) columnvalues; columnvalues.reserve(no_objects);

	
  	// message.Progress("Performing t-test analysis... ", 0 , (no_attributes-1) );
	

	
	for (int a=0;a<no_attributes-1;a++)
	{

		if (!((table->GetAttributeType(a,masked))== (Attribute::TYPE_FLOAT) ||table->GetAttributeType(a,masked)==Attribute::TYPE_INTEGER))
		{
			message.Error("The table contains non-decision attribute(s) that is not of float or integer type \n Either remove or mask these attribute(s)");
			return table.Release();
		}


		if (!do_boot)
		{
			if (!message.Progress("Performing t-test analysis... " , a , no_attributes-1))
			{
				return NULL;
			}
		}

		table->GetEntries(columnvalues, a, 1, 1, 1);


		int present_objects=0;
		for(int abc=0;abc<no_objects;abc++) {
			// Message::Warning("columnvalues[abc]" + String::Format(columnvalues[abc]));
			if (table->GetEntry(abc, a, masked)!=Undefined::Integer() && table->GetEntry(abc, a, masked)!=Undefined::Float())
			{present_objects++;
			}
			else{
				// Message::Warning("undefined value found");
			}
		}
		Vector (float) present_columnvalues; present_columnvalues.reserve(present_objects); 
		Vector (int) present_classvalues; present_classvalues.reserve(present_objects);
		for(int abc2=0;abc2<no_objects;abc2++) {
			if (table->GetEntry(abc2, a, masked)!=Undefined::Integer() && table->GetEntry(abc2, a, masked)!=Undefined::Float())
			{present_columnvalues.push_back(columnvalues[abc2]);
				present_classvalues.push_back(raw_classes[abc2]); 
			}
		}
		



		// calculate cardinality.
	
		Vector (int) present_values;
		Vector (int) present_cardinalities;
		// Make a convenient typedef.

		typedef Map(int, int) EntryMap;
		int present_classes;
		EntryMap entrymap;

		// Fill the map.
		for (int ii = 0; ii <present_objects; ii++) {

			// Get entry value.
			int entry = present_classvalues[ii];


			EntryMap::iterator iterator = entrymap.find(entry);

			// Update map.
			if (iterator == entrymap.end())
				entrymap.insert(Pair(const int, int)(entry, 1));
			else
				(*iterator).second++;

		}

		// Reserve space to minimize allocations.
		present_values.reserve(entrymap.size());
		present_cardinalities.reserve(entrymap.size());
		present_classes=entrymap.size();
		// This should be a const_iterator, but VC++ 6.0 won't let me...
		EntryMap::iterator iterator = entrymap.begin();

		// Fill the vectors.
		while (!(iterator == entrymap.end())) {

			// Get (entry, cardinality) pair.
			int entry       = (*iterator).first;
			int cardinality = (*iterator).second;

			// Update vectors.
			present_values.push_back(entry);
			present_cardinalities.push_back(cardinality);

			iterator++;

		}


// go through all classes
		for (int m=0; m<present_classes;m++) {

			tempStat.n = present_cardinalities[m];
			tempStat.o = present_objects - present_cardinalities[m];
			tempStat.values.reserve(present_cardinalities[m]);
			tempStat.ovalues.reserve(present_objects - present_cardinalities[m]);
	

			for(int v=0;v<present_objects;v++)
			{
			
				if ( present_classvalues[v] == present_values[m]) {
					// message.Warning((table->GetEntry(v,a, 1, masked)));
					tempStat.values.push_back(present_columnvalues[v]);				
				}	
				else{
					tempStat.ovalues.push_back(present_columnvalues[v]);			
				}
			
			}
		

			tempStat.var = MathKit::Variance(tempStat.values);
			tempStat.ovar = MathKit::Variance(tempStat.ovalues);
			tempStat.average = MathKit::Mean(tempStat.values);
			tempStat.oaverage = MathKit::Mean(tempStat.ovalues);
	
	
			df= ((tempStat.var/tempStat.n+tempStat.ovar/tempStat.o) * (tempStat.var/tempStat.n+tempStat.ovar/tempStat.o)) /  	((((tempStat.var/tempStat.n)* (tempStat.var/tempStat.n))/((tempStat.n)-1)) +  (((tempStat.ovar/tempStat.o)* (tempStat.ovar/tempStat.o))/((tempStat.o)-1)));

			StatKit mystat2;


			if(present_classes>1 && ((1.0f*present_objects)/(1.0f*no_objects ))>0.10f) {
				tempStat.tvalue = fabs((tempStat.average  - tempStat.oaverage) / sqrt((tempStat.var/tempStat.n)+(tempStat.ovar/tempStat.o)));
			}
			else{
				tempStat.tvalue=0.0f;
			}
 
			if(present_classes>1 && tempStat.tvalue!=0.0f) {
				tempStat.pvalue = mystat2.Betai(0.5*df,0.5,df/(df+((tempStat.tvalue)*(tempStat.tvalue))));
			}
			else{
				tempStat.pvalue =1.0f;
			}
// message.Warning("m:" + String::Format(m) + " tvalue:" + String::Format(tempStat.tvalue));
 

			
			// bootstrapping!		
		
			if (do_boot) { 
				boot_count=0;
			
				int r;
				for (r=0;r<tempStat.n;r++) {
					boot_vals.push_back(1.0f*tempStat.values[r] -tempStat.average);
		
				}
				for (r=0;r<tempStat.o;r++) {
					boot_ovals.push_back(1.0f*tempStat.ovalues[r]-tempStat.oaverage);
				}

				for(i=0;i<n_straps;i++) {
		
					if (i%50==0){
						if (!message.Progress("Performing t-test analysis... " , a , no_attributes-1)){
							return NULL;
						}
					}			
					
					for (r=0;r<tempStat.n;r++) {
						// make a random vector with replacement
						boot_rvals.push_back (boot_vals[rng2.DrawInteger(0, (tempStat.n)-1)]);

					}
					for  (r=0;r<tempStat.o;r++) {
						// make a random ovector with replacement
						boot_rovals.push_back  (boot_ovals[rng2.DrawInteger(0, (tempStat.o)-1)]);
					}
					// calculate
					boot_rvals_var = MathKit::Variance(boot_rvals);
					boot_rovals_var = MathKit::Variance(boot_rovals);
					boot_rvals_avr = MathKit::Mean(boot_rvals);
					boot_rvals_oavr = MathKit::Mean(boot_rovals);
		
					boot_tval = (boot_rvals_avr - boot_rvals_oavr) / sqrt((boot_rvals_var/tempStat.n)+(boot_rovals_var/tempStat.o));

					// message.Warning("Boot tval:"+ String::Format(boot_tval) + "\nTval: " + String::Format(tempStat.tvalue));
	
					if (fabs(boot_tval)>tempStat.tvalue){
						boot_count++;
						//	message.Warning("bootcount++");
					}
					else if (boot_tval ==0)
					{
						boot_count++;
					}
				 
				
					boot_vals.resize(0);
					boot_ovals.resize(0);
					boot_rvals.resize(0);
					boot_rovals.resize(0);


					// check Bcount/n_straps
				}
					
				// message.Warning("pvals : " + String::Format((1.0f*boot_count)/(1.0f*n_straps)));
				if (((1.0f*boot_count)/(1.0f*n_straps))<pvalues[a]) {		
					pvalues[a] = (1.0f*boot_count)/(1.0f*n_straps);
				
				}
			
			
				boot_count=0;
				
		
				
			} 



			if (fabs(tempStat.pvalue)<fabs(tpvalues[a])) {
				tpvalues[a] = fabs(tempStat.pvalue);
				tvalues[a] = fabs(tempStat.tvalue);
			}
		




	
			// classesstat.push_back(tempStat);
		
			tempStat.ovalues.resize(0);
			tempStat.values.resize(0);
	
	
	
		}


		
	
	}

			
	
	if ((attributes_out<1)){
		Message::Error("The selected number of features is too small.");
		return NULL;
	}
	if ((attributes_out>no_attributes-1)){
		Message::Error("The selected number of features is too large.");
		return NULL;
	}



	int r;
	for (r=0; r<(no_attributes-1);r++) {
		tvalues_old[r] = tvalues[r];
	}


	// set t-values to 0.0f where pvalues[r] > GetThreshold

	if (GetBootStrap()) {	
		for (r=0; r<(no_attributes-1);r++) {
			// message.Warning("pvalues[r]" + String::Format(pvalues[r]) + "Pthreshold" + String::Format(GetPThreshold()));	

			if ((pvalues[r])>=(GetPThreshold())){
				tvalues[r] =0.0f;
			}
			else {
				passed_boot++;
			}
		}

		// if not enough attributes pass the test.

		if (attributes_out>passed_boot) {
			attributes_out=passed_boot;
		}

		if (passed_boot==0) {
			Message::Error("No attributes passed the bootstrap test, you might want to lower the P threshold");
			return NULL;
		}
	
	}

	

	Vector(bool) removes; removes.reserve(no_attributes);

	

	// fill the vector 
	
	for (i = 0; i < (no_attributes); i++) {
		removes.push_back(1);	
	}
	// always keep class column
	removes[dcolumn]=0;

		

	// Message::Warning("T threshold : "+ String::Format(GetThreshold()));
	
	// how to implement this... the attributes_out attributes with highest t-value should be selected.
	int t_keep=0;

	for (k=0; k<attributes_out; k++)	{
		float tempmax=0.0f;
		int tempindex;
		for (j=0;j<no_attributes-1;j++) {
			if (tvalues[j] >=tempmax) {
				tempindex=j;
				tempmax = tvalues[j];
			}	
		}

		if (tpvalues[tempindex] < GetThreshold() ||GetBootStrap()){
			removes[tempindex] = 0;		
			t_keep++;
		}
		else if (k==0) {
			Message::Error("No attributes passed the t significance test.\nYou might want to increase significance level.");
			return NULL;
		}
		tvalues[tempindex] = 0.0f; //??? really?
	}

	
	// This is where we size the new table.
	if (!subtable1->Resize(no_objects, t_keep+1, masked) )
		return NULL;

	message.Notify("Sampling indices...");

	message.Notify("Copying entries...");

	// Copy sampled objects into the subtable.
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






	Handle<DecisionTable> featuretable = Creator::DecisionTable();

	Vector(Attribute::Type) types;
	Vector(String) names;
	Vector(int) scales;
	types.reserve(5);
	names.reserve(5);
	scales.reserve(5);
	types.push_back(Attribute::TYPE_STRING);
	types.push_back(Attribute::TYPE_INTEGER);
	types.push_back(Attribute::TYPE_FLOAT);
	types.push_back(Attribute::TYPE_FLOAT);
	types.push_back(Attribute::TYPE_INTEGER);

	names.push_back("Attribute");
	names.push_back("Index");
	names.push_back("T-Value");
	names.push_back("P");
	names.push_back("Selected");

	scales.push_back(3);
	scales.push_back(3);
	scales.push_back(3);
	scales.push_back(5);
	scales.push_back(3);

	if(!(InitFeatureSelectionTable(*table, *featuretable, removes, names, types, scales))) 
		return NULL;


	for (r=0; r<(no_attributes-1);r++) 
	{
		if(!featuretable->SetEntry(r, 2, tvalues_old[r]*1000, masked))
			return false;
	}


	for (r=0; r<(no_attributes-1);r++) 
	{
		if(!featuretable->SetEntry(r, 3, (tpvalues[r])*100000, masked))
			return false;
	}


	if (GetBootStrap()) {
		for (r=0; r<(no_attributes-1);r++) 
		{
			if(!featuretable->SetEntry(r, 3, (pvalues[r])*100000, masked))
				return false;
		}
	}



	// Set subtable names.
	
	subtable1->SetName(table->GetName() + " (with feature selection)");
	featuretable->SetName("Feature Selection Table of " + table->GetName());


	message.Notify("Copying metadata...");


	// Duplicate the dictionaries.
	if (table->HasDictionary()) {
		
		subtable1->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
		
		// we have to throw away the attributes not needed anymore.
		
		for (j=(table->GetNoAttributes(0))-1;j>=0;j--) {
		    if (table->IsAttributeDisabled(j)) {
				subtable1->GetDictionary()->RemoveAttribute(j);
			}
		}



		for (j=no_attributes-1;j>=0;j--){
			if (removes[j]){
				subtable1->GetDictionary()->RemoveAttribute(j);
			}
		}
		
	}
	else {
		subtable1->SetDictionary(NULL);
		
	}
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


	// Determine fate of subtables and return.
	if (AppendAsChildren()) {
		// Append the subtables as children to the input table.
		if (!table->AppendChild(subtable1.GetPointer())||!table->AppendChild(featuretable.GetPointer())) {
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
 
TTest *
TTest::Clone() {
	return new TTest;
} 
