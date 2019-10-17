//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/rfeatureselector.h>
#include <kernel/genomics/algorithms/loadfeatureselection.h>

#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/decisiontableexporter.h>
#include <kernel/algorithms/mydecisiontableimporter.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>

#include <kernel/system/fstream.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/rng.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>


struct ClassStat {
	int n;
	Vector (float) values;
	int o;
	Vector (float) ovalues;
	float std;
	float ostd;
	float average;
	float oaverage;
	float tvalue;
};



//-------------------------------------------------------------------
// Methods for class RFeatureSelector.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

RFeatureSelector::RFeatureSelector() {
	SetAttributes(1);
	SetThreshold(0.5);
}

RFeatureSelector::~RFeatureSelector() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(RFeatureSelector, RFEATURESELECTOR, FeatureSelector)

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
RFeatureSelector::GetParameters() const {
	
	String parameters;
	
	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAttributes());
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::Threshold();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetThreshold());
	
	parameters += Keyword::Separator();
	
	
	parameters += Keyword::Filename();
	parameters += Keyword::Assignment();
	parameters += GetFilename();
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::Path();
	parameters += Keyword::Assignment();
	parameters += GetRPath();
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::Directory();
	parameters += Keyword::Assignment();
	parameters += GetDirectory();
	
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
RFeatureSelector::SetParameter(const String &keyword, const String &value) {
	
	if (keyword == Keyword::Attributes() && value.IsInteger())
		return SetAttributes(value.GetInteger());
	
	if (keyword == Keyword::Threshold() && value.IsFloat())
		return SetThreshold(value.GetFloat());
	
	if (keyword == Keyword::Path())
		return SetRPath(value);
	
	if (keyword == Keyword::Filename())
		return SetFilename(value);
	
	
	if (keyword == Keyword::Directory())
		return SetDirectory(value);
	
	
	return FeatureSelector::SetParameter(keyword, value);
	
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Jacob Vesterlund
// Date..........: 2004
// Description...: Takes a decision table as input and performs 
//	  						 a feature selection on it using R.  
//
//								The path to R, a temporary directory for temporary files,
//								and the path to the R-script  that performs the selection
//								in R is needed. 
// 
//
//
//===================================================================

Structure *
RFeatureSelector::Apply(Structure &structure) const {
	
	
	
	
	Message message;
	
	// Check input.
	if (!IsApplicable(structure))
		return NULL;
	
	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	
	bool masked = false;
	int attributes_out= GetAttributes();
	// if(GetBootStrap())
	// {
	
	int boot_count=0;
	float boot_rvals_std;
	float boot_rovals_std;
	float boot_rvals_avr;
	float boot_rvals_oavr;
	float boot_tval;
	int passed_boot=0;
	
	
	
	
	message.Warning("Script path" + GetFilename() + "\nTemp directory : "  + (GetDirectory()) +  "\nRPath : " + GetRPath()); 
	
	
	Vector (float) boot_vals; 
	Vector (float) boot_ovals; 
	
	Vector (float) boot_rvals; 
	Vector (float) boot_rovals;
	Vector (bool) boot_filter; 
	// }
	
	
	// Get size of original table.
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);
	int i, j,k;
	
	
	// Create two subtables.
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	Handle<DecisionTable> btable = Creator::DecisionTable();
	Handle<DecisionTable> designtable = Creator::DecisionTable();
	Handle<DecisionTable> classtable = Creator::DecisionTable();
	
	// Handle<DecisionTable> subtable2 = Creator::DecisionTable();
	
	message.Notify("Initializing new tables...");
	
	
	
	if (!(MakeDesignTable(*table, *designtable)))
	{	return NULL;}
	
	if (!(MakeClassTable(*table, *classtable)))
	{	return NULL;}
	
	
	
	
	// let's extract the number of classes, and the numbers of objects belonging to each class, 
	
	
	// find class column.		
	int dcolumn= table->GetDecisionAttribute(masked);
	
	Vector(int) classes; classes.reserve(no_objects);
	Vector(int) card; card.reserve(no_objects);
	
	
	// calculate the number of classes.
	
	table->GetValueSet(classes, card, dcolumn, masked) ;
	
	
	// message.Warning("Attributes :" + String::Format(dcolumn) +"\nClasses:" + String::Format(classes.size()));
	
	int n_classes=classes.size();
	
	
	
	
	
	Vector (ClassStat) classesstat; classesstat.reserve(n_classes);
	Vector (float) tvalues; tvalues.reserve(no_attributes-1);
	Vector (float) tvalues_old; tvalues_old.reserve(no_attributes-1); //
	Vector (float) pvalues; tvalues.reserve(no_attributes-1);
	
	for (i = 0; i < (no_attributes-1); i++) {
		tvalues.push_back(0.0f);	
	}
	
	for (i = 0; i < (no_attributes-1); i++) {
		pvalues.push_back(1.0f);	
	}
	
	ClassStat tempStat;
	
	
	// go through all attributes
	Vector (float) columnvalues; columnvalues.reserve(no_objects);
	
	
	// message.Progress("Performing t-test analysis... ", 0 , (no_attributes-1) );
	
	
	
	Vector(bool) mremoves; mremoves.reserve(no_attributes);
	
	
	for (i = 0; i < (no_attributes); i++) {
		mremoves.push_back(0);	
	}
	// throw away class column
	mremoves[dcolumn]=1;
	
	
	
	
	
	// Message::Warning("T threshold : "+ String::Format(GetThreshold()));
	
	// how to implement this... the attributes_out attributes with highest t-value should be selected.
	
	
	// This is where we size the new table.
	if (!subtable1->Resize(no_objects, no_attributes-1, masked) )
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
			
			if (mremoves[j]==1) {
				skipped++;						
			}
			else if (!subtable1->SetEntry(i, j-skipped, table->GetEntry(i, j, masked), masked))	{	
				return NULL;
			}
			
		}
	}
	
	
	
	
	// Set subtable names.
	subtable1->SetName("MT");
	
	message.Notify("Copying metadata...");
	
	
	// Duplicate the dictionaries.
	if (table->HasDictionary()) {
		
		subtable1->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
		
		// we have to throw away the attributes not needed anymore.
		
		for (j=no_attributes-1;j>=0;j--){
			if (mremoves[j]){
				subtable1->GetDictionary()->RemoveAttribute(j);
			}
		}
		
	}
	else {
		subtable1->SetDictionary(NULL);
		
	}
	
	
	if (!((GetDirectory().EndsWith('\\') ||  (GetDirectory().EndsWith('/')))))
	{
		
		message.Error("Directory must end with a slash or backslash (depending on platform");
		return false;
	}
	
	
	String dfilepath = GetDirectory() + "d.txt";
	
	String classfilepath = GetDirectory() + "c.txt";
	String mtfilepath =  GetDirectory() + "mt.txt" ;
	String outpfilepath = GetDirectory()  + "outp.txt";
	String outp2filepath = GetDirectory()  + "outp2.txt";
	String rfilepath =  GetDirectory() + "calcb.R" ;
	String boutfilename =  GetDirectory() + "bout.txt" ;
	
	
	SaveTableAsR(*designtable, dfilepath);
	SaveTableAsR(*classtable, classfilepath);
	SaveTableAsR(*subtable1, mtfilepath);
	MakeRFile(rfilepath, GetFilename(),mtfilepath, dfilepath, outpfilepath, classfilepath);
	
	
	
	
	String hej = "\"" + GetRPath() + "\""+ " --vanilla <"+rfilepath;
	//  String hej = "\"" + GetRPath() + "\""+ " <"+rfilepath;
	system(hej.GetBuffer());
	
	
	FormatBStatTable2Ros(outpfilepath, outp2filepath);
	MyDecisionTableImporter mytbl;
	mytbl.SetFilename(outp2filepath);
	mytbl.Apply(*btable);
	
	
	btable->SetAttributeName(0, "Index", masked) ;
	Handle<Attribute>  nameattribute; 
	Handle<Attribute>  dattribute; 
	
	
	nameattribute = Creator::StringAttribute();
	dattribute = Creator::IntegerAttribute();
	
	
	nameattribute->SetName("Attribute");
	dattribute->SetName("Decision");
	
	
	
	if (!btable->InsertAttribute(0, nameattribute.GetPointer(), masked))
	{	message.Error("Unable to add attributes");
	return false;
	
	}
	
	if (!btable->AppendAttribute(dattribute.GetPointer(), masked))
	{	message.Error("Unable to add attributes");
	return false;
	
	}
	
	int new_size = btable->GetNoAttributes(masked);
	
	if (!btable->Resize(no_attributes, new_size, masked))
	 		{	message.Error("4");
	return false;
	
	}
	
	
	btable->SortObjects(1,masked);
	
	for (i = 0; i < (no_attributes); i++) {
		
		if (!btable->SetEntry(i, "Attribute", table->GetAttributeName(i,masked), masked))
		{
			message.Warning("error!!!!");
		}
		
	}
	for (i = 0; i < (no_attributes-1); i++) {
		btable->SetEntry(i, new_size-1, 1, false);
	}
	
	
	btable->SetEntry(no_attributes-1,1,no_attributes-1,false);
	btable->SetEntry(no_attributes-1,new_size-1,1, false);
	
	btable->SortObjects(new_size-2,masked);
	
	
	// let's decide....
	
	int keepers=GetAttributes();
	for (i = no_attributes-2; i >=0; i--) {
		
		if (keepers>0)
		{		btable->SetEntry(i, new_size-1, 1, false);
		keepers--;
		}
		else 
		{
			btable->SetEntry(i, new_size-1, 0, false);
		}
		
	}
	
	
	ofstream outstream;
	
	if (!IOKit::Open(outstream, boutfilename)) {
		Message::Error("Error opening for output.");
		return NULL;
	}
	
	btable->Save(outstream);
	IOKit::Close(outstream);
	
	LoadFeatureSelection myLoad;
	
	myLoad.SetFilename(boutfilename);
	myLoad.Apply(*table);
	
	if (GetSaveFeatureSelection())
	{
		ofstream stream;
		
		if (!IOKit::Open(stream, GetSaveFeatureSelectionFilename()) ){
			Message::Error("Error opening" + GetSaveFeatureSelectionFilename() + " for input.");
			return NULL;
		}
		
		
		btable->Save(stream);
		IOKit::Close(stream);
	}
	
	
	
	return subtable1.Release();
	
}

bool
RFeatureSelector:: MakeDesignTable(DecisionTable &table, DecisionTable &designtable) const
{
/*

  
	*/
	bool masked=true;
	
	int no_objects    = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);
	
	int dcolumn= table.GetDecisionAttribute(masked);
	
	Vector(int) classes; classes.reserve(no_objects);
	Vector(int) card; card.reserve(no_objects);
	
	
	// Create an empty dictionary.
	Handle<Dictionary> dictionary = Creator::Dictionary();
	
	// Assign the empty dictionary to the table.
	if (!designtable.SetDictionary(dictionary.GetPointer()))
		return false;
	
	// calculate the number of classes.
	
	table.GetValueSet(classes, card, dcolumn, masked) ;
	
	
	// message.Warning("Attributes :" + String::Format(dcolumn) +"\nClasses:" + String::Format(classes.size()));
	
	int n_classes=classes.size();
	
	if (!designtable.Resize(no_objects, n_classes,masked))
	 		{	
		return false;
		
	}
	
	// fill the table (with the right dimensions)
	
	for (int i = 0; i <  n_classes; i++) {
		
		// Skip non-selected columns.
		//if (removes[i])
		// continue;
		
		Handle<Attribute> attribute;
		
		attribute = (Creator::IntegerAttribute());
		
		if (!attribute->SetName("A"))
		{	
			return false;
			
		}
		if (!designtable.AppendAttribute(attribute.GetPointer(), masked))
		{
			return false;
			
		}
		
	}
	if (!designtable.Resize(no_objects, n_classes,masked))
	 		{	
		return false;
		
	}
	
	// fill designtable
	
	for (int j = 0; j < no_objects; j++) {			
		
		for (int k=0; k<n_classes; k++)
		{
			if (table.GetEntry(j,dcolumn,1) == classes[k])
			{
				if (!designtable.SetEntry(j, k, 1, masked))
				{	return false;}		
			}
			else 
			{	if (!designtable.SetEntry(j, k, 0, masked))
			{	return false;}
			}
		}
		
		
	}
	
	return true;
}

bool
RFeatureSelector:: MakeClassTable(DecisionTable &table, DecisionTable &designtable) const
{
/*

  
	*/
	bool masked=true;
	
	int no_objects    = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);
	
	int dcolumn= table.GetDecisionAttribute(masked);
	
	Vector(int) classes; classes.reserve(no_objects);
	Vector(int) card; card.reserve(no_objects);
	
	
	// Create an empty dictionary.
	Handle<Dictionary> dictionary = Creator::Dictionary();
	
	// Assign the empty dictionary to the table.
	if (!designtable.SetDictionary(dictionary.GetPointer()))
		return false;
	
	// calculate the number of classes.
	
	table.GetValueSet(classes, card, dcolumn, masked) ;
	
	
	// message.Warning("Attributes :" + String::Format(dcolumn) +"\nClasses:" + String::Format(classes.size()));
	
	int n_classes=classes.size();
	
	if (!designtable.Resize(no_objects, 1,masked))
	 		{	
		return false;
		
	}
	
	
	// fill the table (with the right dimensions)
	Message::Warning("1. No attributes for classtable" +  String::Format(designtable.GetNoAttributes(masked)));
	
	
	for (int i = 0; i <  1; i++) {
		
		// Skip non-selected columns.
		//if (removes[i])
		// continue;
		
		
		Handle<Attribute> attribute;
		
		attribute = (Creator::IntegerAttribute());
		
		if (!attribute->SetName("A"))
		{	
			return false;
			
		}
		if (!designtable.AppendAttribute(attribute.GetPointer(), masked))
		{
			return false;
			
		}
		
	}
	
	
	if (!designtable.Resize(no_objects, 1,masked))
	 		{	
		return false;
		
	}
	
	
	Message::Warning("2. No attributes for classtable" +  String::Format(designtable.GetNoAttributes(masked)));
	// fill designtable
	
	for (int j = 0; j < no_objects; j++) {			
		
		for (int k=0; k<1; k++)
		{
			
			
			if (!designtable.SetEntry(j, k, table.GetEntry(j,dcolumn,1), masked))
			{	return false;}		
			
		}
		
		
	}
	
	Message::Warning("3. No attributes for classtable" +  String::Format(designtable.GetNoAttributes(masked)));
	return true;
}

bool
RFeatureSelector:: SaveTableAsR(DecisionTable &table, const String &filename) const
{
	bool masked=false;
	int i,j;
	
	ofstream stream;
	
	int no_objects    = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);
	
	
	
	if (!IOKit::Open(stream, filename)) {
		Message::Error("Error opening" + filename + " for input.");
		return false;
	}
	
	for (i=0; i<no_objects;i++)
	{
		
		for (j=0;j<no_attributes;j++)
			
		{
			IOKit::Save(stream, table.GetEntry(i, j,true, masked),false);
			IOKit::Save(stream, ' ');
		}
		
		IOKit::Save(stream, '\n');
		
	}
	
	
	IOKit::Close(stream);
	
	return true;
}

bool
RFeatureSelector:: MakeRFile( const String &filename, const String &infilename,  const  String &mfilename, const String &dfilename, const String &outfilename, const String &classfilename) const
{
	bool masked=false;
	int i,j;
	String mfilename_new = mfilename;
	String classfilename_new = classfilename;
	String dfilename_new = dfilename;
	String outfilename_new = outfilename;
	String slash="\\";
	String back ="/";
	classfilename_new.Replace(slash,back);
	mfilename_new.Replace(slash, back);
	dfilename_new.Replace(slash, back);	
	outfilename_new.Replace(slash, back);
	char input[201];
	
	ofstream stream;
	ifstream instream;	
	
	if (!IOKit::Open(stream, filename)) {
		Message::Error("Error opening for output.");
		return false;
	}
	
	
	if (!IOKit::Open(instream, infilename)) {
		Message::Error("Error opening for input:" + infilename);
		return false;
	}
	
	// filenames needed for R-template
	
	stream << "mtfilename<-\"" ;
	stream << mfilename_new;
	stream << "\"\n	classfilename<-\"" ;
	stream << classfilename_new;
	stream << "\"\ndfilename<-\"" ;
	stream << dfilename_new;
	stream << "\"\noutfilename <-\"";	
	stream << outfilename_new;
	stream << "\"\n";
	
	
	// load the R-template file and add it to temporary file
	
	while (!instream.eof()){
		instream.getline(input, 200);
		stream << input;
		stream << "\n";
	}
	
	IOKit::Close(instream);
	
	
	IOKit::Close(stream);
	
	//
	return true;
}

bool
RFeatureSelector:: FormatBStatTable2Ros(const String &infilename,  const String &outfilename) const
{
	ifstream instream;
	ofstream outstream;
	int nline=0;
	char input[201];
	int k;
	if (!IOKit::Open(instream, infilename)) {
		Message::Error("Error opening for input.");
		return false;
	}
	
	if (!IOKit::Open(outstream, outfilename)) {
		Message::Error("Error opening for output.");
		return false;
	}
	
	while (!instream.eof()){
		nline++;
		instream.getline(input, 200);
		
		if (nline==2)
		{
			String ourline(input);
			int no_tokens=	ourline.GetNoTokens();
			
			if (no_tokens<2)
			{
				Message::Error("Output table from R has less than two columns.");
				return false;
			}
			
			outstream << "Integer\t";
			
			for (int d=0;d<no_tokens-1; d++)
			{
				outstream <<"Float(3)\t";
			}
			outstream << "\n";
		}
		outstream << input;
		outstream << "\n";
	}
	
	IOKit::Close(instream);
	
	IOKit::Close(outstream);
	
	
	
	return true;
	
} 

RFeatureSelector *
RFeatureSelector::Clone() {
	return new RFeatureSelector(*this);
}
