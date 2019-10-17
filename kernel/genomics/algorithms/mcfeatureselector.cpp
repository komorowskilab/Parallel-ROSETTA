/*
 * mcfs.cpp
 *
 *  Created on: Feb 23, 2011
 *      Author: kruczyk
 */

//-------------------------------------------------------------------
// Author........: Marcin Kruczyk
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
#include <iostream>


#include <kernel/genomics/algorithms/mcfeatureselector.h>
#include <kernel/genomics/algorithms/featureselector.h>
#include <kernel/genomics/algorithms/manualfeatureselector.h>

#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/algorithm.h>
#include <kernel/algorithms/johnsonreducer.h>
#include <kernel/algorithms/rulegenerator.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/floatattribute.h>

#include <kernel/system/fstream.h>

#include <omp.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>
#include <kernel/basic/idholder.h>
#include <kernel/basic/vector.h>

#include <common/objectmanager.h>


#include <time.h>
#include <math.h>

// -N 
#include <kernel/utilities/threadcontrol.h> 

using namespace std;


const String boolean_name = "boolean";
const String float_name = "float";
const String integer_name = "integer";
const String string_name = "string";


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

MCFeatureSelector::MCFeatureSelector() {
//Fraction of features passed to Manual Feature Selector
	SetFeaturesFraction(0.0);
//Number of features passed to Manual Feature Selector
	SetFeaturesNumber(0);
//Number of projections (subsets of attributes on which Reducer is run)
	SetProjections(1000);
//Number of features randomly selected for the Reducer
	SetProjectionSize(0);
//Name of classifier
	SetClassifier("JohnsonReducer");
//Type of classifier
	SetClassifiersType("Reducer");
	InitializeCriterions();
	SetCriterion("Frequency");
	SetClassifierDiscernibility("Object");
	SetClassifierModuloDecision(true);
	SetClassifierApproximate(true);
	SetClassifierFraction(0.9);
	SetClassifierBRT(false);
	SetClassifierSelection("All");
	SetNumberOfBackgroundLoops(0);
	SetTestNormalization(false);
	SetSignificanceCutOff(0.05);
	SetAccuracyWeight(1);
	SetSeed(1);
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

MCFeatureSelector::~MCFeatureSelector() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

// IMPLEMENTIDMETHODS(MCFeatureSelector, MCFeatureSelector, Reducer)
IMPLEMENTIDMETHODS(MCFeatureSelector, MCFEATURESELECTOR, FeatureSelector)

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
MCFeatureSelector::GetParameters() const {

	String parameters;

  	parameters += FeatureSelector::GetParameters();

  	if(GetFeaturesNumber()) {
  		parameters += Keyword::Features() + Keyword::Dot() + Keyword::Number();
  		parameters += Keyword::Assignment();
  		parameters += String::Format(GetFeaturesNumber());
  	}
  	else {
  		parameters += Keyword::Features() + Keyword::Dot() + Keyword::Fraction();
  		parameters += Keyword::Assignment();
  		parameters += String::Format(GetFeaturesFraction());
  	}


  	parameters += Keyword::Separator();

  	parameters += Keyword::BackgroundLoops();
  	parameters += Keyword::Assignment();
  	parameters += String::Format(GetNumberOfBackgroundLoops());

  	parameters += Keyword::Separator();

  	parameters += Keyword::Test() + Keyword::Dot() + Keyword::Normalization();
  	parameters += Keyword::Assignment();
  	parameters += String::Format(GetTestNormalization());

	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());

  	parameters += Keyword::Separator();

	parameters += Keyword::SignificanceCutOff();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSignificanceCutOff());

  	parameters += Keyword::Separator();

	parameters += Keyword::Projections();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetProjections());

  	parameters += Keyword::Separator();

	parameters += Keyword::ProjectionSize();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetProjectionSize());

  	parameters += Keyword::Separator();

	parameters += Keyword::Accuracy() + Keyword::Dot() + Keyword::Weight();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetAccuracyWeight());

	parameters += Keyword::Separator();

	parameters += Keyword::Classifier();
	parameters += Keyword::Assignment();
	parameters += GetClassifier();

	parameters += Keyword::Separator();

	parameters += Keyword::Classifiers() + Keyword::Dot() + Keyword::Type();
	parameters += Keyword::Assignment();
	parameters += GetClassifiersType();

	parameters += Keyword::Separator();

	parameters += Keyword::Criterion();
	parameters += Keyword::Assignment();
	parameters += GetCriterion();

	parameters += Keyword::Separator();

	String reducer = "REDUCER";
	String holte = "HOLTE1RREDUCER";
	String johnson = "JOHNSONREDUCER";
	String genetic = "SAVGENETICREDUCER";
	if(GetClassifiersType() == reducer) {
		String keyword;
		if(GetClassifier() != holte) {
			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Discernibility();
			if(GetIndexOfParameter(keyword, string_name) < 0) {
				parameters += keyword;
				parameters += Keyword::Assignment();
				parameters += GetClassifierDiscernibility();													//Full
				parameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Modulo() + Keyword::Dot() + Keyword::Decision();
			if(GetIndexOfParameter(keyword, boolean_name) < 0) {
				parameters += keyword;
				parameters += Keyword::Assignment();
				parameters += String::Format(GetClassifierModuloDecision());													//True
				parameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::BRT();
			if(GetIndexOfParameter(keyword, boolean_name) < 0) {
				parameters += keyword;
				parameters += Keyword::Assignment();
				parameters += String::Format(GetClassifierBRT());													//False
				parameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Selection();
			if(GetIndexOfParameter(keyword, string_name) < 0) {
				parameters += keyword;
				parameters += Keyword::Assignment();
				parameters += GetClassifierSelection();													//All
				parameters += Keyword::Separator();
			}
		}
		if(GetClassifier() == johnson || GetClassifier() == genetic) {
			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Approximate();
			if(GetIndexOfParameter(keyword, boolean_name) < 0) {
				parameters += keyword;
				parameters += Keyword::Assignment();
				parameters += String::Format(GetClassifierApproximate());														//True
				parameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Fraction();
			if(GetClassifierApproximate() && GetIndexOfParameter(keyword, float_name) < 0) {
				parameters += keyword;
				parameters += Keyword::Assignment();
				parameters += String::Format(GetClassifierFraction());														//0.8
				parameters += Keyword::Separator();
			}
		}
	}

	int size;
	size = GetSizeOfParameterVector(boolean_name);
	for(int i=0; i<size; i++) {
		parameters += GetParameterName(i, boolean_name);
		parameters += Keyword::Assignment();
		parameters += String::Format(GetBoolParameterValue(i));

		parameters += Keyword::Separator();
	}

	size = GetSizeOfParameterVector(float_name);
	for(int i=0; i<size; i++) {
		parameters += GetParameterName(i, float_name);
		parameters += Keyword::Assignment();
		parameters += String::Format(GetFloatParameterValue(i));

		parameters += Keyword::Separator();
	}

	size = GetSizeOfParameterVector(integer_name);
	for(int i=0; i<size; i++) {
		parameters += GetParameterName(i, integer_name);
		parameters += Keyword::Assignment();
		parameters += String::Format(GetIntegerParameterValue(i));

		parameters += Keyword::Separator();
	}

	size = GetSizeOfParameterVector(string_name);
	for(int i=0; i<size; i++) {
		parameters += GetParameterName(i, string_name);
		parameters += Keyword::Assignment();
		parameters += GetStringParameterValue(i);

		parameters += Keyword::Separator();
	}
	return parameters;
}





String
MCFeatureSelector::GetParametersForClassifier() const {
  	String classifierParameters;
	String reducer = "REDUCER";
	String holte = "HOLTE1RREDUCER";
	String johnson = "JOHNSONREDUCER";
	String genetic = "SAVGENETICREDUCER";

	if(GetClassifiersType() == reducer) {
		String keyword;
		if(GetClassifier() != holte) {
			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Discernibility();
			if(GetIndexOfParameter(keyword, string_name) < 0) {
				classifierParameters += Keyword::Discernibility();
				classifierParameters += Keyword::Assignment();
				classifierParameters += GetClassifierDiscernibility();

				classifierParameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Modulo() + Keyword::Dot() + Keyword::Decision();
			if(GetIndexOfParameter(keyword, boolean_name) < 0) {
				classifierParameters += Keyword::Modulo() + Keyword::Dot() + Keyword::Decision();
				classifierParameters += Keyword::Assignment();
				classifierParameters += String::Format(GetClassifierModuloDecision());

				classifierParameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::BRT();
			if(GetIndexOfParameter(keyword, boolean_name) < 0) {
				classifierParameters += Keyword::BRT();
				classifierParameters += Keyword::Assignment();
				classifierParameters += String::Format(GetClassifierBRT());

				classifierParameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Selection();
			if(GetIndexOfParameter(keyword, string_name) < 0) {
				classifierParameters += Keyword::Selection();
				classifierParameters += Keyword::Assignment();
				classifierParameters += GetClassifierSelection();

				classifierParameters += Keyword::Separator();
			}
		}
		if(GetClassifier() == johnson or GetClassifier() == genetic) {
			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Approximate();
			if(GetIndexOfParameter(keyword, boolean_name) < 0) {
				classifierParameters += Keyword::Approximate();
				classifierParameters += Keyword::Assignment();
				classifierParameters += String::Format(GetClassifierApproximate());

				classifierParameters += Keyword::Separator();
			}

			keyword = Keyword::Classifier() + Keyword::Dot() + Keyword::Fraction();
			if(GetIndexOfParameter(keyword, float_name) < 0) {
				classifierParameters += Keyword::Fraction();
				classifierParameters += Keyword::Assignment();
				classifierParameters += String::Format(GetClassifierFraction());

				classifierParameters += Keyword::Separator();
			}
		}
	}


	String name_of_parameter;
	for(int i=0; i<GetSizeOfParameterVector(boolean_name); i++) {
		name_of_parameter = GetParameterName(i, boolean_name);
		if(name_of_parameter.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot()) {
			classifierParameters += name_of_parameter.From(11);
			classifierParameters += Keyword::Assignment();
			classifierParameters += String::Format(GetBoolParameterValue(i));

			classifierParameters += Keyword::Separator();
		}
	}

	for(int i=0; i<GetSizeOfParameterVector(integer_name); i++) {
		name_of_parameter = GetParameterName(i, integer_name);
		if(name_of_parameter.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot()) {
			classifierParameters += name_of_parameter.From(11);
			classifierParameters += Keyword::Assignment();
			classifierParameters += String::Format(GetIntegerParameterValue(i));

			classifierParameters += Keyword::Separator();
		}
	}

	for(int i=0; i<GetSizeOfParameterVector(float_name); i++) {
		name_of_parameter = GetParameterName(i, float_name);
		if(name_of_parameter.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot()) {
			classifierParameters += name_of_parameter.From(11);
			classifierParameters += Keyword::Assignment();
			classifierParameters += String::Format(GetFloatParameterValue(i));

			classifierParameters += Keyword::Separator();
		}
	}

	for(int i=0; i<GetSizeOfParameterVector(string_name); i++) {
		name_of_parameter = GetParameterName(i, string_name);
		if(name_of_parameter.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot()) {
			classifierParameters += name_of_parameter.From(11);
			classifierParameters += Keyword::Assignment();
			classifierParameters += GetStringParameterValue(i);

			classifierParameters += Keyword::Separator();
		}
	}
	return classifierParameters;
}



String
MCFeatureSelector::GetParametersForManualFeatureSelector(String attributes) const {

	String parameters;

	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += attributes;

	parameters += Keyword::Separator();

//	Removed. ManualFeatureSelector does not have a passthrough for FeatureSelector. -N
//	parameters += FeatureSelector::GetParameters();


	int file_keyword_position;
	file_keyword_position = parameters.GetIndex(Keyword::SaveFeatureSelectionFilename());

	if (file_keyword_position > 0) {
		String temp1, temp2;
		int file_start_position;
		file_start_position = parameters.GetIndex(Keyword::Assignment(), file_keyword_position) + 1;

		int file_end_position;
		file_end_position = parameters.GetIndex(Keyword::Separator(), file_start_position);
		if(file_end_position < 0)
			file_end_position = parameters.GetLength() - 1;

		temp1 = parameters.GetSubstring(file_start_position, file_end_position - file_start_position);
		temp2 = temp1;

		int dot_position;
		dot_position = temp2.Search(temp2.GetLength() - 4, '.');
		if(dot_position > 0)
			temp2 = temp2.Before(dot_position) + "_DT." + temp2.After(dot_position);
		else
			temp2 = temp2 + "_DT";

		parameters.Replace(temp1, temp2);

	}


	return parameters;

}



String
MCFeatureSelector::GetParameterName(int index, const String &parameterType) const {
	if(parameterType == boolean_name)
		return boolNames_[index];

	if(parameterType == float_name)
		return floatNames_[index];

	if(parameterType == integer_name)
		return integerNames_[index];

	if(parameterType == string_name)
		return stringNames_[index];

}

int
MCFeatureSelector::GetSizeOfParameterVector(const String &parameterType) const {
	if(parameterType == boolean_name)
		return boolValues_.size();

	if(parameterType == float_name)
		return floatValues_.size();

	if(parameterType == integer_name)
		return integerValues_.size();

	if(parameterType == string_name)
		return stringValues_.size();
	else
		return false;
}

int
MCFeatureSelector::GetIndexOfParameter(String &parameterName, const String &parameterType) const {
	int index = -1;
	if(parameterType == boolean_name) {
		for(int i=0; i<GetSizeOfParameterVector(boolean_name); i++) {
			if(parameterName == GetParameterName(i, boolean_name)) {
				index = i;
				break;
			}
		}
	}

	else if(parameterType == float_name) {
		for(int i=0; i<GetSizeOfParameterVector(float_name); i++) {
			if(parameterName == GetParameterName(i, float_name)) {
				index = i;
				break;
			}
		}
	}

	else if(parameterType == integer_name) {
		for(int i=0; i<GetSizeOfParameterVector(integer_name); i++) {
			if(parameterName == GetParameterName(i, integer_name)) {
				index = i;
				break;
			}
		}
	}
	else if(parameterType == string_name) {
		for(int i=0; i<GetSizeOfParameterVector(string_name); i++) {
			if(parameterName == GetParameterName(i, string_name)) {
				index = i;
				break;
			}
		}
	}

	return index;

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
MCFeatureSelector::SetParameter(const String &keyword, const String &value) {



	// Set attribute descriptions.
	if ((keyword == Keyword::Features() + Keyword::Dot() + Keyword::Number()) && value.IsInteger())
		return SetFeaturesNumber(value.GetInteger());

	if ((keyword == Keyword::Features() + Keyword::Dot() + Keyword::Fraction()) && value.IsFloat())
		return SetFeaturesFraction(value.GetFloat());


	if ((keyword == Keyword::Projections()) && value.IsInteger())
		return SetProjections(value.GetInteger());

	if ((keyword == Keyword::ProjectionSize()) && value.IsInteger())
		return SetProjectionSize(value.GetInteger());

	if (keyword == Keyword::Classifier())
		return SetClassifier(value);

	if (keyword == Keyword::Classifiers() + Keyword::Dot() + Keyword::Type())
		return SetClassifiersType(value);

	if (keyword == Keyword::Criterion())
		return SetCriterion(value);

	if ((keyword == Keyword::BackgroundLoops()) && value.IsInteger())
		return SetNumberOfBackgroundLoops(value.GetInteger());

	if ((keyword == Keyword::Accuracy() + Keyword::Dot() + Keyword::Weight()) && value.IsInteger())
		return SetAccuracyWeight(value.GetInteger());

	if ((keyword == Keyword::Test() + Keyword::Dot() + Keyword::Normalization()) && value.IsBoolean())
		return SetTestNormalization(value.GetBoolean());

	if ((keyword == Keyword::SignificanceCutOff()) && value.IsFloat())
		return SetSignificanceCutOff(value.GetFloat());


	if(value.IsBoolean() && keyword.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot())
		return SetBoolNameAndValue(keyword, value.GetBoolean());

	if(value.IsInteger() && keyword.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot())
		return SetIntegerNameAndValue(keyword, value.GetInteger());

	if(value.IsFloat() && keyword.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot())
		return SetFloatNameAndValue(keyword, value.GetFloat());

	if(keyword.GetSubstring(0, 11) == Keyword::Classifier() + Keyword::Dot())
		return SetStringNameAndValue(keyword, value);

	if ((keyword == Keyword::Seed()) && value.IsInteger())
		return SetSeed(value.GetInteger());


	return FeatureSelector::SetParameter(keyword, value);
}


bool
MCFeatureSelector::SetBoolNameAndValue(const String &boolName, const bool boolValue) {
	boolNames_.push_back(boolName);
	boolValues_.push_back(boolValue);
	return true;
}



bool
MCFeatureSelector::SetFloatNameAndValue(const String &floatName, const float floatValue) {
	floatNames_.push_back(floatName);
	floatValues_.push_back(floatValue);
	return true;
}

bool
MCFeatureSelector::SetIntegerNameAndValue(const String &integerName, const int integerValue) {
	integerNames_.push_back(integerName);
	integerValues_.push_back(integerValue);
	return true;
}


bool
MCFeatureSelector::SetStringNameAndValue(const String &stringName, const String &stringValue) {
	stringNames_.push_back(stringName);
	stringValues_.push_back(stringValue);
	return true;
}

bool
MCFeatureSelector::InitializeCriterions() {
	reducerCriterions_.push_back("FREQUENCY");
	reducerCriterions_.push_back("ACCURACY");
//Place here following criteria for reducer


//Add here a new classifierType and crierions criterion
// + Add new vector in the header
}




bool
MCFeatureSelector::IsCriterionApplicable() const
{
	int index = -1;
	String criterion = GetCriterion();
	String reducer = "REDUCER";

	if(GetClassifiersType() == reducer) {
		int size_of_criterions_vector = reducerCriterions_.size();
		for(int i=0; i<size_of_criterions_vector; i++) {
			if(criterion == reducerCriterions_[i]) {
				index = i;
				break;
			}
		}
	}

/*	Use this code to add new classifiersType
	if(GetClassifiersType() == Keyword:: PUT HERE NEW WORD)
	{
		int size_of_criterions_vector = PUT HERE VECTOR NAME Criterions.size();
		for(int i=0; i<size_of_criterions_vector: i++)
		{
			if(criterion == PUT HERE VECTOR NAME Criterions[i])
			{
				index = i;
				break;
			}
		}
	}
// ADD NEW VECTOR IN HEADER
*/
	if(index < 0)
		return false;
	else
		return true;


}




DecisionTable*
MCFeatureSelector::CreateDecisionTableWithRandColumns(Handle<DecisionTable> mainTable, Vector(int) &vector_of_scores_for_classifier) const {
	int number_of_all_objects, decision_column, projection_size;
	bool masked = true;
	projection_size = vector_of_scores_for_classifier.size();
	number_of_all_objects = mainTable->GetNoObjects(masked);
	decision_column = mainTable->GetDecisionAttribute(masked);

//Create new subtable
	Handle<DecisionTable> subtable = Creator::DecisionTable();

	int n_attributes_in_new_table = projection_size + 1;
//Resize the subtable for all new attributes and decision attribute
	if (!subtable->Resize(number_of_all_objects, n_attributes_in_new_table, masked))
		return NULL;

//fill in the new table with values of chosen attributes
	for(int j = 0; j < projection_size; j++) {
//Fill in the chosen column with values for all objects
		for(int k = 0; k < number_of_all_objects; k++) {
			if (!subtable->SetEntry(k, j, mainTable->GetEntry(k, vector_of_scores_for_classifier[j], masked), masked)) {
					return NULL;
		
			}
		}
	}

//fill in the decision column
	for(int k = 0; k < number_of_all_objects; k++) {
		if (!subtable->SetEntry(k, projection_size, mainTable->GetEntry(k, decision_column, masked), masked)) {
			return NULL;
		}
	}
	return subtable.Release();
}

String
MCFeatureSelector::SelectImportantFeatures(Vector(double) &vector_of_normalized_scores_for_attributes, int number_of_important_features, int decision_column) const {
	int size_of_input_vector, index_of_max;
	size_of_input_vector = vector_of_normalized_scores_for_attributes.size();


	Vector(bool) not_selected_features;
	for(int i=0; i<size_of_input_vector; i++) {
		not_selected_features.push_back(true);
	}


	not_selected_features[decision_column] = false;

	for(int i=0; i<number_of_important_features; i++) {
		index_of_max = SelectPositionOfMaxInVector(vector_of_normalized_scores_for_attributes, not_selected_features);
		not_selected_features[index_of_max] = false;
	}



	String attribute_for_ManualFC;
	for(int i=0; i<size_of_input_vector; i++) {
		if(not_selected_features[i])
			attribute_for_ManualFC += "0";
		else
			attribute_for_ManualFC += "1";
	}
	return attribute_for_ManualFC;
}




int
MCFeatureSelector::SelectPositionOfMaxInVector(Vector(double) &vector_of_normalized_scores_for_attributes, Vector(bool) &mask) const {
	int max_index = 0;
	double max_value = 0;
	int number_of_iterations;
	number_of_iterations = vector_of_normalized_scores_for_attributes.size();

	while(!mask[max_index])
		max_index++;


	for(int i=0; i<number_of_iterations; i++) {
		if(mask[i] && vector_of_normalized_scores_for_attributes[i] > max_value) {
			max_value = vector_of_normalized_scores_for_attributes[i];
			max_index = i;
		}
	}
	return max_index;
}



bool
MCFeatureSelector::DataValidation() const {

	if(GetProjections() < 1) {
		Message::Error("Wrong number of projections.");
		return 0;
	}

	if(GetProjectionSize() < 0) {
		Message::Error("Wrong projection size.");
		return 0;
	}

	if(!IsCriterionApplicable()) {
		Message::Error("Criterion does not fit classifier type.");
		return 0;
	}


	if (IdHolder::GetId(GetClassifier()) == Undefined::Id()) {
		Message::Error("Classifier is not installed.");
		return 0;
	}

  	Handle<Algorithm> classifierAlgorithm = ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(GetClassifier()));

  	if (!classifierAlgorithm->SetParameters(GetParametersForClassifier())) {
  		Message::Error("Wrong parameters for classifier!");
  		return 0;
  	}

  	if(GetAccuracyWeight() < 1) {
  		Message::Error("Wrong accuracy weight!");
  		return 0;
  	}

	return true;
}



Vector(int)
MCFeatureSelector::SortOutput(Vector(double) &vector_of_scores, int decision_column) const
{
	Vector(int) order_of_output;
	Vector(double) operating_vector;
	int number_of_attributes;
	double max_score, temp;
	int max_index, temp_index;


	number_of_attributes = vector_of_scores.size();

	for(int i=0; i<number_of_attributes; i++) {
		order_of_output.push_back(i);
		operating_vector.push_back(vector_of_scores[i]);
	}

	temp_index = order_of_output[decision_column];
	order_of_output[decision_column] = order_of_output[number_of_attributes - 1];
	order_of_output[number_of_attributes - 1] = temp_index;

	for(int i=0; i < number_of_attributes - 1; i++) {
		max_index = i;
		max_score = operating_vector[i];
		for(int j=i; j < number_of_attributes - 1; j++) {
			if(operating_vector[j] > max_score) {
				max_score = operating_vector[j];
				max_index = j;
			}
		}
		temp_index = order_of_output[i];
		temp = operating_vector[i];
		order_of_output[i] = order_of_output[max_index];
		operating_vector[i] = operating_vector[max_index];
		order_of_output[max_index] = temp_index;
		operating_vector[max_index] = temp;
	}
	return order_of_output;
}

//-------------------------------------------------------------------
// Name..........: SaveScores
// Author........: Marcin Kruczyk
// Date..........: ??
// Description...: Saves the ranking list of features sorted by their p-standard. Also includes
//			a cut-off line.
// Comments......: 
// Revisions.....: 14/01/14 -N
//			Added feature score vector (vFeatureScore) to the function. Now features are ranked according
//			to their unique scores, and not the p-standard, which is included after the score now.
//			Also added formatting. Sorting function remains as is, since it works with generic double vectors.
//===================================================================
bool
MCFeatureSelector::SaveScores(Handle<DecisionTable>& table, Vector(double) &vFeatureScore, Vector(double) &vector_of_scores, int number_of_important_features) const {

	ofstream stream;

	bool masked = true, delimiters = false;

	Vector(int) order_of_output;

	//Spacing buffer. Alter at your pleasure. -N
	int nSpacing = 2;

	//Could not open.
	if (!IOKit::Open(stream, GetSaveFeatureSelectionFilename()) ){
		Message::Error("Error opening" + GetSaveFeatureSelectionFilename() + " for input.");
		return 0;
	}

		
	if (!Persistent::Save(stream))
		return false;

	//Set to minimum 6 since that is the length of the header. -N
	int nNameMax = 6;
	//Set to 7 for the same reasons. -N
	int nScoreMax = 7;
	//Go through the lists. We assume that they are all the same size. -N
	for (int i = 0; i < table->GetNoAttributes(masked); ++i) {
		//Get the longest name in the list and use that as the column width.
		if (nNameMax < table->GetAttributeName(i, masked).GetLength() + nSpacing) {
			
			nNameMax = table->GetAttributeName(i, masked).GetLength() + nSpacing;
		}
		//For some reason I can't use Format(x).GetLength() without memory problems. Thus, doing
		//these on two separate lines. -N
		String sScore = String::Format(vFeatureScore[i]);
		if (nScoreMax < sScore.GetLength() + nSpacing) {

			nScoreMax = sScore.GetLength() + nSpacing;
		}
	}
	//Print name header. -N
	String text = "Name";
	if (!IOKit::Save(stream, text + String(' ', nNameMax - text.GetLength()), false))
		return false;

	//Print Score header. -N
	text = "Score";
	if (!IOKit::Save(stream, text + String(' ', nScoreMax - text.GetLength()), false))
		return false;

	//And p-value header. -N
	text = "p-Value";
	if (!IOKit::Save(stream, text, false))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	int number_of_attributes = table->GetNoAttributes(masked);

	//Get a list of indices, sorted on vFeatureScore. -N
	order_of_output = SortOutput(vFeatureScore, table->GetDecisionAttribute(masked));

	for(int i=0; i<number_of_attributes - 1; i++) {
		//If we hit the cut-off, print this line.
		if (i == number_of_important_features) {
			String text = "----------------------------------------------------------------------------------";
			if (!IOKit::Save(stream, text, false))
				return false;
			if (!IOKit::Save(stream, '\n'))
				return false;
		}
		//Print name of the feature.
		if (!IOKit::Save(stream, table->GetAttributeName(order_of_output[i], masked) + String(' ', nNameMax - table->GetAttributeName(order_of_output[i], masked).GetLength()), false))
			return false;

		//Score of the feature.. -N
		String sScore = String::Format(vFeatureScore[order_of_output[i]]);
		String sScoreIndent(' ', nScoreMax - sScore.GetLength());
		if (!IOKit::Save(stream, sScore, false))
			return false;
		if (!IOKit::Save(stream, sScoreIndent, false))
			return false;

		//p-standard of the feature. -N
		if (!IOKit::Save(stream, (float)vector_of_scores[order_of_output[i]], false))
			return false;

		if (!IOKit::Save(stream, '\n'))
			return false;
	}

	IOKit::Close(stream);
	return true;
}

Vector(int)
MCFeatureSelector::EreaseValueFromVector(Vector(int) &input_vector, int index) const {
	Vector(int) output_vector;
	int size_of_input_vector = input_vector.size();
	for(int i=0; i<size_of_input_vector; i++) {
		if(i==index)
			continue;
		output_vector.push_back(input_vector[i]);
	}
	return output_vector;
}

int
MCFeatureSelector::GenerateNewNumberOfFeatures(Vector(double) vector_of_normalized_scores_for_attributes, int decision_column) const {
	int number_of_important_features = 0;
	int number_of_attributes = vector_of_normalized_scores_for_attributes.size();

	double sum_of_scores = 0;
	double score = 0;
	Vector(double) scores;

	for(int i=0; i<number_of_attributes; i++)
	sum_of_scores = sum_of_scores + vector_of_normalized_scores_for_attributes[i];

	double std_score = 1 / (double)number_of_attributes;

	for(int i=0; i<number_of_attributes; i++) {
		score = vector_of_normalized_scores_for_attributes[i] / sum_of_scores;
		scores.push_back(score);
	}

	Vector(bool) not_selected_features;
	for(int i=0; i<number_of_attributes; i++) {
		not_selected_features.push_back(true);
	}
	not_selected_features[decision_column] = false;


	double so_far_sum, diagonal_1 = 1, diagonal_2;
	int index_of_max;
	double max_score;

	index_of_max = SelectPositionOfMaxInVector(scores, not_selected_features);
	max_score = scores[index_of_max];
	not_selected_features[index_of_max] = false;
	so_far_sum = max_score;
	diagonal_2 = (((number_of_important_features + 1) * std_score) * ((number_of_important_features + 1) * std_score)) + ((1 - so_far_sum) * (1 - so_far_sum));


	while(diagonal_1 >= diagonal_2) {
		number_of_important_features++;

		index_of_max = SelectPositionOfMaxInVector(scores, not_selected_features);
		max_score = scores[index_of_max];
		not_selected_features[index_of_max] = false;

		so_far_sum = so_far_sum + max_score;
		diagonal_1 = diagonal_2;
		diagonal_2 = (((number_of_important_features + 1) * std_score) * ((number_of_important_features + 1) * std_score)) + ((1 - so_far_sum) * (1 - so_far_sum));
	}
	return number_of_important_features;

}



int
MCFeatureSelector::GenerateNewNumberOfFeatures(Vector(double) vector_of_scores, double cut_off_pvalue) const {
	int number_of_features, number_of_selected_features = 0;
	number_of_features = vector_of_scores.size();
	double barrier_score = 1 - cut_off_pvalue;

	for(int i=0; i<number_of_features; i++) {
		if(vector_of_scores[i] >= barrier_score)
			number_of_selected_features++;
	}
	return number_of_selected_features;
}







Vector(double)
MCFeatureSelector::GeneratePValues(Vector(Vector(double)) &background_vectors, Vector(double) vector_of_normalized_scores_for_attributes) const {
	int number_of_attributes = vector_of_normalized_scores_for_attributes.size();
	int number_of_background_loops = background_vectors[0].size();
	int sum_of_counts = 1;

	Vector(double) vector_of_scores;

	for(int i=0; i<number_of_attributes; i++) {
		sum_of_counts = 1;
		for(int j=0; j<number_of_background_loops; j++) {
			if(background_vectors[i][j] >= vector_of_normalized_scores_for_attributes[i])
				sum_of_counts++;
		}

		double pvalue, score;
		pvalue = (double)sum_of_counts/(double)(number_of_background_loops + 1);
		score = 1 - pvalue;
		vector_of_scores.push_back(score);

	}

	return vector_of_scores;

}

Vector(double)
MCFeatureSelector::VectorNormalization(Vector(double) vector_of_normalized_scores_for_attributes) const {
	Vector(double) normalized_vector_of_normalizaed_scores_for_attributes;
	double sum_of_elements = 0;
	double mean;
	double sum_of_squares_of_diff = 0;
	double sd;

	for(int i=0; i<vector_of_normalized_scores_for_attributes.size(); i++) {
		sum_of_elements = sum_of_elements + vector_of_normalized_scores_for_attributes[i];
	}

	mean = sum_of_elements / (double)vector_of_normalized_scores_for_attributes.size();

	for(int i=0; i<vector_of_normalized_scores_for_attributes.size(); i++) {
		sum_of_squares_of_diff = sum_of_squares_of_diff + ((vector_of_normalized_scores_for_attributes[i] - mean) * (vector_of_normalized_scores_for_attributes[i] - mean));
	}

	sd = sqrt(sum_of_squares_of_diff/(double)vector_of_normalized_scores_for_attributes.size());

	for(int i=0; i<vector_of_normalized_scores_for_attributes.size(); i++) {
		if(!sd)
			normalized_vector_of_normalizaed_scores_for_attributes.push_back(0);
		else
			normalized_vector_of_normalizaed_scores_for_attributes.push_back((vector_of_normalized_scores_for_attributes[i] - mean)/sd);
	}

	return normalized_vector_of_normalizaed_scores_for_attributes;
}


Vector(Vector(double))
MCFeatureSelector::BackgroundVectorsNormalization(Vector(Vector(double)) background_vectors) const {
	Vector(Vector(double)) normalized_background_vectors;
	normalized_background_vectors.resize(background_vectors.size());

	for(int i=0; i<background_vectors[0].size(); i++) {
		Vector(double) new_background_vector;
		for(int j=0; j<background_vectors.size(); j++)
			new_background_vector.push_back(background_vectors[j][i]);
		Vector(double) normalized_background_vector;
		normalized_background_vector = VectorNormalization(new_background_vector);

		for(int j=0; j<background_vectors.size(); j++)
			normalized_background_vectors[j].push_back(normalized_background_vector[j]);
	}

	return normalized_background_vectors;
}

Vector(double)
MCFeatureSelector::PermutationTest(Handle<DecisionTable>& table, Vector(double) vector_of_normalized_scores_for_attributes, int number_of_background_loops) const {

	bool masked = true;
	int total_number_of_attributes = table->GetNoAttributes(masked);
	Vector(double) background_normalized_scores_for_attributes_vector;
	background_normalized_scores_for_attributes_vector.resize(total_number_of_attributes);
	Vector(Vector(double)) background_vectors;
	Vector(double) error_vector;
	error_vector.push_back(-1);
	background_vectors.resize(total_number_of_attributes);

	int decision_column = table->GetDecisionAttribute(masked);
	int number_of_objects = table->GetNoObjects(masked);
	int random_number;

//Create new decision table and copy values from original one
//********************************************************************************************************************************
	Handle<DecisionTable> random_table = Creator::DecisionTable();

	//Resize the subtable for all new attributes and decision attribute
	if (!random_table->Resize(number_of_objects, total_number_of_attributes, masked) )
		return error_vector;

	//fill in the new table with values of chosen attributes
	for(int i=0; i<total_number_of_attributes; i++) {
		//Fill in the chosen column with values for all objects
		for(int j=0; j<number_of_objects; j++) {
			if (!random_table->SetEntry(j, i, table->GetEntry(j, i, masked), masked)) {
					return error_vector;
			}
		}
	}

//Loop generating background distributions for all the attributes
	for(int i=0; i<number_of_background_loops; i++) {
//Preparing new decision table with randomized decision column

		Vector(int) vector_of_not_randomized_indexes;

		for(int j=0; j<number_of_objects; j++) {
			vector_of_not_randomized_indexes.push_back(j);
		}

		for(int j=0; j<number_of_objects; j++) {

			random_number = rand() % (number_of_objects - j);

			if (!random_table->SetEntry(j, decision_column, table->GetEntry(vector_of_not_randomized_indexes[random_number], decision_column, masked), masked)) {
				return error_vector;
			}
			vector_of_not_randomized_indexes = EreaseValueFromVector(vector_of_not_randomized_indexes, random_number);
		}

		String stars = "*************************************************";

	  	cout << stars << endl;
	  	cout << "Permutation " << (i + 1) << " analysis." << endl;
	  	cout << "Starting analysis: " << GetTimeString() << endl;
	
		background_normalized_scores_for_attributes_vector = MCFSParallel(random_table);
		
	  	cout << "Analysis complete: " << GetTimeString() << endl;
	  	cout << stars << endl;

		for(int j=0; j<total_number_of_attributes; j++)
			background_vectors[j].push_back(background_normalized_scores_for_attributes_vector[j]);

	}

//	Remove decision value before generating p-values
	vector_of_normalized_scores_for_attributes.pop_back();
	background_vectors.pop_back();

	if(GetTestNormalization()) {
		vector_of_normalized_scores_for_attributes = VectorNormalization(vector_of_normalized_scores_for_attributes);
		background_vectors = BackgroundVectorsNormalization(background_vectors);
	}

	Vector(double) vector_of_scores;

	vector_of_scores = GeneratePValues(background_vectors, vector_of_normalized_scores_for_attributes);

//	Add score to scores vector
	vector_of_scores.push_back(0);
//	vector_of_normalized_scores_for_attributes.pop_back();
//	vector_of_normalized_scores_for_attributes.push_back(0);

//	return vector_of_normalized_scores_for_attributes;
	return vector_of_scores;
}



String
MCFeatureSelector::GetTimeString() const {
	time_t rawtime;
	struct tm * ptm;
	String time_string;

	time(&rawtime);

	ptm = gmtime (&rawtime);

	String day, month, year, hour, minute, second;

	if(ptm->tm_mday < 10)
		day = "0" + String::Format(ptm->tm_mday);
	else
		day = String::Format(ptm->tm_mday);

	if(ptm->tm_mon < 9)
		month = "0" + String::Format(ptm->tm_mon + 1);
	else
		month = String::Format(ptm->tm_mon + 1);

	year = String::Format(ptm->tm_year + 1900);

	if(ptm->tm_hour < 10)
		hour = "0" + String::Format(ptm->tm_hour);
	else
		hour = String::Format(ptm->tm_hour);

	if(ptm->tm_min < 10)
		minute = "0" + String::Format(ptm->tm_min);
	else
		minute = String::Format(ptm->tm_min);

	if(ptm->tm_sec < 10)
		second = "0" + String::Format(ptm->tm_sec);
	else
		second = String::Format(ptm->tm_sec);

	time_string =  day + "." +  month + "." + year + " " + hour + ":" + minute + ":" + second;

	return time_string;
}



//-------------------------------------------------------------------
// Name..........: SetProjectionMask
// Author........: Nicholas Baltzer
// Date..........: 9/12/13
// Description...: Creates and sets a mask vector for the decision table. This method replaces the subset
//			generation for faster and less memory bus intensive projection computations.
// Comments......: Whenever this method is used, ensure first that the subsequent algorithm respects the masking
//			vector of the decision table.
// Revisions.....: 09/01/14 -N Moved masking vector to internal memory.
//===================================================================
bool
MCFeatureSelector::SetProjectionMask(Handle<DecisionTable> dtTable, Vector(int) *vAttributeSample, Vector(DecisionTable::Mask)* vMask) const {

	//Now created locally, since all functions depending on the mask vector have been moved or altered. 
	//Vector(DecisionTable::Mask) *vMask;

	//The mask vector must be created prior to this function. However, it does not need to be initalized.
	//Init the vector with a length equal to the decision table attributes vector, with each mask starting
	//as disabling.
	vMask->assign(dtTable->GetNoAttributes(false), DecisionTable::Mask::MASK_DISABLED);
	//Go through the attribute sample inidices, and set each of these masks to enabling. These
	//are the features we want to use for the current projection.
	for (Vector(int)::iterator it = vAttributeSample->begin(); it != vAttributeSample->end(); ++it) {
		(*vMask)[*it] = DecisionTable::Mask::MASK_ENABLED;
	}
	//Finally, add in the decision attribute.
	(*vMask)[dtTable->GetDecisionAttribute(false)] = DecisionTable::Mask::MASK_ENABLED;
	//And now, apply the mask vector to the table.
	for (int j = 0; j < dtTable->GetNoAttributes(false); ++j) {
		dtTable->SetAttributeMask(j, (*vMask)[j]);
	}
	//Done.
	return true;
}

//-------------------------------------------------------------------
// Name..........: random_unique (Fisher-Yates Shuffle)
// Author........: Nicholas Baltzer
// Date..........: 10/09/13
// Description...: Grabs num_random elements from the vector range
//			specified by [begin, end] and modifies the vector
//			to contain these only. Fast and bounded.
// Comments......: Source: http://ideone.com/3A3cv
// Revisions.....:
//===================================================================
template<class bidiiter>
bidiiter random_unique(bidiiter begin, bidiiter end, size_t num_random, unsigned int &seed) {
	
	size_t left = std::distance(begin, end);
	while (num_random--) {
		bidiiter r = begin;
		std::advance(r, rand_r(&seed)%left);
		std::swap(*begin, *r);
		++begin;
		--left;
	}
	return begin;
}




//-------------------------------------------------------------------
// Name..........: SetTableProjection
// Author........: Nicholas Baltzer
// Date..........: 09/01/14
// Description...: Creates and sets the projection mask on the table. Moved into a separate function from the main
//			parallel function.
// Comments......: 
// Revisions.....:
//===================================================================
Vector(int)
MCFeatureSelector::SetTableProjection(Handle<DecisionTable> dtTable, Vector(int) *vIndexTemplate, unsigned int &seed, Vector(int) &vOccurrence, const int nProjectionSize) const {

	//Create the mask vector mold from the index template. 
	Vector(int) vAttributeSample(vIndexTemplate->begin(), vIndexTemplate->end());

	//Randomize the mold and move enough attributes to match the projection size.
	random_unique(vAttributeSample.begin(), vAttributeSample.end(), nProjectionSize, seed);

	//Resize the mold to the projection size.
	vAttributeSample.resize(nProjectionSize);
	
	for (int j = 0; j < nProjectionSize; ++j) {
		vOccurrence[vAttributeSample[j]]++;
	}

	//Now use the mold to create the mask for the projection table.
	Vector(DecisionTable::Mask) *vMask;
	SetProjectionMask(dtTable, &vAttributeSample, vMask);

	//Done.
	return vAttributeSample;
}

//-------------------------------------------------------------------
// Name..........: MCFSParallel
// Author........: Marcin Kruczyk
// Date..........: ?
// Description...: Computational engine of Random Reducts.
// Comments......: 
// Revisions.....: 10/09/13 -N Threading. Added threading and fixed some bugs.
//		   5/12/13 -N Optimizations. Moved several vectors
//			over to optimized generation. Removed the subtable generation
//			and replaced it with a masking vector on the input table.
//===================================================================


Vector(double)
MCFeatureSelector::MCFSParallel(Handle<DecisionTable> table) const {



//Getting Parameters
	int projections = GetProjections();


	String	classifierName = GetClassifier();
  	Id classifier_id = IdHolder::GetId(classifierName);
  	Id ruleGenerator_id = IdHolder::GetId("RuleGenerator");



  	String classifierParameterList;

  	classifierParameterList = GetParametersForClassifier();



  	Handle<Algorithm> classifierAlgorithm = ObjectManager::GetIdentifiedAlgorithm(classifier_id);
  	Handle<Algorithm> ruleGeneratorAlgorithm = ObjectManager::GetIdentifiedAlgorithm(ruleGenerator_id);

  	classifierAlgorithm->SetParameters(classifierParameterList);



	bool masked = true;
	int total_number_of_attributes, decision_column;


	total_number_of_attributes = table->GetNoAttributes(masked);
	decision_column = table->GetDecisionAttribute(masked);


//If projection size attribute is set to 0 square root of number of attributes is set as a projection size
	int projection_size = GetProjectionSize();

	if(projection_size == 0)
	{
		projection_size = (int)sqrt(total_number_of_attributes);
	}


	Vector(double) vector_of_scores_for_attributes;
	Vector(int) vector_of_occurences_of_attributes;
	Vector(double) vector_of_normalized_scores_for_attributes;


	for(int i = 0; i < total_number_of_attributes; i++)
	{
		vector_of_scores_for_attributes.push_back(0);
		vector_of_occurences_of_attributes.push_back(0);
		vector_of_normalized_scores_for_attributes.push_back(0);
	}


// save original sbuf
//There was a problem because reducer prints messages on the screen. In parallel mode it makes no sense, so I redirected the output to empty device
    std::streambuf  *cout_sbuf = std::cout.rdbuf();
    std::ofstream   fout("/dev/null");

// redirect 'cout' to a 'fout'
    std::cout.rdbuf(fout.rdbuf());
    std::ostream	out(cout_sbuf);

    out.rdbuf(cout_sbuf);

	Vector(int) vIndexTemplate;
	for (int i = 0; i < total_number_of_attributes; ++i) {
		if (i != decision_column && table->IsAttributeEnabled(i)) {
			vIndexTemplate.push_back(i);
		}
	}


    bool if_thread_is_finished[projections];
    for(int i = 0; i < projections; i++)
    	if_thread_is_finished[i] = false;

	Vector(unsigned int) vSeed;
	//Added in the whole parallel thing again. -N
	#ifdef _OPENMP
		ThreadControl::GetSplitSize();	
		for (int i = 0; i < projections; ++i) {
			vSeed.push_back(rand());
//			cerr << vSeed[i] << ", ";
		}	
	#endif

	#pragma omp parallel \
	default(shared)
	{
		ThreadControl::AddThreadToCount();
// The real evaluation is done here

		#pragma omp for schedule(dynamic)
		for (int i = 0; i < projections; i++)
		{
			Handle<DecisionTable> subtable;
			Vector(int) vector_of_scores_for_classifier_names;
			Vector(int) vector_of_scores_for_classifier_i(total_number_of_attributes, 0);
			Vector(double) vector_of_scores_for_classifier_d(total_number_of_attributes, 0);
//temporary variable for a new, randomly selected feature for the new table
			int random_feature;

			Vector(int) vFeatureSample(vIndexTemplate.begin(), vIndexTemplate.end());

			int j = 0;
			
			random_unique(vFeatureSample.begin(), vFeatureSample.end(), projection_size, vSeed[i]);

//			while(j < projection_size)
//			{
//Randomize the new feature
//				if((random_feature = rand() % total_number_of_attributes ) == decision_column)
//					continue;
//				bool already_exists = false;
//				for(int l = 0; l < vector_of_scores_for_classifier_names.size();l++)
//				{
//					if(vector_of_scores_for_classifier_names[l] == random_feature)
//						already_exists = true;
//				}
//				if(already_exists)
//					continue;

//				vector_of_scores_for_classifier_names.push_back(random_feature);
//				vector_of_occurences_of_attributes[random_feature]++;
//				vector_of_scores_for_classifier_i.push_back(0);
//				vector_of_scores_for_classifier_d.push_back(0);
//				j++;
//			}
			for (int j = 0; j < vFeatureSample.size(); ++j) {
				++vector_of_occurences_of_attributes[vFeatureSample[j]];
			}
			vFeatureSample.resize(projection_size);
			vector_of_scores_for_classifier_names = vFeatureSample;
			subtable = CreateDecisionTableWithRandColumns(table, vector_of_scores_for_classifier_names);




//Tyoe of returned values is important
			String sReducer = "REDUCER";
			if (GetClassifiersType() == sReducer) {
//Variable for returned value is initialized here
				Handle<Reducts> my_reducts;
//On the subtable the selected algorithm is executed
				my_reducts = dynamic_cast(Reducts *, subtable->Apply(*classifierAlgorithm));

				int number_of_reducts, number_of_attributes_in_reduct;
				number_of_reducts = my_reducts->GetNoReducts();
				if(number_of_reducts == 0)
				{
					delete my_reducts.Release();
					continue;
				}


				Reduct *my_reduct;

				for(int j=0; j<number_of_reducts; j++)
				{
					my_reduct = my_reducts->GetReduct(j);
					number_of_attributes_in_reduct = my_reduct->GetNoAttributes();
				}


//Check criterion which we use for the selection of features
				String sFrequency = "FREQUENCY";
				String sAccuracy = "ACCURACY";
				if(GetCriterion() == sFrequency) {
//We are checking all reduct in list of reducts returned
					for(int k = 0; k < number_of_reducts; k++)
					{
						my_reduct = my_reducts->GetReduct(k);
						number_of_attributes_in_reduct = my_reduct->GetNoAttributes();
//We are checking each feature in each reduct returned
						for(int j = 0; j < number_of_attributes_in_reduct; j++)
//We are incrementing the vector with scores each time a feature appears in a reducts
						{
							vector_of_scores_for_classifier_i[my_reduct->GetAttribute(j)]++;
						}

					}

					int index_in_main_vector, value_to_be_added;
					for(int k = 0; k < projection_size; k++)
					{
						#pragma omp critical(update_score_table)
						{
//We are adding the results for a single classifier to the main vactor with results
//TO MOZNA CHYBA PRZENIESC POZA SECTION CRITICAL (2 PIERWSZE LINIE)
							index_in_main_vector = vector_of_scores_for_classifier_names[k];
							value_to_be_added = vector_of_scores_for_classifier_i[k];
							vector_of_scores_for_attributes[index_in_main_vector] = vector_of_scores_for_attributes[index_in_main_vector] + (double)value_to_be_added;
						}
					}

				}

//Check criterion which we use for the selection of features
				else if(GetCriterion() == sAccuracy) {
//We are checking all reduct in list of reducts returned
					Handle<Rules> rules;
//On the subtable the selected algorithm is executed
					rules = dynamic_cast(Rules*, my_reducts->Apply(*ruleGeneratorAlgorithm));
					int number_of_rules, number_of_attributes_in_rule, number_of_decision_values, support_position, accuracy_weight;
					double support = 0, accuracy = 0, factor = 0;
					number_of_rules = rules->GetNoRules();
					accuracy_weight = GetAccuracyWeight();


					Rule *rule;
					for(int k = 0; k < number_of_rules; k++)
					{
						rule = rules->GetRule(k);

						number_of_decision_values = rule->GetNoDecisionValues();
						if(number_of_decision_values == 1)
						{
							factor = rule->GetSupport(0);
						}
						else if(number_of_decision_values == 0)
							continue;
						else
						{
							support_position = 0;
							support = rule->GetSupport(0);
							for(int l = 1; l < number_of_decision_values; l++)
							{
								if(support < rule->GetSupport(l))
									support_position = l;
							}
							support = rule->GetSupport(support_position);
							accuracy = rule->GetAccuracy(support_position);
							factor = support * pow(accuracy, accuracy_weight);
						}
						number_of_attributes_in_rule = rule->GetNoConditionDescriptors();


//We are checking each feature in each reduct returned
						for(int j = 0; j < number_of_attributes_in_rule; j++)
						{
//We are incrementing the vector with scores each time a feature appears in a reducts
							vector_of_scores_for_classifier_d[rule->GetConditionAttribute(j)] = vector_of_scores_for_classifier_d[rule->GetConditionAttribute(j)] + factor;
						}

					}



					int index_in_main_vector;
					double value_to_be_added;



					for(int k = 0; k < projection_size; k++)
					{
						#pragma omp critical(update_score_table)
						{
//We are adding the results for a single classifier to the main vactor with results
//TO MOZNA CHYBA PRZENIESC POZA SECTION CRITICAL (2 PIERWSZE LINIE)
							index_in_main_vector = vector_of_scores_for_classifier_names[k];
							value_to_be_added = vector_of_scores_for_classifier_d[k];
							vector_of_scores_for_attributes[index_in_main_vector] = vector_of_scores_for_attributes[index_in_main_vector] + value_to_be_added;
						}
					}


				}

				//if(GetCriterion() == PUT HERE DIFFERENT CRITERION AND ADD NEW METHOD FOR EVALUATING FEATURES
				//+ ADD NEW VALUES IN reducerCriterions VECTOR


				delete my_reducts.Release();
			}
			//if(GetClassifiersType() == Keword:: PUT HERE DIFFERENT TYPE AND ADD NEW CLASSIFER TYPE EG. DECISION TREES
			//+ ADD NEW classifierTypeCiterions
			//+ ADD NEW IF TO CHECK MATCHIG CRITERIA AND CLASSIFIERTYPE

			delete subtable.Release();
			if_thread_is_finished[i] = true;

		}
		ThreadControl::RemoveThreadFromCount();
	} // End of parallel section -N

//We return the stdout  as it should be
  	std::cout.rdbuf(cout_sbuf);

  	for(int i = 0; i < total_number_of_attributes; i++) {
  		if(vector_of_occurences_of_attributes[i] == 0)
  			continue;

  		vector_of_normalized_scores_for_attributes[i] = vector_of_scores_for_attributes[i]/(double)vector_of_occurences_of_attributes[i];
  	}

	return vector_of_normalized_scores_for_attributes;

	//Koniec MCFSA
}





/*

Vector(double)
MCFeatureSelector::MCFSParallel(Handle<DecisionTable> table) const {

	// Start of MCFS
	//Getting Parameters
	int projections = GetProjections();

	String classifierName = GetClassifier();
	Id classifier_id = IdHolder::GetId(classifierName);
	Id ruleGenerator_id = IdHolder::GetId("RuleGenerator");

	String classifierParameterList;
	classifierParameterList = GetParametersForClassifier();

		

	bool masked = true;

	//For a correct map, the total number of attributes must be the unmasked number. -N
	int nAttributes = table->GetNoAttributes(false);
	//But we also need to know how many attributes are actually in play. -N
	int nMaskedAttributes = table->GetNoAttributes(masked);
	//The decision column can however respect the masking employed. -N
	int nDecision = table->GetDecisionAttribute(masked);
	//If projection size attribute is set to 0 square root of number of attributes is set as a projection size
	int projection_size;
	if(GetProjectionSize() == 0) {
		//We want to get the ceiling of the masked attributes here. -N
		projection_size = ceil(sqrt(nMaskedAttributes));
		//SetProjectionSize(ceil(sqrt(nMaskedAttributes)));
	}
	else {
		projection_size = GetProjectionSize();
	}

	//Easier, and faster. -N
	Vector(double) 	vNormalScore(nAttributes, 0);
	Vector(double) 	vTotalScore(nAttributes, 0);
	Vector(int)	vTotalOccurrence(nAttributes, 0);
		

	// save original sbuf
	//There was a problem because reducer prints messages on the screen. In parallel mode it makes no sense, so I redirected the output to empty device
	std::streambuf  *cout_sbuf = std::cout.rdbuf();
	std::ofstream   fout("/dev/null");

	// redirect 'cout' to a 'fout'
	std::cout.rdbuf(fout.rdbuf());
	std::ostream	out(cout_sbuf);

	out.rdbuf(cout_sbuf);

	//	bool if_thread_is_finished[projections];
	//	for(int i = 0; i < projections; i++) {
//		if_thread_is_finished[i] = false;
//	}

	Vector(int) vIndexTemplate;
	//We need to get all unmasked attributes, but only add the enabled ones to the random draw pool. -N
	//If we only iterate over enabled ones, we will simply get fewer attributes in the pool, but all the encountered
	//will still be included. -N
	for (int i = 0; i < nAttributes; ++i) {
		if (i != nDecision && table->IsAttributeEnabled(i)) {
			vIndexTemplate.push_back(i);
		}
	}
	Vector(unsigned int) vSeed;
	//Added in the whole parallel thing again. -N
	#ifdef _OPENMP
		ThreadControl::GetSplitSize();
//		cerr << c << endl;	
		for (int i = 0; i < projections; ++i) {
			vSeed.push_back(rand());
//			cerr << vSeed[i] << ", ";
		}	
	#endif

	#pragma omp parallel \
	shared(vSeed, table, vIndexTemplate, vTotalScore, vTotalOccurrence, nAttributes, masked, projections, nDecision, projection_size, out, classifier_id, ruleGenerator_id, classifierParameterList) \
	default(none)
	{
		ThreadControl::AddThreadToCount();
	
		//Created inside parallel now. -N
		Handle<Algorithm> classifierAlgorithm = ObjectManager::GetIdentifiedAlgorithm(classifier_id);
		Handle<Algorithm> ruleGeneratorAlgorithm = ObjectManager::GetIdentifiedAlgorithm(ruleGenerator_id);

		classifierAlgorithm->SetParameters(classifierParameterList);

		//Clone the main table for thread safe manipulation (just in case). -N
		Handle<DecisionTable> threadTable = dynamic_cast(DecisionTable *, table->Duplicate());
		//Or not... This should reduce the footprint for larger tables, but might cause
		//cache invalidations.  -N
		//Handle<DecisionTable> threadTable = table;

		//Moved inside parallel, since it is not used outside. -N
		Vector(int) vOccurrence(nAttributes, 0);
		//Moved inside parallel, since it is not used outside. This way it won't have to be copied into the space. -N
		Vector(double) vFeatureScore(nAttributes, 0.0);

		#pragma omp for schedule(dynamic)
		for (int i = 0; i < projections; i++) {


//			Vector(int) vIndex = SetTableProjection(threadTable, &vIndexTemplate, vSeed[omp_get_thread_num()], vOccurrence, projection_size);
		
			//Vector(int) vector_of_scores_for_classifier_names;

			//Faster, and requiring only a single vector as a template. -N
			Vector(int) vAttributeSample(vIndexTemplate.begin(), vIndexTemplate.end());


			random_unique(vAttributeSample.begin(), vAttributeSample.end(), projection_size, vSeed[i]);
			vAttributeSample.resize(projection_size);
			Vector(int) vIndex(vAttributeSample.begin(), vAttributeSample.end());
			//vector<int> vAttributeIndex(vAttributeSample.begin(), vAttributeSample.begin() + projection_size);
			for (int j = 0; j < projection_size; j++) {
				vOccurrence[vAttributeSample[j]]++;
//				Message::Error(String::Format(vAttributeSample[j]));
			}

			Vector(DecisionTable::Mask) vMask;
			//Moved the masking to internal function. -N
			SetProjectionMask(threadTable, &vAttributeSample, &vMask);
//			Handle<DecisionTable> dtProjection = CreateDecisionTableWithRandColumns(threadTable, vAttributeSample);
			String reducer = "REDUCER";
			if (GetClassifiersType() == reducer) {
	//Variable for returned value is initialized here
				Handle<Reducts> my_reducts;
				//The reduct computation. -N
				my_reducts = dynamic_cast(Reducts *, threadTable->Apply(*classifierAlgorithm));

				int number_of_reducts, number_of_attributes_in_reduct;
				number_of_reducts = my_reducts->GetNoReducts();
				if (number_of_reducts == 0) {
					//Swapped to continue. -N
					delete my_reducts.Release();
					//delete dtProjection.Release();
					continue;
				}

				Reduct *my_reduct;


	//Check criterion which we use for the selection of features
				String frequency = "FREQUENCY";
				String accuracy = "ACCURACY";
				if(GetCriterion() == frequency) {

					//Moved. We only need this with a frequency criterion. -N
					Vector(int) vFrequency(nAttributes, 0);

					//We are checking all reduct in list of reducts returned
					for(int k = 0; k < number_of_reducts; k++) {
						my_reduct = my_reducts->GetReduct(k);
						number_of_attributes_in_reduct = my_reduct->GetNoAttributes();
//						out << number_of_attributes_in_reduct << ", ";
	//We are checking each feature in each reduct returned
						for(int j = 0; j < number_of_attributes_in_reduct; j++) {
	//We are incrementing the vector with scores each time a feature appears in a reducts
							//out << threadTable->GetUnmaskedObject(my_reduct->GetAttribute(j)) << "; ";
							vFeatureScore[vIndex[my_reduct->GetAttribute(j)]]++;
						}

					}
//					for(int k = 0; k < projection_size; k++) {
	//We are adding the results for a single classifier to the main vactor with results
						//index_in_main_vector = vAttributeSample[k];
						//value_to_be_added = vector_of_scores_for_classifier_i[k];
//						vFeatureScore[vAttributeSample[k]] += (double)vFrequency[k];
//						vFeatureScore[vIndex[k]] += (double)vFrequency[vIndex[k]];
//						out << vAttributeSample[k] << "->" << vFeatureScore[vAttributeSample[k]] << ", ";
//					}

				}

	//Check criterion which we use for the selection of features
				else if(GetCriterion() == accuracy) {
	//We are checking all reduct in list of reducts returned
					Handle<Rules> rules;
	//On the subtable the selected algorithm is executed
					rules = dynamic_cast(Rules*, my_reducts->Apply(*ruleGeneratorAlgorithm));
					int number_of_rules, number_of_attributes_in_rule, number_of_decision_values, support_position, accuracy_weight;
					double support = 0, accuracy = 0, factor = 0;
					number_of_rules = rules->GetNoRules();
					accuracy_weight = GetAccuracyWeight();

					Vector(double) vAccuracyScore(nAttributes, 0);

					Rule *rule;
					for(int k = 0; k < number_of_rules; k++) {

						rule = rules->GetRule(k);
						number_of_decision_values = rule->GetNoDecisionValues();
						if(number_of_decision_values == 1) {
							factor = rule->GetSupport(0);
						}
						else if(number_of_decision_values == 0)
							continue;
						else {
							support_position = 0;
							support = rule->GetSupport(0);
							for(int l = 1; l < number_of_decision_values; l++) {
								if(support < rule->GetSupport(l))
									support_position = l;
							}
							support = rule->GetSupport(support_position);
							accuracy = rule->GetAccuracy(support_position);
							factor = support * pow(accuracy, accuracy_weight);
						}
						number_of_attributes_in_rule = rule->GetNoConditionDescriptors();

//We are checking each feature in each reduct returned
						for(int j = 0; j < number_of_attributes_in_rule; j++) {
//We are incrementing the vector with scores each time a feature appears in a reducts
							//out << "{" << k << "}  " << rule->GetConditionAttribute(j) << " -/> ";
							//for (int p = 0; p < vIndex.size();++p) {
							//	out << vIndex[p] << " ";
							//}
							//out << endl;
							//vAccuracyScore[threadTable->GetMaskedAttribute(vIndex[rule->GetConditionAttribute(j)])] += factor;
							vFeatureScore[vIndex[rule->GetConditionAttribute(j)]] += factor;
						}

					}
//					for(int k = 0; k < projection_size; k++) {
						//index_in_main_vector = vAttributeSample[k];
						//value_to_be_added = vector_of_scores_for_classifier_d[k];
//						vFeatureScore[vIndex[k]] += vAccuracyScore[vIndex[k]];
//					}


				}

			//if(GetCriterion() == PUT HERE DIFFERENT CRITERION AND ADD NEW METHOD FOR EVALUATING FEATURES
			//+ ADD NEW VALUES IN reducerCriterions VECTOR
				delete my_reducts.Release();
				//delete dtProjection.Release();
			}
			//if(GetClassifiersType() == Keword:: PUT HERE DIFFERENT TYPE AND ADD NEW CLASSIFER TYPE EG. DECISION TREES
			//+ ADD NEW classifierTypeCiterions
			//+ ADD NEW IF TO CHECK MATCHIG CRITERIA AND CLASSIFIERTYPE
			//delete subtable.Release();

//			if_thread_is_finished[i] = true;

		} // end for -N
	//Tally up the totals. -N
		for(int i = 0; i < vOccurrence.size(); i++) {
			//This should get rid of the masked attributes. -N
		  	if(vOccurrence[i] != 0) {
				#pragma omp critical
				{
					//Add up the feature score (by whatever method is used) and divide it by the
					//number of times the feature was used (standard normalization). the vector
					//vNormalScore is shared across all threads, so don't use it outside 
					//critical sections in the parallel region. -N
					vTotalScore[i] += vFeatureScore[i];
					vTotalOccurrence[i] += vOccurrence[i];
//	  				vNormalScore[i] += vFeatureScore[i]/(double)vOccurrence[i];
//					Message::Error(String::Format(vNormalScore[i]) + ", ");
				}
			}
		}
		
		ThreadControl::RemoveThreadFromCount();
	} //Parallel stop -N
//We return the stdout  as it should be
  	std::cout.rdbuf(cout_sbuf);
	for (int i = 0; i < vNormalScore.size(); ++i) {
		if (vTotalOccurrence[i] != 0) {
			vNormalScore[i] = vTotalScore[i] / (double) vTotalOccurrence[i];
		}
		//cout << vNormalScore[i] << ", ";
	}

	return vNormalScore;

	//Koniec MCFSA
}


*/




//-------------------------------------------------------------------
// Method........: Apply
// Author........: Marcin Kruczyk
// Date..........:
// Description...: Performs a feature selection based on Monte Carlo approach
//						Combined with Random Reducts approach
// Comments......:
// Revisions.....: 18/12/13 -N Added seed to the randomization.
//
//
//
//===================================================================

Structure *
MCFeatureSelector::Apply(Structure &structure) const {
	Message message;
//Variable needed for measuring the time of evaluation
	time_t tstart;
//Start measuring the time
	tstart = time(NULL);

// Check if input is of expected type.
	if (!IsApplicable(structure)) {
		Message::Error("Random Reducts -> The input structure is incompatible. Only a DecisionTable can be handled.");
		return 0;
	}

//Checks if parameters make sens
	if(!DataValidation())
		return 0;

//Creates structure of decision table
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);
	bool masked = true;

	if(GetProjectionSize() > table->GetNoAttributes(masked)) {
		Message::Error("Random Reducts -> The projection size is greater than the number of attributes.");
		return 0;
	}

//MCFS Function

	//Set the randomization seed. It is defaulted to 1 through the constructor. -N
	srand(GetSeed());

  	Vector(double) vector_of_normalized_scores_for_attributes;
  	String stars = "*************************************************";

  	cout << stars << endl;
  	cout << "Original dataset" << endl;
  	cout << "Starting analysis: " << GetTimeString() << endl;

  	vector_of_normalized_scores_for_attributes = MCFSParallel(table);
  	double sum = 0;
  	for(int i=0; i < vector_of_normalized_scores_for_attributes.size(); i++) {
  		sum = sum + vector_of_normalized_scores_for_attributes[i];
  	}

  	if(!sum) {
  		cout << "All the generated reducts are empty. You can increase number of Projections to try again or decrease the approximation value." << endl;
  		cout << "If it does not help it means that there are no features in the dataset correlated with the decision." << endl;
  		return NULL;
  	}

  	cout << "Analysis complete: " << GetTimeString() << endl;
  	cout << stars << endl;

//Redirect cerr bufor to cout bufor, because I want them to be displayed in accurate order
  	streambuf *cerr_sbuf;
  	cerr_sbuf = std::cerr.rdbuf(std::cout.rdbuf());

  	int number_of_background_loops = GetNumberOfBackgroundLoops();

//  	cout << "Number of background loops: " << number_of_background_loops << endl;

	int number_of_important_features;
	Vector(double) vector_of_scores;



	if(number_of_background_loops > 0) {
		vector_of_scores = PermutationTest(table, vector_of_normalized_scores_for_attributes, number_of_background_loops);

		double cut_off;
		cut_off = GetSignificanceCutOff();
		number_of_important_features = GenerateNewNumberOfFeatures(vector_of_scores, cut_off);
	}
	else {
		vector_of_scores = vector_of_normalized_scores_for_attributes;
		if(GetFeaturesNumber()) {
			number_of_important_features = GetFeaturesNumber();

		}
		else if(GetFeaturesFraction()) {
			double procentage_of_important_features;
			procentage_of_important_features = GetFeaturesFraction();
			number_of_important_features = (int)(round(vector_of_normalized_scores_for_attributes.size() * procentage_of_important_features));
		}
		else {
			number_of_important_features = GenerateNewNumberOfFeatures(vector_of_normalized_scores_for_attributes, table->GetDecisionAttribute(masked));
		}

	}

	if(GetSaveFeatureSelection()) {
		//14/01/14 -N Updated to reflect the new schema. Now sorts by actual score, not inverted p-standard.
		if(!SaveScores(table, vector_of_normalized_scores_for_attributes, vector_of_scores, number_of_important_features)) {
			Message::Error("MCFeatureSelector can not write the results to a file.");
			return NULL;
		}
	}
	String input_for_ManualFC;
	input_for_ManualFC = SelectImportantFeatures(vector_of_scores, number_of_important_features, table->GetDecisionAttribute(masked));
  	Id manualFeatureSelector_id = IdHolder::GetId("ManualFeatureSelector");

  	if (manualFeatureSelector_id == Undefined::Id()) {
    		Message::Error("ManualFeatureSelector is not installed.");
    		std::cerr.rdbuf(cerr_sbuf);
    		return 0;
  	}

  	Handle<Algorithm> manualFeatureSelectorAlgorithm = ObjectManager::GetIdentifiedAlgorithm(manualFeatureSelector_id);

  	String ManualFCParameterList;
  	ManualFCParameterList = GetParametersForManualFeatureSelector(input_for_ManualFC);
  	// Set parameters.
  	if (!manualFeatureSelectorAlgorithm->SetParameters(ManualFCParameterList)) {
  		std::cerr.rdbuf(cerr_sbuf);
  		return 0;
  	}

	Handle<Structure> featuretable;


	Handle<Structure> current; // Current structure in pipeline.
	current = &structure;

	featuretable = dynamic_cast (DecisionTable*, current->Apply(*manualFeatureSelectorAlgorithm));

//	delete table.Release();
//	delete current.Release();
	
	std::cerr.rdbuf(cerr_sbuf);

  	time_t tend;
  	tend = time(NULL);

	cout << (long int)(tend - tstart) << " seconds" << endl;

	return featuretable.Release();

}


MCFeatureSelector *
MCFeatureSelector::Clone() {
	return new MCFeatureSelector;
}


