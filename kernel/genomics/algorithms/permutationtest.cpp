//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/permutationtest.h>
#include <kernel/genomics/algorithms/permloocvserialexecutor.h>
#include <kernel/genomics/algorithms/permcvserialexecutor.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/textfile.h>

#include <kernel/utilities/rng.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

static void
StaticCleanUp(PermLeaveOneOutCVSerialExecutor &executor, Id id) {
	
	// Reset rules.
	executor.SetRules(Handle<Rules>(NULL));
	
	// Reset output type.
	executor.SetOutputType(id);
	
}

static void
StaticCleanUp(PermCVSerialExecutor &executor, Id id) {
	
	// Reset rules.
	executor.SetRules(Handle<Rules>(NULL));
	
	// Reset output type.
	executor.SetOutputType(id);
	
}

//-------------------------------------------------------------------
// Methods for class PermutationTest.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

PermutationTest::PermutationTest() {
	
}

PermutationTest::~PermutationTest() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(PermutationTest, PERMUTATIONTEST, Splitter)

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
PermutationTest::GetParameters() const {
	
	String parameters;
	
	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::Algorithm();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAlgorithm());
	
	parameters += Keyword::Separator();
	
	parameters += Keyword::Folds();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNFolds());
	
	parameters += Keyword::Separator();
	
	
	parameters += Keyword::Permutations();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNPermutations());
	
	parameters += Keyword::Separator();
	
	
	// Filename (commands).
	parameters += Keyword::Filename() + Keyword::Dot() + Keyword::Commands();
	parameters += Keyword::Assignment();
	parameters += GetCommandFilename();
	
	parameters += Keyword::Separator();
	
	// Filename (log).
	parameters += Keyword::Filename() + Keyword::Dot() + Keyword::Log();
	parameters += Keyword::Assignment();
	parameters += GetLogFilename();
	
	parameters += Keyword::Separator();
	
	parameters += Splitter::GetParameters();
	
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
PermutationTest::SetParameter(const String &keyword, const String &value) {
	
	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());
	
	if (keyword == Keyword::Folds() && value.IsInteger())
		return SetNFolds(value.GetInteger());
	
	if (keyword == Keyword::Permutations() && value.IsInteger())
		return SetNPermutations(value.GetInteger());
	
	if (keyword == Keyword::Algorithm() && value.IsInteger())
		return SetAlgorithm(value.GetInteger());
	
	// Filename (commands).
	if (keyword == Keyword::Filename() + Keyword::Dot() + Keyword::Commands())
		return SetCommandFilename(value);
	
	// Filename (commands). Backwards compatibility.
	if (keyword == Keyword::Commands())
		return SetCommandFilename(value);
	
	// Filename (log).
	if (keyword == Keyword::Filename() + Keyword::Dot() + Keyword::Log())
		return SetLogFilename(value);
	
	// Filename (log). Backwards compatibility.
	if (keyword == Keyword::Log())
		return SetLogFilename(value);
	
	return Splitter::SetParameter(keyword, value);
	
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...: Takes a decision table as input and splits it
//                 in two according to some splitting scheme.
//
//                 If specified, the resulting subtables are appended
//                 to the input table as children, and the input table
//                 itself is returned.  Alternatively, the subtables are
//                 not appended as children, but are instead returned as
//                 a decision table set.
//
// Comments......: Assumes that library clients use handles.
// Revisions.....: Aš 000229: Simplified implementation a bit.
//===================================================================

Structure *
PermutationTest::Apply(Structure &structure) const {
	
	Message message;
	
	// Check input.
	if (!IsApplicable(structure))
		return NULL;
	
	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	
	bool masked = false;
	String myhej1;
	// Get size of original table.
	int no_objects    = table->GetNoObjects(masked);
	int no_attributes = table->GetNoAttributes(masked);
	
	int higher_auc=0, higher_accuracy=0;  // the 
	// Calculate ideal subtable sizes.
	int ideal_size1 = no_objects;
	
	// Create two subtables.
	// Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	// 	Handle<DecisionTable> subtable2 = Creator::DecisionTable();
	float auc,accuracy;
	PermLeaveOneOutCVSerialExecutor * executor= new PermLeaveOneOutCVSerialExecutor();
	PermCVSerialExecutor *executor2 = new PermCVSerialExecutor()  ;
	
	
	message.Notify("Initializing new tables...");
	
	// Resize them.
	//	if (!subtable1->Resize(ideal_size1, no_attributes, masked)) //  || !subtable2->Resize(ideal_size2, no_attributes, masked))
	//		return NULL;
	
	message.Notify("Sampling indices...");
	
	// Set random number generator seed.
	RNG rng(GetSeed());
	int b=GetNPermutations();
	
	Vector(int) indices;  indices.reserve(no_objects);
	Vector(int) indices1; indices1.reserve(no_objects);
	float original_auc, original_accuracy;
	Vector(float) accuracies; accuracies.reserve(b);
	Vector(float)	aucs;	 aucs.reserve(b);
	
	int i, j, k;
	
	
	message.Warning("B :" + String::Format(b));
	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
	
	// Save log header.
	
	ofstream stream;
	
	if (!IOKit::Open(stream, GetLogFilename())) {
		Message::Error("Failed to open " + GetLogFilename() + " for output.");
		return NULL;
	}
	
	
	
	
	if (!SaveLogHeader(stream)) {
		Message::Error("Failed to save log file header.");
		return NULL;
	}
	
	
	
	
	if (GetAlgorithm()==0)
		
	{
		message.Warning("Algorithm 0 - loocv");
		//	Handle<DecisionTable> subtable1 = Creator::DecisionTable();
		
		if (!subtable1->Resize(ideal_size1, no_attributes, masked)) //  || !subtable2->Resize(ideal_size2, no_attributes, masked))
			return NULL;
		
		
		for (i = 0; i < ideal_size1; i++) {
			for (j = 0; j < no_attributes; j++) {
				
				if (!subtable1->SetEntry(i, j, table->GetEntry(i, j, masked), masked))
				{	return NULL;}
		}}
		// Set subtable names.
		subtable1->SetName(String::Format(subtable1->GetNoObjects(masked)) + " objects");
		// subtable2->SetName(String::Format(subtable2->GetNoObjects(masked)) + " objects");
		
		message.Notify("Copying metadata...");
		
		// Duplicate the dictionaries.
		if (table->HasDictionary()) {
			subtable1->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
			// subtable2->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
		}
		else {
			subtable1->SetDictionary(NULL);
			// subtable2->SetDictionary(NULL);
		}
		
		// Set attribute status fields.
		for (i = 0; i < no_attributes; i++) {
			subtable1->SetStatus(i, table->GetStatus(i, masked), masked);
			// subtable2->SetStatus(i, table->GetStatus(i, masked), masked);
		}
		
		// Set attribute masks.
		for (i = 0; i < no_attributes; i++) {
			subtable1->SetAttributeMask(i, table->GetAttributeMask(i));
			// subtable2->SetAttributeMask(i, table->GetAttributeMask(i));
		}
		
		// Set object masks.
		for (i = 0; i < indices1.size(); i++)
			subtable1->SetObjectMask(i, table->GetObjectMask(indices1[i]));
		//for (i = 0; i < indices2.size(); i++)
		// subtable2->SetObjectMask(i, table->GetObjectMask(indices2[i]));
		
		
		
		
		// PermLeaveOneOutCVSerialExecutor *executor = new PermLeaveOneOutCVSerialExecutor();
		
		executor->SetCommandFilename(GetCommandFilename());
		executor->SetLogFilename(GetLogFilename());
		executor->SetN(no_objects);
		executor->SetPermutation(0);
		// executor->InvertSampling(m_InvertSampling);
		executor->SetLength(GetLength());
		//problem
		executor->SetSeed(GetSeed());
		
		executor->Apply(*subtable1);
		
		original_auc=executor->GetAUC();
		original_accuracy=executor->GetAccuracy();
		
		message.Warning("Original AUC :" + String::Format(original_auc) + "\nOriginal Accuracy :" + String::Format(original_accuracy));
		
		StaticCleanUp(*executor, Undefined::Id()) ;
		
		
		myhej1 = "Classifier statistics for original data\n-------------------------------------------------\nAccuracy:" + String::Format(original_accuracy) + "\nAUC:" + String::Format(original_auc) + "\n\n\n";
		/*
		SaveLogEntry(stream, myhej1 );
		*/
		
		// do b permutations
		for (k=0;k<b;k++)
		{
			
			
			// Sample object indices,
			for (i = 0; i < no_objects; i++)
				indices.push_back(i);
			
			while (!indices.empty()) {
				
				// Sample an index into the vector of remaining indices.
				int index = rng.DrawInteger(0, indices.size() - 1);
				
				if (indices1.size() < ideal_size1)	
					indices1.push_back(indices[index]);
				
				// To remove the index, move the tail into its slot and remove the tail.
				indices[index] = indices[indices.size() - 1];
				indices.pop_back();
				
			}
			
			// This isn't necessary, but a user might appreciate it if the table has masked attributes...
			// std::sort(indices1.begin(), indices1.end());
			//	std::sort(indices2.begin(), indices2.end());
			
			message.Notify("Copying entries...");
			
			// Copy sampled objects into the two already resized tables.
			for (i = 0; i < indices1.size(); i++) {
				int index = indices1[i];
				for (j = 0; j < no_attributes; j++) {
					if (j==(no_attributes-1)){
						if (!subtable1->SetEntry(i, j, table->GetEntry(index, j, masked), masked))
							return NULL;
					}
					else{
						if (!subtable1->SetEntry(i, j, table->GetEntry(i, j, masked), masked))
							return NULL;
				}}
				
			}
			
			
			
			// PermLeaveOneOutCVSerialExecutor executor;
			executor->SetCommandFilename(GetCommandFilename());
			executor->SetLogFilename(GetLogFilename());
			executor->SetN(no_objects);
			executor->SetPermutation(k+1);
			// executor->InvertSampling(m_InvertSampling);
			executor->SetLength(GetLength());
			//problem
			executor->SetSeed(GetSeed());
			//
			executor->Apply(*subtable1);
			
			auc=executor->GetAUC();
			accuracy=executor->GetAccuracy();
			aucs.push_back(auc);
			accuracies.push_back(accuracy);
			if (auc>=original_auc)	{
				higher_auc++;
			}
			if (accuracy>=original_accuracy)	{
				higher_accuracy++;
			}
			accuracies.push_back(executor->GetAccuracy());
			message.Warning("AUC :" + String::Format(auc) + "\nAccuracy :" + String::Format(accuracy));
			StaticCleanUp(*executor, Undefined::Id()) ;
			table->AppendChild(subtable1.GetPointer());
			
			indices1.resize(0);
		}
}

else if (GetAlgorithm()==1)

{
	message.Warning("Algorithm 0 - loocv");
	// subtable1 = Creator::DecisionTable();
	
	if (!subtable1->Resize(ideal_size1, no_attributes, masked)) //  || !subtable2->Resize(ideal_size2, no_attributes, masked))
		return NULL;
	
	
	for (i = 0; i < ideal_size1; i++) {
		for (j = 0; j < no_attributes; j++) {
			
			if (!subtable1->SetEntry(i, j, table->GetEntry(i, j, masked), masked))
			{	return NULL;}
	}}
	// Set subtable names.
	subtable1->SetName(String::Format(subtable1->GetNoObjects(masked)) + " objects");
	// subtable2->SetName(String::Format(subtable2->GetNoObjects(masked)) + " objects");
	
	message.Notify("Copying metadata...");
	
	// Duplicate the dictionaries.
	if (table->HasDictionary()) {
		subtable1->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
		// subtable2->SetDictionary(dynamic_cast(Dictionary *, table->GetDictionary()->Duplicate()));
	}
	else {
		subtable1->SetDictionary(NULL);
		// subtable2->SetDictionary(NULL);
	}
	
	// Set attribute status fields.
	for (i = 0; i < no_attributes; i++) {
		subtable1->SetStatus(i, table->GetStatus(i, masked), masked);
		// subtable2->SetStatus(i, table->GetStatus(i, masked), masked);
	}
	
	// Set attribute masks.
	for (i = 0; i < no_attributes; i++) {
		subtable1->SetAttributeMask(i, table->GetAttributeMask(i));
		// subtable2->SetAttributeMask(i, table->GetAttributeMask(i));
	}
	
	// Set object masks.
	for (i = 0; i < indices1.size(); i++)
		subtable1->SetObjectMask(i, table->GetObjectMask(indices1[i]));
	//for (i = 0; i < indices2.size(); i++)
	// subtable2->SetObjectMask(i, table->GetObjectMask(indices2[i]));
	
	
	//PermCVSerialExecutor executor2 ;
	
	executor2->SetCommandFilename(GetCommandFilename());
	executor2->SetLogFilename(GetLogFilename());
	executor2->SetN(GetNPermutations());
	executor2->SetPermutation(0);
	// executor2->InvertSampling(m_InvertSampling);
	executor2->SetLength(GetLength());
	//problem
	executor2->SetSeed(GetSeed());
	
	executor2->Apply(*subtable1);
	
	original_auc=executor2->GetAUC();
	original_accuracy=executor2->GetAccuracy();
	
	message.Warning("\n\n\nOriginal AUC :" + String::Format(original_auc) + "\nOriginal Accuracy :" + String::Format(original_accuracy));
	StaticCleanUp(*executor2, Undefined::Id()) ;
	
	myhej1 = "Classifier statistics for original data\n-------------------------------------------------\nAccuracy:" + String::Format(original_accuracy) + "\nAUC:" + String::Format(original_auc) + "\n\n\n";
	/*
	SaveLogEntry(stream, myhej1 );
	*/
	
	// do b permutations
	for (k=0;k<b;k++)
	{
		
		
		// Sample object indices,
		for (i = 0; i < no_objects; i++)
			indices.push_back(i);
		
		while (!indices.empty()) {
			
			// Sample an index into the vector of remaining indices.
			int index = rng.DrawInteger(0, indices.size() - 1);
			
			if (indices1.size() < ideal_size1)	
				indices1.push_back(indices[index]);
			
			// To remove the index, move the tail into its slot and remove the tail.
			indices[index] = indices[indices.size() - 1];
			indices.pop_back();
			
		}
		
		// This isn't necessary, but a user might appreciate it if the table has masked attributes...
		// std::sort(indices1.begin(), indices1.end());
		//	std::sort(indices2.begin(), indices2.end());
		
		message.Notify("Copying entries...");
		
		// Copy sampled objects into the two already resized tables.
		for (i = 0; i < indices1.size(); i++) {
			int index = indices1[i];
			for (j = 0; j < no_attributes; j++) {
				if (j==(no_attributes-1)){
					if (!subtable1->SetEntry(i, j, table->GetEntry(index, j, masked), masked))
						return NULL;
				}
				else{
					if (!subtable1->SetEntry(i, j, table->GetEntry(i, j, masked), masked))
						return NULL;
			}}
			
		}
		
		
		
		
		//PermCVSerialExecutor executor2;
		
		executor2->SetCommandFilename(GetCommandFilename());
		executor2->SetLogFilename(GetLogFilename());
		executor2->SetN(GetNPermutations());
		executor2->SetPermutation(k+1);
		// executor2->InvertSampling(m_InvertSampling);
		executor2->SetLength(GetLength());
		//problem
		executor2->SetSeed(GetSeed());
		
		executor2->Apply(*subtable1);
		
		auc=executor2->GetAUC();
		accuracy=executor2->GetAccuracy();
		aucs.push_back(auc);
		accuracies.push_back(accuracy);
		if (auc>=original_auc)	{
			higher_auc++;
		}
		if (accuracy>=original_accuracy)	{
			higher_accuracy++;
		}
		aucs.push_back(executor2->GetAUC());
		accuracies.push_back(executor2->GetAccuracy());
		message.Warning("AUC :" + String::Format(auc) + "\nAccuracy :" + String::Format(accuracy));
		StaticCleanUp(*executor2, Undefined::Id()) ;
		table->AppendChild(subtable1.GetPointer());
		
		indices1.resize(0);
	}
	
}

IOKit::Close(stream);
delete executor;
delete executor2;
executor=0;
executor2=0;

if (!IOKit::Open(stream, GetLogFilename(),true)) {
	Message::Error("Failed to open " + GetLogFilename() + " for output.");
	return NULL;
}


// Calculate how many in accuracies and aucs that is > the original ones.
// save the output to a text file, along with the original AUC and accuracy.

SaveLogEntry(stream, myhej1 );


String myhej = "Average classifier statistics for permutations\n-------------------------------------------------\nAccuracy:" + String::Format(MathKit::Mean(accuracies)) + "\nAUC:" + String::Format(MathKit::Mean(aucs)) + "\n\n\n";

SaveLogEntry(stream, myhej );

String myhej3 = "Permutations with better statistics than original data\n-------------------------------------------------\nAccuracy:" + String::Format(float(1.0f*higher_accuracy/b)) + "\nAUC:" + String::Format(float(1.0f*higher_auc/b)) + "\n\n\n";

SaveLogEntry(stream, myhej3 );

// Execute commands.

IOKit::Close(stream);


ifstream instream;

Handle<Structure> textfile = dynamic_cast(Structure *, new TextFile);
dynamic_cast(TextFile *, const_cast(Structure *, textfile.GetPointer()))->Load(instream);
dynamic_cast(TextFile *, const_cast(Structure *, textfile.GetPointer()))->SetFilename(GetLogFilename());
textfile->SetName(GetLogFilename());
table->AppendChild(textfile.GetPointer());




// Determine fate of subtables and return.
if (AppendAsChildren()) {
	// Append the subtables as children to the input table.
	if (!table->AppendChild(subtable1.GetPointer())){
		///|| !table->AppendChild(subtable2.GetPointer())) {
		Message::Error("Error appending the subtables to the parent table.");
		return NULL;
	}
	// Return the input table itself.
	return table.Release();
}
else {
	// Create a decision table set.
    Handle<DecisionTables> subtables = Creator::DecisionTables();
	if (!subtables->AppendStructure(subtable1.GetPointer())) {
		// || !subtables->AppendStructure(subtable2.GetPointer())) {
		Message::Error("Error appending the subtables to the subtable set.");
		return NULL;
	}
	// Return the set of subtables.
	return subtables.Release();
}

}

bool
PermutationTest::SaveLogHeader(ofstream &stream) const {
	
	stream << "% Output from ROSETTA, " << SystemKit::GetUser() << " " << SystemKit::GetTimestamp() << endl;
	stream << "%" << endl;
	stream << "% " << IdHolder::GetClassname(GetId()) << endl;
	stream << "% {" << GetParameters() << "}" << endl << endl;
	
	return true;
	
}


bool
PermutationTest::SaveLogEntry(ofstream &stream, String mystring) const {
	stream <<mystring;
	/*
	String timestamp = SystemKit::GetTimestamp();
	String indent(' ', timestamp.GetLength());
	
	  stream << timestamp << " Executing " << IdHolder::GetClassname(algorithm.GetId()) << "..." << endl;
	  stream << indent    << " Parameters = {" << algorithm.GetParameters() << "}" << endl;
	*/
	return true;
	
} 

PermutationTest *
PermutationTest::Clone() {
	return new PermutationTest;
}
