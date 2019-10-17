//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <copyright.h>

#include <kernel/structures/structure.h>

#include <kernel/basic/bits.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;

//-------------------------------------------------------------------
// Class.........: Graph
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A no-frills directed graph over a finite domain of
//                 integers, not necessarily {0, 1, 2, ..., n}.
//
//                 Implemented by an adjacency matrix. Best for dense
//                 or small graphs.
//
//                 Graph topology typically specified in file.
// Comments......:
// Revisions.....:
//===================================================================

class Graph : public Structure {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<Graph>) Handles;
	typedef int                   SPDistance;
	typedef Vector(SPDistance)    SPVector;
	typedef Vector(SPVector)      APSPMatrix;

private:

  //- Implementation structures......................................
	String                        name_;      // Name of graph.
	int                           attribute_; // Index of attribute, if relevant.
	Map(int, int)                 map_;       // Maps from node/domain values to adjacency matrix indices.
	Vector(Bits)                  matrix_;    // The adjacency matrix.

protected:

	//- Constructors...................................................
  Graph(const Graph &in);

	//- Load/parse methods.............................................
	virtual bool                  ParseHeader(ifstream &stream, const String &prefix, const DecisionTable *table, bool masked, String &name, int &attribute);
	virtual bool                  ParseNodes(ifstream &stream, const String &prefix, const DecisionTable *table, bool masked, int attribute);
	virtual bool                  ParseEdges(ifstream &stream, const String &footer, const DecisionTable *table, bool masked, int attribute);
	virtual bool                  ParseEdge(const String &entry, const DecisionTable *table, bool masked, int attribute);
	virtual bool                  ParseExpansion(const String &lhs, const String &rhs, const String &connective, const DecisionTable *table, bool masked, int attribute);

public:

  //- Constructors and destructor....................................
	Graph();
	Graph(const String &filename, const DecisionTable *table = NULL, bool masked = true);
  virtual ~Graph();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool                  Load(ifstream &stream);
  virtual bool                  Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual Structure            *Duplicate() const;
  virtual void                  Clear();

	//- Graph identification methods...................................
	virtual const String         &GetName() const;
	virtual bool                  SetName(const String &name);

	virtual int                   GetAttribute() const;
	virtual bool                  SetAttribute(int attribute);

	//- Node methods...................................................
	virtual int                   GetNoNodes() const;

	virtual bool                  AddNode(int value);

	virtual int                   GetNodeIndex(int value) const;
	virtual int                   GetNodeValue(int index) const;

	virtual int                   GetInDegreeByIndex(int index) const;
	virtual int                   GetInDegreeByValue(int value) const;

	virtual int                   GetOutDegreeByIndex(int index) const;
	virtual int                   GetOutDegreeByValue(int value) const;

	//- Edge methods..................................................
	virtual int                   GetNoEdges() const;

	virtual bool                  GetEdgeByIndex(int index_from, int index_to) const;
	virtual bool                  SetEdgeByIndex(int index_from, int index_to, bool flag);

	virtual bool                  GetEdgeByValue(int value_from, int value_to) const;
	virtual bool                  SetEdgeByValue(int value_from, int value_to, bool flag);

	virtual bool                  MakeAdjacencyMatrix();

	virtual bool                  MakeReflexive();
	virtual bool                  MakeSymmetric();
	virtual bool                  MakeTransitive();
	virtual bool                  MakeDistance(int radius);
	virtual bool                  MakeComplement();

	//- Graph algorithms..............................................
	virtual bool                  Dijkstra(int index, SPVector &distances) const;
	virtual bool                  Floyd(APSPMatrix &distances) const;
	virtual bool                  Warshall();

  //- Query methods.................................................
	virtual bool                  IsReflexive() const;
	virtual bool                  IsSymmetric() const;
	virtual bool                  IsTransitive() const;

	virtual bool                  IsAntiReflexive() const;
	virtual bool                  IsAntiSymmetric() const;
	virtual bool                  IsAntiTransitive() const;

	//- Persistence methods...........................................
	virtual bool                  Load(const String &filename, const DecisionTable *table = NULL, bool masked = true);
	virtual bool                  Load(ifstream &stream, const DecisionTable *table = NULL, bool masked = true);

	virtual bool                  Save(const String &filename, const DecisionTable *table = NULL, bool masked = true) const;
	virtual bool                  Save(ofstream &stream, const DecisionTable *table = NULL, bool masked = true) const;

};

#endif 
