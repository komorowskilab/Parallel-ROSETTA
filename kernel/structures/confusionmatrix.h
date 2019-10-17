//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CONFUSIONMATRIX_H__
#define __CONFUSIONMATRIX_H__

#include <copyright.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/map.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class.........: ConfusionMatrix
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A confusion matrix is a matrix where the rows
//                 (resp. columns) are labeled with the target
//                 (resp. response) classification.  Used to summarize
//                 the results of a discrete batch classification.
//
//                 A mapping is needed to map between the matrix
//                 indices and their "meaning".  For instance, the
//                 matrix indices may run from 0 to 3, but their
//                 "meaning" does not necessarily have to be
//                 {0, 1, 2, 3}, but could e.g. be {-18, 3, 7, 12}.
//
// Comments......: Kept inheritance from AnnotatedStructure to preserve
//                 backwards I/O compatibility.
// Revisions.....:
//===================================================================

class ConfusionMatrix : public AnnotatedStructure {
private:

  //- Type definitions...............................................
	typedef Map(int, int) IMap;

private:

  //- Implementation structures......................................
  Handle<DecisionTable>  matrix_;        // Physical representation of matrix.
	IMap                  *map_;           // Maps between matrix axis indices and their "meaning".

protected:

	//- Constructors...................................................
  ConfusionMatrix(const ConfusionMatrix &in);

public:

  //- Type definitions..............................................
	typedef Vector(Handle<ConfusionMatrix>) Handles;

public:

  //- Constructors/destructor........................................
	ConfusionMatrix();
	ConfusionMatrix(int dimension);
  virtual ~ConfusionMatrix();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent.............................
  virtual bool       Load(ifstream &stream);
	virtual bool       Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure *Duplicate() const;
  virtual void       Clear();

  //- Operators......................................................
	ConfusionMatrix   &operator=(const ConfusionMatrix &in);
  bool               operator==(const ConfusionMatrix &in) const;
  bool               operator!=(const ConfusionMatrix &in) const;
	ConfusionMatrix 	&operator+(const ConfusionMatrix &in);
	ConfusionMatrix 	&operator+=(const ConfusionMatrix &in);

  //- Matrix methods.................................................
	int                GetDimension() const;
	bool               SetDimension(int dimension);

  int                GetEntry(int i, int j) const;
	bool               SetEntry(int i, int j, int value);

	bool               IncrementEntry(int i, int j);

	//- Index mapping methods............................................
	bool               SetMap(int index, int value);

	int                GetValue(int index) const;
	int                GetIndex(int value) const;

  //- Sum/ratio methods................................................
	int                GetSum() const;
	int                GetRowSum(int i) const;
	int                GetColumnSum(int j) const;
	int                GetDiagonalSum() const;

	float              GetRowRatio(int i) const;
	float              GetColumnRatio(int j) const;
	float              GetDiagonalRatio() const;

  //- Formatting methods...............................................
	bool               Format(String &formatted, const String &indent = "") const;
	String             Format(const String &indent = "") const;

  //- I/O methods for backwards compatibility..........................
  bool               Load(ifstream &stream, bool &has_extra_data, int &attribute, int &roc_class, float &roc_area, float &roc_stderr, float &roc_threshold_nw, float &roc_threshold_accuracy);

};

//-------------------------------------------------------------------
// Inlined methods.
//===================================================================

inline int
ConfusionMatrix::GetDimension() const {
	return matrix_->GetNoObjects(false);
}

inline int
ConfusionMatrix::GetEntry(int i, int j) const {
	return matrix_->GetEntry(i, j, false);
}

inline bool
ConfusionMatrix::SetEntry(int i, int j, int value) {
	return matrix_->SetEntry(i, j, value, false);
}

inline bool
ConfusionMatrix::IncrementEntry(int i, int j) {
	return SetEntry(i, j, GetEntry(i, j) + 1);
}

#endif 
