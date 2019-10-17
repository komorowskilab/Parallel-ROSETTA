//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KSDECISIONTABLE_H__
#define __KSDECISIONTABLE_H__

#include <copyright.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/basic/vector.h>

//-------------------------------------------------------------------
// Class.........: KSDecisionTable
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: A simple realization of a decision table.
//                 First pass, crude implementation, improve later on.
// Revisions.....:
//===================================================================

class KSDecisionTable : public DecisionTable {
protected:

	//- Member variables...............................................
	int                           no_objects_;           // The number of objects in the unmasked table.
	int                           no_attributes_;        // The number of attributes in the unmasked table.
	Vector(Vector(int))           table_;                // The table itself, a vector of inf. vectors.
	Vector(DecisionTable::Mask)   attribute_masks_;      // Masking of attributes.
	Vector(int)                   attribute_map_;        // Maps from masked indices to unmasked indices for attributes.

protected:

  //- Constructors...................................................
  KSDecisionTable(const KSDecisionTable &in);

  //- Helper methods.................................................
	bool                          BuildAttributeMap(int attribute_no);

public:

  //- Constructors/destructor........................................
  KSDecisionTable();
  virtual ~KSDecisionTable();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Structure..............................
  virtual Structure            *Duplicate() const;
  virtual void                  Clear();

  //- Methods inherited from DecisionTable..........................
  virtual int                   GetNoObjects(bool masked) const;
  virtual int                   GetNoAttributes(bool masked) const;

	virtual bool                  Reserve(int no_objects, int no_attributes);

  virtual int                   GetEntry(int object_no, int attribute_no, bool masked) const;
  virtual bool                  SetEntry(int object_no, int attribute_no, int value, bool masked);

  virtual bool                  InsertObject(int object_no, bool masked);
  virtual bool                  RemoveObject(int object_no, bool masked);

  virtual bool                  InsertAttribute(int attribute_no, const Attribute *attribute, bool masked);
  virtual bool                  RemoveAttribute(int attribute_no, bool masked);

  virtual bool                  SwapAttributes(int i, int j, bool masked);

	virtual DecisionTable::Status GetStatus(int attribute_no, bool masked) const;
	virtual bool                  SetStatus(int attribute_no, DecisionTable::Status status, bool masked);

	virtual DecisionTable::Mask   GetAttributeMask(int attribute_no) const;
	virtual bool                  SetAttributeMask(int attribute_no, DecisionTable::Mask mask, bool safe = true);

	virtual DecisionTable::Mask   GetObjectMask(int object_no) const;
	virtual bool                  SetObjectMask(int object_no, DecisionTable::Mask mask, bool safe = true);

	virtual int                   GetUnmaskedObject(int masked_object_no) const;
	virtual int                   GetUnmaskedAttribute(int masked_attribute_no) const;

};

#endif 
