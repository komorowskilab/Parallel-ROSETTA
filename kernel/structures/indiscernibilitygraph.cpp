//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/indiscernibilitygraph.h>
#include <kernel/structures/discernibilitymatrix.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/generalizeddecision.h>

#include <kernel/utilities/partitionkit.h>
#include <kernel/utilities/discerner.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/map.h>

//-------------------------------------------------------------------
// Static helpers (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticVerifyNames
// Author........: Aleksander šhrn
// Date..........:
// Description...: Verifies validity of index (unmasked) of attribute
//                 that specifies node names.
//
//                 Also checks that attribute is valid for unique
//                 object naming (with a masked table).
// Comments......:
// Revisions.....:
//===================================================================

static bool
StaticVerifyNames(int index, const DecisionTable &table) {

	if (index == Undefined::Integer())
		return true;

	// Verify range.
	if (index < 0 || index >= table.GetNoAttributes(false))
		return false;

	Vector(int) values, cardinalities;

	// Get value set.
	if (!table.GetValueSet(values, cardinalities, index, false))
		return false;

	// Verify "1-1"-ness with the masked table.
	if (values.size() != table.GetNoObjects(true))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Methods for class IndiscernibilityGraph.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Copy constructor
// Comments......:
// Revisions.....:
//===================================================================

IndiscernibilityGraph::IndiscernibilityGraph(const IndiscernibilityGraph &in) : Graph(in) {
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

IndiscernibilityGraph::IndiscernibilityGraph() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

IndiscernibilityGraph::~IndiscernibilityGraph() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(IndiscernibilityGraph, INDISCERNIBILITYGRAPH, Graph)

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: AreIndiscernible
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given objects x and y (or more precisely, object
//                 indices i and j) and the discernibility matrix M,
//                 returns true iff x and y are indiscernible.
//
//                 Default implementation returns true iff M(x, y)
//                 contains less than k elements. Can be overloaded to
//                 provide other behaviours.
//
// Comments......: Called from the Create method. The AddEdges1 and
//                 AddEdges2 methods should be generalized to allow
//                 similar flexibility through a related AreIndiscernible
//                 method.
//
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraph::AreIndiscernible(int i, int j, const DiscernibilityMatrix &matrix, int cardinality) const {

	if (matrix.IsEmpty(i, j))
		return true;

	// Assuming that only non-empty entries are stored in the matrix.
	if (cardinality == 0)
		return false;

	return (matrix.GetEntry(i, j)->Count(true) <= cardinality);

}

//-------------------------------------------------------------------
// Method........: AreIndiscernible
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given objects x and y (or more precisely, object
//                 indices i and j) and the decision table to which they
//                 belong, returns true iff x and y are indiscernible.
//
//                 Can be overloaded to provide other behaviours, e.g.,
//                 Euclidian or Mahalonobis distance.
//
// Comments......: Called from the AddEdges1 method.
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraph::AreIndiscernible(int i, int j, const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int cardinality) const {
	return AreIndiscernible(i, j, table, masked, discerner, all, cardinality, table.GetNoAttributes(masked));
}

bool
IndiscernibilityGraph::AreIndiscernible(int i, int j, const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int cardinality, int no_attributes) const {

	int k, count = 0;

	// Are objects (i, j) indiscernible?
	for (k = 0; k < no_attributes; k++) {
		if (all || table.IsCondition(k, masked)) {
			int value_i = table.GetEntry(i, k, masked);
			int value_j = table.GetEntry(j, k, masked);
			if (discerner.Discerns(k, value_i, value_j)) {
				count++;
				if (count > cardinality)
					return false;
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Constructs a graph (V, E).
//
//                 V: The set of nodes. One node per object in the
//                    discernibility matrix.
//                 E: The set of edges. Typically, an edge is present
//                    between node i and node j iff entry (i, j) in
//                    the matrix is empty.
//
// Comments......:
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraph::Create(const DiscernibilityMatrix &matrix, int cardinality) {

	// Delete current graph contents.
	Clear();

	int i, j, no_objects = matrix.GetDimension();

	// Initialize node set.
	for (i = 0; i < no_objects; i++) {
		if (!AddNode(i))
			return false;
	}

	// Create edge matrix.
	if (!MakeAdjacencyMatrix())
		return false;

	// Add an undirected edge between two objects if the objects are indiscernible.
	// Assume a symmetric matrix.
	for (i = 0; i < no_objects; i++) {
		for (j = 0; j <= i; j++) {
			if (AreIndiscernible(i, j, matrix, cardinality)) {
				if (!SetEdgeByIndex(i, j, true) || !SetEdgeByValue(j, i, true))
					return false;
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Constructs a graph (V, E).
//
//                 V: The set of nodes. One node per object in the
//                    table.
//                 E: The set of edges. An edge is present between
//                    node i and node j iff objects i and j are
//                    indiscernible.
//
//                 table:
//                    The decision table to work on.
//                 masked:
//                    Work on a masked table?
//                 discerner:
//                    Defines the discernibility predicate.
//                 all:       (default true)
//                    For discernibility, consider all attributes
//                    (i.e., both condition and decision attributes)
//                    or condition attributes only?
//                 names:     (default Undefined::Integer())
//                    The index (unmasked) of a 1-1 attribute in the
//                    table that defines node names. If Undefined,
//                    no names are used.
//                 decisions: (default NULL)
//                    If supplied, returned in-place. In effect
//                    defines the generalized decision values for
//                    each object, since it compiles the decision
//                    values of all indiscernible objects, i.e.,
//                    neighbouring nodes.
//
// Comments......: The table can be masked to change the attribute
//                 subset under consideration.
//
//                 Creation of graph can be done quicker if we know
//                 that the indiscernibility relation is an
//                 equivalence relation. Hence, a specialized
//                 implementation is provided if the discernibility
//                 predicate does not use a IDG. Then, strict inequality
//                 assumed used.
//
//                 Can be optimized by only considering one
//                 representative from each equivalence class.
//
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraph::Create(const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int cardinality, int names, GeneralizedDecision::Handles *decisions) {

	// Delete current graph contents.
	Clear();

	int no_objects    = table.GetNoObjects(masked);

	// Current implementation is not perfect...
	if (table.GetNoObjects(true) != table.GetNoObjects(false)) {
		Message::Error("Current implementation does not handle masked objects.");
		return false;
	}

	// Verify validity of name attribute.
	if (!StaticVerifyNames(names, table)) {
		Message::Error("Name attribute not accepted. Does it assign unique names?");
		return false;
	}

	int i;

	// Initialize node set.
	for (i = 0; i < no_objects; i++) {
		int name = (names == Undefined::Integer()) ? i : table.GetEntry(i, names, false);
		if (!AddNode(name))
			return false;
	}

	// Create edge matrix.
	if (!MakeAdjacencyMatrix())
		return false;

	// Add edges.
	if (discerner.HasIDGs() || (cardinality > 0)) {
		if (!AddEdges1(table, masked, discerner, all, cardinality, names, decisions))
			return false;
	}
	else {
		if (!AddEdges2(table, masked, all, names, decisions))
			return false;
	}

	// Set index of attribute graph node values refer to (if any).
	SetAttribute(names);

	return true;

}

//-------------------------------------------------------------------
// Method........: AddEdges1
// Author........: Aleksander šhrn
// Date..........:
// Description...: Helper, called from Create. Adds edges to graph.
//                 General O(n^2) implementation.
//
// Comments......: Future optimization: Compute equivalence classes
//                 and consider representatives in the computations.
//
// Revisions.....: 990717 Aš: Added check to see if symmetry
//                            exploitation is valid.
//===================================================================

bool
IndiscernibilityGraph::AddEdges1(const DecisionTable &table, bool masked, const Discerner &discerner, bool all, int cardinality, int names, GeneralizedDecision::Handles *decisions) {

	int no_objects    = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);

	int i, j, k;

	bool is_symmetric = true;

	// Can we exploit symmetry?
	for (k = 0; k < no_attributes; k++) {
		if (discerner.HasIDG(k)) {
			if (!discerner.GetIDG(k)->IsSymmetric()) {
				is_symmetric = false;
				Message::Warning("At least one IDG is not symmetric.", false);
				break;
			}
		}
	}

	// Add an undirected edge between two objects if they are indiscernible.
	for (i = 0; i < no_objects; i++) {

		// Get "name" of object i.
		int name_i = (names == Undefined::Integer()) ? i : table.GetEntry(i, names, false);

		for (j = 0; j <= (is_symmetric ? i : (no_objects - 1)); j++) {

			// Skip to next pair if objects i and j aren't indiscernible, i.e., if we are not to add an edge.
			if (!AreIndiscernible(i, j, table, masked, discerner, all, cardinality, no_attributes))
				continue;

			// Get "name" of object j.
			int name_j = (names == Undefined::Integer()) ? j : table.GetEntry(j, names, false);

			// Add edge(s).
			if (names == Undefined::Integer()) {
				if (is_symmetric) {
					if (!SetEdgeByIndex(name_i, name_j, true) || !SetEdgeByIndex(name_j, name_i, true))
						return false;
				}
				else {
					if (!SetEdgeByIndex(name_i, name_j, true))
						return false;
				}
			}
			else {
				if (is_symmetric) {
					if (!SetEdgeByValue(name_i, name_j, true) || !SetEdgeByValue(name_j, name_i, true))
						return false;
				}
				else {
					if (!SetEdgeByValue(name_i, name_j, true))
						return false;
				}
			}

		}
	}

	// Collect decision values, if specified.
	if (decisions != NULL) {
		if (!GetDecisions1(*decisions, table, masked))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AddEdges2
// Author........: Aleksander šhrn
// Date..........:
// Description...: Helper, called from Create. Adds edges to graph.
//
//                 Specialized O(nlogn) implementation, suitable only
//                 if the discerner defines an equivalence relation
//                 through strict inequality on the attribute values.
// Comments......:
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraph::AddEdges2(const DecisionTable &table, bool masked, bool all, int names, GeneralizedDecision::Handles *decisions) {

	int no_objects    = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);

	int i, j, k;

	// Initialize vector of decisions, if specified.
	if (decisions != NULL) {
		decisions->reserve(no_objects);
		decisions->erase(decisions->begin(), decisions->end());
		for (i = 0; i < no_objects; i++)
			decisions->push_back(Handle<GeneralizedDecision>(NULL));
	}

	Vector(int) attributes;
	attributes.reserve(no_attributes);

	// Set up attribute set to partition by.
	for (i = 0; i < no_attributes; i++) {
		if (all || table.IsCondition(i, masked))
			attributes.push_back(i);
	}

	int         no_partitions;
	Vector(int) indices;

	// Compute partition.
	if (!PartitionKit::ComputePartitionIndices(indices, no_partitions, attributes, table, masked))
		return false;

	Vector(Vector(int)) partitions;
	partitions.reserve(no_partitions);

	for (i = 0; i < no_partitions; i++) {
		partitions.push_back(Vector(int)());
		partitions[i].reserve(no_objects / no_partitions);
	}

	// Collect equivalence classes.
	for (i = 0; i < no_objects; i++)
		partitions[indices[i]].push_back(i);

	Handle<GeneralizedDecision> prototype = Creator::GeneralizedDecision();

	// Each equivalence class defines a complete subgraph.
	for (i = 0; i < no_partitions; i++) {

		// Get size of current subgraph.
		int size = partitions[i].size();

		// Connect all nodes in subgraph to each other.
		for (j = 0; j < size; j++) {
			int name_j = (names == Undefined::Integer()) ? partitions[i][j] : table.GetEntry(partitions[i][j], names, false);
			for (k = 0; k <= j; k++) {
				int name_k = (names == Undefined::Integer()) ? partitions[i][k] : table.GetEntry(partitions[i][k], names, false);
				if (names == Undefined::Integer()) {
					if (!SetEdgeByIndex(name_j, name_k, true) || !SetEdgeByIndex(name_k, name_j, true))
						return false;
				}
				else {
					if (!SetEdgeByValue(name_j, name_k, true) || !SetEdgeByValue(name_k, name_j, true))
						return false;
				}
			}
		}

		// Collect decision values in complete subgraph, if specified.
		// Note that the same physical object is shared.
		if (decisions != NULL) {
			Handle<GeneralizedDecision> partition_decisions = dynamic_cast(GeneralizedDecision *, prototype->Duplicate());
			if (!GetDecisions2(*partition_decisions, partitions[i], table, masked))
				return false;
			for (j = 0; j < size; j++)
				(*decisions)[partitions[i][j]] = partition_decisions;
		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetDecisions1
// Author........: Aleksander šhrn
// Date..........:
// Description...: For each node i in V, returns D(x_i) for object
//                 x_i in the table, where:
//
//                    D(x_i) = {d(x_j) | (i, j) in E}
//
// Comments......: This is a slow but general implementation,
//                 called from AddEdges1.
//
//                 Future optimization: Compute equivalence classes
//                 and consider representatives in the computations.
//
// Revisions.....: Aš 990416: Fixed an initialization bug.
//                 Aš 990418: Minor optimization: Assume symmetry.
//===================================================================

bool
IndiscernibilityGraph::GetDecisions1(GeneralizedDecision::Handles &decisions, const DecisionTable &table, bool masked) const {

	int i, j, no_objects = table.GetNoObjects(masked);

	// Verify compatibility.
	if (GetNoNodes() != no_objects)
		return false;

	// Acquire index of decision attribute.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	// Make a convenient typedef. Increases code compatibility across compilers/platforms.
	typedef Map(int, int) IntegerMap;

	Vector(IntegerMap) maps;

	maps.reserve(no_objects);

	// Initialize vector of maps.
	for (i = 0; i < no_objects; i++)
		maps.push_back(IntegerMap());

	int  decision_i, decision_j;
	bool is_ij_edge, is_symmetric(true);

	// Collect decision values. Assume symmetry/undirectedness, but verify it on the fly.
	for (i = 0; i < no_objects; i++) {
		decision_i = table.GetEntry(i, decision_attribute, masked);
		for (j = i; j < no_objects; j++) {
			is_ij_edge = GetEdgeByIndex(i, j);
			is_symmetric = (is_ij_edge == GetEdgeByIndex(j, i));
			if (!is_symmetric)
				break;
			if (is_ij_edge) {
				decision_j = table.GetEntry(j, decision_attribute, masked);
				maps[i].insert(Pair(const int, int)(decision_j, 0));
				maps[i][decision_j]++;
				if (j != i) {
					maps[j].insert(Pair(const int, int)(decision_i, 0));
					maps[j][decision_i]++;
				}
			}
		}
		if (!is_symmetric)
			break;
	}

	// Did assumption about symmetry fail? If so, recompute.
	if (!is_symmetric) {
		Message::Warning("Indiscerniblity graph is directed.", false);
		for (i = 0; i < no_objects; i++) {
			maps[i].erase(maps[i].begin(), maps[i].end());
			for (j = 0; j < no_objects; j++) {
				if (GetEdgeByIndex(i, j)) {
					decision_j = table.GetEntry(j, decision_attribute, masked);
					maps[i].insert(Pair(const int, int)(decision_j, 0));
					maps[i][decision_j]++;
				}
			}
		}
	}

	// Initialize structure to return (in-place).
	if (!decisions.empty())
		decisions.erase(decisions.begin(), decisions.end());
	decisions.reserve(no_objects);

	Handle<GeneralizedDecision> prototype = Creator::GeneralizedDecision();

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	IntegerMap::iterator it;

	// Build generalized decisions.
	for (i = 0; i < no_objects; i++) {
		decisions.push_back(dynamic_cast(GeneralizedDecision *, prototype->Duplicate()));
		decisions[i]->Reserve(maps[i].size());
		for (it = maps[i].begin(); it != maps[i].end(); it++)
			decisions[i]->AppendDecision((*it).first, (*it).second);
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetDecisions2
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns in-place the set of decision values of all
//                 objects in the specified set (which is assumed to
//                 define a fully connected subgraph).
//
// Comments......: This is a specialized implementation,
//                 called from AddEdges2.
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraph::GetDecisions2(GeneralizedDecision &decisions, const Vector(int) &objects, const DecisionTable &table, bool masked) const {

	// Clear current contents, if any.
	decisions.Clear();

	// Acquire index of decision attribute.
	int decision_attribute = table.GetDecisionAttribute(masked);

	if (decision_attribute == Undefined::Integer())
		return false;

	int i, no_objects = objects.size();

	Map(int, int) map;

	// Collect decision values.
	for (i = 0; i < no_objects; i++) {
		int decision = table.GetEntry(objects[i], decision_attribute, masked);
		map.insert(Pair(const int, int)(decision, 0));
		map[decision]++;
	}

	decisions.Reserve(map.size());

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	Map(int, int)::iterator it;

	// Build generalized decision.
	for (it = map.begin(); it != map.end(); it++)
		decisions.AppendDecision((*it).first, (*it).second);

	return true;

}
 
