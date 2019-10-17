//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CVSERIALEXECUTOR_H__
#define __CVSERIALEXECUTOR_H__

#include <copyright.h>

#include <kernel/algorithms/serialexecutorloop.h>

#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Class.........: CVSerialExecutor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Implements n-fold crossvalidation.
// Revisions.....:
//===================================================================

class CVSerialExecutor : public SerialExecutorLoop {
protected:

	//- Parameters.....................................................
	bool                invert_;  // Invert sampling?
	Vector(int)         indices_; // For internal use, maps an object to a partition index.

protected:

  //- Methods inherited from SerialExecutorLoop......................
	virtual bool        InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng);
	virtual bool        SampleTables(int i, const RNG &rng, DecisionTable &training, DecisionTable &testing, bool masked) const;

public:

  //- Constructors/destructor........................................
  CVSerialExecutor();
  virtual ~CVSerialExecutor();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm..............................
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual CVSerialExecutor	*Clone();

	//- Local methods..................................................
	bool                InvertSampling() const {return invert_;}
	bool                InvertSampling(bool invert) {invert_ = invert; return true;}

};

#endif
 
