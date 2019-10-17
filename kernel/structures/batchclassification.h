//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BATCHCLASSIFICATION_H__
#define __BATCHCLASSIFICATION_H__

#include <copyright.h>

#include <kernel/structures/annotatedstructure.h>
#include <kernel/structures/confusionmatrix.h>
#include <kernel/structures/classification.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: BatchClassification
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: The result structure of a batch classification task.
//                 Contains a confusion matrix plus other pertinent
//                 information.
//
// Revisions.....: AÛ 981218: Added non-persistent ROC target/output
//                            details.
//===================================================================

class BatchClassification : public AnnotatedStructure {

// Torgeir 14.2.2002 ------------------------------
protected:

	//- For internal use...............................................
	typedef Pair(int, Handle<Classification>) ICPair;
// Torgeir 14.2.2002 ------------------------------

protected:

  //- Implementation structures......................................
	int                    attribute_;              // Index of attribute the classification is for.
	ConfusionMatrix        matrix_;                 // Confusion matrix.
	int                    roc_class_;              // The class ROC is generated for, if relevant.
	float                  roc_area_;               // The area under the ROC curve, if relevant.
	float                  roc_stderr_;             // The estimated std. error for the area under the ROC curve, if relevant.
	float                  roc_threshold_nw_;       // The threshold used for most NW ROC point, if relevant.
	float                  roc_threshold_accuracy_; // The threshold used to maximize accuracy, if relevant.
	Vector(int)            roc_targets_;            // Detailed data (actual targets) used to generate ROC data, if relevant. Binarized wrt. the specified ROC class. NOT PERSISTENT!
	Vector(float)          roc_outputs_;            // Detailed data (model outputs) used to generate ROC data, if relevant. Binarized wrt. the specified ROC class. NOT PERSISTENT!
	// Torgeir 27.02.01 ------------------------------
	float				   roc_accuracy_nw_;
	float				   roc_NW_tp_;
	float				   roc_NW_fp_;
	float				   roc_NW_tn_;
	float				   roc_NW_fn_;
	float				   roc_sensitivity_nw_;
	float				   roc_specificity_nw_;
	Vector(ICPair) classifications_;
	Vector(Vector(float)) roc_data_;
	// Torgeir 27.02.01 ------------------------------

protected:

	//- Constructors...................................................
  BatchClassification(const BatchClassification &in);

public:

  //- Type definitions..............................................
	typedef Vector(Handle<BatchClassification>) Handles;

public:

  //- Constructors/destructor........................................
	BatchClassification();
  virtual ~BatchClassification();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent.............................
  virtual bool           Load(ifstream &stream);
	virtual bool           Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure     *Duplicate() const;
  virtual void           Clear();

  //- Operators......................................................
	BatchClassification   &operator=(const BatchClassification &in);
  bool                   operator==(const BatchClassification &in) const;
  bool                   operator!=(const BatchClassification &in) const;

  //- Local methods..................................................
	int                    GetDecisionAttribute() const {return attribute_;}
	bool                   SetDecisionAttribute(int attribute) {attribute_ = attribute; return true;}

	const ConfusionMatrix &GetConfusionMatrix() const {return matrix_;}
	ConfusionMatrix       &GetConfusionMatrix() {return matrix_;}
	bool                   SetConfusionMatrix(const ConfusionMatrix &matrix) {matrix_ = matrix; return true;}

	int                    GetROCClass() const {return roc_class_;}
	bool                   SetROCClass(int roc_class) {roc_class_ = roc_class; return true;}

	float                  GetROCArea() const {return roc_area_;}
	bool                   SetROCArea(float roc_area) {roc_area_ = roc_area; return true;}

	float                  GetROCStandardError() const {return roc_stderr_;}
	bool                   SetROCStandardError(float roc_stderr) {roc_stderr_ = roc_stderr; return true;}

	float                  GetROCNWThreshold() const {return roc_threshold_nw_;}
	bool                   SetROCNWThreshold(float roc_threshold) {roc_threshold_nw_ = roc_threshold; return true;}

	float                  GetROCAccuracyThreshold() const {return roc_threshold_accuracy_;}
	bool                   SetROCAccuracyThreshold(float roc_threshold) {roc_threshold_accuracy_ = roc_threshold; return true;}

	const Vector(int)     &GetROCTargetVector() const {return roc_targets_;}
	bool                   SetROCTargetVector(const Vector(int) &roc_targets) {roc_targets_ = roc_targets; return true;}

	const Vector(float)   &GetROCOutputVector() const {return roc_outputs_;}
	bool                   SetROCOutputVector(const Vector(float) &roc_outputs) {roc_outputs_ = roc_outputs; return true;}
	
	// Torgeir 27.02.01 ------------------------------
	float                  GetROCNWAccuracy(float &sensitivity, float &specificity) const {sensitivity = roc_sensitivity_nw_; specificity = roc_specificity_nw_; return roc_accuracy_nw_;}
	bool                   SetROCNWAccuracy(float accuracy, float sensitivity, float specificity) {roc_accuracy_nw_ = accuracy; roc_sensitivity_nw_ = sensitivity; roc_specificity_nw_ = specificity; return true;}
	bool                   GetROCNWTPFPTNFN(float &tp, float &fp, float &tn, float &fn) const {tp = roc_NW_tp_; fp = roc_NW_fp_; tn = roc_NW_tn_; fn = roc_NW_fn_; return true; }
	bool                   SetROCNWTPFPTNFN(float tp, float fp, float tn, float fn) {roc_NW_tp_ = tp; roc_NW_fp_ = fp; roc_NW_tn_ = tn; roc_NW_fn_ = fn; return true;}

	bool                   SetClassificationDetails(Vector(ICPair) classifications) {classifications_ = classifications; return true;};
	Vector(ICPair)				 GetClassificationDetails() {
		int test = classifications_.size();
		return classifications_;
	};
	bool                   SetROCData(Vector(Vector(float)) &roc_data) { 
														roc_data_ = roc_data; 
														return true;
	}
	Vector(Vector(float))  GetROCData() { return roc_data_; }
	// Torgeir 27.02.01 ------------------------------

};

#endif 
