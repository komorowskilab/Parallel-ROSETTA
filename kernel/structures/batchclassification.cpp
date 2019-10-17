//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/batchclassification.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

//-------------------------------------------------------------------
// Methods for class BatchClassification.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BatchClassification::BatchClassification(const BatchClassification &in) : AnnotatedStructure(in) {
	attribute_              = in.attribute_;
	matrix_                 = in.matrix_;
	roc_class_              = in.roc_class_;
	roc_area_               = in.roc_area_;
	roc_stderr_             = in.roc_stderr_;
	roc_threshold_nw_       = in.roc_threshold_nw_;
	roc_threshold_accuracy_ = in.roc_threshold_accuracy_;
	roc_targets_            = in.roc_targets_;
	roc_outputs_            = in.roc_outputs_;
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BatchClassification::BatchClassification() {
	attribute_              = Undefined::Integer();
	roc_class_              = Undefined::Integer();
	roc_area_               = Undefined::Float();
	roc_stderr_             = Undefined::Float();
	roc_threshold_nw_       = Undefined::Float();
	roc_threshold_accuracy_ = Undefined::Float();
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BatchClassification::~BatchClassification() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(BatchClassification, BATCHCLASSIFICATION, AnnotatedStructure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: A temporary hack to ensure backwards compatibility.
//                 Rewrite method after some time has passed.
//
//                 ROC target/output details are currently non-persistent.
// Revisions.....:
//===================================================================

bool
BatchClassification::Load(ifstream &stream) {

	if (!AnnotatedStructure::Load(stream))
		return false;

	bool has_extra_data;

	// Load confusion matrix.
	if (!matrix_.Load(stream, has_extra_data, attribute_, roc_class_, roc_area_, roc_stderr_, roc_threshold_nw_, roc_threshold_accuracy_))
		return false;

	// Load extra data, if not already done.
	if (!has_extra_data) {
		if (!IOKit::Load(stream, attribute_))
			return false;
		if (!IOKit::Load(stream, roc_class_))
			return false;
		if (!IOKit::Load(stream, roc_area_))
			return false;
		if (!IOKit::Load(stream, roc_stderr_))
			return false;
		if (!IOKit::Load(stream, roc_threshold_nw_))
			return false;
		if (!IOKit::Load(stream, roc_threshold_accuracy_))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: ROC target/output details are currently non-persistent.
// Revisions.....:
//===================================================================

bool
BatchClassification::Save(ofstream &stream) const {

	// Save stuff higher up.
	if (!AnnotatedStructure::Save(stream))
		return false;

	// Save confusion matrix.
	if (!matrix_.Save(stream))
		return false;

	// Save attribute index.
	if (!IOKit::Save(stream, attribute_))
		return false;

	// Save ROC stuff.
	if (!IOKit::Save(stream, roc_class_))
		return false;
	if (!IOKit::Save(stream, roc_area_))
		return false;
	if (!IOKit::Save(stream, roc_stderr_))
		return false;
	if (!IOKit::Save(stream, roc_threshold_nw_))
		return false;
	if (!IOKit::Save(stream, roc_threshold_accuracy_))
		return false;

	if (!IOKit::Save(stream, '\n'))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
BatchClassification::Duplicate() const {
	return new BatchClassification(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
BatchClassification::Clear() {

	// Clear local stuff.
	matrix_.Clear();
	attribute_              = Undefined::Integer();
	roc_class_              = Undefined::Integer();
	roc_area_               = Undefined::Float();
	roc_stderr_             = Undefined::Float();
	roc_threshold_nw_       = Undefined::Float();
	roc_threshold_accuracy_ = Undefined::Float();

	roc_targets_.erase(roc_targets_.begin(), roc_targets_.end());
	roc_outputs_.erase(roc_outputs_.begin(), roc_outputs_.end());

	// Do stuff higher up.
	AnnotatedStructure::Clear();

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: =
// Author........: Aleksander šhrn
// Date..........:
// Description...: Assignment operator.
// Comments......:
// Revisions.....:
//===================================================================

BatchClassification &
BatchClassification::operator=(const BatchClassification &in) {

	// Protect against self-assignment.
	if (this == &in)
		return *this;

	attribute_              = in.attribute_;
	matrix_                 = in.matrix_;
	roc_class_              = in.roc_class_;
	roc_area_               = in.roc_area_;
	roc_stderr_             = in.roc_stderr_;
	roc_threshold_nw_       = in.roc_threshold_nw_;
	roc_threshold_accuracy_ = in.roc_threshold_accuracy_;
	roc_targets_            = in.roc_targets_;
	roc_outputs_            = in.roc_outputs_;

	// This should really be done on a higher, more general level...
	if (in.IsAnnotated())
		SetAnnotation(dynamic_cast(Annotation *, in.GetAnnotation()->Duplicate()));
	else
		SetAnnotation(NULL);

  return *this;

}

//-------------------------------------------------------------------
// Operator......: ==
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassification::operator==(const BatchClassification &in) const {

  // Are the two matrices physically equal?
  if (&in == this)
    return true;

	// Unequal attribute index?
	if (attribute_ != in.attribute_)
		return false;

	// Unequal confusion matrices?
	if (matrix_ != in.matrix_)
		return false;

	// Unequal ROC stuff?
	Message::Debug("Comparison of ROC information not implemented yet.");

  return true;

}

//-------------------------------------------------------------------
// Operator......: !=
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
BatchClassification::operator!=(const BatchClassification &in) const {
  return !(*this == in);
}


 
