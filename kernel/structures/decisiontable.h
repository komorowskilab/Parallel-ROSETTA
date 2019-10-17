//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DECISIONTABLE_H__
#define __DECISIONTABLE_H__

#include <copyright.h>

#include <kernel/structures/parentstructure.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/generalizeddecision.h>
#include <kernel/system/iostream.h>


//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Discerner;
class Dictionary;
class Rules;

//-------------------------------------------------------------------
// Class.........: DecisionTable
// Author........: Aleksander Ûhrn
// Date..........: 960307
// Description...: Base class for decision tables.
// Comments......: A decision table can also be viewed as a set of
//                 information vectors.  Consider therefore deriving
//                 this class from Structures instead of ParentStructure.
// Revisions.....:
//===================================================================

class DecisionTable : public ParentStructure {
public:

	//- Enumerated types...............................................
	enum Status {STATUS_CONDITION, STATUS_DECISION};
	enum Mask   {MASK_DISABLED, MASK_ENABLED};

	//- Textual representations........................................
	static String                 GetString(DecisionTable::Status status);
	static String                 GetString(DecisionTable::Mask mask);

public:

  //- Type definitions...............................................
	typedef Vector(Handle<DecisionTable>) Handles;

protected:

	//- Member variables...............................................
  Handle<Dictionary>            dictionary_; // The associated dictionary (optional).

protected:

  //- Constructors...................................................
  DecisionTable(const DecisionTable &in);

public:

	//- Constructors/destructor........................................
  	DecisionTable();
  	virtual ~DecisionTable();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Persistent..............................
	virtual bool                  Load(ifstream &stream);
	virtual bool                  Save(ofstream &stream) const;

	//- Methods inherited from Structure...............................
	virtual void                  Clear();
  //- Dimensionality methods.........................................
  virtual int                   GetNoObjects(bool masked) const = 0;
  virtual int                   GetNoAttributes(bool masked) const = 0;

	virtual bool                  Reserve(int no_objects, int no_attributes);
	virtual bool                  Resize(int no_objects, int no_attributes, bool masked);

  //- Access methods (slow but general)..............................
  virtual int                   GetEntry(int object_no, int attribute_no, bool masked) const = 0;                        // Bypasses the dictionary.
  virtual bool                  SetEntry(int object_no, int attribute_no, int value, bool masked) = 0;                   // Bypasses the dictionary.

	virtual const String         &GetEntry(int object_no, int attribute_no, bool use_dictionary, bool masked) const;       // Goes through the dictionary, if specified.
	virtual const String         &GetEntry(int object_no, const String &attribute_name, bool masked) const;                // Goes through the dictionary.
	virtual bool                  SetEntry(int object_no, const String &attribute_name, const String &value, bool masked); // Goes through the dictionary.

  virtual bool                  GetEntries(Vector(int) &entries, int attribute_no, bool masked, bool missing) const;
  virtual bool                  GetEntries(Vector(int) &entries1, Vector(int) &entries2, int attribute_no1, int attribute_no2, bool masked, bool missing) const;
  virtual bool                  GetEntries(Vector(float) &entries, int attribute_no, bool masked, bool missing, bool use_dictionary) const;                                               // Goes through the dictionary, if specified.
  virtual bool                  GetEntries(Vector(float) &entries1, Vector(float) &entries2, int attribute_no1, int attribute_no2, bool masked, bool missing, bool use_dictionary) const; // Goes through the dictionary, if specified.

	virtual bool                  GetGeneralizedDecisions(GeneralizedDecision::Handles &decisions, const Discerner &discerner, bool masked) const;
	//20/08/13 -N.
	Handle<Rules>	GetRuleset() const;

  //- Object (row) methods...........................................
  virtual bool                  InsertObject(int object_no, bool masked) = 0;
  virtual bool                  AppendObject(bool masked);
  virtual bool                  RemoveObject(int object_no, bool masked) = 0;

	virtual bool                  SwapObjects(int i, int j, bool masked);
	virtual bool                  SortObjects(int attribute_no, bool masked);

	virtual bool                  GetObjects(Vector(int) &objects, int attribute_no, int value, bool masked) const;

  //- Attribute (column) methods.....................................
  virtual bool                  InsertAttribute(int attribute_no, const Attribute *attribute, bool masked) = 0;
  virtual bool                  AppendAttribute(const Attribute *attribute, bool masked);
  virtual bool                  RemoveAttribute(int attribute_no, bool masked) = 0;

	virtual bool                  SwapAttributes(int i, int j, bool masked);

	//- Attribute status methods.......................................
	virtual DecisionTable::Status GetStatus(int attribute_no, bool masked) const = 0;
	virtual bool                  SetStatus(int attribute_no, DecisionTable::Status status, bool masked) = 0;

	virtual bool                  IsCondition(int attribute_no, bool masked) const;
	virtual bool                  IsDecision(int attribute_no, bool masked) const;

	virtual int                   GetDecisionAttribute(bool masked) const;

	//- Attribute range methods........................................
	virtual bool                  GetValueSet(Vector(int) &values, Vector(int) &cardinalities, int attribute_no, bool masked) const;

	//- Missing values querying........................................
	virtual bool                  IsMissing(int object_no, int attribute_no, bool masked) const;

	virtual bool                  HasMissingEntries(bool masked) const;
	virtual bool                  HasMissingEntries(int attribute_no, bool masked) const;

	//- Masking methods................................................
	virtual DecisionTable::Mask   GetAttributeMask(int attribute_no) const = 0;
	virtual bool                  SetAttributeMask(int attribute_no, DecisionTable::Mask mask, bool safe = true) = 0;

	virtual bool                  GetAttributeMasks(Vector(Mask) &masks) const;
	virtual bool                  SetAttributeMasks(const Vector(Mask) &masks, bool safe = true);

	virtual bool                  EnableAttribute(int attribute_no, bool enable, bool safe = true);
	virtual bool                  EnableAllAttributes(bool enable, bool safe = true);

	virtual bool                  IsAttributeEnabled(int attribute_no) const;
	virtual bool                  IsAttributeDisabled(int attribute_no) const;

	virtual DecisionTable::Mask   GetObjectMask(int object_no) const = 0;
	virtual bool                  SetObjectMask(int object_no, DecisionTable::Mask mask, bool safe = true) = 0;

	virtual bool                  GetObjectMasks(Vector(Mask) &masks) const;
	virtual bool                  SetObjectMasks(const Vector(Mask) &masks, bool safe = true);

	virtual bool                  EnableObject(int object_no, bool enable, bool safe = true);
	virtual bool                  EnableAllObjects(bool enable, bool safe = true);

	virtual bool                  IsObjectEnabled(int object_no) const;
	virtual bool                  IsObjectDisabled(int object_no) const;

	//- Index conversion methods.......................................
	virtual int                   GetMaskedObject(int unmasked_object_no) const;
	virtual int                   GetUnmaskedObject(int masked_object_no) const;

	virtual int                   GetMaskedAttribute(int unmasked_attribute_no) const;
	virtual int                   GetUnmaskedAttribute(int masked_attribute_no) const;

  //- Dictionary (meta-data) related methods.........................
  Dictionary                   *GetDictionary() const;                                                       // Use with care.
  bool                          SetDictionary(Dictionary *dictionary);                                       // Use with care.

	bool                          HasDictionary() const;

	const String                 &GetAttributeName(int attribute_no, bool masked) const;
	bool                          SetAttributeName(int attribute_no, const String &name, bool masked);

	const String                 &GetAttributeUnit(int attribute_no, bool masked) const;
	bool                          SetAttributeUnit(int attribute_no, const String &unit, bool masked);

	int                           GetAttributeScalingExponent(int attribute_no, bool masked) const;
	bool                          SetAttributeScalingExponent(int attribute_no, int exponent, bool masked);

	Attribute::Type               GetAttributeType(int attribute_no, bool masked) const;

	int                           GetAttributeIndex(const String &attribute_name, bool case_sensitive, bool masked) const;

	bool                          IsNumeric(int attribute_no, bool masked) const;
	bool                          IsSymbolic(int attribute_no, bool masked) const;

	bool                          IsInteger(int attribute_no, bool masked) const;
	bool                          IsFloat(int attribute_no, bool masked) const;
	bool                          IsString(int attribute_no, bool masked) const;

	int                           GetDictionaryEntry(int attribute_no, const String &text, bool masked) const;
	const String                  GetDictionaryEntry(int attribute_no, int value, bool masked) const;

	int                           SuggestDictionaryEntry(int attribute_no, const String &text, bool masked) const;
	bool                          SetDictionaryEntry(int attribute_no, int value, const String &text, bool masked);

	int                           GetNoDictionaryEntries(int attribute_no, bool masked) const;
	const String                 &GetEnumeratedDictionaryEntry(int attribute_no, int entry_no, bool masked) const;

  //- Operators......................................................
	DecisionTable                &operator=(const DecisionTable &in);

  bool                          operator==(const DecisionTable &in) const;
  bool                          operator!=(const DecisionTable &in) const;

};

#endif 
