//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/indiscernibilitygraphexporter.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/discernibilitymatrix.h>
#include <kernel/structures/indiscernibilitygraph.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/discerner.h>

#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class IndiscernibilityGraphExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

IndiscernibilityGraphExporter::IndiscernibilityGraphExporter() {
	reflexive_   = true;
	deg_         = false;
	apsp_        = false;
	ignore_      = false;
	cardinality_ = 0;
}

IndiscernibilityGraphExporter::~IndiscernibilityGraphExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(IndiscernibilityGraphExporter, INDISCERNIBILITYGRAPHEXPORTER, DiscernibilityExporter)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
IndiscernibilityGraphExporter::GetParameters() const {

	String parameters = DiscernibilityExporter::GetParameters();

	if (!parameters.EndsWith(Keyword::Separator()))
		parameters += Keyword::Separator();

	// Include reflexive stuff?
	parameters += Keyword::Data() + Keyword::Dot() + Keyword::Reflexive();
	parameters += Keyword::Assignment();
	parameters += String::Format(IncludeReflexiveData());

	parameters += Keyword::Separator();

	// Include vertex degree data?
	parameters += Keyword::Data() + Keyword::Dot() + Keyword::Degree();
	parameters += Keyword::Assignment();
	parameters += String::Format(IncludeDegreeData());

	parameters += Keyword::Separator();

	// Include APSP data?
	parameters += Keyword::Data() + Keyword::Dot() + Keyword::APSP();
	parameters += Keyword::Assignment();
	parameters += String::Format(IncludeAPSPData());

	parameters += Keyword::Separator();

	// Ignore low-cardinality entries?
	parameters += Keyword::Cardinality();
	parameters += Keyword::Assignment();
	parameters += String::Format(IgnoreLowCardinalityEntries());

	// Maximum cardinality, if relevant.
	if (IgnoreLowCardinalityEntries()) {

		parameters += Keyword::Separator();

		parameters += Keyword::Cardinality() + Keyword::Dot() + Keyword::Threshold();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetMaximumCardinality());

	}

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraphExporter::SetParameter(const String &keyword, const String &value) {

	// Reflexive data.
	if (keyword == Keyword::Data() + Keyword::Dot() + Keyword::Reflexive() && value.IsBoolean())
		return IncludeReflexiveData(value.GetBoolean());

	// Vertex degree data.
	if (keyword == Keyword::Data() + Keyword::Dot() + Keyword::Degree() && value.IsBoolean())
		return IncludeDegreeData(value.GetBoolean());

	// APSP data.
	if (keyword == Keyword::Data() + Keyword::Dot() + Keyword::APSP() && value.IsBoolean())
		return IncludeAPSPData(value.GetBoolean());

	// Ignore low-cardinality entries?
	if (keyword == Keyword::Cardinality() && value.IsBoolean())
		return IgnoreLowCardinalityEntries(value.GetBoolean());

	// Cardinality threshold.
	if ((keyword == Keyword::Cardinality() + Keyword::Dot() + Keyword::Threshold()) && value.IsInteger())
		return SetMaximumCardinality(value.GetInteger());

	return DiscernibilityExporter::SetParameter(keyword, value);

}

//-------------------------------------------------------------------
// Methods inherited from Exporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportPrologue
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Overloaded since we want the prologue in GraphViz
//                 comment format.
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraphExporter::ExportPrologue(ofstream &stream, const Structure &structure) const {

	stream << "/* ===========================================================" << endl;
	stream << "Output from ROSETTA, " + SystemKit::GetUser() + " " + SystemKit::GetTimestamp() << endl;
	stream << endl;
	stream << IdHolder::GetClassname(GetId()) << endl;
	stream << '{' << GetParameters() << '}' << endl;
	stream << endl;
	stream << structure.GetName() << endl;
	stream << endl;
	stream << "This graph can be visualized by GraphViz from AT&T Research." << endl;
	stream << "=========================================================== */" << endl;
	stream << endl;

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from DiscernibilityExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Creating a graph from a full matrix is really
//                 only suitable for not too large datasets.
//                 Resolve later.
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraphExporter::ExportData(ofstream &stream, const Discerner &discerner, const DecisionTable &table, bool masked, int name_index_unmasked) const {

	DiscernibilityMatrix  matrix;
	IndiscernibilityGraph graph;

	Message message;

	message.Notify("Creating indiscernibility graph...");

	float precision   = Undefined::Float();
	int   cardinality = IgnoreLowCardinalityEntries() ? GetMaximumCardinality() : 0;

	// Create it...
	if (ModuloDecision()) {
		if (!matrix.Create(table, masked, true, discerner, precision) || !graph.Create(matrix, cardinality))
			return false;
	}
	else {
		if (!graph.Create(table, masked, discerner, true, cardinality, name_index_unmasked))
			return false;
	}

	graph.SetName("indiscernibility");

	// ...and export it.
	return ExportGraph(stream, graph, table, masked);

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportGraph
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from the ExportData method
//.
//                 Graph::GetAttribute() should return the index
//                 (unmasked) of the attribute in the given table that
//                 specifies the names of the nodes in the graph, if
//                 such an attribute was given by the user.
//
//                 The graph is assumed to be symmetric.
//
// Revisions.....: Aš991023: Changed output format to include APSP
//                           and vertex degree data.
//===================================================================

bool
IndiscernibilityGraphExporter::ExportGraph(ofstream &stream, const IndiscernibilityGraph &graph, const DecisionTable &table, bool masked) const {

	int i, j;

	Message message;

	message.Notify("Exporting indiscernibility graph...");

	/*
	// Export graph, internal format.
	if (graph.GetAttribute() == Undefined::Integer()) {
		if (!graph.Save(stream, NULL, masked))
			return false;
	}
	else {
		if (!graph.Save(stream, &table, false))
			return false;
	}

	stream << endl;
	*/

	// Export header, "dot" format.
	stream << "graph " << graph.GetName() << " {" << endl;

	// Export body, "dot" format. Graph is assumed to be symmetric.
	for (i = 0; i < graph.GetNoNodes(); i++) {
		String nodename_i = (graph.GetAttribute() == Undefined::Integer()) ? String::Format(graph.GetNodeValue(i)) : table.GetDictionaryEntry(graph.GetAttribute(), graph.GetNodeValue(i), false);
		for (j = 0; j <= i; j++) {
			if ((j == i) && !IncludeReflexiveData())
				continue;
			if (graph.GetEdgeByIndex(i, j)) {
				String nodename_j = (graph.GetAttribute() == Undefined::Integer()) ? String::Format(graph.GetNodeValue(j)) : table.GetDictionaryEntry(graph.GetAttribute(), graph.GetNodeValue(j), false);
				stream << '\t' << nodename_i << " -- " << nodename_j << endl;
			}
		}
	}

	// Export footer, "dot" format.
	stream << "}" << endl << endl;

	// Export vertex degree data?
	if (IncludeDegreeData() && !ExportDegreeData(stream, graph, table, masked))
		return false;

	// Export APSP data?
	if (IncludeAPSPData() && !ExportAPSPData(stream, graph, table, masked))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportDegreeData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from the ExportGraph method
//.
//                 Graph::GetAttribute() should return the index
//                 (unmasked) of the attribute in the given table that
//                 specifies the names of the nodes in the graph, if
//                 such an attribute was given by the user.
//
//                 The graph is assumed to be symmetric.
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraphExporter::ExportDegreeData(ofstream &stream, const IndiscernibilityGraph &graph, const DecisionTable &table, bool masked) const {

	int i;

	Message message;

	message.Notify("Exporting vertex degree data...");

	stream << "/* ===========================================================" << endl;
	stream << "Reflexive loops not included in degree count." << endl;
	stream << "Only vertices with non-zero degree counts are listed." << endl << endl;

	// Graph is assumed to be symmetric.
	for (i = 0; i < graph.GetNoNodes(); i++) {
		int deg = graph.GetInDegreeByIndex(i);
		if (graph.GetEdgeByIndex(i, i))
			deg--;
		if (deg >= 1) {
			String nodename = (graph.GetAttribute() == Undefined::Integer()) ? String::Format(graph.GetNodeValue(i)) : table.GetDictionaryEntry(graph.GetAttribute(), graph.GetNodeValue(i), false);
			stream << "deg(" << nodename << ") = " << deg << endl;
		}
	}

	stream << "=========================================================== */" << endl << endl;

	return true;

}

//-------------------------------------------------------------------
// Method........: ExportAPSPData
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Called from the ExportGraph method
//.
//                 Graph::GetAttribute() should return the index
//                 (unmasked) of the attribute in the given table that
//                 specifies the names of the nodes in the graph, if
//                 such an attribute was given by the user.
//
//                 The graph is assumed to be symmetric.
// Revisions.....:
//===================================================================

bool
IndiscernibilityGraphExporter::ExportAPSPData(ofstream &stream, const IndiscernibilityGraph &graph, const DecisionTable &table, bool masked) const {

	int i, j;

	Message message;

	message.Notify("Computing APSP data...");

	Graph::APSPMatrix distances;

	// Compute all-pairs shortest paths.
	if (!graph.Floyd(distances)) {
		message.Error("Floyd's algorithm failed.");
		return false;
	}

	// Sanity check.
	if (distances.size() != graph.GetNoNodes()) {
		message.Error("Sanity check on Floyd output failed.");
		return false;
	}

	message.Notify("Exporting APSP data...");

	stream << "/* ===========================================================" << endl;
	stream << "sp(x, x) = 0 for all x." << endl;
	stream << "sp(x, y) = sp(y, x) for all x, y." << endl;
	stream << "sp(x, z) <= sp(x, y) + sp(y, z) for all x, y, x." << endl;
	stream << endl;
	stream << "Only finite distances are listed." << endl;
	stream << "Reflexivity and symmetry are implicit." << endl << endl;

	// Graph is assumed to be symmetric.
	for (i = 0; i < distances.size(); i++) {
		String nodename_i = (graph.GetAttribute() == Undefined::Integer()) ? String::Format(graph.GetNodeValue(i)) : table.GetDictionaryEntry(graph.GetAttribute(), graph.GetNodeValue(i), false);
		for (j = i + 1; j < distances.size(); j++) {
			if (distances[i][j] != Undefined::Integer()) {
				String nodename_j = (graph.GetAttribute() == Undefined::Integer()) ? String::Format(graph.GetNodeValue(j)) : table.GetDictionaryEntry(graph.GetAttribute(), graph.GetNodeValue(j), false);
				stream << "sp(" << nodename_i << ", " << nodename_j << ") = " << distances[i][j] << endl;
			}
		}
	}

	stream << "=========================================================== */" << endl << endl;

	return true;

}
 
IndiscernibilityGraphExporter *
IndiscernibilityGraphExporter::Clone() {
	return new IndiscernibilityGraphExporter;
}
