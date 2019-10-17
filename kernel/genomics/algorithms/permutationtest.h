//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PERMUTATIONTEST_H__
#define __PERMUTATIONTEST_H__

#include <copyright.h>

#include <kernel/algorithms/splitter.h>

//-------------------------------------------------------------------
// Class.........: PermutationTest
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Randomly splits a decision table into subtables.
// Revisions.....:
//===================================================================

class PermutationTest : public Splitter {
protected:
	
	//- Parameters.....................................................
	int                seed_;             // Seed to the RNG.
	String              commandfile_;   // Location of file with commands.
	String              logfile_;       // Location of log file.
	int									nfolds_;					// number of folds in n-fold permutation test
	int									npermutations_;		// number of permutations in permutation test
	int									algorithm_;				// evaluation algorithm. 
	int									length_;					// length of training pipeline
public:
	
	//- Constructors/destructor........................................
	PermutationTest();
	virtual ~PermutationTest();
	
	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()
		
	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual bool			SaveLogHeader(ofstream &stream) const ;
	virtual bool			 SaveLogEntry(ofstream &stream, String mystring) const;
	virtual PermutationTest		*Clone();

	//- Parameter management...........................................
	int                GetSeed() const {return seed_;}
	bool               SetSeed(int seed) {seed_ = seed; return true;}
	
	const String       &GetCommandFilename() const {return commandfile_;}
	bool                SetCommandFilename(const String &commandfile) {commandfile_ = commandfile; return true;}
	
	const String       &GetLogFilename() const {return logfile_;}
	bool                SetLogFilename(const String &logfile) {logfile_ = logfile; return true;}
	
	int                 GetNFolds() const {return nfolds_;}
	bool                SetNFolds(int nfolds) {nfolds_ = nfolds; return true;}
	int                 GetNPermutations() const {return npermutations_;}
	bool                SetNPermutations(int npermutations) {npermutations_ = npermutations; return true;}
	int                 GetAlgorithm() const {return algorithm_;}
	bool                SetAlgorithm(int algorithm) {algorithm_ = algorithm; return true;}
	int                 GetLength() const {return length_;}
	bool                SetLength(int length) {length_ = length; return true;}
	
};

#endif 
