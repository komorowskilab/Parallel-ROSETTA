//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/graph.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Static helpers (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticGetValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static int
StaticGetValue(const String &text, const DecisionTable *table, int attribute, bool masked) {

	if (text == Undefined::String())
		return Undefined::Integer();

	if (table == NULL)
		return text.IsInteger() ? text.GetInteger() : Undefined::Integer();
	else {
		int value = table->GetDictionaryEntry(attribute, text, masked);
		if (value == Undefined::Integer()) {
			if (text.IsInteger())
				value = table->SuggestDictionaryEntry(attribute, text, masked);
		}
		return value;
	}

}

//-------------------------------------------------------------------
// Method........: StaticGetText
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

static String
StaticGetText(int value, const DecisionTable *table, int attribute, bool masked) {

	if (value == Undefined::Integer())
		return Undefined::String();

	if (table == NULL) {
		return String::Format(value);
	}
	else {
		String text = table->GetDictionaryEntry(attribute, value, masked);
		if (text == Undefined::String())
			text = String::Format(value);
		return text;
	}

}

//-------------------------------------------------------------------
// Methods for class Graph.
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

Graph::Graph(const Graph &in) : Structure(in) {
	name_      = in.name_;
	attribute_ = in.attribute_;
	map_       = in.map_;
	matrix_    = in.matrix_;
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

Graph::Graph() {
	name_      = Undefined::String();
	attribute_ = Undefined::Integer();
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Initializes graph with file contents.
// Comments......:
// Revisions.....:
//===================================================================

Graph::Graph(const String &filename, const DecisionTable *table, bool masked) {

	ifstream stream;

	if (!IOKit::Open(stream, filename)) {
		Message::Error("Unable to open " + filename + ".");
	}
	else {
		if (!Load(stream, table, masked))
			Clear();
	}

	IOKit::Close(stream);

}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Graph::~Graph() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Graph, GRAPH, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Load(ifstream &stream) {
	return Load(stream, NULL, true);
}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Save(ofstream &stream) const {
	return Save(stream, NULL, true);
}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
Graph::Duplicate() const {
  return new Graph(*this);
}

//-------------------------------------------------------------------
// Method........: Clear
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
Graph::Clear() {
	name_ = Undefined::String();
	attribute_ = Undefined::Integer();
	map_.erase(map_.begin(), map_.end());
	matrix_.erase(matrix_.begin(), matrix_.end());
}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Graph identification methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetName/SetName
// Author........: Aleksander šhrn
// Date..........:
// Description...: The name of the graph. If the domain of the graph
//                 is the value set of an attribute in a decision
//                 table, the graph name is typically equal to the
//                 attribute name.
// Comments......:
// Revisions.....:
//===================================================================

const String &
Graph::GetName() const {
	return name_;
}

bool
Graph::SetName(const String &name) {
	name_ = name;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetAttribute/SetAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: If the domain of the graph is the value set of an
//                 attribute in a decision table, this denotes the
//                 index of the attribute in the table.
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetAttribute() const {
	return attribute_;
}

bool
Graph::SetAttribute(int attribute) {
	attribute_ = attribute;
	return true;
}

//-------------------------------------------------------------------
// Querying methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: IsReflexive
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::IsReflexive() const {

	int i;

	for (i = GetNoNodes() - 1; i >= 0; i--) {
		if (!GetEdgeByIndex(i, i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsSymmetric
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::IsSymmetric() const {

	int i, j, no_nodes = GetNoNodes();

	for (i = 1; i < no_nodes; i++) {
		for (j = 0; j < i; j++) {
			if (GetEdgeByIndex(i, j) != GetEdgeByIndex(j, i))
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsTransitive
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::IsTransitive() const {

	int i, j, k, no_nodes = GetNoNodes();

  for (i = 0; i < no_nodes; i++) {
    for (j = 0; j < no_nodes; j++) {
      if (!GetEdgeByIndex(i, j)) {
				for (k = 0; k < no_nodes; k++) {
					if (GetEdgeByIndex(i, k) && GetEdgeByIndex(k, j))
						return false;
				}
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsAntiReflexive
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::IsAntiReflexive() const {

	int i;

	for (i = GetNoNodes() - 1; i >= 0; i--) {
		if (GetEdgeByIndex(i, i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsAntiSymmetric
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::IsAntiSymmetric() const {

	int i, j, no_nodes = GetNoNodes();

	for (i = 1; i < no_nodes; i++) {
		for (j = 0; j < i; j++) {
			if (GetEdgeByIndex(i, j) == GetEdgeByIndex(j, i))
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsAntiTransitive
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::IsAntiTransitive() const {
	Message::Error("Graph::IsAntiTransitive() not implemented.");
	return false;
}

//-------------------------------------------------------------------
// Persistence methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Load(const String &filename, const DecisionTable *table, bool masked) {

	ifstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return Load(stream, table, masked);

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Save(const String &filename, const DecisionTable *table, bool masked) const {

	ofstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return Save(stream, table, masked);

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...: Parses the text file and constructs the graph.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Load(ifstream &stream, const DecisionTable *table, bool masked) {

	// Clear current contents.
	Clear();

	String name;
	int    attribute;

	// Parse header.
	if (!ParseHeader(stream, "begin", table, masked, name, attribute))
		return false;

	SetName(name);
	SetAttribute(attribute);

	// Parse nodes/vertices/domain.
	if (!ParseNodes(stream, "nodes", table, masked, attribute))
		return false;

	// Parse edges.
	if (!ParseEdges(stream, "end", table, masked, attribute))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Save(ofstream &stream, const DecisionTable *table, bool masked) const {

	const String &keyword_header_prefix = "begin ";
	const String &keyword_domain_prefix = "nodes ";
	const String &keyword_footer_prefix = "end ";
	const String &keyword_directed      = " -> ";
	const String &keyword_undirected    = " -- ";
	const String &keyword_expansion     = "*";
	const String &keyword_indent        = "\t";

	// Sanity check.
	if (GetAttribute() < 0 || (table != NULL && GetAttribute() >= table->GetNoAttributes(masked))) {
		Message::Error("Attribute index is negative or does not match table.");
		return false;
	}

	int i, j, no_nodes = GetNoNodes();

	// Save header.
	stream << keyword_header_prefix << GetName() << endl;

	// Save nodes/domain.
	stream << keyword_indent << keyword_domain_prefix;
#if 0
	if (table != NULL && no_nodes == table->GetNoObjects(masked)) {
		stream << keyword_expansion << endl;
	}
	else {
#endif
		for (i = 0; i < no_nodes; i++)
			stream << StaticGetText(GetNodeValue(i), table, GetAttribute(), masked) << " ";
#if 0
	}
#endif
	stream << endl;

	// Save edges/matrix.
	for (i = 0; i < no_nodes; i++) {
		String text1 = StaticGetText(GetNodeValue(i), table, GetAttribute(), masked);
		for (j = 0; j < no_nodes; j++) {
			if (GetEdgeByIndex(i, j)) {   // Only save existing edges.
				String text2 = StaticGetText(GetNodeValue(j), table, GetAttribute(), masked);
				if (GetEdgeByIndex(j, i)) { // Bidirectional edge
					if (i <= j) {             // Avoid writing out duplicates.
						if (i != j)
							stream << keyword_indent << text1 << keyword_undirected << text2 << endl;
						else                    // Format as a unidirectional edge.
							stream << keyword_indent << text1 << keyword_directed << text2 << endl;
					}
				}
				else {                      // Unidirectional edge
					stream << keyword_indent << text1 << keyword_directed << text2 << endl;
				}
			}
		}
	}

	// Save dimensional information.
	stream << keyword_indent << "% |nodes| = " << no_nodes << endl;
	stream << keyword_indent << "% |edges| = " << GetNoEdges() << endl;

	const int max_no_nodes_for_transitivity_check = 100;

	// Compute properties.
	bool is_reflexive     = IsReflexive();
	bool is_symmetric     = IsSymmetric();
	bool is_antisymmetric = IsAntiSymmetric();
	bool is_transitive    = (no_nodes > max_no_nodes_for_transitivity_check) ? false : IsTransitive();

	// Save general property information.
	if (is_reflexive)
		stream << keyword_indent << "% reflexive" << endl;
	if (is_symmetric)
		stream << keyword_indent << "% symmetric" << endl;
	if (is_antisymmetric)
		stream << keyword_indent << "% antisymmetric" << endl;
	if (is_transitive)
		stream << keyword_indent << "% transitive" << endl;
	if (no_nodes > max_no_nodes_for_transitivity_check)
		stream << keyword_indent << "% transitivity too time-consuming to check" << endl;

	bool is_tolerance   = is_reflexive && is_symmetric;
	bool is_equivalence = is_reflexive && is_symmetric && is_transitive;
	bool is_partial     = is_reflexive && is_antisymmetric && is_transitive;

	if (is_tolerance && !is_equivalence)
		stream << keyword_indent << "% => tolerance relation" << endl;
	if (is_equivalence)
		stream << keyword_indent << "% => equivalence relation" << endl;
	if (is_partial)
		stream << keyword_indent << "% => partial order" << endl;

	// Save degree information.
	if (is_symmetric) {
		for (i = 0; i < no_nodes; i++) {
			String text = StaticGetText(GetNodeValue(i), table, GetAttribute(), masked);
			stream << keyword_indent << "% deg(" << text << ") = " << GetInDegreeByIndex(i) << endl;
		}
	}
	else {
		for (i = 0; i < no_nodes; i++) {
			String text = StaticGetText(GetNodeValue(i), table, GetAttribute(), masked);
			stream << keyword_indent << "% deg(in , " << text << ") = " << GetInDegreeByIndex(i) << endl;
			stream << keyword_indent << "% deg(out, " << text << ") = " << GetOutDegreeByIndex(i) << endl;
		}
	}

	// Save footer.
	stream << keyword_footer_prefix << GetName() << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ParseHeader
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reads and parses header.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::ParseHeader(ifstream &stream, const String &prefix, const DecisionTable *table, bool masked, String &name, int &attribute) {

	// Load header line.
	if (!IOKit::LoadLine(stream, name))
		return false;

	// Verify and parse header line.
	if (!name.StartsWith(prefix)) {
		Message::Error("Header line not found, should start with " + prefix + ".");
		return false;
	}

	name.Delete(prefix);
	name.Trim(" \t");

	if (table == NULL && !name.IsInteger()) {
		Message::Error("Could not parse name.");
		return false;
	}

	attribute = (table == NULL) ? name.GetInteger() : table->GetAttributeIndex(name, true, masked);

	if (table != NULL && attribute == Undefined::Integer()) {
		Message::Error("Could not translate \"" + name + "\" to an attribute index.");
		return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ParseNodes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reads and parses node/vertex/domain specification.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::ParseNodes(ifstream &stream, const String &prefix, const DecisionTable *table, bool masked, int attribute) {

	const String &keyword_expansion = "*";

	String domain;

	// Load domain line.
	if (!IOKit::LoadLine(stream, domain))
		return false;

	// Verify and parse domain line.
	if (!domain.StartsWith(prefix)) {
		Message::Error("Line starting with " + prefix + " expected.");
		return false;
	}

	domain.Delete(prefix);
	domain.Trim(" \t");

	String name;

	int i;

	// Build node/vertex/domain index map.
	while (domain.GetToken(name)) {

		// Set/range of integers?
		if (name.Contains("..")) {
			String from = name.Before("..");
			String to = name.After("..");
			if (!from.IsInteger() || !to.IsInteger()) {
				Message::Error("Could not parse integer set specification \"" + name + "\".");
				continue;
			}
			int lower = from.GetInteger();
			int upper = to.GetInteger();
			for (i = lower; i <= upper; i++) {
				if (!AddNode(i)) {
					// ...
					break;
				}
			}
		}

		// Expansion?
		else if (name == keyword_expansion) {
			if (table == NULL) {
				Message::Error("Domain expansion without table not implemented.");
				continue;
			}
			int no_objects    = table->GetNoObjects(masked);
			int no_attributes = table->GetNoAttributes(masked);
			if (attribute < 0 || attribute >= no_attributes) {
				Message::Error("Name of graph (" + GetName() + ") translates to an illegal attribute index.");
				continue;
			}
			for (i = 0; i < no_objects; i++) {
				if (!AddNode(table->GetEntry(i, attribute, masked))) {
					// ...
					break;
				}
			}
		}

		// Proper name?
		else {
			if (!AddNode(StaticGetValue(name, table, attribute, masked))) {
				// ...
			}
		}

	}

	// Build edge representation.
	return MakeAdjacencyMatrix();

}

//-------------------------------------------------------------------
// Method........: ParseEdges
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reads and parses edges/commands.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::ParseEdges(ifstream &stream, const String &footer, const DecisionTable *table, bool masked, int attribute) {

	String line;

	// Continue until footer is encountered or to EOF.
	while (true) {

		// Load next command/edge specification.
		if (!IOKit::LoadLine(stream, line))
			return false;

		// Are all lines read?
		if (line.StartsWith(footer)) {
			String copy(line);
			copy.Delete(footer);
			copy.Trim(" \t");
			if (copy == GetName())
				return true;
		}

		// Parse edge/command.
		if (!ParseEdge(line, table, masked, attribute))
			return false;

	}

	// Programmer's paranoia.
	return false;

}

//-------------------------------------------------------------------
// Method........: ParseEdge
// Author........: Aleksander šhrn
// Date..........:
// Description...: Parses edge/command.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::ParseEdge(const String &entry, const DecisionTable *table, bool masked, int attribute) {

	const String &keyword_reflexive   = "make-reflexive";
	const String &keyword_symmetric   = "make-symmetric";
	const String &keyword_transitive  = "make-transitive";
	const String &keyword_distance    = "make-distance";
	const String &keyword_complement  = "make-complement";
	const String &keyword_directed    = "->";
	const String &keyword_undirected  = "--";
	const String &keyword_expansion   = "*";

	// Reflexivity command?
	if (entry == keyword_reflexive) {
		return MakeReflexive();
	}

	// Symmetry command?
	else if (entry == keyword_symmetric) {
		return MakeSymmetric();
	}

	// Transitivity command?
	else if (entry == keyword_transitive) {
		return MakeTransitive();
	}

	// Inversion command?
	else if (entry == keyword_complement) {
		return MakeComplement();
	}

	// Distance command?
	else if (entry.StartsWith(keyword_distance)) {
		String radius_string = entry.After(keyword_distance);
		radius_string.Trim(" \t");
		if (!radius_string.IsFloat() || (table == NULL && !radius_string.IsInteger())) {
			Message::Error("Illegal radius supplied.");
			return false;
		}
		if (table == NULL)
			return MakeDistance(radius_string.GetInteger());
		float radius_float = radius_string.GetFloat();
#ifndef OBSOLETE
		int radius_integer = MathKit::Round(radius_float * MathKit::Power(10, table->GetAttributeScalingExponent(attribute, masked)));
#else
		int radius_integer = MathKit::Round(radius_float * MathKit::Power((double)10, table->GetAttributeScalingExponent(attribute, masked)));
#endif
		return MakeDistance(radius_integer);
	}

	// Edge entry?
	else if (entry.Contains(keyword_directed) || entry.Contains(keyword_undirected)) {

		// Directed or undirected edge?
		bool is_undirected = entry.Contains(keyword_undirected);

		// Split.
		String text_l = entry.Before(is_undirected ? keyword_undirected : keyword_directed);
		String text_r = entry.After(is_undirected ? keyword_undirected : keyword_directed);

		text_l.Trim(" \t");
		text_r.Trim(" \t");

		// Expand?
		if (text_l == keyword_expansion || text_r == keyword_expansion)
			return ParseExpansion(text_l, text_r, is_undirected ? keyword_undirected : keyword_directed, table, masked, attribute);

		// Lookup node/domain values.
		int value_l = StaticGetValue(text_l, table, attribute, masked);
		int value_r = StaticGetValue(text_r, table, attribute, masked);

		if (value_l == Undefined::Integer() && text_l != Undefined::String()) {
			Message::Error("Failed to look up " + text_l + " in domain map.");
			return false;
		}

		if (value_r == Undefined::Integer() && text_r != Undefined::String()) {
			Message::Error("Failed to look up " + text_r + " in domain map.");
			return false;
		}

		// Lookup matrix indices.
		int index_l = GetNodeIndex(value_l);
		int index_r = GetNodeIndex(value_r);

		if (index_l == Undefined::Integer()) {
			Message::Error("Failed to look up matrix index for " + text_l + ".");
			return false;
		}

		if (index_r == Undefined::Integer()) {
			Message::Error("Failed to look up matrix index for " + text_r + ".");
			return false;
		}

		// Update matrix.
		if (is_undirected)
			return SetEdgeByIndex(index_l, index_r, true) && SetEdgeByIndex(index_r, index_l, true);
		else
			return SetEdgeByIndex(index_l, index_r, true);

	}

	// Unrecognized entry.
	Message::Error("Could not parse entry \"" + entry + "\".");

	return false;

}

//-------------------------------------------------------------------
// Method........: ParseExpansion
// Author........: Aleksander šhrn
// Date..........:
// Description...: Expands and calls ParseEdge.
// Comments......: Called from ParseEdge.
// Revisions.....:
//===================================================================

bool
Graph::ParseExpansion(const String &lhs, const String &rhs, const String &connective, const DecisionTable *table, bool masked, int attribute) {

	const String &keyword_expansion = "*";

	String expansion;

	int i, no_nodes = GetNoNodes();

	// Expand and parse LHS.
	if (lhs == keyword_expansion) {
		for (i = no_nodes - 1; i >= 0; i--) {
			expansion = StaticGetText(GetNodeValue(i), table, attribute, masked) + connective + rhs;
			if (!ParseEdge(expansion, table, masked, attribute))
				return false;
		}
	}

	// Expand and parse RHS.
	if (rhs == keyword_expansion) {
		for (i = no_nodes - 1; i >= 0; i--) {
			expansion = lhs + connective + StaticGetText(GetNodeValue(i), table, attribute, masked);
			if (!ParseEdge(expansion, table, masked, attribute))
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Node/domain methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoNodes
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetNoNodes() const {

#ifdef _DEBUG
	if (map_.size() != matrix_.size())
		Message::Error("Size mismatch between map and matrix!");
#endif

	return map_.size();

}

//-------------------------------------------------------------------
// Method........: AddNode
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Adjacency matrix does not get updated.
// Revisions.....:
//===================================================================

bool
Graph::AddNode(int value) {

	// Is value already a domain member?
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	Map(int, int)::iterator iterator = map_.find(value);

	if (!(iterator == map_.end()))
		return true;

	// Get a free index.
	int index = map_.size();

	// Add to domain.
	map_.insert(Pair(const int, int)(value, index));

	return true;

}

//-------------------------------------------------------------------
// Method........: GetNodeIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetNodeIndex(int value) const {

	Map(int, int)::const_iterator iterator = map_.find(value);

#ifdef _DEBUG
	if (!(iterator == map_.end())) {
		return (*iterator).second;
	}
	else {
		Message::Error("Graph " + GetName() + " does not recognize node value " + String::Format(value) + ".");
		return Undefined::Integer();
	}
#else
	return (*iterator).second;
#endif

}

//-------------------------------------------------------------------
// Method........: GetNodeValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetNodeValue(int index) const {

	Map(int, int)::const_iterator iterator = map_.begin();

	while (!(iterator == map_.end())) {
		if ((*iterator).second == index)
			return (*iterator).first;
		iterator++;
	}

	return Undefined::Integer();

}

//-------------------------------------------------------------------
// Method........: GetInDegreeByIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetInDegreeByIndex(int index) const {

	int no_nodes = GetNoNodes();

	// Sanity check.
	if (index < 0 || index >= no_nodes)
		return 0;

	int from, degree = 0;

	// Compute in-degree.
	for (from = 0; from < no_nodes; from++) {
		if (GetEdgeByIndex(from, index))
			degree++;
	}

	return degree;

}

//-------------------------------------------------------------------
// Method........: GetInDegreeByValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetInDegreeByValue(int value) const {
	return GetInDegreeByIndex(GetNodeIndex(value));
}

//-------------------------------------------------------------------
// Method........: GetOutDegreeByIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetOutDegreeByIndex(int index) const {

	int no_nodes = GetNoNodes();

	// Sanity check.
	if (index < 0 || index >= no_nodes)
		return 0;

	int to, degree = 0;

	// Compute out-degree.
	for (to = 0; to < no_nodes; to++) {
		if (GetEdgeByIndex(index, to))
			degree++;
	}

	return degree;

}

//-------------------------------------------------------------------
// Method........: GetOutDegreeByValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetOutDegreeByValue(int value) const {
	return GetOutDegreeByIndex(GetNodeIndex(value));
}

//-------------------------------------------------------------------
// Edge methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoEdges
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
Graph::GetNoEdges() const {

	int i, j, no_nodes = GetNoNodes(), no_edges = 0;

	for (i = 0; i < no_nodes; i++) {
		for (j = 0; j < no_nodes; j++) {
			if (GetEdgeByIndex(i, j))
				no_edges++;
		}
	}

	return no_edges;

}

//-------------------------------------------------------------------
// Method........: MakeAdjacencyMatrix
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::MakeAdjacencyMatrix() {

	// Resize current matrix.
	matrix_.erase(matrix_.begin(), matrix_.end());

	int i, dimension = map_.size();

	// Avoid spurious allocations.
	matrix_.reserve(dimension);

	Bits bits(dimension, false);

	// Build matrix.
	for (i = 0; i < dimension; i++)
		matrix_.push_back(bits);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetEdgeByIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::GetEdgeByIndex(int index_from, int index_to) const {

#ifdef _DEBUG
	if (index_from < 0 || index_from >= matrix_.size()) {
		Message::Error("Illegal source node index (" + String::Format(index_from) + ") passed to graph " + GetName() + ".");
		return false;
	}

	if (index_to < 0 || index_to >= matrix_[index_from].GetSize()) {
		Message::Error("Illegal destination node index (" + String::Format(index_to) + ") passed to graph " + GetName() + ".");
		return false;
	}
#endif

	return matrix_[index_from][index_to];

}

//-------------------------------------------------------------------
// Method........: SetEdgeByIndex
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::SetEdgeByIndex(int index_from, int index_to, bool flag) {

#ifdef _DEBUG
	if (index_from < 0 || index_from >= matrix_.size()) {
		Message::Error("Illegal source node index (" + String::Format(index_from) + ") passed to graph " + GetName() + ".");
		return false;
	}

	if (index_to < 0 || index_to >= matrix_[index_from].GetSize()) {
		Message::Error("Illegal destination node index (" + String::Format(index_to) + ") passed to graph " + GetName() + ".");
		return false;
	}
#endif

	// Update matrix.
	matrix_[index_from][index_to] = flag;

	return true;

}

//-------------------------------------------------------------------
// Method........: GetEdgeByValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::GetEdgeByValue(int value_from, int value_to) const {

	int index_from = GetNodeIndex(value_from);

	if (value_from == value_to)
		return GetEdgeByIndex(index_from, index_from);
	else
		return GetEdgeByIndex(index_from, GetNodeIndex(value_to));

}

//-------------------------------------------------------------------
// Method........: SetEdgeByValue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::SetEdgeByValue(int value_from, int value_to, bool flag) {

	int index_from = GetNodeIndex(value_from);

	if (value_from == value_to)
		return SetEdgeByIndex(index_from, index_from, flag);
	else
		return SetEdgeByIndex(index_from, GetNodeIndex(value_to), flag);

}

//-------------------------------------------------------------------
// Method........: MakeReflexive
// Author........: Aleksander šhrn
// Date..........:
// Description...: Adds a "loop edge" to each vertex.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::MakeReflexive() {

	int i;

	for (i = GetNoNodes() - 1; i >= 0; i--)
		SetEdgeByIndex(i, i, true);

	return true;

}

//-------------------------------------------------------------------
// Method........: MakeSymmetric
// Author........: Aleksander šhrn
// Date..........:
// Description...: For each directed edge, adds an edge in the other
//                 direction.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::MakeSymmetric() {

	int i, j, no_nodes = GetNoNodes();

	for (i = 1; i < no_nodes; i++) {
		for (j = 0; j < i; j++) {
			if (GetEdgeByIndex(i, j))
				SetEdgeByIndex(j, i, true);
			else if (GetEdgeByIndex(j, i))
				SetEdgeByIndex(i, j, true);
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: MakeTransitive
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the transitive closure.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::MakeTransitive() {
	return Warshall();
}

//-------------------------------------------------------------------
// Method........: MakeComplement
// Author........: Aleksander šhrn
// Date..........:
// Description...: Inverts the adjacency matrix.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::MakeComplement() {

	int i;

	for (i = 0; i < matrix_.size(); i++)
		matrix_[i].Invert();

	return true;

}

//-------------------------------------------------------------------
// Method........: MakeDistance
// Author........: Aleksander šhrn
// Date..........:
// Description...: Looks up the value at each node. Adds an edge
//                 between two nodes if their values are "close
//                 enough". Existing edges are left alone.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::MakeDistance(int radius) {

	int i, j, no_nodes = GetNoNodes();

	for (i = 1; i < no_nodes; i++) {

		// Get value at node i.
		int value_i = GetNodeValue(i);

		// Skip it if it's the magic "missing value" node.
		if (value_i == Undefined::Integer())
			continue;

		for (j = 0; j <= i; j++) {

			// Get value at node j.
			int value_j  = GetNodeValue(j);

			// Skip it if it's the magic "missing value" node.
			if (value_j == Undefined::Integer())
				continue;

			// Compute absolute distance.
			int distance = value_i - value_j;
			if (distance < 0)
				distance = -distance;

			// Add an (i, j) edge if the distance is small enough.
			if (distance <= radius) {
				SetEdgeByIndex(i, j, true);
				SetEdgeByIndex(j, i, true);
			}

		}
	}

	return true;

}

//-------------------------------------------------------------------
// Misc. algorithms.
//===================================================================

//-------------------------------------------------------------------
// Method........: Warshall
// Author........: Aleksander šhrn
// Date..........:
// Description...: Transitive closure by Warshall's algorithm.
// Comments......:
// Revisions.....:
//===================================================================

bool
Graph::Warshall() {

	int i, j, k, no_nodes = GetNoNodes();

  for (k = 0; k < no_nodes; k++) {
    for (i = 0; i < no_nodes; i++) {
      for (j = 0; j < no_nodes; j++) {
        if (!GetEdgeByIndex(i, j))
					SetEdgeByIndex(i, j, GetEdgeByIndex(i, k) && GetEdgeByIndex(k, j));
			}
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Dijkstra
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the length of the shortest path
//                 from one node to every other node, computed by
//                 Dijkstra's algorithm.
//
// Comments......: Current implementation uses unit costs/distances.
//                 Could take advantage of symmetry, if present.
// Revisions.....:
//===================================================================

bool
Graph::Dijkstra(int index, SPVector &distances) const {

	int i, j, k, no_nodes = GetNoNodes();

	// Sanity check.
	if (index < 0 || index >= no_nodes)
		return false;

	// Define a value for "infinite distance".
	const SPDistance d_infinite = Undefined::Integer();

	// Resize and initialize SP vector.
	distances.erase(distances.begin(), distances.end());
	distances.reserve(no_nodes);

	for (i = 0; i < no_nodes; i++) {
		distances.push_back(d_infinite);
		if (GetEdgeByIndex(index, i))
			distances[i] = 1;
	}

	distances[index] = 0;

	// Initialize working structures.
	Vector(int) S; S.reserve(no_nodes);
	Vector(int) R; R.reserve(no_nodes);

	for (i = 0; i < no_nodes; i++) {
		if (i == index)
			S.push_back(i);
		else
			R.push_back(i);
	}

	// Do greedy algorithm.
	while (!R.empty()) {

		SPDistance d_minimum = d_infinite;

		// Choose a vertex k in R such that d(k) is a minimum.
		for (j = R.size() - 1; j >= 0; j--) {
			SPDistance d_current = distances[R[j]];
			if (d_current == d_infinite)
				continue;
			if (d_minimum == d_infinite) {
				i = j;
				k = R[j];
				d_minimum = d_current;
			}
			else {
				if (d_current < d_minimum) {
					i = j;
					k = R[j];
					d_minimum = d_current;
				}
			}
		}

		if (d_minimum == d_infinite)
			return true;

		// Move k to S from R.
		S.push_back(k);
		R[i] = R[R.size() - 1];
		R.pop_back();

		// Update d for each vertex in R.
		for (j = R.size() - 1; j >= 0; j--) {
			i = R[j];
			SPDistance d_old = distances[i];
			SPDistance d_new = (GetEdgeByIndex(k, i)) ? (d_minimum + 1) : (d_infinite);
			if (d_new == d_infinite)
				continue;
			if ((d_old == d_infinite) || (d_new < d_old))
				distances[i] = d_new;
		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Floyd
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) the all-pairs shortest paths
//                 (APSP) matrix, computed using Floyd's algorithm.
//
// Comments......: Current implementation uses unit costs/distances.
//                 Could take advantage of symmetry, if present.
// Revisions.....:
//===================================================================

bool
Graph::Floyd(APSPMatrix &distances) const {

	int i, j, k, no_nodes = GetNoNodes();

	// Define a value for "infinite distance".
	const SPDistance d_infinite = Undefined::Integer();

	// Resize and initialize APSP matrix.
	distances.erase(distances.begin(), distances.end());
	distances.reserve(no_nodes);

	for (i = 0; i < no_nodes; i++) {
		distances.push_back(SPVector());
		distances[i].reserve(no_nodes);
		for (j = 0; j < no_nodes; j++) {
			if (GetEdgeByIndex(i, j))
				distances[i].push_back(1);
			else
				distances[i].push_back(d_infinite);
		}
		distances[i][i] = 0;
	}

	// Compute APSPs.
  for (k = 0; k < no_nodes; k++) {
    for (i = 0; i < no_nodes; i++) {
			if (distances[i][k] == d_infinite)
				continue;
      for (j = 0; j < no_nodes; j++) {
				if (distances[k][j] == d_infinite)
					continue;
				SPDistance d_old = distances[i][j];
				SPDistance d_new = distances[i][k] + distances[k][j];
				if ((d_old == d_infinite) || (d_new < d_old))
					distances[i][j] = d_new;
			}
		}
	}

	return true;

}
 
