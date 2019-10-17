//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/costinformation.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/reduct.h>

#include <kernel/utilities/iokit.h>

#include <kernel/system/fstream.h>

//-------------------------------------------------------------------
// Methods for class CostInformation.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Copy constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

CostInformation::CostInformation(const CostInformation &in) {
	costs_ = in.costs_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

CostInformation::CostInformation() {
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

CostInformation::~CostInformation() {
}

//-------------------------------------------------------------------
// Local methods.
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
CostInformation::Load(const String &filename, const DecisionTable &table, float defaultcost) {

	ifstream stream;

	if (!IOKit::Open(stream, filename))
		return false;

	return Load(stream, table, defaultcost);

}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: To do: Add support for shared costs.
// Revisions.....:
//===================================================================

bool
CostInformation::Load(ifstream &stream, const DecisionTable &table, float defaultcost) {

	// Operate on a masked table.
	bool masked = true;

	int no_attributes = table.GetNoAttributes(masked);

	// Initialize cost vector.
	costs_.erase(costs_.begin(), costs_.end());
	costs_.reserve(no_attributes);

	int i;

	for (i = 0; i < no_attributes; i++)
		costs_.push_back(defaultcost);

	int line_no = 0;

	String line;

	// Process all lines in the file.
	while (IOKit::LoadLine(stream, line, &line_no)) {

		// Split line into (name, cost) pair.
		int index = line.GetIndex("=");

		String name = line.Before(index);
		String cost = line.After(index);

		// Strip leading/trailing whitespace.
		name.Trim(" \t");
		cost.Trim(" \t");

		// Check cost validity.
		if (!cost.IsFloat()) {
			Message::Error("Cost on line number " + String::Format(line_no) + " (" + cost + ") is not a float.");
			return false;
		}

		// Check name validity.
		int attribute = table.GetAttributeIndex(name, false, masked);
		if (attribute == Undefined::Integer()) {
			Message::Error("Attribute name on line number " + String::Format(line_no) + " not recognized.");
			return false;
		}

		costs_[attribute] = cost.GetFloat();

	}

	return true;

}

//-------------------------------------------------------------------
// Method........: GetCost
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the cost of an individual attribute.
// Comments......:
// Revisions.....:
//===================================================================

float
CostInformation::GetCost(int attribute) const {

#ifdef _DEBUG
	if (attribute < 0 || attribute >= costs_.size()) {
		Message::Error("Index passed to cost information out of range.");
		return Undefined::Float();
	}
#endif

	return costs_[attribute];

}

//-------------------------------------------------------------------
// Method........: GetCost
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the cost of the attribute set.
// Comments......: To do: Add support for shared costs.
// Revisions.....:
//===================================================================

float
CostInformation::GetCost(const Vector(int) &attributes) const {

	int i;

	float cost = 0.0;

	for (i = attributes.size() - 1; i >= 0; i--)
		cost += GetCost(attributes[i]);

	return cost;

}

//-------------------------------------------------------------------
// Method........: GetCost
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the cost of the attribute set.
// Comments......: To do: Add support for shared costs.
// Revisions.....:
//===================================================================

float
CostInformation::GetCost(const Reduct &attributes) const {

	int i;

	float cost = 0.0;

	for (i = attributes.GetNoAttributes() - 1; i >= 0; i--)
		cost += GetCost(attributes.GetAttribute(i));

	return cost;

}


 
