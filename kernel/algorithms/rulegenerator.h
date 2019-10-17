//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960419
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RULEGENERATOR_H__
#define __RULEGENERATOR_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class.........: RuleGenerator
// Author........: Aleksander Ûhrn
// Date..........: 950419
// Description...: Overlays a set of reducts a decision table in order
//                 to produce a set of rules.
// Revisions.....: AÛ 971017: Added protected table member.
//                 AÛ 990727: Added IDG parameters.
//===================================================================

class RuleGenerator : public Algorithm {
protected:

	//- Parameters.....................................................
	bool                  idg_;                    // Use IDG in matching?
	String                filename_idg_;           // Location of IDG, if any.

	//- Protected variables............................................
	Handle<DecisionTable> table_;                  // Backup table to use if suitable parent not found.

public:

  //- Constructors/destructor........................................
  RuleGenerator();
  virtual ~RuleGenerator();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String        GetParameters() const;
	virtual bool          SetParameter(const String &keyword, const String &value);
	virtual bool          IsApplicable(const Structure &structure, bool warn = true) const;
	virtual RuleGenerator	*Clone() = 0;

	//- Parameter methods..............................................
	virtual bool          UseIDG() const;
	virtual bool          UseIDG(bool idg);

	virtual const String &GetIDGFilename() const;
	virtual bool          SetIDGFilename(const String &filename);

	//- Local methods..................................................
	Handle<DecisionTable> GetDecisionTable() const {return table_;}
	bool                  SetDecisionTable(Handle<DecisionTable> table) {table_ = table; return true;}

};

#endif 
