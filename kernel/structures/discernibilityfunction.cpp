//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/discernibilityfunction.h>
#include <kernel/structures/discernibilitymatrix.h>
#include <kernel/structures/decisiontable.h>

#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class DiscernibilityFunction.
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

DiscernibilityFunction::DiscernibilityFunction(const DiscernibilityFunction &in) : BooleanPOSFunction(in) {
}

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn
// Date..........:
// Description...: Empty constructor
// Comments......:
// Revisions.....:
//===================================================================

DiscernibilityFunction::DiscernibilityFunction() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DiscernibilityFunction::~DiscernibilityFunction() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(DiscernibilityFunction, DISCERNIBILITYFUNCTION, BooleanPOSFunction)

//-------------------------------------------------------------------
// Creation methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the discernibility function from the
//                 given non-NULL discernibility matrix entries.
//
//                 Simplifies the function if specified.
//
// Comments......: Non-public method, for internal use.
//
//                 The weight vector should have been properly
//                 initialized, if present.
//
//                 The non-NULL entries should be from a matrix of
//                 representatives, if simplification is specified.
//
// Revisions.....: Aš 9809: Introduced weight vector, one weight per
//                          function sum.
//===================================================================

bool
DiscernibilityFunction::Create(Vector(const Bits *) &entries, bool copy, bool simplify, const Vector(float) *weights) {

	int i, no_entries = entries.size();

#ifdef _DEBUG
	if (weights != NULL) {
		if (weights->size() != no_entries) {
			Message::Error("Dimension mismatch in function creator.");
			return false;
		}
	}
#endif

	// Reset current internal representation.
	Clear();

	// Reserve space for new sums.
	if (!Reserve(GetNoSums() + no_entries))
		return false;

	// Append sums.
	for (i = 0; i < no_entries; i++) {
		if (!AppendSum(entries[i], copy, (weights == NULL) ? 1.0 : (*weights)[i]))
			return false;
	}

	// Simplify?
	if (simplify && !Simplify(weights != NULL))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the full discernibility function,
//                 simplified if specified.
//
// Comments......: For a simplified function, we can consider the
//                 reduced matrix only.
//
// Revisions.....: Aš 9809: Introduced weight vector, one weight
//                          per function sum.
//                 Aš 9810: Introduced option to run over reduced
//                          matrix without simplifying. That amounts
//                          to a kind of "semi-simplification".
//                 Aš 9810: Introduced copy argument.
//                 Aš 9907: Moved weight stuff internally. Added
//                          parameter to signify if the created
//                          function should be "weighted".
//===================================================================

bool
DiscernibilityFunction::Create(const DiscernibilityMatrix &matrix, bool copy, bool reduce, bool simplify, bool weighted) {

	// Some parameter combinations do not make much sense.
	if (simplify && !reduce)
		reduce = true;

	// Get dimension of discernibility matrix.
	int no_objects = matrix.GetDimension(reduce);

	Vector(const Bits *) entries; // Function sums.
	Vector(float)        weights; // Weight per sum, if weighted.

	entries.reserve(no_objects * (no_objects - 1) / 2);

	if (weighted)
		weights.reserve(no_objects * (no_objects - 1) / 2);

	int i, j;

	// Collect non-empty entries in upper (or lower) diagonal.
	for (i = 0; i < no_objects; i++) {
		for (j = i + 1; j < no_objects; j++) {
			const Bits *entry = matrix.GetEntry(i, j, reduce);
			if (entry != NULL) {
				entries.push_back(entry);
				if (weighted) {
					if (reduce)
						weights.push_back(matrix.GetCardinality(i, j));
					else
						weights.push_back(1.0);
				}
			}
		}
	}

	// Build function.
	if (!Create(entries, copy, simplify, weighted ? &weights : NULL))
		return false;

	String name = "f(system)";

	// Name function.
	if (simplify) {
		name += ", simplified";
	}
	else {
		if (reduce)
			name += ", reduced, not simplified";
	}

	return SetName(name);

}

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the discernibility function for the given
//                 object, simplified if specified.
//
// Comments......: For a simplified function, we can consider the
//                 reduced matrix only.
//
// Revisions.....: Aš 9809: Introduced weight vector, one weight per
//                          function sum.
//                 Aš 9810: Introduced option to run over reduced
//                          matrix without simplifying. That amounts
//                          to a kind of "semi-simplification".
//                 Aš 9810: Introduced copy argument.
//                 Aš 9907: Moved weight stuff internally. Added
//                          parameter to signify if the created
//                          function should be "weighted".
//===================================================================

bool
DiscernibilityFunction::Create(const DiscernibilityMatrix &matrix, int object_no, bool copy, bool reduce, bool simplify, bool weighted) {

	// Some parameter combinations do not make much sense.
	if (simplify && !reduce)
		reduce = true;

	// Get dimension of discernibility matrix.
	int no_objects = matrix.GetDimension(reduce);

	if (reduce)
		object_no = matrix.GetEquivalenceClass(object_no);

	// Verify dimensions.
	if (object_no < 0 || object_no >= no_objects)
		return false;

	Vector(const Bits *) entries; // Function sums.
	Vector(float)        weights; // Weight per sum, if weighted.

	entries.reserve(no_objects - 1);

	if (weighted)
		weights.reserve(no_objects - 1);

	int i;

	// Collect non-empty entries.
	for (i = 0; i < no_objects; i++) {
		const Bits *entry = matrix.GetEntry(i, object_no, reduce);
		if (entry != NULL) {
			entries.push_back(entry);
			if (weighted) {
				if (reduce)
					weights.push_back(matrix.GetCardinality(i)); // ...or GetCardinality(i, object_no), but their ratio is constant.
				else
					weights.push_back(1.0);
			}
		}
	}

	// Build function.
	if (!Create(entries, copy, simplify, weighted ? &weights : NULL))
		return false;

	String name = "f(" + String::Format(object_no) + ")";

	// Name function.
	if (simplify) {
		name += ", simplified";
	}
	else {
		if (reduce)
			name += ", reduced, not simplified";
	}

	return SetName(name);

}

//-------------------------------------------------------------------
// Method........: CreateSimplified
// Author........: Aleksander šhrn
// Date..........:
// Description...: Memory-friendly method for creating the
//                 simplified full discernibility function.
// Comments......:
// Revisions.....: Aš 990629: Added BRT precision.
//===================================================================

bool
DiscernibilityFunction::CreateSimplified(const DecisionTable &table, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, const DiscernibilityMatrix *matrix, bool weighted) {

	// Create...
	if (!CreateSimplified(true, table, Undefined::Integer(), masked, modulo, discerner, decisions, precision, matrix, weighted))
		return false;

	// ...and baptize.
	SetName("f(system), simplified");

	return true;

}

//-------------------------------------------------------------------
// Method........: CreateSimplified
// Author........: Aleksander šhrn
// Date..........:
// Description...: Memory-friendly method for creating the
//                 simplified discernibility function relative to the
//                 specified object.
// Comments......:
// Revisions.....: Aš 990629: Added BRT precision.
//===================================================================

bool
DiscernibilityFunction::CreateSimplified(const DecisionTable &table, int object_no, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, const DiscernibilityMatrix *matrix, bool weighted) {

	// Create...
	if (!CreateSimplified(false, table, object_no, masked, modulo, discerner, decisions, precision, matrix, weighted))
		return false;

	// ...and baptize.
	SetName("f(" + String::Format(object_no) + "), simplified");

	return true;

}

//-------------------------------------------------------------------
// Method........: CreateSimplified
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the specified discernibility function, and
//                 does not explicitly store all the matrix entries
//                 while doing so.
//
//                 Generally requires far less memory resources than
//                 precomputing all the relevant discernibility matrix
//                 entries. However, can sometimes be slightly slower.
//
//                 Some optional structures can be passed along to
//                 help speed up the creation:
//
//                 *) If a set of generalized decisions are given, these
//                    are used. Otherwise, they are computed if needed.
//
//                 *) If a discernibility matrix is given, this is used.
//                    Otherwise, one is computed. (If one is given, it
//                    is assumed computed from the given table with the
//                    same masked/modulo settings as specified here.)
//
// Comments......:
// Revisions.....: Aš 990629: Added BRT precision.
//                 Aš 990726: Moved weight stuff internally. Added
//                            parameter to signify if the created
//                            function should be "weighted".
//===================================================================

bool
DiscernibilityFunction::CreateSimplified(bool full, const DecisionTable &table, int object_no, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, const DiscernibilityMatrix *matrix, bool weighted) {

	Message message;

	GeneralizedDecision::Handles  decisions_local; // Holds locally computed stuff, if anything.
	GeneralizedDecision::Handles *decisions_used;  // Points to the stuff actually used, locally computed or provided.

	// If needed, acquire generalized decisions.
	if (modulo) {
		if (decisions == NULL) {
			message.Notify("Computing generalized decisions...");
			if (!table.GetGeneralizedDecisions(decisions_local, discerner, masked))
				return false;
			decisions_used = &decisions_local;
		}
		else {
			if (table.GetNoObjects(masked) != decisions->size())
				return false;
			decisions_used = const_cast(GeneralizedDecision::Handles *, decisions);
		}
	}
	else {
		decisions_used = NULL;
	}

	DiscernibilityMatrix  matrix_local; // Holds locally computed stuff, if anything.
	DiscernibilityMatrix *matrix_used;  // Points to the stuff actually used, locally computed or provided.

	// If needed, compute discernibility matrix framework.
	if (matrix == NULL) {
		message.Notify("Creating matrix framework....");
		if (!matrix_local.Initialize(table, masked, modulo))
			return false;
		matrix_used = &matrix_local;
	}
	else {
		matrix_used = const_cast(DiscernibilityMatrix *, matrix);
	}

	bool reduced = true;

	// Get the number of eq. classes.
	int dimension = matrix_used->GetDimension(reduced);

	// Initialize function, guesstimate size.
	Clear();
	Reserve(dimension);

	// Initialize counters for progress stuff.
	// Display a progress message every "denominator" processed entry, if at all.
	int counter     = 0;
	int entries     = (full) ? (dimension * (dimension - 1) / 2) : (dimension);
	int frequency   = 20;
	int denominator = std::max(entries / frequency, 1000000);

	int i;

	// Compute entries and absorb continuously.
	if (full) {
		for (i = 0; i < dimension; i++) {
			if (!AppendSimplified(*matrix_used, table, masked, modulo, discerner, decisions_used, precision, weighted, i, i + 1, dimension, counter, denominator, entries))
				return false;
		}
	}
	else {
		if (!AppendSimplified(*matrix_used, table, masked, modulo, discerner, decisions_used, precision, weighted, matrix_used->GetEquivalenceClass(object_no), 0, dimension, counter, denominator, entries))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AppendSimplified
// Author........: Aleksander šhrn
// Date..........:
// Description...: Helper method called from Create. Operates on the
//                 reduced/mapped matrix. Runs over columns [j, k)
//                 in row i.
// Comments......:
// Revisions.....: Aš 990629: Added BRT precision.
//                 Aš 990726: Moved weight stuff internally. Added
//                            parameter to signify if the created
//                            function should be "weighted".
//===================================================================

bool
DiscernibilityFunction::AppendSimplified(const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, bool modulo, const Discerner &discerner, const GeneralizedDecision::Handles *decisions, float precision, bool weighted, int i, int j, int k, int &counter, int denominator, int total) {

	Message message;
#if 0
	// Get the number of eq. classes.
	int dimension = matrix.GetDimension(true);
#endif
	
	// Get a representative from the indicated eq. class.
	int representative_i = matrix.GetRepresentative(i);

	static int previous;
//	int previous;

	if (counter == 0)
		previous = -1;

	int size, representative_j, current;

	Bits entry(table.GetNoAttributes(masked), false);
	// Compute entries of reduced matrix, absorb continuously.
	for (; j < k; j++, counter++) {

		// Give progress?
		if ((denominator < total) && (counter % denominator == 0 || counter == total - 1)) {
			current = static_cast(int, 0.5 + (100 * (static_cast(float, counter + 1) / total)));
			if (current != previous) {
				message.Notify("Computing simplified function (" + String::Format(current) + "%)...");
				previous = current;
			}
		}

		representative_j = matrix.GetRepresentative(j);

		// Logically diagonal entries are empty.
		if (representative_i == representative_j)
			continue;

		// Skip entry?
		if (modulo && matrix.SkipEntry(*decisions, representative_i, representative_j, precision))
			continue;

		// Compute the entry.
		if (!matrix.ComputeEntry(table, masked, modulo, representative_i, representative_j, discerner, entry, size))
			return false;

		// Append and absorb.
		if (size > 0)
			BooleanFunction::AppendSimplified(entry, weighted, weighted ? matrix.GetCardinality(i, j) : 1.0);

	}
	return true;

}
 
