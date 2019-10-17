/*
 * mcfs.h
 *
 *  Created on: Feb 23, 2011
 *      Author: kruczyk
 */

#ifndef __MCFeatureSelector_H_
#define __MCFeatureSelector_H_



//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================


#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: MCFeatureSelector
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs a manual feature selection
// Revisions.....:
//===================================================================

class MCFeatureSelector : public FeatureSelector {
protected:

	// Parameters......................................................
	int		number_of_important_fetures_;
	float		fraction_of_important_features_;
	int		projections_;
	int		projection_size_;
	String		classifier_name_;
	String		classifier_discernibility_;
	bool		classifier_modulo_decision_;
	bool		classifier_approximate_;
	bool		classifier_BRT_;
	String		classifier_selection_;
	float		classifier_fraction_;
	String	   	classifiers_type_;
	String		criterion_;
	int		number_of_background_loops_;
	float		significance_cut_off_;
	bool		test_normalization_;
	int		accuracy_weight_;
	unsigned int	seed_;


	Vector(String)	boolNames_;
	Vector(bool)	boolValues_;
	Vector(String)	floatNames_;
	Vector(float)	floatValues_;
	Vector(String)	integerNames_;
	Vector(int)		integerValues_;
	Vector(String)	stringNames_;
	Vector(String)	stringValues_;
	Vector(String)  reducerCriterions_;




public:

	//- Constructors/destructor........................................
	MCFeatureSelector();
	virtual ~MCFeatureSelector();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

		//- Methods inherited from Algorithm...............................
		virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);

	virtual Structure *Apply(Structure &structure) const;

	//- Local parameter methods........................................
	String			   GetParametersForManualFeatureSelector(String attribute) const;
	String			   GetParametersForClassifier() const;

	bool			   SetFeaturesNumber(const int number_of_important_fetures) {number_of_important_fetures_ = number_of_important_fetures; return true;}
	bool			   SetFeaturesFraction(const double fraction_of_important_features) {fraction_of_important_features_ = fraction_of_important_features; return true;}
	bool			   SetSignificanceCutOff(const double significance_cut_off) {significance_cut_off_ = significance_cut_off; return true;}
	bool			   SetProjections(const int projections) {projections_ = projections; return true;}
	bool			   SetProjectionSize(const int projection_size) {projection_size_ = projection_size; return true;}
	bool			   SetClassifier(String classifier_name) {classifier_name.ToUppercase(); classifier_name_ = classifier_name; return true;}
	bool			   SetClassifierDiscernibility(const String &classifier_discernibility) {classifier_discernibility_ = classifier_discernibility; return true;}
	bool			   SetClassifierModuloDecision(const bool classifier_modulo_decision) {classifier_modulo_decision_ = classifier_modulo_decision; return true;}
	bool			   SetClassifierApproximate(const bool classifier_approximate) {classifier_approximate_ = classifier_approximate; return true;}
	bool			   SetClassifierFraction(const double classifier_fraction) {classifier_fraction_ = classifier_fraction; return true;}
	bool			   SetClassifierBRT(const bool classifier_BRT) {classifier_BRT_ = classifier_BRT; return true;}
	bool			   SetClassifierSelection(const String &classifier_selection) {classifier_selection_ = classifier_selection; return true;}
	bool			   SetClassifiersType(String classifiers_type) {classifiers_type.ToUppercase(); classifiers_type_ = classifiers_type; return true;}
	bool			   SetCriterion(String criterion) {criterion.ToUppercase(); criterion_ = criterion; return true;}
	bool			   SetNumberOfBackgroundLoops(const int number_of_background_loops) {number_of_background_loops_ = number_of_background_loops; return true;}
	bool			   SetTestNormalization(const bool test_normalization) {test_normalization_ = test_normalization; return true;}
	bool			   SetAccuracyWeight(const int accuracy_weight) {accuracy_weight_ = accuracy_weight; return true;}

	bool			   SetBoolNameAndValue(const String &boolName, const bool boolValue);
	bool			   SetFloatNameAndValue(const String &floatName, const float floatValue);
	bool			   SetIntegerNameAndValue(const String &integerName, const int integerValue);
	bool			   SetStringNameAndValue(const String &stringName, const String &stringValue);
	bool			   InitializeCriterions();
	//18/12/13 -N Added Seed functions for reproducibility. Keep in mind that threading may
	// cause the bit sequences to be applied out of order.
	bool		SetSeed(const int value) {seed_ = (unsigned int) value; return true;}
	unsigned int	GetSeed() const {return seed_;}

	const int      	   GetFeaturesNumber() const {return number_of_important_fetures_;}
	const double       GetFeaturesFraction() const {return fraction_of_important_features_;}
	const double       GetSignificanceCutOff() const {return significance_cut_off_;}
	const int      	   GetProjections() const {return projections_;}
	const int      	   GetProjectionSize() const {return projection_size_;}
	const String      &GetClassifier() const {return classifier_name_;}
	const String      &GetClassifierDiscernibility() const {return classifier_discernibility_;}
	const String      &GetClassifierSelection() const {return classifier_selection_;}
	const bool         GetClassifierModuloDecision() const {return classifier_modulo_decision_;}
	const bool         GetClassifierApproximate() const {return classifier_approximate_;}
	const bool         GetClassifierBRT() const {return classifier_BRT_;}
	const double       GetClassifierFraction() const {return classifier_fraction_;}
	String      	GetClassifiersType() const {return classifiers_type_;}
	const String      &GetCriterion() const {return criterion_;}
	const int      	   GetNumberOfBackgroundLoops() const {return number_of_background_loops_;}
	const bool		   GetTestNormalization() const {return test_normalization_;}
	const int      	   GetAccuracyWeight() const {return accuracy_weight_;}

	String	   			GetParameterName(int index, String const &parameterType) const;
	bool				GetBoolParameterValue(int index) const {return boolValues_[index];}
	float				GetFloatParameterValue(int index) const {return floatValues_[index];}
	int					GetIntegerParameterValue(int index) const {return integerValues_[index];}
	String				GetStringParameterValue(int index) const {return stringValues_[index];}
	int					GetSizeOfParameterVector(const String &parameterType) const;
	int					GetIndexOfParameter(String &parameterName, const String &parameterType) const;


	//- Abstractions. -N
	bool		SetProjectionMask(Handle<DecisionTable> dtTable, Vector(int) *vSample, Vector(DecisionTable::Mask)* vMask) const;
	Vector(int)	SetTableProjection(Handle<DecisionTable>, Vector(int)*, unsigned int&, Vector(int)&, const int) const;


	//- Local evaluation methods........................................
	DecisionTable	  *CreateDecisionTableWithRandColumns(Handle<DecisionTable> mainTable, Vector(int) &vector_of_scores_for_classifier) const;
	String			   SelectImportantFeatures(Vector(double) &vector_of_scores_for_attributes, int number_of_important_features, int decision_column) const;
	int				   GenerateNewNumberOfFeatures(Vector(double) vector_of_normalized_scores_for_attributes, int decision_column) const;
	int				   GenerateNewNumberOfFeatures(Vector(double) vector_of_scores, double cut_off_pvalue) const;
	int			   	   SelectPositionOfMaxInVector(Vector(double) &vector_of_scores_for_attributes, Vector(bool) &mask) const;
	bool			   IsCriterionApplicable() const;
	bool			   DataValidation() const;
	Vector(double)	   MCFSParallel(Handle<DecisionTable> table) const;
	Vector(double)	   GeneratePValues(Vector(Vector(double)) &background_vectors, Vector(double) vector_of_normalized_scores_for_attributes) const;
	Vector(double)	   PermutationTest(Handle<DecisionTable>& table, Vector(double) vector_of_normalized_scores_for_attributes, int number_of_background_loops) const;
	Vector(int)		   EreaseValueFromVector(Vector(int) &input_vector, int index) const;
	double			   MeanValue(Vector(double) &scores) const;
//	bool			   SaveScores(Handle<DecisionTable>& table, Vector(double) &right_tails, Vector(double) &vector_of_scores_for_attributes) const;
	//14/01/14 -N Updated to reflect new schema.
	bool			   SaveScores(Handle<DecisionTable>& dtTable, Vector(double) &vFeatureScore, Vector(double) &vector_of_scores, int number_of_important_features) const;
//	Vector(int)		   SortOutput(Vector(double) &right_tails, Vector(double) &vector_of_scores_for_attributes, int decision_column) const;
	Vector(int)		   SortOutput(Vector(double) &vector_of_scores, int decision_column) const;
	String 			   GetTimeString() const;
	Vector(double)	   VectorNormalization(Vector(double) vector_of_normalized_scores_for_attributes) const;
	Vector(Vector(double))	BackgroundVectorsNormalization(Vector(Vector(double)) background_vectors) const;
	virtual MCFeatureSelector *Clone();
};

#endif
