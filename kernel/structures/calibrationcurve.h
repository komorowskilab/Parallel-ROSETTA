//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CALIBRATIONCURVE_H__
#define __CALIBRATIONCURVE_H__

#include <copyright.h>

#include <kernel/structures/binaryoutcomecurve.h>

#include <kernel/basic/pair.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Classification;

//-------------------------------------------------------------------
// Class.........: CalibrationCurve
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents a calibration curve, constructed from
//                 a number of (target, output) pairs.
// Comments......:
// Revisions.....:
//===================================================================

class CalibrationCurve : public BinaryOutcomeCurve {
public:

  //- Type definitions..............................................
	typedef Pair(int, Handle<Classification>) ICPair;

protected:

  //- Implementation structures......................................
	int                       decision_attribute_; // Attribute curve is relevant for.
	int                       decision_class_;     // Decision class curve is for.
	Vector(int)               targets_;            // Actual (target) classes, binarized wrt the decision class.
	Vector(float)             outputs_;            // Model outputs, binarized wrt the decision class.
	Vector(int)               indices_;            // Object indices.
	Vector(float)             targets_mean_;       // Actual (target) classes, binarized and summed and averaged per group.
	Vector(float)             outputs_mean_;       // Model outputs, binarized and summed and averaged per group.
	Vector(int)               indices_summed_;     // Object indices, summed per group.
	Vector(int)               group_sizes_;        // Group sizes, so that external averaging is made possible.

protected:

  //- Constructors...................................................
  CalibrationCurve(const CalibrationCurve &in);

public:

  //- Constructors/destructor........................................
  CalibrationCurve();
  virtual ~CalibrationCurve();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent..............................
  virtual bool              Load(ifstream &stream);
  virtual bool              Save(ofstream &stream) const;

  //- Methods inherited from Structure...............................
  virtual Structure        *Duplicate() const;
  virtual void              Clear();

  //- Methods inherited from BinaryOutcomeCurve......................
	virtual bool              Create(const Vector(int) &targets, const Vector(float) &outputs);

  //- Attribute/value methods........................................
	int                       GetDecisionAttribute() const {return decision_attribute_;}
	bool                      SetDecisionAttribute(int decision_attribute) {decision_attribute_ = decision_attribute; return true;}

	int                       GetDecisionClass() const {return decision_class_;}
	bool                      SetDecisionClass(int decision_class) {decision_class_ = decision_class; return true;}

  //- Creation methods...............................................
	bool                      Create(const Vector(ICPair) &pairs, int decision_class, int no_groups = 10, bool progress = true);
	bool                      Create(const Vector(int) &targets, const Vector(float) &outputs, int decision_class, int no_groups = 10, bool progress = true);
	bool                      Create(const Vector(int) &targets, const Vector(float) &outputs, const Vector(int) &indices, int decision_class, int no_groups = 10, bool progress = true);

  //- Curve data methods.............................................
	int                       GetNoPairs() const {return targets_.size();}

	int                       GetTarget(int pair_no) const {return targets_[pair_no];}
	float                     GetOutput(int pair_no) const {return outputs_[pair_no];}
	int                       GetIndex(int pair_no) const  {return indices_[pair_no];}

	const Vector(int)        &GetTargets() const {return targets_;}
	const Vector(float)      &GetOutputs() const {return outputs_;}
	const Vector(int)        &GetIndices() const {return indices_;}

	int                       GetNoGroups() const {return targets_mean_.size();}

	float                     GetMeanTarget(int group_no) const {return targets_mean_[group_no];}
	float                     GetMeanOutput(int group_no) const {return outputs_mean_[group_no];}
	int                       GetIndexSum(int group_no) const {return indices_summed_[group_no];}
	int                       GetGroupSize(int group_no) const {return group_sizes_[group_no];}

  //- Curve data property methods....................................
	float                     ComputeMeanTarget() const;
	float                     ComputeMeanOutput() const;

	float                     ComputeBrierScore() const;

	float                     ComputeBias() const;
	float                     ComputeSlope(float *mean_0s = NULL, float *mean_1s = NULL) const;
	float                     ComputeScatter(float *variance_0s = NULL, float *variance_1s = NULL) const;

	bool                      ComputeLinearRegression(float *slope = NULL, float *intercept = NULL) const;

  //- New I/O methods................................................
  bool                      Save(ofstream &stream, Id id, const String &parameters) const;
  bool                      Save(const String &filename, Id id, const String &parameters) const;

};

#endif 
