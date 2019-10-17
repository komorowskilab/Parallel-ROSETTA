//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/ksdecisiontable.h>

//-------------------------------------------------------------------
// Methods for class KSDecisionTable.
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

KSDecisionTable::KSDecisionTable(const KSDecisionTable &in) : DecisionTable(in) {
	no_objects_           = in.no_objects_;
	no_attributes_        = in.no_attributes_;
	table_                = in.table_;
	attribute_masks_      = in.attribute_masks_;
	attribute_map_        = in.attribute_map_;
}

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSDecisionTable::KSDecisionTable() {
	no_objects_           = 0;
	no_attributes_        = 0;
}

//-------------------------------------------------------------------
// Method........: Destructor.
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KSDecisionTable::~KSDecisionTable() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(KSDecisionTable, KSDECISIONTABLE, DecisionTable)

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
KSDecisionTable::Duplicate() const {
  return new KSDecisionTable(*this);
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
KSDecisionTable::Clear() {
	DecisionTable::Clear();
}

//-------------------------------------------------------------------
// Methods inherited from DecisionTable.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoObjects
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of objects in the table.
// Comments......: Object masking not implemented yet.
// Revisions.....:
//===================================================================

int
KSDecisionTable::GetNoObjects(bool /*masked*/) const {
	return no_objects_;
}

//-------------------------------------------------------------------
// Method........: GetNoAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the number of attributes in the table.
// Comments......:
// Comments......:
// Revisions.....:
//===================================================================

int
KSDecisionTable::GetNoAttributes(bool masked) const {

	if (masked)
		return attribute_map_.size();
	else
		return no_attributes_;

}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........: Aleksander šhrn
// Date..........:
// Description...: Reserves space, if relevant for the physical
//                 implementation, for a decision table of the
//                 specified size.
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::Reserve(int no_objects, int no_attributes) {

	table_.reserve(no_objects);

	int i;

	for (i = 0; i < table_.size(); i++)
		table_[i].reserve(no_attributes);

	attribute_masks_.reserve(no_attributes);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: -N 21/05/13 Added critical to return statement,
//			hopefully avoiding issues with the stl.
//		   -N 14/06/13
//		   stl containers are no longer shared, and thus
//			critical enclosures only slow things down.
//			Removed them.
//		   -N 17/06/13 This is the third most time consuming
//		   function of the program in CV. Attempting to inline.
//===================================================================

int
inline KSDecisionTable::GetEntry(int object_no, int attribute_no, bool masked) const {
/*
#ifdef _DEBUG
	// Are the indices legal?
	if ((object_no < 0) || (object_no >= GetNoObjects(masked)) || (attribute_no < 0) || (attribute_no >= GetNoAttributes(masked))) {
		Message::Error("Entry indices out of range.");
		return Undefined::Integer();
	}
#endif
*/
	int object_no_unmasked    = (masked) ? GetUnmaskedObject(object_no)       : object_no;
	int attribute_no_unmasked = (masked) ? GetUnmaskedAttribute(attribute_no) : attribute_no;

	return table_[object_no_unmasked][attribute_no_unmasked];
}

//-------------------------------------------------------------------
// Method........: SetEntry
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::SetEntry(int object_no, int attribute_no, int value, bool masked) {

#ifdef _DEBUG
	// Are the indices legal?
	if ((object_no < 0) || (object_no >= GetNoObjects(masked)) || (attribute_no < 0) || (attribute_no >= GetNoAttributes(masked))) {
		Message::Error("Entry indices out of range.");
		return false;
	}
#endif

	int object_no_unmasked    = (masked) ? GetUnmaskedObject(object_no)       : object_no;
	int attribute_no_unmasked = (masked) ? GetUnmaskedAttribute(attribute_no) : attribute_no;

	table_[object_no_unmasked][attribute_no_unmasked] = value;

	return true;

}

//-------------------------------------------------------------------
// Method........: InsertObject
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::InsertObject(int object_no, bool masked) {

#ifdef _DEBUG
	// Is the index legal?
	if ((object_no < 0) || (object_no > GetNoObjects(masked))) {
		Message::Error("Object index out of range.");
		return false;
	}
#endif

	Vector(int) inf;
	inf.reserve(no_attributes_);

	int i;

	// Prepare an information vector with default values.
	for (i = 0; i < no_attributes_; i++)
		inf.push_back(Undefined::Integer());

	int object_no_unmasked = (masked) ? GetUnmaskedObject(object_no) : object_no;

	// Insert it into the table.
	table_.insert(table_.begin() + object_no_unmasked, inf);

	// Update size variables.
	no_objects_++;

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveObject
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::RemoveObject(int object_no, bool masked) {

#ifdef _DEBUG
	// Is the index legal?
	if ((object_no < 0) || (object_no >= GetNoObjects(masked))) {
		Message::Error("Object index out of range.");
		return false;
	}
#endif

	int object_no_unmasked = (masked) ? GetUnmaskedObject(object_no) : object_no;

	table_.erase(table_.begin() + object_no_unmasked);

	// Update size variables.
	no_objects_--;

	return true;

}

//-------------------------------------------------------------------
// Method........: InsertAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::InsertAttribute(int attribute_no, const Attribute *attribute, bool masked) {

#ifdef _DEBUG
	// Is the index legal?
	if ((attribute_no < 0) || (attribute_no > GetNoAttributes(masked))) {
		Message::Error("Attribute index out of range.");
		return false;
	}
#endif

	// Does the decision table have any children?
	if (HasChildren()) {
		Message::Error("Cannot insert attribute, the decision table has children.");
		return false;
	}

	int i, attribute_no_unmasked = (masked) ? GetUnmaskedAttribute(attribute_no) : attribute_no;

	// Insert values into table.
	for (i = 0; i < no_objects_; i++) {
		table_[i].insert(table_[i].begin() + attribute_no_unmasked, Undefined::Integer());
	}

	// Update size variables.
	no_attributes_++;

	// Deal with dictionary stuff higher up.
	if (!DecisionTable::InsertAttribute(attribute_no, attribute, masked))
		return false;

	attribute_masks_.insert(attribute_masks_.begin() + attribute_no_unmasked, DecisionTable::MASK_ENABLED);

	if (!BuildAttributeMap(attribute_no_unmasked))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: RemoveAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::RemoveAttribute(int attribute_no, bool masked) {

#ifdef _DEBUG
	// Is the index legal?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes(masked))) {
		Message::Error("Attribute index out of range.");
		return false;
	}
#endif

	int i, attribute_no_unmasked = (masked) ? GetUnmaskedAttribute(attribute_no) : attribute_no;

	for (i = 0; i < no_objects_; i++)
		table_[i].erase(table_[i].begin() + attribute_no_unmasked);

	// Update size variables.
	no_attributes_--;

	// Deal with dictionary stuff higher up.
	if (!DecisionTable::RemoveAttribute(attribute_no, masked))
		return false;

	attribute_masks_.erase(attribute_masks_.begin() + attribute_no_unmasked);

	if (!BuildAttributeMap(attribute_no_unmasked))
		return false;

	return true;

}

//-------------------------------------------------------------------
// Method........: SwapAttributes
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::SwapAttributes(int i, int j, bool masked) {
	return DecisionTable::SwapAttributes(i, j, masked);
}

//-------------------------------------------------------------------
// Method........: GetStatus
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable::Status
KSDecisionTable::GetStatus(int attribute_no, bool masked) const {

#ifdef _DEBUG
	// Is the index legal?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes(masked))) {
		Message::Error("Attribute index out of range.");
		return DecisionTable::STATUS_CONDITION;
	}
#endif

	if (attribute_no == GetNoAttributes(masked) - 1)
		return DecisionTable::STATUS_DECISION;
	else
		return DecisionTable::STATUS_CONDITION;

}

//-------------------------------------------------------------------
// Method........: SetStatus
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::SetStatus(int attribute_no, DecisionTable::Status status, bool masked) {

#ifdef _DEBUG
	// Is the index legal?
	if ((attribute_no < 0) || (attribute_no >= GetNoAttributes(masked))) {
		Message::Error("Attribute index out of range.");
		return false;
	}
#endif

	if (attribute_no == no_attributes_ - 1 && status == DecisionTable::STATUS_DECISION)
		return true;

	if (attribute_no < no_attributes_ && status == DecisionTable::STATUS_CONDITION)
		return true;

	return false;

}

//-------------------------------------------------------------------
// Operator......: GetAttributeMask
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the current mask of the given attribute.
//
//                 Note that the given attribute index is considered
//                 to be actual and not virtual.
//
//                 Cf. method header of SetAttributeMask concerning how
//                 RSES handles masking and decision attribute management.
//
//                 Example:
//
//                    Actual, unmasked table:     | a b c d e
//                                             ---+-----------
//                                              0 | 5 3 1 2 8
//                                              1 | 6 7 1 7 2
//                                              2 | 4 3 6 2 8
//                                              3 | 1 2 1 2 6
//
//                    Attribute enablement mask:   {1 0 0 1 1}
//
//                    Virtual, masked table:      | a d e
//                                             ---+-------
//                                              0 | 5 2 8
//                                              1 | 6 7 2
//                                              2 | 4 2 8
//                                              3 | 1 2 6
//
//                    Actual table:
//
//                      GetNoAttributes(false) returns 5.
//                      GetEntry(2, 1, false) returns (2, b), i.e. 3.
//
//                    Virtual table:
//
//                      GetNoAttributes(true) returns 3.
//                      GetEntry(2, 1, true) returns (2, d), i.e. 2.
//
// Comments......:
// Revisions.....:
//===================================================================

DecisionTable::Mask
KSDecisionTable::GetAttributeMask(int attribute_no) const {

#ifdef _DEBUG
	if ((attribute_no < 0) || (attribute_no > GetNoAttributes(false))) {
		Message::Error("Attribute index out of range.");
		return DecisionTable::MASK_DISABLED;
	}
#endif

	return attribute_masks_[attribute_no];

}

//-------------------------------------------------------------------
// Method........: SetAttributeMask
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Enables or disables the given attribute.
//
//                 Note that the given attribute index is considered
//                 to be actual and not virtual.
//
//                 If the safety flag is specified (true by default), it
//                 is checked that the table has no children. This
//                 since it may be unsafe to alter the masking of a table
//                 with children, since these chldren may refer to the
//                 parent table through indices, which necessarly change
//                 if the masking changes.
// Revisions.....:
//===================================================================

bool
KSDecisionTable::SetAttributeMask(int attribute_no, DecisionTable::Mask mask, bool safe) {

#ifdef _DEBUG
	if ((attribute_no < 0) || (attribute_no > GetNoAttributes(false))) {
		Message::Error("Attribute index out of range.");
		return false;
	}
#endif

	if (mask == GetAttributeMask(attribute_no))
		return true;

	// If the table has children, altering the enablement masking may cause
	// trouble, since most children store the masked indices that were valid when
	// they were created.
	if (safe && HasChildren()) {
		Message::Error("A table with children cannot have its masking altered.");
		return false;
	}

	attribute_masks_[attribute_no] = mask;

	return BuildAttributeMap(attribute_no);

}

//-------------------------------------------------------------------
// Operator......: GetObjectMask
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the current mask of the given object.
//
//                 Note that the given object index is considered
//                 to be actual and not virtual.
//
// Comments......: Object masking not implemented yet.
// Revisions.....:
//===================================================================

DecisionTable::Mask
KSDecisionTable::GetObjectMask(int object_no) const {

#ifdef _DEBUG
	if ((object_no < 0) || (object_no >= GetNoObjects(false))) {
		Message::Error("Object index out of range.");
		return DecisionTable::MASK_DISABLED;
	}
#endif

	return DecisionTable::MASK_ENABLED;

}

//-------------------------------------------------------------------
// Operator......: SetObjectMask
// Author........: Aleksander šhrn
// Date..........:
// Description...: Sets the mask of the given object.
//
//                 Note that the given object index is considered
//                 to be actual and not virtual.
//
//                 If the safety flag is specified (true by default), it
//                 is checked that the table has no children. This
//                 since it may be unsafe to alter the masking of a table
//                 with children, since these chldren may refer to the
//                 parent table through indices, which necessarly change
//                 if the masking changes.
//
// Comments......: Object masking not implemented yet.
// Revisions.....:
//===================================================================

bool
KSDecisionTable::SetObjectMask(int object_no, DecisionTable::Mask mask, bool safe) {

#ifdef _DEBUG
	if ((object_no < 0) || (object_no >= GetNoObjects(false))) {
		Message::Error("Object index out of range.");
		return false;
	}
#endif

	if (mask == GetObjectMask(object_no))
		return true;

	// If the table has children, altering the enablement masking may cause
	// trouble, since most children store the masked indices that were valid when
	// they were created.
	if (safe && HasChildren()) {
		Message::Error("A table with children cannot have its masking altered.");
		return false;
	}

	return false;

}

//-------------------------------------------------------------------
// Method........: GetUnmaskedObject
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from a virtual (masked) object index to
//                 an actual (unmasked) object index.
//
// Comments......: Object masking not implemented yet.
// Revisions.....:
//===================================================================

int
KSDecisionTable::GetUnmaskedObject(int masked_object_no) const {

#ifdef _DEBUG
	if ((masked_object_no < 0) || (masked_object_no > GetNoObjects(false))) {
		Message::Error("Object index out of range.");
		return Undefined::Integer();
	}
#endif

	return masked_object_no;

}

//-------------------------------------------------------------------
// Method........: GetUnmaskedAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Maps from a virtual (masked) attribute index to
//                 an actual (unmasked) attribute index.
// Comments......:
// Revisions.....: -N 21/05/13 Added critical section to stl.
//		   -N 14/06/13 stl containers are no longer shared.
//			Removing critical enclosures.
//===================================================================

int
KSDecisionTable::GetUnmaskedAttribute(int masked_attribute_no) const {

#ifdef _DEBUG
	if ((masked_attribute_no < 0) || (masked_attribute_no > GetNoAttributes(false))) {
		Message::Error("Attribute index out of range.");
		return Undefined::Integer();
	}
#endif

	return attribute_map_[masked_attribute_no];
}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: BuildAttributeMap
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates the map from masked indices to unmasked
//                 indices for attributes.
// Comments......:
// Revisions.....:
//===================================================================

bool
KSDecisionTable::BuildAttributeMap(int attribute_no) {

	// Clear current map.
	attribute_map_.erase(attribute_map_.begin(), attribute_map_.end());
	attribute_map_.reserve(no_attributes_);

	int i;

	// For now, rebuild map from scratch.
	for (i = 0; i < no_attributes_; i++) {
		if (attribute_masks_[i] == DecisionTable::MASK_ENABLED) {
			attribute_map_.push_back(i);
		}
	}

	return true;

}
 
