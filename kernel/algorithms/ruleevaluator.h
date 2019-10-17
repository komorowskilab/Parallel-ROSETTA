//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEEVALUATOR_H__
#define __RULEEVALUATOR_H__

#include <copyright.h>

#include <kernel/basic/referent.h>

#include <kernel/basic/macros.h>
#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Rule;

//-------------------------------------------------------------------
// Class.........: RuleEvaluator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Abstract helper class.
//                 Computes the "quality" of a rule.
// Revisions.....:
//===================================================================

class RuleEvaluator : public Referent {
protected:

	//- Members variables for internal use.............................
	Vector(int)   decisions_;         // Decision classes in originating table. For computing coverage.
	Vector(int)   cardinalities_;     // Cardinalities for each decision class. For computing coverage.
	int						table_cardinality_; // |U|, cardinality of originating table. For computing contingency table.

protected:

  //- Constructors/destructor........................................
  RuleEvaluator();

  //- Helper methods.................................................
	float         GetAccuracy(const Rule &rule, int i) const;
	float         GetCoverage(const Rule &rule, int i) const;
	bool					GetContingencyTable(const Rule &rule, int position, int &a, int &b, int &c, int &d, int &B, int &A, int &K) const;

  //- New virtual methods............................................
	virtual float Evaluate(const Rule &rule, int i) const = 0;

	virtual float EvaluateDominating(const Rule &rule) const;
	virtual float EvaluateBest(const Rule &rule) const;

public:

  //- Constructors/destructor........................................
  RuleEvaluator(const DecisionTable &table);
  virtual ~RuleEvaluator();

	//- Rule evaluation................................................
	virtual float Evaluate(const Rule &rule) const;

};

//-------------------------------------------------------------------
// Class.........: MichalskiRuleEvaluator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class.
// Revisions.....:
//===================================================================

class MichalskiRuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  MichalskiRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

  //- Parameters.....................................................
	float         w1_; // Accuracy weighting.
	float         w2_; // Coverage weighting.

public:

  //- Constructors/destructor........................................
  MichalskiRuleEvaluator(const DecisionTable &table, float w1, float w2);
  virtual ~MichalskiRuleEvaluator();

};

//-------------------------------------------------------------------
// Class.........: TorgoRuleEvaluator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class.
// Revisions.....:
//===================================================================

class TorgoRuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  TorgoRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  TorgoRuleEvaluator(const DecisionTable &table);
  virtual ~TorgoRuleEvaluator();

};

//-------------------------------------------------------------------
// Class.........: BrazdilRuleEvaluator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Helper class.
// Revisions.....:
//===================================================================

class BrazdilRuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  BrazdilRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  BrazdilRuleEvaluator(const DecisionTable &table);
  virtual ~BrazdilRuleEvaluator();

};

//-------------------------------------------------------------------
// Class.........: PearsonRuleEvaluator
// Author........: Thomas Êgotnes
// Date..........: 1998-10-29
// Description...: Helper class.
// Revisions.....:
//===================================================================

class PearsonRuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  PearsonRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  PearsonRuleEvaluator(const DecisionTable &table);
  virtual ~PearsonRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule) const;

};

//-------------------------------------------------------------------
// Class.........: G2RuleEvaluator
// Author........: Thomas Êgotnes
// Date..........: 981029
// Description...: Helper class.
// Revisions.....:
//===================================================================

class G2RuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  G2RuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  G2RuleEvaluator(const DecisionTable &table);
  virtual ~G2RuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule) const;

};

//-------------------------------------------------------------------
// Class.........: JRuleEvaluator
// Author........: Thomas Êgotnes/Aleksander Ûhrn
// Date..........: 981029
// Description...: Helper class.
// Revisions.....:
//===================================================================

class JRuleEvaluator : public G2RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  JRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  JRuleEvaluator(const DecisionTable &table);
  virtual ~JRuleEvaluator();

};

//-------------------------------------------------------------------
// Class.........: CohenRuleEvaluator
// Author........: Thomas Êgotnes
// Date..........: 981029
// Description...: Helper class.
// Revisions.....:
//===================================================================

class CohenRuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  CohenRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  CohenRuleEvaluator(const DecisionTable &table);
  virtual ~CohenRuleEvaluator();

};

//-------------------------------------------------------------------
// Class.........: ColemanRuleEvaluator
// Author........: Thomas Êgotnes
// Date..........: 981029
// Description...: Helper class.
// Revisions.....: AÛ 981217: Inherit from CohenRuleEvaluator.
//===================================================================

class ColemanRuleEvaluator : public CohenRuleEvaluator {
public:

	//- Enumerated types...............................................
	enum Normalization {NORMALIZATION_NONE, NORMALIZATION_C1, NORMALIZATION_C2};

	//- Textual representations........................................
	static String GetString(Normalization normalization);

protected:

	//- Member variables...............................................
	Normalization normalization_;

protected:

  //- Constructors/destructor........................................
  ColemanRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

	//- New virtual methods............................................
	virtual float EvaluateUnnormalized(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  ColemanRuleEvaluator(const DecisionTable &table, Normalization normalization);
  virtual ~ColemanRuleEvaluator();

};

//-------------------------------------------------------------------
// Class.........: KononenkoRuleEvaluator
// Author........: Thomas Êgotnes
// Date..........: 981029
// Description...: Helper class.
// Revisions.....:
//===================================================================

class KononenkoRuleEvaluator : public RuleEvaluator {
protected:

  //- Constructors/destructor........................................
  KononenkoRuleEvaluator();

	//- Methods inherited from RuleEvaluator...........................
	virtual float Evaluate(const Rule &rule, int i) const;

public:

  //- Constructors/destructor........................................
  KononenkoRuleEvaluator(const DecisionTable &table);
  virtual ~KononenkoRuleEvaluator();

};


#endif 
