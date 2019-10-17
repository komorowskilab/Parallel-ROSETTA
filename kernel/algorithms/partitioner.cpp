//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/partitioner.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/equivalenceclass.h>
#include <kernel/structures/equivalenceclasses.h>

#include <kernel/utilities/partitionkit.h>
#include <kernel/utilities/creator.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class Partitioner.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Partitioner::Partitioner() {
	SetAttributes(Undefined::String());
}

Partitioner::~Partitioner() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Partitioner, PARTITIONER, Algorithm)

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
Partitioner::GetParameters() const {

	String parameters;

	parameters += Keyword::Attributes();
	parameters += Keyword::Assignment();
	parameters += GetAttributes();

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
Partitioner::SetParameter(const String &keyword, const String &value) {

	// Partitioning attributes.
	if (keyword == Keyword::Attributes())
		return SetAttributes(value);

	return false;

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
Partitioner::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Assumes that library clients use handles.
// Revisions.....:
//===================================================================

Structure *
Partitioner::Apply(Structure &structure) const {

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Operate on a masked table.
	bool masked = true;

	Vector(int) indices;
	int no_partitions;

	// Compute the partition.
	if (!PartitionKit::ComputePartitionIndices(indices, no_partitions, GetAttributes(), *table, masked))
		return NULL;

	Map(int, int) histogram;

	// Gather some cardinality statistics.
	if (!MathKit::Distribution(histogram, indices))
		return NULL;

	// Initialize partition holder.
	Handle<EquivalenceClasses> partition = Creator::EquivalenceClasses();

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	Map(int, int)::iterator iterator = histogram.begin();

	while (!(iterator == histogram.end())) {
		int size = (*iterator).second;
		Handle<EquivalenceClass> eqclass = Creator::EquivalenceClass();
		if (!eqclass->Reserve(size) || !partition->AppendStructure(eqclass.GetPointer()))
			return NULL;
		iterator++;
	}

	int i;

	// Fill the partition holder.
	for (i = 0; i < indices.size(); i++) {
		if (!partition->GetEquivalenceClass(indices[i])->InsertObject(i))
			return NULL;
	}

  return partition.Release();

}

 

Partitioner *
Partitioner::Clone() {
	return new Partitioner;
}
