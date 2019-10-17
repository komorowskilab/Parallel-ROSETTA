//-------------------------------------------------------------------
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/genomics/algorithms/templatescaler.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/basic/set.h>
#include <kernel/basic/algorithm.h>
#include <kernel/basic/message.h>
#include <kernel/utilities/creator.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/dictionary.h>

#include <common/objectmanager.h>

#include <math.h>
#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Methods for class TemplateScaler.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

TemplateScaler::TemplateScaler() {

	template_length_ = 2;
	constant_template_length_ = 3;
	crease_thr_ = 0.600f;
	constant_thr_ = 0.200f;
	//start_end_thr_ = 0.100f;
	//tolerance_thr_ = 0.200f;
	start_end_thr_ = 0.000f;
	tolerance_thr_ = 99.00f;
	template_length_max = Undefined::Integer();

	supervised_ = false;

	breaks_.erase(breaks_.begin(), breaks_.end());
	skip_ = false;

	//MOTIFS
	//skip_ = true;
	//breaks_.reserve(1);
	//breaks_.push_back(17);
	//breaks_.push_back(1);
	//YEAST
	//breaks_.reserve(16);
	//breaks_.push_back(8);breaks_.push_back(13);breaks_.push_back(18);breaks_.push_back(22);
	//breaks_.push_back(25);breaks_.push_back(28);breaks_.push_back(38);breaks_.push_back(47);
	//breaks_.push_back(55);breaks_.push_back(62);breaks_.push_back(70);breaks_.push_back(77);
	//breaks_.push_back(82);breaks_.push_back(87);breaks_.push_back(97);breaks_.push_back(105);

}

TemplateScaler::~TemplateScaler() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(TemplateScaler, TEMPLATESCALER, Scaler)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
TemplateScaler::GetParameters() const {

	return "";

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
TemplateScaler::SetParameter(const String &keyword, const String &value) {

	if (Scaler::SetParameter(keyword, value))
		return true;

	if (keyword == Keyword::TemplateLength() && value.IsInteger()) {
		template_length_ = value.GetInteger();
		return true;
	}
	
	if (keyword == Keyword::ConstantTemplateLength() && value.IsInteger()) {
		constant_template_length_ = value.GetInteger();
		return true;
	}

	if (keyword == Keyword::CreaseThr() && value.IsFloat()) {
		crease_thr_ = value.GetFloat();
		return true;
	}

	if (keyword == Keyword::ConstantThr() && value.IsFloat()) {
		constant_thr_ = value.GetFloat();
		return true;
	}

	if (keyword == Keyword::StartEndThr() && value.IsFloat()) {
		start_end_thr_ = value.GetFloat();
		return true;
	}

	if (keyword == Keyword::ToleranceThr() && value.IsFloat()) {
		tolerance_thr_ = value.GetFloat();
		return true;
	}

	if (keyword == Keyword::Supervised() && value.IsBoolean()) {
		supervised_ = value.GetBoolean();
		return true;
	}

	return false;
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
TemplateScaler::Apply(Structure &structure) const {

	Message message;

	// Check if input is of expected type.
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> input = dynamic_cast(DecisionTable *, &structure);

	Handle<DecisionTable> output;

	// If possible, do the actual scaling
	if (input->GetNoAttributes(true) > 1) {
		message.Notify("Templatizing table \"" + input->GetName() + "\"...");
		output = Templatize(*input);
	} else {
		Message::Error("Table has no condition attributes.", false);
		return NULL;
	}

	if (output == NULL) {
		return NULL;
	}

	// Append string to table name.
	output->SetName(input->GetName() + ", templatized");

	return output.Release();
}

//-------------------------------------------------------------------
// Method........: Templatize
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable *
TemplateScaler::Templatize(DecisionTable &table) const {

	Message message;

	bool masked = false;

	// Determine table size.
	int no_attributes = table.GetNoAttributes(masked);

	// Ititialize the output table
	Handle<DecisionTable> output = InitializeTemplateTable(table, masked);
	if (output == NULL) {
		return NULL;
	}

	int k = 1;
	// Iterate over all attributes.
	for (int i = 1; i < no_attributes - 1 - template_length_; i++) {
		for (int j = i + template_length_; j < no_attributes - 1; j++) {

			if (j-i > template_length_max) break;

			if (skip_ && (i > breaks_[breaks_.size()-1] || j > breaks_[breaks_.size()-1])) {
				continue;
			}

			bool found = false;
			for (int b = 0; b < breaks_.size(); b++) {
				if (breaks_[b] >= i && breaks_[b] < j) {
					found = true;
					break;
				}
			}
			if (found) continue;

			// Match templates in this sub-interval
			if (!MatchTemplates(table, *output, i, j, k, masked)) {
				return NULL;
			}

			k++;
		}
	}

	//Supervise the templatized table
	if (supervised_) {
		message.Notify("  Supervising table \"" + output->GetName() + "\"...");
		if (!Supervise(*output, masked)) {
			return NULL;
		}
	}

	// Mask unique attribute
	Vector(DecisionTable::Mask) masks;

	if (!GetAttributeMasks(*output, masks))
		return NULL;

	masks[0] = DecisionTable::MASK_DISABLED;

	if (!SetAttributeMasks(*output, masks))
		return NULL;

	return output.Release();
}

//-------------------------------------------------------------------
// Method........: InitializeTemplateTable
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable *
TemplateScaler::InitializeTemplateTable(const DecisionTable &table, bool masked) const {

	int exponent = table.GetDictionary()->GetAttribute(1)->GetScalingExponent();
	float missing_value = Undefined::Integer() * MathKit::Power(10.0, -exponent);

	// Determine table size of the orignal (old) table 
	int no_attributes_old = table.GetNoAttributes(masked);

	// Determine table size of new table
	int i,j, no_attributes_new = 0;
	for (i = 1; i < no_attributes_old - 1 - template_length_; i++) {
		for (j = i + template_length_; j < no_attributes_old - 1; j++) {
	
			if (j-i > template_length_max) break;

			if (skip_ && (i > breaks_[breaks_.size()-1] || j > breaks_[breaks_.size()-1])) {
				continue;
			}
			bool found = false;
			for (int b = 0; b < breaks_.size(); b++) {
				if (breaks_[b] >= i && breaks_[b] < j) {
					found = true;
					break;
				}
			}
			if (found) continue;
			no_attributes_new++;
		}
	}
	no_attributes_new = no_attributes_new + 2;

	int skip_start = no_attributes_new-1;
	if (skip_)
		no_attributes_new = no_attributes_new + 2 + no_attributes_old - breaks_[breaks_.size()-1] - 4;

	//Inititalize new table
	Handle<DecisionTable> output = dynamic_cast(DecisionTable *,Creator::DecisionTable());
	output->RemoveAllChildren();
	output->Resize(1, no_attributes_new, masked);

	// Assign the table a new, virgin dictionary.
	output->SetDictionary(Creator::Dictionary());
	//output->SetDictionary(table.GetDictionary());

	// Create string type attribute for the uniqe attribute
	Handle<Attribute> attribute = Creator::StringAttribute();
	attribute->SetName(table.GetAttributeName(0, masked));
	if (!output->GetDictionary()->AppendAttribute(attribute.GetPointer())) {
		Message::Error("Error appending attribute " + String::Format(0) + " to dictionary.");
		return NULL;
	}

	// Iterate over all attributes.
	for (i = 1; i < no_attributes_old - 1 - template_length_; i++) {
		for (j = i + template_length_; j < no_attributes_old - 1; j++) {

			if (j-i > template_length_max) break;

			if (skip_ && (i > breaks_[breaks_.size()-1] || j > breaks_[breaks_.size()-1])) {
				continue;
			}
			bool found = false;
			for (int b = 0; b < breaks_.size(); b++) {
				if (breaks_[b] >= i && breaks_[b] < j) {
					found = true;
					break;
				}
			}
			if (found) continue;

			// Create string type attribute
			Handle<Attribute> attribute = Creator::StringAttribute();
			//Handle<Attribute> attribute = Creator::FloatAttribute(exponent, NULL, false);

			String name = table.GetAttributeName(i, masked) + " - " + table.GetAttributeName(j, masked);
			attribute->SetName(name);

			if (!output->GetDictionary()->AppendAttribute(attribute.GetPointer())) {
				Message::Error("Error appending attribute " + String::Format(i + 1) + " to dictionary.");
				return NULL;
			}
		}
	}

	// Add attributes for non-time series data
	if (skip_) {
		for (i = breaks_[breaks_.size()-1] + 1; i < no_attributes_old-1; i++) {
			// Create string type attribute
			Handle<Attribute> attribute = Creator::StringAttribute();

			String name = table.GetAttributeName(i, masked);
			attribute->SetName(name);

			if (!output->GetDictionary()->AppendAttribute(attribute.GetPointer())) {
				Message::Error("Error appending attribute " + String::Format(i + 1) + " to dictionary.");
				return NULL;
			}
		}
	}

	// Create string type attribute for the decision
	attribute = Creator::StringAttribute();
	attribute->SetName(table.GetAttributeName(no_attributes_old - 1, masked));
	if (!output->GetDictionary()->AppendAttribute(attribute.GetPointer())) {
		Message::Error("Error appending attribute " + String::Format(i + 1) + " to dictionary.");
		return NULL;
	}

	int test = output->GetNoAttributes(masked);

	// Set default table status fields.
	for (i = 0; i < no_attributes_new; i++) {
		if (i == no_attributes_new - 1)
			output->SetStatus(i, DecisionTable::STATUS_DECISION, masked);
		else
			output->SetStatus(i, DecisionTable::STATUS_CONDITION, masked);
	}


	int no_objects_old = table.GetNoObjects(masked);
	int k;

	for (k = 0; k < no_objects_old; k++) {

		// Append an object to the table
		if ((k > 0) && !output->AppendObject(masked)) {
			Message::Error("Error appending object " + String::Format(k + 1) + " to decision table.");
			return NULL;
		}

		int i; int skip_i;

		if (skip_) skip_i = breaks_[breaks_.size()-1] + 1;

		// Update table and dictionary.
		for (i = 0; i < no_attributes_new; i++) {

			String real_value;
			int dictonary_value;

			if (skip_ && i > skip_start && i < no_attributes_new - 1) {
				real_value = table.GetEntry(k, skip_i, true, masked);
				dictonary_value = table.GetEntry(k, skip_i, masked);
				skip_i++;
			} else if (i == 0) {
				real_value = table.GetEntry(k, 0, true, masked);
				dictonary_value = table.GetEntry(k, 0, masked);
			} else if (i == no_attributes_new - 1) {
				real_value = table.GetEntry(k, no_attributes_old - 1, true, masked);
				dictonary_value = table.GetEntry(k, no_attributes_old - 1, masked);
			} else {
				//real_value = "NoMatch";
				real_value = "Undefined";
				dictonary_value = output->SuggestDictionaryEntry(i, real_value, masked);
			}

			if (!output->SetEntry(k, i, dictonary_value, masked)) {
				Message::Error("Error setting table entry (" + String::Format(k) + ", " + String::Format(i) + ").");
				return NULL;
			}
			if (!output->SetDictionaryEntry(i, dictonary_value, real_value, masked)) {
				Message::Error("Error setting dictionary entry (" + String::Format(i) + ", " + String::Format(dictonary_value) + ", \"" + real_value + "\").");
				return NULL;
			}
		}
	}

	return output.Release();

}

//-------------------------------------------------------------------
// Method........: MatchTemplates
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
TemplateScaler::MatchTemplates(const DecisionTable &tablein, DecisionTable &tableout, int i, int j, int ij, bool masked) const {

	int exponent = tablein.GetDictionary()->GetAttribute(1)->GetScalingExponent();
	float missing_value = Undefined::Integer() * MathKit::Power(10.0, -exponent);

	int no_objects = tablein.GetNoObjects(masked);

	Vector(float)   status;	  
	status.erase(status.begin(), status.end());
	status.reserve(no_objects);

	Vector(float) average;
	average.erase(average.begin(), average.end());
	average.reserve(no_objects);

	Vector(float) max;
	max.erase(max.begin(), max.end());
	max.reserve(no_objects);

	Vector(float) min;
	min.erase(min.begin(), min.end());
	min.reserve(no_objects);

	Vector(float) entries1;
	entries1.erase(entries1.begin(), entries1.end());
	entries1.reserve(no_objects);
	Vector(float) entries2;	
	entries2.erase(entries2.begin(), entries2.end());
	entries2.reserve(no_objects);

	// Initiate status vector, average and max/min
	tablein.GetEntries(entries1, i, masked, true, true);
	tablein.GetEntries(entries2, j, masked, true, true);

	int l;
	for (l = 0; l < no_objects; l++) {
		// Missing values
		if (entries1[l] == missing_value || entries2[l] == missing_value) {
			status[l] = missing_value; continue;
		}	
		if (entries2[l] > entries1[l] + crease_thr_) {
			//status[l] = entries2[l] - entries1[l];
			status[l] = 1;
		} else if (entries1[l] > entries2[l] + crease_thr_) {
			//status[l] = entries2[l] - entries1[l];
			status[l] = 2;
		} else {
			status[l] = missing_value;
		}
		average[l] = min[l] = max[l] = entries1[l];
	}

	for (int k = i + 1; k < j + 1; k++) {
		tablein.GetEntries(entries1, k-1, masked, false, true);
		tablein.GetEntries(entries2, k, masked, false, true);
		for (l = 0; l < no_objects; l++) {
			if (entries1[l] == missing_value || entries2[l] == missing_value) continue;
			// Update average and max/min
			average[l] = average[l] + entries2[l];
			if (entries2[l] > max[l]) max[l] = entries2[l];
			if (entries2[l] < min[l]) min[l] = entries2[l];
			if (!status[l]) continue;
			// First or last sub-interval
			if (k == i + 1 || k == j) {
				if (status[l] == 1 && !(entries2[l] > entries1[l] + start_end_thr_)) {
					status[l] = missing_value;
				} else if (status[l] == 2 && !(entries1[l] > entries2[l] + start_end_thr_)) {
					status[l] = missing_value;
				}
			} else {
				// Intermediate sub-intervals
				if (status[l] == 1 && !(entries2[l] + tolerance_thr_ > entries1[l])) {
					status[l] = missing_value;
				} else if (status[l] == 2 && !(entries1[l] + tolerance_thr_ > entries2[l])) {
					status[l] = missing_value;
				}
			}
		}
	}

	for (l = 0; l < no_objects; l++) {
		// Compute average
		average[l] = average[l]/(j - i + 1);

		// Constant
		if ((j - i > constant_template_length_ - 1) && (max[l] - average[l] < constant_thr_) && (average[l] - min[l] < constant_thr_))
			status[l] = 3;

		if (status[l] == missing_value) continue;

		// Set real value
		String real_value;
		//real_value = String::Format(status[l])
		if (status[l] == 1)      { real_value = "Increasing"; }
		else if (status[l] == 2) { real_value = "Decreasing"; }
		else if (status[l] == 3) { real_value = "Constant";   }

		// Insert values
		int dictonary_value = tableout.SuggestDictionaryEntry(ij, real_value, masked);
		if (!tableout.SetEntry(l, ij, dictonary_value, masked)) {
			Message::Error("Error setting table entry (" + String::Format(l) + ", " + String::Format(ij) + ").");
			return false;
		}
		if (!tableout.SetDictionaryEntry(ij, dictonary_value, real_value, masked)) {
			Message::Error("Error setting dictionary entry (" + String::Format(i) + ", " + String::Format(dictonary_value) + ", \"" + real_value + "\").");
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------
// Method........: Supervise
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
TemplateScaler::Supervise(DecisionTable &table, bool masked) const {

	Message message;

	int no_objects = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);

	Handle<DecisionTable> oldtable = dynamic_cast(DecisionTable *, table.Duplicate());

	Vector(int) decisions; 
	decisions.erase(decisions.begin(), decisions.end());
	decisions.reserve(no_objects);
	table.GetEntries(decisions, table.GetDecisionAttribute(masked), masked, false);

	int i,j,k;
	int no_entries_removed = 0;
	int no_entries = 0;

	Vector(int) remove_list;
	remove_list.erase(remove_list.begin(), remove_list.end());
	remove_list.reserve(no_objects);

	for (i = 0; i < no_objects; i++) {
		bool object_ok = false;
		for (k = 1; k < no_attributes - 1; k++) {
			bool attribute_ok = false;
			if (oldtable->GetEntry(i, k, true, masked) != "NoMatch") {
				no_entries++;
				for (j = 0; j < no_objects; j++) {
					if (i != j && decisions[i] == decisions[j] && 
						oldtable->GetEntry(i, k, true, masked) == oldtable->GetEntry(j, k, true, masked)) {
						attribute_ok = true;
						object_ok = true;
						break;
					}
				}
				// Remove this entry
				if (!attribute_ok) {
					no_entries_removed++;
					int dictonary_value = table.SuggestDictionaryEntry(k, "NoMatch", masked);
					if (!table.SetEntry(i, k, dictonary_value, masked)) {
						Message::Error("Error setting table entry (" + String::Format(i) + ", " + String::Format(k) + ").");
						return false;
					}
					if (!table.SetDictionaryEntry(k, dictonary_value, "NoMatch", masked)) {
						Message::Error("Error setting dictionary entry (" + String::Format(k) + ", " + String::Format(dictonary_value) + ", \" - \").");
						return false;
					}
				}
			}
		}
		// Remove this object
		if (!object_ok) {
			remove_list.push_back(i);
		}
	}

	int z = remove_list.size();
	for (i = remove_list.size() - 1; i > - 1; i--) { 
		if (!table.RemoveObject(remove_list[i], masked)) {
			return false;
		}
	}

	String no_entries_removed_string = String::Format(no_entries_removed);
	String no_entries_string = String::Format(no_entries);
	message.Notify("No. of entries removed during supervization: " + no_entries_removed_string + "/" + no_entries_string);
#ifdef X86_64
	String no_objects_removed_string = String::Format((int)remove_list.size());
#else
	String no_objects_removed_string = String::Format(remove_list.size());
#endif
	message.Notify("No. of objects removed during supervization: " + no_objects_removed_string);

	return true;

}

//-------------------------------------------------------------------
// Method........: Supervise2
// Author........: Torgeir R. Hvidsten
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
TemplateScaler::Supervise2(DecisionTable &table, bool masked) const {

	Message message;

	int no_objects = table.GetNoObjects(masked);
	int no_attributes = table.GetNoAttributes(masked);

	Handle<DecisionTable> oldtable = dynamic_cast(DecisionTable *, table.Duplicate());

	Vector(int) decisions; decisions.reserve(no_objects);
	table.GetEntries(decisions, table.GetDecisionAttribute(masked), masked, false);

	int i,j,k;
	int no_entries_removed = 0;
	int no_entries = 0;

	Vector(int) remove_list; remove_list.reserve(no_objects);

	for (i = 0; i < no_objects; i++) {
		bool object_ok = false;
		for (k = 1; k < no_attributes - 1; k++) {
			if (oldtable->GetEntry(i, k, true, masked) != "NoMatch") {
				int pro = 0, con = 0;
				no_entries++;
				for (j = 0; j < no_objects; j++) {
					if (i != j && decisions[i] == decisions[j] && 
						oldtable->GetEntry(i, k, true, masked) == oldtable->GetEntry(j, k, true, masked)) {
						pro++;
					}
					if (i != j && decisions[i] != decisions[j] && 
						oldtable->GetEntry(i, k, true, masked) == oldtable->GetEntry(j, k, true, masked)) {
						con++;
					}
				}
				// Remove this entry
				if (1 > pro) {
					no_entries_removed++;
					int dictonary_value = table.SuggestDictionaryEntry(k, "NoMatch", masked);
					if (!table.SetEntry(i, k, dictonary_value, masked)) {
						Message::Error("Error setting table entry (" + String::Format(i) + ", " + String::Format(k) + ").");
						return false;
					}
					if (!table.SetDictionaryEntry(k, dictonary_value, "NoMatch", masked)) {
						Message::Error("Error setting dictionary entry (" + String::Format(k) + ", " + String::Format(dictonary_value) + ", \" - \").");
						return false;
					}
				} else {
					object_ok = true;
				}
			}
		}
		// Remove this object
		if (!object_ok) {
			remove_list.push_back(i);
		}
	}

	int z = remove_list.size();
	for (i = remove_list.size() - 1; i > - 1; i--) { 
		if (!table.RemoveObject(remove_list[i], masked)) {
			return false;
		}
	}

	String no_entries_removed_string = String::Format(no_entries_removed);
	String no_entries_string = String::Format(no_entries);
	message.Notify("No. of entries removed during supervization: " + no_entries_removed_string + "/" + no_entries_string);
#ifdef X86_64
	String no_objects_removed_string = String::Format((int)remove_list.size());
#else
	String no_objects_removed_string = String::Format(remove_list.size());
#endif
	message.Notify("No. of objects removed during supervization: " + no_objects_removed_string);

	return true;

}

TemplateScaler *
TemplateScaler::Clone() {
	return new TemplateScaler;
} 
