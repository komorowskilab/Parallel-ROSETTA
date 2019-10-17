//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/orthogonalscaler.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/stringattribute.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/mathkit.h>
#include <kernel/utilities/iokit.h>

#include <kernel/basic/interval.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>

#include <kernel/system/fstream.h>
#include <kernel/system/math.h>

#include <common/configuration.h>

//-------------------------------------------------------------------
// Static methods (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticLoadCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Loads cuts from file, returns them in-place.
// Comments......: Partial overlap with code elsewhere, generalize
//                 later.  Note masked/unmasked conversion.
//                 Called from CreateDictionary method.
// Revisions.....:
//===================================================================

static bool
StaticLoadCuts(const String &filename, const DecisionTable &table, bool masked, Vector(OrthogonalScaler::Cuts) &cuts) {

	int no_attributes = table.GetNoAttributes(masked);

	// Initialize vector of all cuts.
	cuts.erase(cuts.begin(), cuts.end());
	cuts.reserve(no_attributes);

	int i;

	for (i = 0; i < no_attributes; i++)
		cuts.push_back(OrthogonalScaler::Cuts());

	ifstream cutfile;

	if (!IOKit::Open(cutfile, filename)) {
		Message::Error("Could not open file with cuts.");
		return false;
	}

	// Read contents of cut file.
	while (!IOKit::IsEOF(cutfile)) {

		String line;

		// Get index (virtual) and cut of attribute.
		if (!IOKit::Load(cutfile, line, false))
			return false;

		// Allow for blank lines.
		if (line.IsEmpty())
			continue;

		int nrtok = line.GetNoTokens(" \t");
		if (line.GetNoTokens(" \t") != 2) {
			Message::Error("Expected two tokens (index, cut).");
			Message::Error("Line: " + line + "Number of tokens " + String::Format(line.GetNoTokens(" \t")) + " / " + String::Format(nrtok));
			return false;
		}

		// Split string into (index, cut) pair.
		String istr = line.Before('\t');
		String cstr = line.After('\t');

		// Verify types.
		if (!istr.IsInteger() || !cstr.IsFloat()) {
			Message::Error("Error reading (index, cut) pair from cut file.");
			return false;
		}

		// Extract values.
		int   index = istr.GetInteger();
		float cut   = cstr.GetFloat();

    // Convert from virtual (masked) to actual (unmasked) indexing scheme.
		index = table.GetUnmaskedAttribute(index);

    // Make sure the index is in range.
		if ((index < 0) || (index >= cuts.size())) {
			Message::Error("Index in (index, cut) pair in cutfile out of range.");
			return false;
		}

		// Insert cut into appropriate cut vector.
		cuts[index].push_back(cut);

	}

	// Sort the cuts.
	for (i = 0; i < cuts.size(); i++)
		std::sort(cuts[i].begin(), cuts[i].end());

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticCreateDummyDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates a dummy dictionary.
// Comments......: Called from CreateDictionary method.
// Revisions.....:
//===================================================================

static bool
StaticCreateDummyDictionary(DecisionTable &table, Dictionary &dictionary) {

	int no_attributes_unmasked = table.GetNoAttributes(false);

	// Clear present dictionary.
	dictionary.RemoveAllAttributes();

	int i;

	// Create dummy dictionary.
	for (i = 0; i < no_attributes_unmasked; i++) {

		// Create an integer attribute.
		Handle<Attribute> attribute = Creator::IntegerAttribute();

		// Set dummy name/unit.
		attribute->SetName("A" + String::Format(i));
		attribute->SetUnit(Undefined::String());

		// Append to dictionary.
		if (!dictionary.AppendAttribute(attribute.GetPointer()))
			return false;

	}

	// Assign new dictionary.
	table.SetDictionary(&dictionary);

	Message::Warning("A dummy dictionary was created.");

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticHasEqualMasking
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the two tables have the same
//                 masking.
// Comments......: Called from CreateDictionary method.
// Revisions.....:
//===================================================================

static bool
StaticHasEqualMasking(const DecisionTable &table1, const DecisionTable &table2, bool masked) {

	int no_attributes1 = table1.GetNoAttributes(masked);
	int no_attributes2 = table2.GetNoAttributes(masked);

	if (no_attributes1 != no_attributes2)
		return false;

	int i;

	for (i = 0; i < no_attributes1; i++) {
		if (table1.GetAttributeMask(i) != table2.GetAttributeMask(i))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: StaticHasChanged
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the entries in the two tables
//                 differ on the given attribute.
// Comments......: Called from CreateDictionary method.
// Revisions.....:
//===================================================================

static bool
StaticHasChanged(const DecisionTable &table1, const DecisionTable &table2, int attribute_no, bool masked) {

	int no_objects1 = table1.GetNoObjects(masked);
	int no_objects2 = table2.GetNoObjects(masked);

	if (no_objects1 != no_objects2)
		return true;

	int i;

	for (i = 0; i < no_objects1; i++) {
		if (table1.GetEntry(i, attribute_no, masked) != table2.GetEntry(i, attribute_no, masked))
			return true;
	}

	return false;

}

//-------------------------------------------------------------------
// Methods for class OrthogonalScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

OrthogonalScaler::OrthogonalScaler() {
}

OrthogonalScaler::~OrthogonalScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(OrthogonalScaler, ORTHOGONALSCALER, Scaler)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
OrthogonalScaler::Apply(Structure &structure) const {

	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> input = dynamic_cast(DecisionTable *, &structure);

	Vector(DecisionTable::Mask) original_masks;
	Vector(DecisionTable::Mask) temporary_masks;

	if (!GetAttributeMasks(*input, original_masks))
		return NULL;

	// Temporarily hide away non-numerical attributes if specified.
	if (MaskSymbolic()) {
		if (!GetTemporaryAttributeMasks(*input, temporary_masks))
			return NULL;
		if (!SetAttributeMasks(*input, temporary_masks))
			return NULL;
	}

	Handle<DecisionTable> output;

	// If possible, do the actual discretization. (Assuming the output table "inherits" the masking of the input table.)
	if (input->GetNoAttributes(true) > 1) {
		output = Discretize(*input);
	}
	else {
		if (MaskSymbolic()) {
			Message::Warning("Table has no condition attributes after masking.", false);
			SetAttributeMasks(*input, original_masks);
			return &structure;
		}
		else {
			Message::Error("Table has no condition attributes.", false);
			return NULL;
		}
	}

	if (output == NULL) {
		if (MaskSymbolic())
			SetAttributeMasks(*input, original_masks);
		return NULL;
	}

	// Construct a new dictionary. (Masking of both tables should be the same and seen as by the
	// discretization method.)
	if (!CreateDictionary(*input, *output, GetFilename())) {
		Message::Error("Failed to create dictionary.");
		if (MaskSymbolic())
			SetAttributeMasks(*input, original_masks);
		return NULL;
	}

	// Reset masking.
	if (MaskSymbolic()) {
		if (!SetAttributeMasks(*input, original_masks))
			return NULL;
		if (!SetAttributeMasks(*output, original_masks))
			return NULL;
	}

	// Append string to table name.
	output->SetName(input->GetName() + ", discretized");

	return output.Release();

}

//-------------------------------------------------------------------
// Dictionary creation methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: CreateDictionary
// Author........: Aleksander šhrn
// Date..........:
// Description...: After a decision table has been scaled, the old
//                 dictionary may no longer be 100% valid as the result
//                 of e.g. a float attribute having been transformed
//                 to an "interval" attribute.
//
//                 This method takes as input the decision tables before
//                 and after scaling, and on the basis of these and the
//                 dictionary associated with the before table, creates a
//                 new dictionary which in turn is assigned to the after
//                 table.
//
//                 The filename refers to the file where the cut-values
//                 are stored.
//
// Comments......: Note that the dictionary is indexed with actual (i.e.
//                 unmasked) attributes indices, while the tables are indexed
//                 with virtual (i.e. masked) attribute indices.  That is,
//                 dictionaries associated with decision tables have the
//                 dimension of the unmasked table.
//
// Revisions.....: Aš 971203: No change is not enough for duplicate existing
//                            attributes! Causes potential problems when
//                            discretizing two different tables with the same
//                            cut file, as the dictionaries may be different.
//                            An example is when CVSerialExecutor is used.
//                 Aš 971204: Decomposed to make less complex, moved some code
//                            into separate static methods.
//===================================================================

bool
OrthogonalScaler::CreateDictionary(const DecisionTable &before, DecisionTable &after, const String &filename) const {
	// Are the contents of the before and after tables equal?
	if ((before == after) && before.HasDictionary())
		return true;

	// Get the number of attributes and objects (masked).
	int b_no_attributes = before.GetNoAttributes(true);
	int a_no_attributes = after.GetNoAttributes(true);

	// Get the number of attribute and objects (unmasked).
	int b_no_attributes_unmasked = before.GetNoAttributes(false);
	int a_no_attributes_unmasked = after.GetNoAttributes(false);

	// Get old dictionary, and create a new dictionary to be assigned to the after table.
	Handle<Dictionary> b_dictionary = before.GetDictionary();
	Handle<Dictionary> a_dictionary = Creator::Dictionary();
	// Check if the tables have the same attribute masking.
	bool equal_masking = StaticHasEqualMasking(before, after, false);
	Handle<Attribute> b_attribute;
	Handle<Attribute> a_attribute;

	// Is there a before-dictionary?  If not, create a dummy dictionary for the after-table.
	// Also, do the same if the number of attributes differ or if the masking is different.
	if (b_dictionary.IsNULL() ||
		 (b_no_attributes != a_no_attributes) ||
		 (b_no_attributes_unmasked != a_no_attributes_unmasked) ||
		 !equal_masking)
		return StaticCreateDummyDictionary(after, *a_dictionary);

 	// Determine if a cutfile was given.
	bool has_cutfile = (!filename.IsEmpty() && !(filename == Undefined::String()));

	// A set of cuts for each attribute is a float vector.  The set of all cuts is a vector of such vectors.
	// Note that the cuts vector here will have the dimension of the actual (unmasked) table.
	Vector(Cuts) cuts;

  // Process the cutfile if present/given.
/*
	if (has_cutfile && !StaticLoadCuts(filename, before, false, cuts)) {
		Message::Error("Failed to load cuts.");
		return false;
	}
*/
	if (has_cutfile) {
		int test;
		#pragma omp critical(SLC) 
		{
			test = StaticLoadCuts(filename, before, false, cuts);
		}
		if (!test) {
			Message::Error("Failed to load cuts.");
			return false;
		}
	}
	int i;

	// Loop over all attributes in the table/dictionary.
	for (i = 0; i < b_no_attributes_unmasked; i++) {
		// Detect if the table entries for the current attribute have changed.
		bool has_changed = StaticHasChanged(before, after, i, false);
		// Get the attribute from the before-dictionary.
		b_attribute = b_dictionary->GetAttribute(i);

		// Can we simply duplicate the attribute?
		if (!has_changed && has_cutfile && cuts[i].empty()) {
			a_attribute = dynamic_cast(Attribute *, b_attribute->Duplicate());
		}

    // If not, "interpret" the discretized attribute if possible.
		else {

			bool did_interpret = false;

			// If we have any cuts, use them to interpret the new attribute.
			if (has_cutfile && !cuts[i].empty()) {
				a_attribute = CreateDiscretizedAttribute(*b_attribute, cuts[i]);
				if (!a_attribute.IsNULL())
					did_interpret = true;
			}

			// If interpretation failed, use an integer dictionary attribute.
			if (!did_interpret)
				a_attribute = Creator::IntegerAttribute();

			// Set attribute name. The attribute unit may be different after discretization.
			a_attribute->SetName(b_attribute->GetName());
			a_attribute->SetUnit(Undefined::String());

		}

		// Append the attribute to the new dictionary.
		if (!a_dictionary->AppendAttribute(a_attribute.GetPointer()))
			return false;

	}

	// Assign the new dictionary to the after table.
	after.SetDictionary(a_dictionary.GetPointer());

	return true;

}

//-------------------------------------------------------------------
// Method........: CreateSubsetAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Cf. the CreateIntervalAttribute method.
// Comments......: The non-numeric equivalent to CreateIntervalAttribute.
//                 For such attributes, intervals translate to subsets.
//
//                 Assumes sorted cuts.
//
//                 Ensure that the attribute construction is compatible with the
//                 updated (discretized) table! (Cf. UpdateTable method.)
//
// Revisions.....:
//===================================================================

Attribute *
OrthogonalScaler::CreateSubsetAttribute(const Attribute &before, const Cuts &cuts) const {

	String subset;

	// Create a new attribute capable of representing subsets.
	Handle<StringAttribute> after = Creator::StringAttribute();

	// If there are no cuts, return the virgin attribute.
	if (cuts.size() == 0)
		return after.Release();

	int i, j;

	// How many indexed entries does the attribute have?
	int no_entries = before.GetNoEntries();

	subset = "";

	// Build first subset.
	for (i = 0; i < no_entries; i++) {

		String entry = before.GetEnumeratedEntry(i);
		int    coded = before.GetEntry(entry);
		float  cut   = cuts[0];

		if (coded < cut) {
			if (!subset.IsEmpty())
				subset += ", ";
			subset += entry;
		}

	}

	subset = "{" + subset + "}";

	if (!after->SetEntry(0, subset))
		return NULL;

	// Build intermediate subsets.
	for (i = 1; i < cuts.size(); i++) {

		subset = "";

		float cut_lower = cuts[i - 1];
		float cut_upper = cuts[i];

		for (j = 0; j < no_entries; j++) {

			String entry = before.GetEnumeratedEntry(j);
			int    coded = before.GetEntry(entry);

			if (cut_lower <= coded && coded < cut_upper) {
				if (!subset.IsEmpty())
					subset += ", ";
				subset += entry;
			}

		}

		subset = "{" + subset + "}";

		if (!after->SetEntry(i, subset))
			return NULL;

	}

	subset = "";

	// Build last subset.
	for (i = no_entries - 1; i >= 0; i--) {

		String entry = before.GetEnumeratedEntry(i);
		int    coded = before.GetEntry(entry);
		float  cut   = cuts[cuts.size() - 1];

		if (cut <= coded) {
			if (subset.IsEmpty())
				subset = entry;
			else
				subset = entry + ", " + subset;
		}

	}

	subset = "{" + subset + "}";

	if (!after->SetEntry(cuts.size(), subset))
		return NULL;

	// Return subset-coded attribute.
	return after.Release();

}

//-------------------------------------------------------------------
// Method........: CreateIntervalAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a set of (sorted, ascending) cuts, creates
//                 and returns a new attribute. The attribute that the
//                 cuts refer to must also be given.
//
//                 Example:
//
//                    Cuts =
//
//                       {a, b, c}, a < b < c
//
//                    Created intervals:
//
//                       [*, a), [a, b), [b, c), [c, *)
//
//                 Note the closed/open endpoints.
//
// Comments......: Ensure that the attribute construction is compatible with the
//                 updated (discretized) table! (Cf. UpdateTable method.)
//
// Revisions.....: Aš 970726 - Changed endpoints from "(...]" to "[...)",
//                             and improved endpoint determination.
//                 Aš 990610 - Added special case of "[*, *)" interval.
//===================================================================

Attribute *
OrthogonalScaler::CreateIntervalAttribute(const Attribute &before, const Cuts &cuts) const {

	// Create a new attribute capable of representing intervals.
	Handle<StringAttribute> after = Creator::StringAttribute();

	float  infinity = 2e38f;
	String interval;

	// Special case?
	if (cuts.size() == 2 && cuts[0] <= -infinity && cuts[1] >= infinity) {
		interval += Interval::FromInclude();
		interval += Interval::Infinity();
		interval += Interval::Separator();
		interval += ' ';
		interval += Interval::Infinity();
		interval += Interval::To();
		after->SetEntry(1, interval);
		return after.Release();
	}

	String currstr;
	String prevstr;

	int i;

	// Create intervals.
	for (i = 0; i < cuts.size() + 1; i++) {
		interval = Interval::FromInclude();
		if (i == 0)
			interval += Interval::Infinity();                      // Handle the first interval specially.
		else
			interval += prevstr;
		interval += Interval::Separator();
		interval += ' ';
		if (i == cuts.size()) {                                  // Handle the last interval specially.
			interval += Interval::Infinity();
		}
		else {
			float cut = cuts[i];                                   // Get the cut.
			bool  is_integer = (fmod((double) cut, 1.0) == 0.0);   // Is the fractional part zero?
			int   endpoint;                                        // The integral endpoint of the current interval.
			if (cut > 0.0) {
				if (is_integer)
					endpoint = static_cast(int, floor(cut));
				else
					endpoint = static_cast(int, floor(cut)) + 1;
			}
			else {
				endpoint = -static_cast(int, floor(-cut));
			}
			currstr = before.GetEntry(endpoint);                   // Look it up in the soon-to-be-obsolete dictionary.
			prevstr = currstr;
			interval += currstr;
		}
		interval += Interval::To();
		after->SetEntry(i, interval);
	}

	return after.Release();

}

//-------------------------------------------------------------------
// Method........: CreateDiscretizedAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Dispatches on type.
// Revisions.....:
//===================================================================

Attribute *
OrthogonalScaler::CreateDiscretizedAttribute(const Attribute &before, const Cuts &cuts) const {

	// If no cuts are supplied, no intervals can be constructed.
	if (cuts.size() == 0)
		return NULL;

	// Dispatch on type.
	if (before.IsNumeric())
		return CreateIntervalAttribute(before, cuts);
	else if (before.IsSymbolic())
		return CreateSubsetAttribute(before, cuts);

	// Unknown type.
	return NULL;

}

//-------------------------------------------------------------------
// Discretization methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: Discretize
// Author........: Aleksander šhrn
// Date..........:
// Description...: Default implementation, outer loop for the actual
//                 discretization process.
//
// Comments......: Typically, only FindCuts needs to be overloaded in
//                 subclasses.
//
//                 Whether discretization should take place directly
//                 on the input table or on a duplicate, should probably
//                 be a library client option.
// Revisions.....:
//===================================================================

DecisionTable *
OrthogonalScaler::Discretize(DecisionTable &table) const {

	Message message;

	// Duplicate the input table.
	Handle<DecisionTable> duplicate = dynamic_cast(DecisionTable *, table.Duplicate());

	// Operate on a masked table.
	bool masked = true;

	ofstream cutfile;

	// Open cutfile for output, if required.
	if (GetMode() == MODE_SAVE) {
		if (!IOKit::Open(cutfile, GetFilename())) {
			Message::Error("Failed to open output cutfile.");
			return NULL;
		}
	}

	// Determine table size.
	int no_attributes = duplicate->GetNoAttributes(masked);

	int i, j;

	// Iterate over all attributes.
	for (i = 0; i < no_attributes; i++) {

		// Don't discretize the decision attribute.
		if (duplicate->IsDecision(i, masked))
			continue;

		//if (!message.Progress("Discretizing...", i, no_attributes))
		//	return NULL;

		Cuts cuts;

		// Find cuts for current attribute.
		if (!FindCuts(*duplicate, i, masked, cuts))
			return NULL;

		// Make sure the cuts are sorted (ascending).
		std::sort(cuts.begin(), cuts.end());

		// Output status.
		if (cuts.empty()) {
//			Message::Warning("No cuts found for attribute " + duplicate->GetAttributeName(i, masked), false);	// sus - tmp
		}
		else {
			if (Configuration::Kernel::IsVerbose())
#ifdef X86_64
				message.Notify("Found " + String::Format((int)cuts.size()) + " cut(s) for attribute " + duplicate->GetAttributeName(i, masked) + ".");
#else
				message.Notify("Found " + String::Format(cuts.size()) + " cut(s) for attribute " + duplicate->GetAttributeName(i, masked) + ".");

#endif
		}

		// Discretize current attribute with the cuts.
		if (!UpdateTable(*duplicate, i, masked, cuts))
			return NULL;

		// Save cuts to file, if required.
		if (GetMode() == MODE_SAVE) {
			for (j = 0; j < cuts.size(); j++)
				cutfile << i << '\t' << cuts[j] << endl;
		}

	}

	// Close cutfile, if required.
	if (GetMode() == MODE_SAVE)
		IOKit::Close(cutfile);

	return duplicate.Release();

}

//-------------------------------------------------------------------
// Method........: FindCuts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns (in-place) a set of cut-values for the
//                 specified attribute.
//
// Comments......: Default implementation does nothing, usually
//                 overloaded.
// Revisions.....:
//===================================================================

bool
OrthogonalScaler::FindCuts(const DecisionTable &/*table*/, int /*attribute_no*/, bool /*masked*/, Cuts &/*cuts*/) const {
	return false;
}

//-------------------------------------------------------------------
// Method........: UpdateTable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a set of cuts (sorted, ascending) and an
//                 attribute index, updates the integer entries in the
//                 table according to the cuts.
//
// Comments......: Note that the dictionary gets updated elsewhere.
//
// Revisions.....: Aš 260797 - Use "[...)" intervals instead of "(...]".
//                 Aš 260897 - Don't update Undefined entries.
//                 Aš 280499 - Fixed a bug. Can hardly believe it
//                             hasn't been detected before...
//===================================================================

bool
OrthogonalScaler::UpdateTable(DecisionTable &table, int attribute_no, bool masked, const Cuts &cuts) const {

	// If there are no cuts, don't do anything.
	if (cuts.empty())
		return true;

	int no_objects = table.GetNoObjects(masked);
	int no_cuts    = cuts.size();

	int i, j;

	// Update all objects.
	for (i = 0; i < no_objects; i++) {

		// Get current entry.
		int entry = table.GetEntry(i, attribute_no, masked);

		// Skip "missing" values.
		if (entry == Undefined::Integer())
			continue;

		// Inside first interval?
		if (entry < cuts[0]) {
			if (!table.SetEntry(i, attribute_no, 0, masked))
				return false;
		}

		// Inside last interval?
		else if (cuts[no_cuts - 1] <= entry) {
			if (!table.SetEntry(i, attribute_no, no_cuts, masked))
				return false;
		}

		// Inside an intermediate interval. But which one?
		else {
			for (j = 1; j < no_cuts; j++) {
				float cut1 = cuts[j - 1];
				float cut2 = cuts[j];
				if (cut1 <= entry && entry < cut2) {
					if (!table.SetEntry(i, attribute_no, j, masked))
						return false;
					break;
				}
			}
		}

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: IsConsistent
// Author........: Aleksander šhrn
// Date..........:
// Description...: Given a vector of (object, value) pairs sorted
//                 according to the value-component and an index into
//                 this vector, returns true if the eq. class defined
//                 through the specified value is consistent.
//
//                 Also, returns (in-place) various information
//                 about the eq. class defined by the value, such as
//                 the different decision classes and their
//                 respective cardinalities.
//
// Comments......: The (begin, end) pair determines the index range
//                 to consider, namely [begin, end).
//
//                 Using a map for temporary storage ensures uniqueness
//                 and logarithmic insert/lookup times.
//
// Revisions.....:
//===================================================================

bool
OrthogonalScaler::IsConsistent(const Vector(OVPair) &objects, int i, int begin, int end, const DecisionTable &table, int decision_attribute, bool masked, Vector(int) &decisions, Vector(int) &cardinalities) const {

	// Clear input.
	decisions.erase(decisions.begin(), decisions.end());
	cardinalities.erase(cardinalities.begin(), cardinalities.end());

	// Verify input,
	if (begin < 0 || end > objects.size() || begin >= end) {
		Message::Error("Invalid [begin, end) range specified.");
		return false;
	}

	if (i < begin || i >= end) {
		Message::Error("Specified index not in [begin, end) range.");
		return false;
	}

	// Make a convenient typedef.
	typedef Map(int, int) IMap;

	IMap map;

	// Get (object, value) pair of specified index.
#if 0
	int specified_object = objects[i].first;
#endif
	int specified_value  = objects[i].second;

	int j;

	// Check the specified object and the objects to the left of it.
	for (j = i; j >= begin; j--) {

		// Get current object index and value.
		int object = objects[j].first;
		int value  = objects[j].second;

		// Are we outside our eq. class?
		if (value != specified_value)
			break;

		// Get decision value of current object.
		int decision = table.GetEntry(object, decision_attribute, masked);

		// Update map.
		IMap::iterator iterator = map.find(decision);

		if (!(iterator == map.end()))
			(*iterator).second++;
		else
			map.insert(Pair(const int, int)(decision, 1));

	}

	// Check to the right of the specified object.
	for (j = i + 1; j < end; j++) {

		// Get current object index and value.
		int object = objects[j].first;
		int value  = objects[j].second;

		// Are we outside our eq. class?
		if (value != specified_value)
			break;

		// Get decision value of current object.
		int decision = table.GetEntry(object, decision_attribute, masked);

		// Update map.
		IMap::iterator iterator = map.find(decision);

		if (!(iterator == map.end()))
			(*iterator).second++;
		else
			map.insert(Pair(const int, int)(decision, 1));

	}

	// Reserve space to minimize allocations.
	decisions.reserve(map.size());
	cardinalities.reserve(map.size());

	// Fill vectors with contents of temporary map.
	// This should be a const_iterator, but VC++ 6.0 won't let me...
	IMap::iterator iterator = map.begin();

	while (!(iterator == map.end())) {
		int decision    = (*iterator).first;
		int cardinality = (*iterator).second;
		decisions.push_back(decision);
		cardinalities.push_back(cardinality);
		iterator++;
	}

	return (decisions.size() == 1);

}

 
OrthogonalScaler *
OrthogonalScaler::Clone() {
	return new OrthogonalScaler;
}
