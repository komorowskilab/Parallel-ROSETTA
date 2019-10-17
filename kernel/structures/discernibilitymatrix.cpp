 //-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/discernibilitymatrix.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/generalizeddecision.h>

#include <kernel/utilities/partitionkit.h>
#include <kernel/utilities/discerner.h>
#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>

#include <kernel/basic/algorithm.h>

#include <kernel/system/fstream.h>

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

DiscernibilityMatrix::DiscernibilityMatrix(const DiscernibilityMatrix &in) : Structure(in) {

	// Copy index stuff.
	map_             = in.map_;
	representatives_ = in.representatives_;

	matrix_.reserve(in.matrix_.size());

	int i;

	// Duplicate matrix entries.
	for (i = 0; i < in.matrix_.size(); i++) {
		if (in.matrix_[i] == NULL)
			matrix_.push_back(NULL);
		else
			matrix_.push_back(new Bits(*(in.matrix_[i])));
	}

}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

DiscernibilityMatrix::DiscernibilityMatrix() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DiscernibilityMatrix::~DiscernibilityMatrix() {
	Clear();
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DiscernibilityMatrix, DISCERNIBILITYMATRIX, Structure)

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
DiscernibilityMatrix::Load(ifstream &/*stream*/) {
	return false;
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
DiscernibilityMatrix::Save(ofstream &stream) const {
	return Save(stream, '-');
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
DiscernibilityMatrix::Duplicate() const {
  return new DiscernibilityMatrix(*this);
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
DiscernibilityMatrix::Clear() {

	int i;

	// Delete matrix entries.
	for (i = matrix_.size() - 1; i >= 0; i--) {
		if (matrix_[i] != NULL)
			delete matrix_[i];
	}

	map_.erase(map_.begin(), map_.end());
	representatives_.erase(representatives_.begin(), representatives_.end());
	matrix_.erase(matrix_.begin(), matrix_.end());

}

//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: AllocateMatrix
// Author........: Aleksander šhrn
// Date..........:
// Description...: Physically allocates the matrix. Enables lazy
//                 allocation.
//
//                 Returns true if allocation took place.
//
// Comments......: The matrix framework (representatives and mappings)
//                 must have been initialized prior to calling this
//                 method!
// Revisions.....:
//===================================================================

bool
DiscernibilityMatrix::AllocateMatrix() {

	// Matrix already allocated?
	if (HasAllocatedMatrix())
		return false;

	// How many unique objects are there?
	unsigned int no_unique_objects = representatives_.size();

	if (no_unique_objects == 0)
		return false;

	Message::Debug("Allocating matrix...");

	unsigned int tmp0 = no_unique_objects;
	unsigned int tmp1 = no_unique_objects - 1;

	// How many entries will there be in the reduced matrix? Try to avoid numerical overflows.
	if (tmp0 % 2 == 0)
		tmp0 /= 2;
	else
		tmp1 /= 2;

	unsigned int no_entries = tmp0 * tmp1;

	// Erase, resize and initialize.
	matrix_.erase(matrix_.begin(), matrix_.end());
	matrix_.reserve(no_entries);
	matrix_.insert(matrix_.end(), no_entries, NULL);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetDimension
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the logical (or reduced, physical)
//                 dimension of the matrix.
// Comments......:
// Revisions.....:
//===================================================================

int
DiscernibilityMatrix::GetDimension(bool reduced) const {

	if (!reduced)
		return map_.size();
	else
		return representatives_.size();

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990301: Added lazy allocation stuff.
//===================================================================

const Bits *
DiscernibilityMatrix::GetEntry(int i, int j, bool reduced) const {

	// Allocation not done?
	if (!HasAllocatedMatrix())
		return NULL;

	// Map to eq. class indices, if needed.
	if (!reduced) {
		i = GetMap(i);
		j = GetMap(j);
	}

	// Diagonal entry?
	if (i == j)
		return NULL;

	return matrix_[GetIndex(i, j)];

}

//-------------------------------------------------------------------
// Method........: RemoveEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets entry (i, j) in the matrix to NULL. As a
//                 side-effect, all entries for objects belonging
//                 to the same pair of equivalence classes are
//                 set to NULL as well.
// Comments......:
// Revisions.....: Aš 990301: Added lazy allocation stuff.
//===================================================================

bool
DiscernibilityMatrix::RemoveEntry(int i, int j, bool destroy, bool reduced) {

	// Allocation not done?
	if (!HasAllocatedMatrix())
		return true;

	// Map to eq. class indices, if needed.
	if (!reduced) {
		i = GetMap(i);
		j = GetMap(j);
	}

	// Diagonal entries are already empty since indiscernibility is reflexive.
	if (i == j)
		return true;

	// Convert to triangular matrix indexing scheme.
	int index = GetIndex(i, j);

	// Empty current contents. Delete it, if specified.
	if (destroy && matrix_[index] != NULL)
		delete matrix_[index];

	matrix_[index] = NULL;

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets all entries for the specified row/column in
//                 the matrix to NULL. As a side-effect, all entries
//                 for object pairs in which one object belongs to the
//                 equivalence class are set to NULL as well.
// Comments......:
// Revisions.....: Aš 990301: Added lazy allocation stuff.
//===================================================================

bool
DiscernibilityMatrix::RemoveEntries(int i, bool destroy, bool reduced) {

	// Allocation not done?
	if (!HasAllocatedMatrix())
		return true;

	// Get size of reduced matrix.
	int dimension = GetDimension(true);

	// Compute eq. class index, if needed.
	if (!reduced)
		i = GetMap(i);

	int j;

	// Loop over row/column in reduced matrix.
	for (j = 0; j < dimension; j++) {
		if (!RemoveEntry(i, j, destroy, true))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsEmpty
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
DiscernibilityMatrix::IsEmpty(int i, int j, bool reduced) const {
	return (GetEntry(i, j, reduced) == NULL);
}

//-------------------------------------------------------------------
// Method........: Initialize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Initializes the matrix and map, sizewise. The
//                 matrix entries are not computed.
// Comments......:
// Revisions.....: Aš 990301: Introduced lazy allocation.
//===================================================================

bool
DiscernibilityMatrix::Initialize(const DecisionTable &table, bool masked, bool modulo) {

	int no_partitions;

	// Compute partition indices. This functions as a map.
	if (!PartitionKit::ComputePartitionIndices(map_, no_partitions, table, !modulo, masked))
		return false;

#if 0
	// Get table dimension.
	int no_objects = table.GetNoObjects(masked);
#endif
	
	// How many unique objects are there?
	int no_unique_objects = no_partitions;

	representatives_.erase(representatives_.begin(), representatives_.end());
	representatives_.reserve(no_unique_objects);

	cardinalities_.erase(cardinalities_.begin(), cardinalities_.end());
	cardinalities_.reserve(no_unique_objects);

	int i, index;

	// Find one representative from each eq. class, and compute eq. class cardinalities.
	for (i = 0; i < no_unique_objects; i++) {
		representatives_.push_back(Undefined::Integer());
		cardinalities_.push_back(0);
	}

	for (i = 0; i < map_.size(); i++) {
		index = map_[i];
		representatives_[index] = i;
		cardinalities_[index]++;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetRepresentatives
// Author........: Aleksander šhrn
// Date..........:
// Description...: From a set of logical object indices, computes a
//                 set of distinct, logical indices such that the
//                 collection of eq. classes defined through the two
//                 sets are the same.
//
//                 Used to find a set of representatives (i.e., one
//                 object per eq. class) among a collection of objects.
//
//                 Example:
//
//                    Objects         = {1, 3, 5}
//                    Map             = {1->0, 2->1, 3->0, 4->2, 5->3, 6->3}
//                    Representatives = {1, 5} or {3, 5} or {1, 6} or {3, 6}
//
// Comments......: The matrix framework must have been initialized.
// Revisions.....:
//===================================================================

bool
DiscernibilityMatrix::GetRepresentatives(const Vector(int) &objects, Vector(int) &representatives) const {

	representatives.erase(representatives.begin(), representatives.end());
	representatives.reserve(objects.size());

	int i, j;

	// Lookup indices.
	for (i = objects.size() - 1; i >= 0; i--)
		representatives.push_back(GetRepresentative(GetMap(objects[i])));

	// Sort them.
	std::sort(representatives.begin(), representatives.end());

	int no_removed = 0;

	// As default, keep all.
	Bits remove(representatives.size(), false);

	// Schedule duplicates for removal.
	for (j = representatives.size() - 1; j > 0; j--) {
		if (representatives[j] == representatives[j - 1]) {
			remove.SetState(j, true);
			no_removed++;
		}
	}

	int size = representatives.size();
	int last = size - 1;

	// Compact array.
	for (j = 0; j < size; j++) {
		if (!remove.GetState(j))
			continue;
		while (remove.GetState(last) && last > j)
			last--;
		if (last <= j)
			break;
		representatives[j] = representatives[last];
		last--;
	}

	// Erase tail of array.
	representatives.erase(representatives.begin() + size - no_removed, representatives.end());

	return true;

}

//-------------------------------------------------------------------
// Method........: CreateEntries
// Author........: Aleksander šhrn
// Date..........:
// Description...: Helper method. Creates the entries in the matrix
//                 for row i, columns [j, k).
//
//                 The indices are assumed to refer to the
//                 mapped/reduced matrix.
//
// Comments......: Assumed called right after the matrix framework
//                 has been initialized.
//
//                 If the generalized decisions are provided, then
//                 they're used (i.e., computation of entries modulo
//                 decision is assumed).
//
//                 If an entry already exists, it is kept.
//
//                 If boundary region thinning (BRT) is not used,
//                 pass a non-sensical BRT precision value, e.g.,
//                 Undefined::Float(). Alternatively, pass 0.
//
//                 NOTE: The Discerner should define a reflexive and
//                       symmetric relation! This since only less than
//                       half the matrix is stored internally.
//
// Revisions.....: Aš 990301: Added lazy allocation stuff.
//                 Aš 990629: Added BRT stuff.
//===================================================================

bool
DiscernibilityMatrix::CreateEntries(const DecisionTable &table, bool masked, int i, int j, int k, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision) {

	// Allocation cannot be delayed any longer.
	if (!HasAllocatedMatrix() && !AllocateMatrix())
		return false;

	int no_attributes = table.GetNoAttributes(masked);

	// Keep a static variable to minimize allocaton/deallocation overhead. Overkill?
//	static Bits entry;
	Bits entry;	// sus - remove static for multithreading

	// If needed, resize bit set.
	if (entry.GetSize() != no_attributes)
		entry = Bits(no_attributes, false);

	// Look up representative.
	int representative_i = GetRepresentative(i);

	// Compute specified matrix entries.
	for (; j < k; j++) {

		// Physically diagonal entries are empty...
		if (j == i)
			continue;

		int representative_j = GetRepresentative(j);

		// .. and so are logically diagonal entries.
		if (representative_j == representative_i)
			continue;

		// If modulo decision, entries between objects in the same approximation region are empty.
		if ((decisions != NULL) && SkipEntry(*decisions, representative_i, representative_j, precision))
			continue;

		int index = GetIndex(i, j);
		int count = 0;

		// Matrix entry already computed?
		if (matrix_[index] != NULL)
			continue;

		// Compute matrix entry.
		if (!ComputeEntry(table, masked, decisions != NULL, representative_i, representative_j, discerner, entry, count))
			return false;

		// Update matrix.
		if (count > 0)
			matrix_[index] = new Bits(entry);

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: ComputeEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Helper method. Computes the specified matrix entry.
//                 The object indices (i, j) are assumed to be non-mapped,
//                 i.e., indices into the decision table.
//
//                 Used in conjunction with SkipEntry, which determines
//                 if this method should be invoked or not.
//
// Comments......: The bit set is assumed to possess the proper size.
// Revisions.....:
//===================================================================

bool
DiscernibilityMatrix::ComputeEntry(const DecisionTable &table, bool masked, bool modulo, int i, int j, const Discerner &discerner, Bits &entry, int &count) const {

	int k, no_attributes = entry.GetSize();

#ifdef _DEBUG
	if (no_attributes != table.GetNoAttributes(masked))
		return false;
	if (i < 0 || i >= table.GetNoObjects(masked))
		return false;
	if (j < 0 || j >= table.GetNoObjects(masked))
		return false;
#endif

	// Initialize as empty set.
	entry = false;
	count = 0;

	// Compute matrix entry.
	for (k = 0; k < no_attributes; k++) {
		if (modulo && table.IsDecision(k, masked))
			continue;
		int value1 = table.GetEntry(i, k, masked);
		int value2 = table.GetEntry(j, k, masked);
		if (discerner.Discerns(k, value1, value2)) {
			entry.SetState(k, true);
			count++;
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: SkipEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...: Skip the entry between the objects that have the
//                 two generalized decisions?
//
//                 If a boundary region thinning (BRT) precision threshold
//                 is supplied, takes the distribution of decision values
//                 into account. The precision level should normally lie
//                 in the range [0, 0.5].
//
//                 If no sensible BRT precision threshold is provided, e.g.,
//                 it is passed as Undefined::Float(), then "plain"
//                 generalized decisions are used.
//
// Comments......:
// Revisions.....: Aš 990629: Added BRT stuff.
//===================================================================

bool
DiscernibilityMatrix::SkipEntry(const GeneralizedDecision::Handles &decisions, int i, int j, float precision) const {

#ifdef _DEBUG
	if (i < 0 || i >= decisions.size())
		return false;
	if (j < 0 || j >= decisions.size())
		return false;
#endif

	// No sensible BRT threshold supplied? (Allow some slack...)
	if (precision <= 0.0f || precision >= 1.01f)
		return (decisions[i]->HasEqualDecisions(*(decisions[j]), 0.0f));
	else
		return (decisions[i]->HasEqualDecisions(*(decisions[j]), precision));

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the full discernibility matrix, i,e,, all
//                 matrix entries.
// Comments......:
// Revisions.....: Aš 990629: Added BRT precision.
//===================================================================

bool
DiscernibilityMatrix::Create(const DecisionTable &table, bool masked, bool modulo, const Discerner &discerner, float precision) {

	// Delete current contents.
	Clear();

	GeneralizedDecision::Handles decisions;

	// Compute generalized decisions, if needed.
	if (modulo && !table.GetGeneralizedDecisions(decisions, discerner, masked))
		return false;

	// Initialize matrix framework and create an empty matrix.
	if (!Initialize(table, masked, modulo))
		return false;

	int i, dimension = GetDimension(true);

	// Create all entries in reduced, diagonal matrix.
	for (i = 0; i < dimension; i++) {
		if (!CreateEntries(table, masked, i, i + 1, dimension, discerner, modulo ? &decisions : NULL, precision))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the matrix entries only for the specified
//                 set of objects.
//
//                 The indices of the objects are assumed to be
//                 logical, i.e., non-mapped.
// Comments......:
// Revisions.....: Aš 990629: Added BRT precision.
//===================================================================

bool
DiscernibilityMatrix::Create(const DecisionTable &table, const Vector(int) &objects, bool masked, bool modulo, const Discerner &discerner, float precision) {

	// Delete current contents.
	Clear();

	GeneralizedDecision::Handles decisions;

	// Compute generalized decisions, if needed.
	if (modulo && !table.GetGeneralizedDecisions(decisions, discerner, masked))
		return false;

	// Initialize matrix framework and map.
	if (!Initialize(table, masked, modulo))
		return false;

	Vector(int) representatives;

	// Convert logical indices of selected objects to set of unique (non-mapped) indices of representatives.
	if (!GetRepresentatives(objects, representatives))
		return false;

	int i;

	// Create entries.
	for (i = 0; i < representatives.size(); i++) {
		if (!Create(table, representatives[i], masked, discerner, modulo ? &decisions : NULL, precision))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the entries in the matrix for the specified
//                 object. The index of the object is assumed to refer
//                 to a non-mapped index, i.e., an index into the
//                 decision table.
//
// Comments......: See CreateEntries method for comments.
// Revisions.....:
//===================================================================

bool
DiscernibilityMatrix::Create(const DecisionTable &table, int object_no, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision) {
	return CreateEntries(table, masked, GetMap(object_no), 0, GetDimension(true), discerner, decisions, precision);
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
DiscernibilityMatrix::Save(ofstream &stream, char emptychar) const {

	// Allocation not done?
	if (!HasAllocatedMatrix())
		return false;

	int i, j, size = 1;

	// Determine size of bit set (assuming they all have the same size).
	for (i = matrix_.size() - 1; i >= 0; i--) {
		if (matrix_[i] != NULL) {
			size = matrix_[i]->GetSize();
			break;
		}
	}

	// Format appearance of empty set.
	String emptyset = String(emptychar, size);

	// Get logical dimension of matrix.
	int dimension = GetDimension();

	// Save matrix, as viewed logically.
	for (i = 0; i < dimension; i++) {
		for (j = 0; j < dimension; j++) {
			const Bits *entry = GetEntry(i, j);
			if (entry == NULL)
				stream << emptyset;
			else
				stream << entry->Format();
			stream << ' ';
		}
		stream << endl;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetCardinality
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the cardinality associated with the
//                 specified, reduced matrix entry.
//
// Comments......:
// Revisions.....:
//===================================================================

int
DiscernibilityMatrix::GetCardinality(int i) const {

#ifdef _DEBUG
	if (i < 0 || i >= cardinalities_.size()) {
		Message::Error("Illegal index passed to GetCardinality method.");
		return 0;
	}
#endif

	int ci = cardinalities_[i];

	return ci;

}

int
DiscernibilityMatrix::GetCardinality(int i, int j) const {

#ifdef _DEBUG
	if (i < 0 || i >= cardinalities_.size() || j < 0 || j >= cardinalities_.size()) {
		Message::Error("Illegal index passed to GetCardinality method.");
		return 0;
	}
#endif

	int ci = cardinalities_[i];
	int cj = cardinalities_[j];

	return (ci * cj);

}

//-------------------------------------------------------------------
// Method........: GetEquivalenceClass
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the index of the equivalence class of the
//                 object. The eq. class index is the index into the
//                 reduced matrix.
//
// Comments......:
// Revisions.....:
//===================================================================

int
DiscernibilityMatrix::GetEquivalenceClass(int i) const {
	return map_[i];
}

//-------------------------------------------------------------------
// Method........: GetRepresentative
// Author........: Aleksander šhrn
// Date..........:
// Description...: Gets the (non-mapped) index of an object that
//                 represents equivalence class number i.
// Comments......:
// Revisions.....:
//===================================================================

int
DiscernibilityMatrix::GetRepresentative(int i) const {
	return representatives_[i];
}
 
