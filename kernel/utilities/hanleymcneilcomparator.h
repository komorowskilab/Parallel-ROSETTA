//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __HANLEYMCNEILCOMPARATOR_H__
#define __HANLEYMCNEILCOMPARATOR_H__

#include <copyright.h>

#include <kernel/utilities/binaryoutcomecomparator.h>

//-------------------------------------------------------------------
// Class.........: HanleyMcNeilComparator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Implements Hanley-McNeil's method for statistical
//                 hypothesis testing of the AUC from two correlated
//                 ROC curves derived from the same set of cases.
// Comments......:
// Revisions.....: AÛ 990404: Added c-index option.
//===================================================================

class HanleyMcNeilComparator : public BinaryOutcomeComparator {
public:

	//- Enumerated types...............................................
	enum Correlation {CORRELATION_PEARSON, CORRELATION_KENDALL};

	//- Textual representations........................................
	static String GetString(Correlation correlation);

protected:

	//- Member variables...............................................
	float         auc1_;    // Area under ROC curve #1.
	float         auc2_;    // Area under ROC curve #2
	float         se1_;     // Standard error of area under ROC curve #1.
	float         se2_;     // Standard error of area under ROC curve #2.
	float         corr0s_;  // Correlation between objects belonging to class 0.
	float         corr1s_;  // Correlation between objects belonging to class 1.
	float         r_;       // Computed correlation modifier.

protected:

	//- Helper methods.................................................
	bool          ComputeROCQuantities(const Vector(int) &targets, const Vector(float) &outputs, float &auc, float &se, bool cindex) const;
	bool          ComputeCorrelations(Correlation correlation, const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, float &corr0s, float &corr1s) const;
	bool          ComputeRValue(float auc1, float auc2, float corr0s, float corr1s, float &r) const;

public:

	//- Constructors/destructor........................................
	HanleyMcNeilComparator();
	HanleyMcNeilComparator(const Vector(String) &filenames, const Vector(bool) &swap, Correlation correlation, bool cindex);
	virtual ~HanleyMcNeilComparator();

	//- New methods....................................................
	bool          Create(const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, Correlation correlation, bool copy = true, bool cindex = false);

	float         GetAUC1() const {return auc1_;}
	float         GetAUC2() const {return auc2_;}
	float         GetSE1() const {return se1_;}
	float         GetSE2() const {return se2_;}
	float         GetCorrelation0s() const {return corr0s_;}
	float         GetCorrelation1s() const {return corr1s_;}
	float         GetR() const {return r_;}
	float         GetZ() const;
	float         GetP() const;

};

#endif 
