//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/partitionkit.h>
#include <kernel/utilities/creator.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reduct.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// STL comparator methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Comparison operator
// Author........: Aleksander šhrn
// Date..........:
// Description...: Compares the two specified objects in the a
//                 decision table with respect to a given
//                 set of attributes. See STL specifications.
// Comments......: Lexicographical comparison between objects.
// Revisions.....:
//===================================================================

bool
PartitionKit::Compare::operator()(int i, int j) const {

	// No comparison needed.
	if (i == j)
		return false;

	int k, no_attributes = attributes_->size();

	// Compare all relevant table entries.
	for (k = 0; k < no_attributes; k++) {

		// Get attribute.
		int attribute = (*attributes_)[k];

		// Get table entries.
		int entry_i = table_->GetEntry(i, attribute, masked_);
		int entry_j = table_->GetEntry(j, attribute, masked_);

		// Object i is "smaller than" object j.
		if (entry_i < entry_j)
			return true;

		// Object i is "larger than" object j.
		else if (entry_i > entry_j)
			return false;

	}

	// The objects are "equal".
	return false;

}

//-------------------------------------------------------------------
// Methods for class PartitionKit.
//===================================================================

//-------------------------------------------------------------------
// Method........: ComputePartitionIndices
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
PartitionKit::ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const DecisionTable &table, bool all, bool masked, Vector(int) *cardinalities) {

	int i, no_attributes = table.GetNoAttributes(masked);

	Vector(int) attributes;
	attributes.reserve(no_attributes);

	for (i = 0; i < no_attributes; i++) {
		if (all || table.IsCondition(i, masked))
			attributes.push_back(i);
	}

	return ComputePartitionIndices(indices, no_partitions, attributes, table, masked, cardinalities);

}

//-------------------------------------------------------------------
// Method........: ComputePartitionIndices
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
PartitionKit::ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const String &attributes, const DecisionTable &table, bool masked, Vector(int) *cardinalities) {

	// Create the attribute set to partition by.
	Handle<Reduct> reduct = Creator::Reduct(attributes, &table, masked);

	if (reduct == NULL) {
		Message::Error("The partitioning attribute set is not properly specified.");
		//Corrected 040823 Robin Andersson
		//return NULL;
		return false;
	}

	return ComputePartitionIndices(indices, no_partitions, *reduct, table, masked, cardinalities);

}

//-------------------------------------------------------------------
// Method........: ComputePartitionIndices
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
PartitionKit::ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const Reduct &attributes, const DecisionTable &table, bool masked, Vector(int) *cardinalities) {

	int i, no_attributes = attributes.GetNoAttributes();

	Vector(int) v;
	v.reserve(no_attributes);

	for (i = 0; i < no_attributes; i++)
		v.push_back(attributes.GetAttribute(i));

	return ComputePartitionIndices(indices, no_partitions, v, table, masked, cardinalities);

}

//-------------------------------------------------------------------
// Method........: ComputePartitionIndices
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a vector that maps from
//                 object indices to eq. class indices. Hence, two
//                 objects that map to the same index belong to the
//                 same eq. class.
//
//                 The returned vector has indices from 0 to n - 1,
//                 where n is the number of equivalence classes.
//                 The number n is also returned (in-place).
//
//                 Optionally, the cardinalities of each equivalence
//                 class are returned (in-place).
//
// Comments......: Sort-and-scan O(nlogn + n) implementation.
//
//                 To do: Return generalized decisions, too.
// Revisions.....:
//===================================================================

bool
PartitionKit::ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const Vector(int) &attributes, const DecisionTable &table, bool masked, Vector(int) *cardinalities) {

	int i, no_objects = table.GetNoObjects(masked);

	Vector(int) sorted;

	// Initialize vectors.
	indices.erase(indices.begin(), indices.end());
	indices.reserve(no_objects);
	sorted.reserve(no_objects);

	for (i = 0; i < no_objects; i++) {
		sorted.push_back(i);
		indices.push_back(0);
	}

	Compare comparator(table, masked, attributes);

	// Sort object indices.
	std::sort(sorted.begin(), sorted.end(), comparator);

	int j, k = 0, partition_no = 0;

	// Scan through the virtually sorted table and fill the partition index vector.
	while (k < no_objects) {
		int size = 1;
		while (k + size < no_objects && !comparator(sorted[k], sorted[k + size]))
			size++;
		for (j = 0; j < size; j++)
			indices[sorted[k + j]] = partition_no;
		partition_no++;
		k += size;
	}

	// Return how many equivalence classes there are.
	no_partitions = partition_no;

	// Return cardinalities as well?
	if (cardinalities != NULL) {
		cardinalities->erase(cardinalities->begin(), cardinalities->end());
		cardinalities->reserve(no_partitions);
		for (i = 0; i < no_partitions; i++)
			cardinalities->push_back(0);
		for (i = 0; i < no_objects; i++)
			(*cardinalities)[indices[i]]++;
	}

	return true;

}
 
