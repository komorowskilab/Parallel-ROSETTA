//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __LeaveOneOutCVSerialExecutor_H__
#define __LeaveOneOutCVSerialExecutor_H__

#include <copyright.h>

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
// Class.........: LeaveOneOutCVSerialExecutor
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Implements n-fold crossvalidation.
// Revisions.....:
//===================================================================

class LeaveOneOutCVSerialExecutor : public SerialExecutorLoop {
protected:
	virtual Structure  *ExecuteCommands(Structure &structure, const Algorithm::Handles &algorithms, const Vector(String) &parameters, ofstream &stream) const;
	
	//- Parameters.....................................................
	bool                invert_;  // Invert sampling?
	Vector(int)         indices_; // For internal use, maps an object to a partition index.
	
protected:
	
	//- Methods inherited from SerialExecutorLoop......................
	virtual bool        InitializeSamplingScheme(const DecisionTable &table, bool masked, const RNG &rng);
	virtual bool        SampleTables(int i, const RNG &rng, DecisionTable &training, DecisionTable &testing, bool masked) const;
	virtual bool				SaveAUCLogStatistics(ofstream &stream, const Vector(float) &statistics, const String &name) const;
	
public:
	
	//- Constructors/destructor........................................
	LeaveOneOutCVSerialExecutor();
	virtual ~LeaveOneOutCVSerialExecutor();
	
	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
		//- Methods inherited from Algorithm..............................
		virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual	LeaveOneOutCVSerialExecutor	*Clone();
	
	//- Local methods..................................................
	bool                InvertSampling() const {return invert_;}
	bool                InvertSampling(bool invert) {invert_ = invert; return true;}
	
};

#endif
 
