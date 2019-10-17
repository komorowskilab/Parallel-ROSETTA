//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/discerner.h>
#include <kernel/utilities/creator.h>
#include <kernel/utilities/iokit.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class Discerner.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Discerner::Discerner() {
}

Discerner::Discerner(const Discerner &in) {
	graphs_ = in.graphs_;
}

Discerner::~Discerner() {
}

//-------------------------------------------------------------------
// IDG methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: SetIDG
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if an IDG is present for the given
//                 attribute.
// Comments......:
// Revisions.....:
//===================================================================

bool
Discerner::SetIDG(Handle<Graph> graph, int attribute) {

	if (attribute < 0)
		return false;

	// Grow array, if needed.
	while (graphs_.size() <= attribute)
		graphs_.push_back(Handle<Graph>(NULL));

	graphs_[attribute] = graph;

	return true;

}

//-------------------------------------------------------------------
// Method........: LoadIDGs
// Author........: Aleksander šhrn
// Date..........:
// Description...: Loads the indiscernibility definition graphs (IDGs)
//                 from file.
// Comments......:
// Revisions.....:
//===================================================================

bool
Discerner::LoadIDGs(const String &filename, const DecisionTable &table, bool masked) {

	ifstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return LoadIDGs(stream, table, masked);

}

//-------------------------------------------------------------------
// Method........: LoadIDGs
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Discerner::LoadIDGs(ifstream &stream, const DecisionTable &table, bool masked) {

	int i, no_attributes = table.GetNoAttributes(masked);

	// Initialize present IDG array.
	graphs_.erase(graphs_.begin(), graphs_.end());
	graphs_.reserve(no_attributes);

	for (i = 0; i < no_attributes; i++)
		graphs_.push_back(Handle<Graph>(NULL));

	// Instantiate first graph.
	Handle<Graph> graph = Creator::Graph();

	// Load all graphs.
	while (graph->Load(stream, &table, masked)) {

		// Get index of attribute the current graph applies to.
		int attribute = graph->GetAttribute();

		if (attribute < 0 || attribute >= no_attributes) {
			Message::Error("Graph " + graph->GetName() + " does not correspond to a valid attribute index.");
			graphs_.erase(graphs_.begin(), graphs_.end());
			return false;
		}

		// Place graph in its proper slot.
		SetIDG(graph, attribute);

		// Instantiate next graph.
		graph = Creator::Graph();

	}

	// Make sure the last load failed because it reached EOF.
	if (!IOKit::IsEOF(stream)) {
		graphs_.erase(graphs_.begin(), graphs_.end());
		return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AllReflexive
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if all IDGs are reflexive.
// Comments......:
// Revisions.....:
//===================================================================

bool
Discerner::AllReflexive() const {

	int i;

	for (i = GetNoIDGs() - 1; i >= 0; i--) {
		if (HasIDG(i)) {
			if (!GetIDG(i)->IsReflexive())
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AllSymmetric
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if all IDGs are symmetric.
// Comments......:
// Revisions.....:
//===================================================================

bool
Discerner::AllSymmetric() const {

	int i;

	for (i = GetNoIDGs() - 1; i >= 0; i--) {
		if (HasIDG(i)) {
			if (!GetIDG(i)->IsSymmetric())
				return false;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AllTransitive
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if all IDGs are transitive.
// Comments......:
// Revisions.....:
//===================================================================

bool
Discerner::AllTransitive() const {

	int i;

	for (i = GetNoIDGs() - 1; i >= 0; i--) {
		if (HasIDG(i)) {
			if (!GetIDG(i)->IsTransitive())
				return false;
		}
	}

	return true;

}
 
