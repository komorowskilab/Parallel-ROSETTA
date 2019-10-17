//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __PARTITIONKIT_H__
#define __PARTITIONKIT_H__

#include <copyright.h>

#include <kernel/basic/vector.h>
#include <cstddef>
//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Reduct;
class String;

//-------------------------------------------------------------------
// Class.........: PartitionKit
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Utility functions for computing the equivalence
//                 classes in a decision table.
// Revisions.....:
//===================================================================

class PartitionKit {
private:

	//- STL comparator.................................................
	struct Compare {
	private:

		//- Private variables............................................
		const DecisionTable *table_;
		bool                 masked_;
		const Vector(int)   *attributes_;

	public:

		//- Constructor..................................................
		Compare(const DecisionTable &table, bool masked, const Vector(int) &attributes) {table_ = &table, masked_ = masked; attributes_ = &attributes;}

		//- Comparison operator..........................................
		bool operator()(int i, int j) const;

	};

public:

	//- Partitioning methods...........................................
	static bool ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const DecisionTable &table, bool all, bool masked, Vector(int) *cardinalities = NULL);
	static bool ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const Reduct &attributes, const DecisionTable &table, bool masked, Vector(int) *cardinalities = NULL);
	static bool ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const String &attributes, const DecisionTable &table, bool masked, Vector(int) *cardinalities = NULL);
	static bool ComputePartitionIndices(Vector(int) &indices, int &no_partitions, const Vector(int) &attributes, const DecisionTable &table, bool masked, Vector(int) *cardinalities = NULL);

};

#endif 
