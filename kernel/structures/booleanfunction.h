//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __BOOLEANFUNCTION_H__
#define __BOOLEANFUNCTION_H__

#include <copyright.h>

#include <kernel/structures/structure.h>

#include <kernel/basic/bits.h>
#include <kernel/basic/vector.h>
#include <kernel/basic/map.h>
#include <kernel/basic/set.h>
#include <kernel/basic/hashmap.h>
#include <kernel/basic/hashset.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class DecisionTable;
class Discerner;

//-------------------------------------------------------------------
// Class.........: BooleanFunction
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Base class for Boolean functions of a certain type,
//                 namely POS or SOP functions.
//
// Revisions.....: AÛ 990726: Changed the internal component to
//                            BFComponent, to simplify management of
//                            associated weights. Assign a unit weight
//                            as default.
//                 AÛ 981007: Changed the internal component
//                            representation from Vector(Bits) to
//                            Vector(const Bits *), and added an
//                            array of component ownership flags.
//                            The reason for this is that objects of
//                            type BooleanFunction are very often
//                            used as "views" into a collection of
//                            Bits objects, e.g., as a subset of entries
//                            in a DiscernibilityMatrix object.
//                            With the new representation, we often
//                            will then not need to physically copy
//                            the Bits objects (thus saving both space and
//                            time), but can "share" it. The ownership
//                            flag is set for a Bits object if the function
//                            is responsible for its deletion.
//                 AÛ 990323: Added a seemingly unecessary typedef to make
//                            code more compatible with compilers on other
//                            platforms (specifically, egcs 1.1.2 under
//                            SunOS 5.5).
//===================================================================

class BooleanFunction : public Structure {
public:

  //- Type definitions..............................................
	typedef Vector(Handle<BooleanFunction>) Handles;
	// -N 10/07/13 - To directly handle the Bits functions.
	typedef unsigned int Block;
protected:

  //- Type definitions..............................................
	typedef STLSet(int)                 		IntegerSet;
	typedef HashSet(int)           		IntegerHashSet; // HashSet
  	typedef HashMap(int, HashSet(int))  	IntegerHashSetHashMap;

	//- Internal helper structure......................................
	class BFComponent {
	public:

		//- Member variables.............................................
		Bits	*bits_;     // A sum or product.
		bool	is_owner_; // Responsible for deletion? Not persistent.
		float	weight_;   // Associated weight. Not persistent.
		//- Constructor..................................................
		BFComponent() {bits_ = NULL; is_owner_ = false; weight_ = 1.0;}

	};

protected:

  //- STL helper, "quasi-lexicographical" comparison of bit sets.....
	struct LCompare {
	private:
		const Vector(BFComponent) *v_;
    LCompare() {v_ = NULL;}
	public:
		bool operator()(int i, int j) const {
			return (-1 == (*v_)[i].bits_->Compare(*((*v_)[j].bits_)));
		}
    LCompare(Vector(BFComponent) *v) {v_ = v;}
	};

  //- STL helper, cardinality comparison for bit sets................
	struct CCompare {
	private:
		const Vector(BFComponent) *v_;
    CCompare() {v_ = NULL;}
	public:
		bool operator()(int i, int j) const {
			return ((*v_)[i].bits_->Count(true) < (*v_)[j].bits_->Count(true));
		}
    CCompare(Vector(BFComponent) *v) {v_ = v;}
	};

protected:

  //- Implementation structures......................................
	String             	name_;       // Name of function.
	Vector(BFComponent)	components_;//Function components (sums or products), with meta-data.

protected:

	//- Constructors...................................................
  BooleanFunction(const BooleanFunction &in);

	//- Parsing........................................................
	virtual bool                        ParseName(const String &function, String &name, String &body) const;
	virtual bool                        ParseBody(const String &body, char separator, Vector(String) &components) const;
	virtual bool                        ParseComponent(const String &component, char separator, Vector(int) &variables, const DecisionTable *table = NULL, bool masked = true, Map(String, int) *names = NULL) const;

  //- Initialization methods.........................................
	virtual bool                        Create(const String &function, char inter, char intra, const DecisionTable &table, bool masked);
	virtual bool                        Create(const String &function, char inter, char intra, Map(String, int) &names);

	//- Simplification.................................................
	virtual bool                        RemoveComponents(const Bits &remove);
	virtual bool                        RemoveDuplicates();
	virtual bool                        RemoveSupersets(bool weighted);
	virtual bool                        RemoveRSupersets(bool weighted);

	//- Formatting methods............................................
	virtual bool                        Format(String &formatted, char inter, char intra, const DecisionTable *table = NULL, bool masked = true) const;
	virtual String                      Format(char inter, char intra, const DecisionTable *table = NULL, bool masked = true) const;

	//- Persistence methods...........................................
	virtual bool                        Load(ifstream &stream, char inter, char intra, const DecisionTable *table = NULL, bool masked = true);
	virtual bool                        Save(ofstream &stream, char inter, char intra, const DecisionTable *table = NULL, bool masked = true) const;

public:

  //- Constructors and destructor....................................
	BooleanFunction();
  virtual ~BooleanFunction();

  //- Methods inherited from Identifier.............................
	DECLAREIDMETHODS()

	//- Methods inherited from Persistent.............................
  virtual bool                        Load(ifstream &stream);
  virtual bool                        Save(ofstream &stream) const;

  //- Methods inherited from Structure..............................
  virtual void                        Clear();

	//- Basic function information....................................
	virtual const String               &GetName() const;
	virtual bool                        SetName(const String &name);

	virtual int                         GetArity(bool minimize) const;

	virtual bool                        IsValid() const;

	//- Component methods.............................................
	virtual int                         GetNoComponents() const;

	virtual const Bits                 *GetComponent(int i) const;
	virtual bool                        SetComponent(int i, const Bits *component, bool copy, float weight = 1.0);

	virtual float                       GetWeight(int i) const;
	virtual bool                        SetWeight(int i, float weight);

	virtual bool                        InsertComponent(const Bits *component, int i, bool copy, float weight = 1.0);
	virtual bool                        AppendComponent(const Bits *component, bool copy, float weight = 1.0);
	virtual bool                        RemoveComponent(int i);
	virtual bool                        RemoveAllComponents();

	virtual bool                        Simplify(bool weighted);

	virtual bool                        AppendSimplified(const Bits &component, bool weighted, float weight = 1.0);


	// Comparators and initialization procedures for serialized bits structures (for SSE). -N
	// -N 10/07/13
	bool		IsSubtree(const Block* tptr, const Block* xptr, int blockCount) const;
	bool		IsEqual(const Block* tptr, const Block* xptr, int blockCount) const;
	int		GetSingleRelation(Block* tptr, Block* xptr, bool superset);	
	//- Representation related methods.................................
	virtual bool                        Reserve(int no_components);

  //- Initialization methods.........................................
	virtual bool                        Create(const String &function, const DecisionTable &table, bool masked) = 0;
	virtual bool                        Create(const String &function, Map(String, int) &names) = 0;

	//- Formatting methods............................................
	virtual bool                        Format(String &formatted, const DecisionTable *table = NULL, bool masked = true) const = 0;
	virtual String                      Format(const DecisionTable *table = NULL, bool masked = true) const = 0;

	//- Persistence methods...........................................
	virtual bool                        Load(const String &filename, const DecisionTable *table = NULL, bool masked = true);
	virtual bool                        Load(ifstream &stream, const DecisionTable *table = NULL, bool masked = true) = 0;

	virtual bool                        Save(const String &filename, const DecisionTable *table = NULL, bool masked = true) const;
	virtual bool                        Save(ofstream &stream, const DecisionTable *table = NULL, bool masked = true) const = 0;

};

#endif
 
