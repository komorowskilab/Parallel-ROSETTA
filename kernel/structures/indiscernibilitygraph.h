//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __INDISCERNIBILITYGRAPH_H__
#define __INDISCERNIBILITYGRAPH_H__

#include <copyright.h>

#include <kernel/structures/graph.h>
#include <kernel/structures/generalizeddecision.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Discerner;
class DiscernibilityMatrix;

//-------------------------------------------------------------------
// Class.........: IndiscernibilityGraph
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....: AÛ 991015: Made methods virtual, added method
//                            AreIndiscernible.
//                 AÛ 991110: Added cardinality stuff.
//
// Comments......: To do: Add a more general option to Create than simply
//                        allowing low-cardinality entries to be ignored.
//                        Allow, e.g., general cost information, or even
//                        thresholding on weighted distances (i.e., Manhattan
//                        or city-block metric).
//===================================================================

class IndiscernibilityGraph : public Graph {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<IndiscernibilityGraph>) Handles;

protected:

	//- Constructors...................................................
  IndiscernibilityGraph(const IndiscernibilityGraph &in);

	//- Creation helpers, general......................................
	virtual bool   AddEdges1(const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int cardinality, int names, GeneralizedDecision::Handles *decisions = NULL);
	virtual bool   GetDecisions1(GeneralizedDecision::Handles &decisions, const DecisionTable &table, bool masked) const;

	//- Creation helpers, specialized..................................
	virtual bool   AddEdges2(const DecisionTable &table, bool masked, bool all, int names, GeneralizedDecision::Handles *decisions = NULL);
	virtual bool   GetDecisions2(GeneralizedDecision &decisions, const Vector(int) &objects, const DecisionTable &table, bool masked) const;

	//- Creation helpers, overloading candidates......................
	virtual bool   AreIndiscernible(int i, int j, const DiscernibilityMatrix &matrix, int cardinality) const;
	virtual bool   AreIndiscernible(int i, int j, const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int cardinality) const;
	virtual bool   AreIndiscernible(int i, int j, const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int no_attributes, int cardinality) const;

public:

  //- Constructors and destructor....................................
	IndiscernibilityGraph();
  virtual ~IndiscernibilityGraph();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Creation methods...............................................
	virtual bool   Create(const DiscernibilityMatrix &matrix, int cardinality = 0);
	virtual bool   Create(const DecisionTable &table, bool masked, const Discerner &discerner, bool all = true, int cardinality = 0, int names = Undefined::Integer(), GeneralizedDecision::Handles *decisions = NULL);

};

#endif 
