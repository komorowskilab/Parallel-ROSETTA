//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PermPermLeaveOneOutCVSerialExecutor_H__
#define __PermPermLeaveOneOutCVSerialExecutor_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/loocvserialexecutor.h>

#include <kernel/algorithms/serialexecutorloop.h>
#include <kernel/algorithms/keyword.h>
#include <kernel/algorithms/batchclassifier.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/project.h>
#include <kernel/structures/projectmanager.h>

#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/rng.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>

#include <kernel/structures/rules.h>

//-------------------------------------------------------------------
// Class.........: PermLeaveOneOutCVSerialExecutor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Implements n-fold crossvalidation.
// Revisions.....:
//===================================================================

class PermLeaveOneOutCVSerialExecutor : public LeaveOneOutCVSerialExecutor{
protected:
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const;
	
	//- Parameters.....................................................
	bool                invert_;  // Invert sampling?
	Vector(int)         indices_; // For internal use, maps an object to a partition index.
	float								auc_;
	float								accuracy_;
	int									permutation_;
protected:
	
	//- Methods inherited from SerialExecutorLoop......................
	virtual bool        InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng);
	virtual bool        SampleTables(int i, const RNG &rng, DecisionTable &training, DecisionTable &testing, bool masked) const;
	
public:
	
	//- Constructors/destructor........................................
	PermLeaveOneOutCVSerialExecutor();
	virtual ~PermLeaveOneOutCVSerialExecutor();
	
	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
		//- Methods inherited from Algorithm..............................
		virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	
	//- Local methods..................................................
	bool                InvertSampling() const {return invert_;}
	bool                InvertSampling(bool invert) {invert_ = invert; return true;}
	float                GetAccuracy() const {return accuracy_;}
	bool               SetAccuracy(float accuracy) {accuracy_ = accuracy; return true;}
	float                GetAUC() const {return auc_;}
	bool               SetAUC(float auc) {auc_ = auc; return true;}
	int                 GetPermutation() const {return permutation_;}
	bool               SetPermutation(int permutation) {permutation_ = permutation; return true;}
	Structure *				ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters) const;
	bool SaveAUCLogStatistics(ofstream &stream, const Vector(float) &statistics, const String &name) const ;
	
};

#endif
 
