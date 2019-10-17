//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/ftest.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/genomics/utilities/statkit.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/utilities/rng.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/creator.h>
#include <kernel/basic/message.h>

#include <kernel/system/time.h>

struct ClassStat2 {
	int n;
	Vector (float) values;
	float var;
	float average;
};

struct Bootcounter{
	int n;
	Vector (float) values;
	float var;
	float average;
};

//-------------------------------------------------------------------
// Methods for class FTest.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

FTest::FTest() {
	SetAttributes(1);
	SetThreshold(0.05f);
}

FTest::~FTest() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(FTest, FTEST, FeatureSelector)

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
FTest::GetParameters() const {

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
FTest::SetParameter(const String &keyword, const String &value) {

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
//	  						 a f-test on it's attributes, filtering out  
//								 a number of the attributes, using a f probability test,
//								 after which also the the lowest f-values may be removed
//								 if we have more than the specified maximum number of 
//								 attributes left. 
//
//								 The optional bootstrapping feature, performs a
//                 bootstrap test on the attributes, where attributes
//								 are removed if they fail the test, before filtering 
//								 according to f-values.
//
//
//					The algorithm allows for missing values, which are not included in the calculations
//
//                 If specified, the resulting subtable is appended
//                 to the input table as a child, and the input table
//                 itself is returned.  Alternatively, the subtable is
//                 not appended as a child, but is instead returned as
//                 a decision table set.
//
//===================================================================

Structure *
FTest::Apply(Structure &structure) const {

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
	//message.Warning("pthreshold:" + String::Format(GetPThreshold()));
	if ((GetPThreshold()>1.0f || GetPThreshold()<0.0f ) && (GetBootStrap())) {
		message.Error("Error : The bootstrap significance level has to be between 0 and 1");
		return table.Release();
	}
	
		
	
	bool masked = true;

	int attributes_out= GetAttributes();

	int n_straps=GetStraps();
	int boot_count=0;
	float boot_rvals_std;
	float boot_rovals_std;
	float boot_rvals_avr;
	float boot_rvals_oavr;
	float boot_tval;
	int passed_boot=0;
	int my_seed=0;

	if (GetGenerateSeed()) { 
		RNG  rng3(time(0));
		my_seed= rng3.DrawInteger(0,INT_MAX);			
	}
	else {
		my_seed=GetSeed();
	}
		
	RNG rng2(my_seed);
	message.Notify("Seed:" + String::Format(my_seed));


	bool do_boot=GetBootStrap();

	// Get size of original table.
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);
	int i, j,k;
	

	// Create subtable.
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	

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

	int n_classes=classes.size();

	Vector (Bootcounter) boot_vals;

	boot_vals.reserve(n_classes);
	Bootcounter bootcounter;

	for(i=0; i< n_classes ; i++)
	{
		bootcounter.values.reserve(card[i]);

		for (j=0;j<card[i]; j++)
		{
			bootcounter.values.push_back(0.0f);		
		}


		boot_vals.push_back(bootcounter);
		bootcounter.values.resize(0);
	}

	Vector (float) tvalues; tvalues.reserve(no_attributes-1);
	Vector (float) tvalues_old; tvalues_old.reserve(no_attributes-1); //
	Vector (float) pvalues; pvalues.reserve(no_attributes-1);
	Vector (float) fpvalues; fpvalues.reserve(no_attributes-1);

	for (i = 0; i < (no_attributes-1); i++) {
		tvalues.push_back(0.0f);	
	}

	for (i = 0; i < (no_attributes-1); i++) {
		pvalues.push_back(1.0f);	
	}

	for (i = 0; i < (no_attributes-1); i++) {
		fpvalues.push_back(0.0f);	
	}

	Vector (ClassStat2) tempStat;
	tempStat.reserve(no_attributes);
	
	ClassStat2 myStat;
	for (i=0;i<no_attributes;i++ ) {
		tempStat.push_back(myStat);
	}

	Vector (float) columnvalues; // columnvalues.reserve(no_objects);

		
  	// message.Progress("Performing f-test analysis... ", 0 , (no_attributes-1) );
	

	float attribute_average;
	float attribute_std;	

	for (int a=0;a<no_attributes-1;a++) {

		if (!((table->GetAttributeType(a,masked))== (Attribute::TYPE_FLOAT) ||table->GetAttributeType(a,masked)==Attribute::TYPE_INTEGER))
		{
			message.Error("The table contains non-decision attribute(s) that is not of float or integer type \n Either remove or mask these attribute(s)");
			return table.Release();
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


		//

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
		// This should be a const_iterator, but VC++ 6.0 doesn't allow it...
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


		attribute_std =  MathKit::StandardDeviation(present_columnvalues);
		attribute_average= MathKit::Mean(present_columnvalues);


// 	message.Warning("Average: " + String::Format(attribute_average) + "\nStd: " + String::Format(attribute_std)); 
	
		for (int m=0; m<present_classes;m++) {
			tempStat[m].n = present_cardinalities[m];
			tempStat[m].values.reserve(present_cardinalities[m]);
	
			for(int v=0;v<present_objects;v++) {
				if (present_classvalues[v] == present_values[m]) {
					
					tempStat[m].values.push_back(present_columnvalues[v]);
					
				}			
			}
		
			tempStat[m].var = MathKit::Variance(tempStat[m].values);
			tempStat[m].average = MathKit::Mean(tempStat[m].values);
		}

		float ssv=0.0f;  
		float ssb=0.0f;

		int w;
		for (w=0;w<present_classes;w++)
		{

			ssb= ssb  + (tempStat[w].n)*	((tempStat[w].average)-attribute_average)*((tempStat[w].average)-attribute_average);
			ssv = ssv + ((tempStat[w].n) -1)*(tempStat[w].var);
		}

		if(present_classes>1 && ssv!=0 && ((1.0f*present_objects)/(1.0f*no_objects ))>0.10f) {
			tvalues[a] = (ssb*(present_objects-present_classes))/(ssv*(present_classes-1));
		}
		else{
			tvalues[a] =0.0f;
		}
		float   xx = tvalues[a] ; 

		int  df1 = present_classes -1;
		int df2 = present_objects-present_classes;

		StatKit mystat;


		if(present_classes>1) {
			fpvalues[a] = (mystat.Betai(0.5*df2,0.5*df1, df2 / (df2 + df1 * xx)));
		}
		else{
			fpvalues[a] =1.0f;
		}

		// bootstrapping!		

		if (do_boot) { 
			boot_count=0;

			for (int qq=0;qq<n_straps;qq++){
		
				float boot_average=0.0f;
				float boot_total=0.0f;
			
				float boot_ssb=0.0f;
				float boot_ssv=0.0f;
				for(int r=0; r<present_classes; r++) {


					for (int q=0; q< tempStat[r].n; q++) {

						boot_vals[r].values[q] =  tempStat[r].values[rng2.DrawInteger(0, ((tempStat[r].n)-1))] - tempStat[r].average;
						boot_total=boot_total+boot_vals[r].values[q];
					}

			
					boot_vals[r].average=MathKit::Mean(boot_vals[r].values);
					boot_vals[r].var=MathKit::Variance(boot_vals[r].values);
					boot_vals[r].n = tempStat[r].n; 
				}
				boot_average = boot_total / present_objects;

				//
				//
			

				for (int w=0;w<present_classes;w++) {	
					boot_ssb= boot_ssb  + (boot_vals[w].n)*((boot_vals[w].average)-boot_average)*((boot_vals[w].average)-boot_average);
					boot_ssv = boot_ssv + ((boot_vals[w].n) -1)*(boot_vals[w].var);
				}

				float  boot_xx=1.0f;
				if (present_classes>1)
				{
					boot_xx = (boot_ssb*(present_objects-present_classes))/(boot_ssv*(present_classes-1));
				}
				int  boot_df1 = present_classes -1;
				int boot_df2 = present_objects-present_classes;


				if((boot_xx)>tvalues[a])
				{

					boot_count++;
				}
				else
				{
					//
				}

			}

	
			// message.Warning("Boot Count: " + String::Format(boot_count) + "\nN_straps: " + String::Format(n_straps));
			pvalues[a] = (1.0f*boot_count)/(1.0f*n_straps);
			// message.Warning("P-value:" + String::Format(pvalues[a]));
			boot_count=0;
		
		}




		for (w=0;w<no_attributes;w++)  {
			tempStat[w].values.resize(0);
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





	// set f-values to 0.0f where pvalues[r] > GetThreshold
	if (GetBootStrap()) {	
		for (r=0; r<(no_attributes-1);r++) {
			tvalues_old[r] = tvalues[r];

			if (pvalues[r]>GetPThreshold()){
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
			return table.Release();
			//		return NULL;
		}
	
	}

	

	Vector(bool) removes; removes.reserve(no_attributes);

	

	// fill the vector 
	
	for (i = 0; i < (no_attributes); i++) {
		removes.push_back(1);	
	}
	// always keep class column
	removes[dcolumn]=0;

		

		
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

		if (fpvalues[tempindex] < GetThreshold()||GetBootStrap()){
			removes[tempindex] = 0;		
			t_keep++;
		}
		else if (k==0) {
			Message::Error("No attributes passsed the f significance test.\nYou might want to increase significance level..");
			return NULL;
		}
		tvalues[tempindex] = 0.0f; 
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
	names.push_back("F-Value");
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
		if(!featuretable->SetEntry(r, 3, (fpvalues[r])*100000, masked))
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
		
		//
		// get dictionary entries 

		// 
		// 
		//  räkna inte upp om attributen är maskad
		//
		// 
		//  unmasked_removes... 
		//

		for (j=(table->GetNoAttributes(0))-1;j>=0;j--) {
		    if (table->IsAttributeDisabled(j)) {
				subtable1->GetDictionary()->RemoveAttribute(j);
			}
		}


		for (j=no_attributes-1;j>=0;j--){
			if (removes[j]){
				subtable1->GetDictionary()->RemoveAttribute(j)	;
			}
		}
		
	}
	else {
		subtable1->SetDictionary(NULL);
		
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
 
FTest *
FTest::Clone() {
	return new FTest;
} 
