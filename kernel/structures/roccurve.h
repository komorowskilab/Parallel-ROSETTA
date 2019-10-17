//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __ROCCURVE_H__
#define __ROCCURVE_H__

#include <copyright.h>

#include <kernel/structures/binaryoutcomecurve.h>
#include <kernel/structures/classification.h>

#include <kernel/basic/pair.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: ROCCurve
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Represents a ROC curve.
// Comments......:
// Revisions.....:
//===================================================================

class ROCCurve : public BinaryOutcomeCurve {
public:

  //- Type definitions..............................................
	typedef Pair(int, Handle<Classification>) ICPair;

private:

	//- STL helper....................................................
	struct FloatVectorCompareXY {
		bool operator () (const Vector(float) &a, const Vector(float) &b) const {
			if (a[0] != b[0])
				return a[0] < b[0]; // Compare 1 - specificity.
			else
				return a[1] < b[1]; // Compare sensitivity (1 - specificity values are equal).
		}
	};

protected:

  //- Implementation structures......................................
	Vector(Vector(float)) data_;               // Points on the ROC curve with additional data.
	int                   no_0s_;              // Number of true negatives (binarized), for internal use.
	int                   no_1s_;              // Number of true positives (binarized), for internal use.
	float                 area_;               // Area under ROC curve, cached.
	float                 stderr_;             // Standard error of area estimate, cached.
	int                   decision_attribute_; // Attribute ROC curve is relevant for.
	int                   decision_class_;     // Decision class ROC curve is for.
	// Torgeir 07.06.01 -------------------------------
	float									weight_;						 // Weight on false positives
	// Torgeir 07.06.01 -------------------------------

protected:

  //- Constructors...................................................
  ROCCurve(const ROCCurve &in);

  //- Local methods..................................................
	bool                  ComputeArea();
	bool                  ComputeStandardError();

public:

  //- Constructors/destructor........................................
  ROCCurve();
  virtual ~ROCCurve();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent..............................
  virtual bool          Load(ifstream &stream);
  virtual bool          Save(ofstream &stream) const;

  //- Methods inherited from Structure...............................
  virtual Structure    *Duplicate() const;
  virtual void          Clear();

  //- Methods inherited from BinaryOutcomeCurve......................
	virtual bool          Create(const Vector(int) &targets, const Vector(float) &outputs);

  //- Attribute/value methods........................................
	int                   GetDecisionAttribute() const {return decision_attribute_;}
	bool                  SetDecisionAttribute(int decision_attribute) {decision_attribute_ = decision_attribute; return true;}

	int                   GetDecisionClass() const {return decision_class_;}
	bool                  SetDecisionClass(int decision_class) {decision_class_ = decision_class; return true;}

  //- Creation methods...............................................
	bool                  Create(const Vector(ICPair) &pairs, int decision_class, int resolution = 251, bool progress = true, Vector(int) *targets = NULL, Vector(float) *outputs = NULL, bool cindex = false);
	bool                  Create(const Vector(int) &targets, const Vector(float) &outputs, int resolution = 251, bool progress = true, bool cindex = false);

  //- Curve data methods.............................................
	int                   GetNoPoints() const {return data_.size();}

	float                 GetSpecificity(int point_no) const {return 1 - data_[point_no][0];}
	float                 GetSensitivity(int point_no) const {return data_[point_no][1];}
	float                 GetPPV(int point_no) const {return data_[point_no][2];}
	float                 GetNPV(int point_no) const {return data_[point_no][3];}
	float                 GetAccuracy(int point_no) const {return data_[point_no][4];}
	float                 GetThreshold(int point_no) const {return data_[point_no][5];}

  //- Curve property methods.........................................
	float                 GetArea() const {return area_;}
	float                 GetStandardError() const {return stderr_;}

	float                 GetNWThreshold() const;
	float                 GetAccuracyThreshold() const;

  //- New I/O methods................................................
  bool                  Save(ofstream &stream, Id id, const String &parameters) const;
  bool                  Save(const String &filename, Id id, const String &parameters) const;

	// Torgeir 27.02.01 -------------------------------
	float			GetNWAccuracy(float &sensitivity, float &specificity) const {
							for (int t = 0; t < GetNoPoints(); t++) {
								if (GetThreshold(t) == GetNWThreshold()) {
									sensitivity = GetSensitivity(t);
									specificity = GetSpecificity(t);
									return GetAccuracy(t);
								}
							}
							return false;
	}

	bool			GetTPFPTNFN(float &tp, float &fp, float &tn, float &fn) const {							
							float sensitivity,specificity;
							if (GetNWAccuracy(sensitivity, specificity)) {
								tp = sensitivity*no_1s_;
								fp = (1-specificity)*no_0s_;
								tn = no_0s_-fp;
								fn = no_1s_-tp;
								return true;
							}
							return false;
	}

	bool			SetWeight(float weight) {weight_ = weight; return true;}
	// Torgeir 27.02.01 -------------------------------
};

#endif 
