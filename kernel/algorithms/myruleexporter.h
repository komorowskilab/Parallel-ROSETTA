//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __MYRULEEXPORTER_H__
#define __MYRULEEXPORTER_H__

#include <copyright.h>

#include <kernel/algorithms/ruleexporter.h>
#include <kernel/algorithms/keyword.h>
#include <cmath>

//-------------------------------------------------------------------
// Class.........: MYRuleExporter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Exports a set of rules to plain "native" format.
// Revisions.....: 24/11/14 -N
// 			Added Odds Ratio and Risk Ratio as options.
// 			Required an overloaded SetParameter(...).
// 		   20/02/15 -N
// 		   	Added confidence intervals to Odds ratios
// 		   	via the boost math library.
// 		   02/03/15 -N
// 		   	Removed Boost, removed distribution models.
// 		   	Simplified CIs. Moved confidence level to Set/Get CI().
//===================================================================
class MyRuleExporter : public RuleExporter {


private:

	//Flags
	float		fCI_;
	bool		bOR_;
	bool		bRR_;

protected:

  //- Methods inherited from Exporter................................
	virtual bool 	ExportPrologue(ofstream &stream, const Structure &structure) const;

  //- Methods inherited from RuleExporter............................
	virtual bool 	ExportRule(ofstream &stream, int rule_no, const Rule &rule) const;

	//For OR and RR -N
	virtual bool 	SetParameter(const String &keyword, const String &value);
	virtual String	GetParameters() const;
	
	//Set Flags
	bool		SetOR() {bOR_ = true; return true;}
	bool		SetRR() {bRR_ = true; return true;}
	bool		SetCI(const String &sType) { fCI_ = (sType.Contains(".") ? sType.GetFloat() : (String("0.") + sType).GetFloat()); return true;}

	//Get Flags
	bool		GetOR()	const {return bOR_;}
	bool		GetRR() const {return bRR_;}
	bool		GetComputeCI() const {return fCI_ != 0.0;}
	float		GetCI() const {return fCI_;}

public:

  //- Constructors/destructor........................................
  MyRuleExporter();
  virtual ~MyRuleExporter();

  virtual MyRuleExporter *Clone();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

};

#endif 
