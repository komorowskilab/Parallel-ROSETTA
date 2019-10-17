//-------------------------------------------------------------------
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __TEMPLATESCALER_H__
#define __TEMPLATESCALER_H__

#include <copyright.h>

#include <kernel/algorithms/scaler.h>
#include <kernel/basic/pair.h>

//-------------------------------------------------------------------
// Class.........: TemplateScaler
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...: An algorithm for discretizing a decision table
//                 using templates.
// Revisions.....:
//===================================================================

class TemplateScaler : public Scaler {

protected:

	int template_length_;
	int constant_template_length_;
	float crease_thr_;
	float constant_thr_;
	float start_end_thr_;
	float tolerance_thr_;
	int template_length_max;

	bool supervised_;

	bool skip_;

	Vector(int) breaks_;

public:

  //- Constructors/destructor........................................
  TemplateScaler();
  virtual ~TemplateScaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String         GetParameters() const;
	virtual bool           SetParameter(const String &keyword, const String &value);
	virtual Structure      *Apply(Structure &structure) const;
	virtual TemplateScaler	*Clone();

	//- New methods for TemplateScaler
	DecisionTable *Templatize(DecisionTable &table) const;
	DecisionTable *InitializeTemplateTable(const DecisionTable &table, bool masked) const;

	bool MatchTemplates(const DecisionTable &tablein, DecisionTable &tableout, int i, int j, int ij, bool masked) const;
	bool Supervise(DecisionTable &table, bool masked) const;
	bool Supervise2(DecisionTable &table, bool masked) const;
};

#endif 
