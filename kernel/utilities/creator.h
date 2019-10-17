//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __CREATOR_H__
#define __CREATOR_H__

#include <copyright.h>

#include <kernel/basic/types.h>

#include <kernel/system/stdio.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Annotation;
class Approximation;
class Attribute;
class BatchClassification;
class Classification;
class DecisionTable;
class DecisionTables;
class Dictionary;
class EquivalenceClass;
class EquivalenceClasses;
class FloatAttribute;
class GeneralizedDecision;

//Ewa Makosa 03.06.2005
class GeneralRule;
class GeneralRules;

class Graph;
class InformationVector;
class IntegerAttribute;
class Project;
class Reduct;
class Reducts;
class Rule;
class RuleBasedClassification;
class Rules;
class StringAttribute;
class Structure;
class Structures;

//-------------------------------------------------------------------
// Class.........: Creator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Kit for dynamic creation of structural objects.
// Comments......: Some specific creation methods can't have default
//                 arguments since this would cause signature
//                 confusion.
// Revisions.....:
//===================================================================

class Creator {
public:

	//- General creation...............................................
	static ::Structure               *Create(Id id, ::Structure *parent = NULL, bool append = false);

	static ::Structure               *Load(const String &filename, ::Structure *parent = NULL, bool append = false);
	static ::Structure               *Load(const String &filename, Id id, ::Structure *parent = NULL, bool append = false);

	//- Specific creation..............................................
  static ::Annotation              *Annotation(::Structure *parent = NULL, bool append = false);
  static ::Approximation           *Approximation(::Structure *parent = NULL, bool append = false);
  static ::Attribute               *Attribute(::Structure *parent = NULL, bool append = false);
  static ::BatchClassification     *BatchClassification(::Structure *parent = NULL, bool append = false);
  static ::Classification          *Classification(::Structure *parent = NULL, bool append = false);
  static ::DecisionTable           *DecisionTable(::Structure *parent = NULL, bool append = false);
  static ::DecisionTables          *DecisionTables(::Structure *parent = NULL, bool append = false);
  static ::Dictionary              *Dictionary(::Structure *parent = NULL, bool append = false);
  static ::EquivalenceClass        *EquivalenceClass(::Structure *parent = NULL, bool append = false);
  static ::EquivalenceClasses      *EquivalenceClasses(::Structure *parent = NULL, bool append = false);
  static ::FloatAttribute          *FloatAttribute(::Structure *parent, bool append);
  static ::FloatAttribute          *FloatAttribute(int exponent, ::Structure *parent, bool append);
  static ::GeneralizedDecision     *GeneralizedDecision(::Structure *parent = NULL, bool append = false);
 
  //Ewa Makosa 03.06.2005
  static ::GeneralRule		   *GeneralRule(::Structure *parent = NULL, bool append = false);
  static ::GeneralRules		   *GeneralRules(::Structure *parent = NULL, bool append = false); 
 
  static ::Graph                   *Graph(::Structure *parent = NULL, bool append = false);
  static ::InformationVector       *InformationVector(::Structure *parent = NULL, bool append = false);
  static ::IntegerAttribute        *IntegerAttribute(::Structure *parent = NULL, bool append = false);
  static ::Project                 *Project(::Structure *parent = NULL, bool append = false);
  static ::Reduct                  *Reduct(::Structure *parent = NULL, bool append = false);
  static ::Reduct                  *Reduct(const String &description, const ::DecisionTable *table = NULL, bool masked = true);
  static ::Reducts                 *Reducts(::Structure *parent = NULL, bool append = false);
  static ::Rule                    *Rule(::Structure *parent = NULL, bool append = false);
  static ::RuleBasedClassification *RuleBasedClassification(::Structure *parent = NULL, bool append = false);
  static ::Rules                   *Rules(::Structure *parent = NULL, bool append = false);
  static ::StringAttribute         *StringAttribute(::Structure *parent = NULL, bool append = false);
  static ::Structures              *Structures(::Structure *parent = NULL, bool append = false);

};

#endif 
