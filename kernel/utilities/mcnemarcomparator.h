//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MCNEMARCOMPARATOR_H__
#define __MCNEMARCOMPARATOR_H__

#include <copyright.h>

#include <kernel/utilities/binaryoutcomecomparator.h>

//-------------------------------------------------------------------
// Class.........: McNemarComparator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Implements McNemar's method for statistical
//                 hypothesis testing of accuracy.
// Comments......:
// Revisions.....:
//===================================================================

class McNemarComparator : public BinaryOutcomeComparator {
protected:

	//- Member variables...............................................
	int          n_a_; // Number of objects that classifier #1 and #2 both got wrong.
	int          n_b_; // Number of objects that classifier #1 got right but #2 both got wrong.
	int          n_c_; // Number of objects that classifier #1 got wrong but #2 both got right.
	int          n_d_; // Number of objects that classifier #1 and #2 both got right.

protected:

	//- Helper methods.................................................
	bool         ComputeMatrixEntries(const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, float threshold1, float threshold2, int &n_a, int &n_b, int &n_c, int &n_d) const;

public:

	//- Constructors/destructor........................................
	McNemarComparator();
	McNemarComparator(const Vector(String) &filenames, const Vector(bool) &swap);
	McNemarComparator(const Vector(String) &filenames, const Vector(bool) &swap, float threshold1, float threshold2);
	virtual ~McNemarComparator();

	//- New methods....................................................
	bool         Create(const Vector(int) &targets, const Vector(float) &outputs1, const Vector(float) &outputs2, float threshold1, float threshold2, bool copy = true);

	int          GetNa() const {return n_a_;}
	int          GetNb() const {return n_b_;}
	int          GetNc() const {return n_c_;}
	int          GetNd() const {return n_d_;}
	float        GetZ() const;
	float        GetP() const;

};

#endif 
