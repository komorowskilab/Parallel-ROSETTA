//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/utilities/creator.h>
#include <kernel/utilities/iokit.h>

#include <kernel/structures/annotation.h>
#include <kernel/structures/approximation.h>
#include <kernel/structures/attribute.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/classification.h>
#include <kernel/structures/decisiontable.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/equivalenceclass.h>
#include <kernel/structures/equivalenceclasses.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/generalizeddecision.h>

//Ewa Makosa 03.06.2005
#include <kernel/structures/generalrule.h>
#include <kernel/structures/generalrules.h>

#include <kernel/structures/graph.h>
#include <kernel/structures/informationvector.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/project.h>
#include <kernel/structures/reduct.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/rule.h>
#include <kernel/structures/rulebasedclassification.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/structure.h>
#include <kernel/structures/structures.h>

#include <kernel/system/fstream.h>

#include <common/objectmanager.h>
// -N 08/05/13
#include <string>

using namespace std;

//-------------------------------------------------------------------
// Methods for class Creator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Create
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates and returns a structure of the specified
//                 type.
// Comments......: If this fails, check the order of the installation.
//
// Revisions.....: Aš 990323: Added a seemingly unnecessary typedef to
//                            increase code compatibility across
//                            compilers/platforms (specifically, egcs
//                            1.1.2 under SunOS 5.5).
//===================================================================

::Structure *
Creator::Create(Id id, ::Structure *parent, bool append) {
	String message;

	// Make a convenient typedef. Increases code compatibility across compilers/platforms.
	typedef ::Structure S;

	// Check the list of installed structures.
	Handle<S> structure;

	structure = ObjectManager::GetIdentifiedStructure(id);

	// Did no structures match?
	if (structure == NULL) {
	  message += "The object manager has been scanned for prototypes.\n";
		message += "Unable to create structure of type " + IdHolder::GetClassname(id) + ".  ";
		message += "Check that such a structure has been properly installed.";
		Message::FatalError(message);
	}

	// Duplicate the match.
	Handle<S> duplicate = structure->Duplicate();

	// Set parent/child relationship.
	if (append && (parent != NULL))
		parent->AppendChild(duplicate.GetPointer());

	return duplicate.Release();
}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates (from file) and returns a structure of the
//                 specified type.
// Comments......:
// Revisions.....:
// Revisions.....: Aš 990323: Added a seemingly unnecessary typedef to
//                            increase code compatibility across
//                            compilers/platforms (specifically, egcs
//                            1.1.2 under SunOS 5.5).
//===================================================================

::Structure *
Creator::Load(const String &filename, Id id, ::Structure *parent, bool append) {

	// Check that the structure type is installed.
	if (ObjectManager::Count(id) == 0)
//		cout << "Count 0"<<endl;
		return NULL;

	// Make a convenient typedef. Increases code compatibility across compilers/platforms.
	typedef ::Structure S;

	// Create an empty structure.
	Handle<S> structure = Create(id, parent, append);

	if (structure == NULL)
//		cout << "Structure null"<< endl;
		return NULL;

//	cout << "load: "<<filename<<"|"<<endl;
	// Load from file.
	if (!structure->Load(filename))
//		cout << "load failed"<<endl;
		return NULL;

	if (structure == NULL)
		cout << "Structure is NULL"<< endl;

	return structure.Release();
}

//-------------------------------------------------------------------
// Method........: Load
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates (from file) and returns a structure.
//                 Attempts auto-detection of type.
//
// Comments......: Too much code overlap with ParentStructure::Load...
//                 Unify later.
//
// Revisions.....: Aš 980605: Added hack for backwards compatibility.
//===================================================================

::Structure *
Creator::Load(const String &filename, ::Structure *parent, bool append) {

	ifstream stream;

	if (!IOKit::Open(stream, filename)) {
		Message::Error("Unable to open " + filename + ".");
		return NULL;
	}

	String header;

	// Take a peek inside file to determine the type of file.
	if (!IOKit::Load(stream, header)) {
		Message::Debug("Error peeking inside file to determine file type. Probably an alien file.");
		return NULL;
	}

//	cout << "Header: "<<header<<"|"<<endl;
	header.ToUppercase();
//	cout << "Header: "<<header<<"|"<<endl;
//	cout << "Match: "<<(header == "ROSETTA") << endl;
	string header_match = header.GetBuffer();
//	cout << "Match: "<<(header_match == "ROSETTA") << endl;
	// Check file format.
	if (header_match != "ROSETTA") {
//		cout << "Alien file" << endl;
		Message::Debug("Alien file format detected.");
		return NULL;
	}

	String type;

	// Take another peek to determine the type of object.
	if (!IOKit::Load(stream, type)) {
//		cout << "Stream failed" << endl;
		Message::Debug("Error peeking inside file to determine object type. Probably an alien file.");
		return NULL;
	}

	IOKit::Close(stream);

	// Deduce identifier.
	Id id = IdHolder::GetId(type);

	// Check that the structure type is installed.
	// If not, take appropriate measures.
	if (ObjectManager::Count(id) == 0) {

		// Ugly hack: To allow component interchangeability.
		if (type.Contains("DecisionTable"))
			type = "DecisionTable";

		id = IdHolder::GetId(type);

	}

	// Hack: For backwards compatibility.
	if (id == CONFUSIONMATRIX)
		id = BATCHCLASSIFICATION;

	if (id == Undefined::Id()) {
		Message::Error("Unable to load an object of type " + type + ". Is it installed?");
		return NULL;
	}

	return Load(filename, id, parent, append);

}

//-------------------------------------------------------------------
// Method........: Instantiation methods
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates and returns a structure of the given type.
//                 Hides away the implied casting.
// Comments......:
// Revisions.....:
//===================================================================

::Annotation *
Creator::Annotation(::Structure *parent, bool append) {
	return dynamic_cast(::Annotation *, Create(ANNOTATION, parent, append));
}

::Approximation *
Creator::Approximation(::Structure *parent, bool append) {
	return dynamic_cast(::Approximation *, Create(APPROXIMATION, parent, append));
}

::Attribute *
Creator::Attribute(::Structure *parent, bool append) {
	return dynamic_cast(::Attribute *, Create(ATTRIBUTE, parent, append));
}

::BatchClassification *
Creator::BatchClassification(::Structure *parent, bool append) {
	return dynamic_cast(::BatchClassification *, Create(BATCHCLASSIFICATION, parent, append));
}

::Classification *
Creator::Classification(::Structure *parent, bool append) {
	return dynamic_cast(::Classification *, Create(CLASSIFICATION, parent, append));
}

::DecisionTable *
Creator::DecisionTable(::Structure *parent, bool append) {
	return dynamic_cast(::DecisionTable *, Create(DECISIONTABLE, parent, append));
}

::DecisionTables *
Creator::DecisionTables(::Structure *parent, bool append) {
	return dynamic_cast(::DecisionTables *, Create(DECISIONTABLES, parent, append));
}

::Dictionary *
Creator::Dictionary(::Structure *parent, bool append) {
	return dynamic_cast(::Dictionary *, Create(DICTIONARY, parent, append));
}

::EquivalenceClass *
Creator::EquivalenceClass(::Structure *parent, bool append) {
	return dynamic_cast(::EquivalenceClass *, Create(EQUIVALENCECLASS, parent, append));
}

::EquivalenceClasses *
Creator::EquivalenceClasses(::Structure *parent, bool append) {
	return dynamic_cast(::EquivalenceClasses *, Create(EQUIVALENCECLASSES, parent, append));
}

::FloatAttribute *
Creator::FloatAttribute(::Structure *parent, bool append) {
	return dynamic_cast(::FloatAttribute *, Create(FLOATATTRIBUTE, parent, append));
}

::GeneralizedDecision *
Creator::GeneralizedDecision(::Structure *parent, bool append) {
	return dynamic_cast(::GeneralizedDecision *, Create(GENERALIZEDDECISION, parent, append));
}

//Ewa Makosa 03.06.2005
::GeneralRule *
Creator::GeneralRule(::Structure *parent, bool append) {
	return dynamic_cast(::GeneralRule *, Create(GENERALRULE, parent, append));
}

::GeneralRules *
Creator::GeneralRules(::Structure *parent, bool append) {
	return dynamic_cast(::GeneralRules *, Create(GENERALRULES, parent, append));
}

::Graph *
Creator::Graph(::Structure *parent, bool append) {
	return dynamic_cast(::Graph *, Create(GRAPH, parent, append));
}

::InformationVector *
Creator::InformationVector(::Structure *parent, bool append) {
	return dynamic_cast(::InformationVector *, Create(INFORMATIONVECTOR, parent, append));
}

::IntegerAttribute *
Creator::IntegerAttribute(::Structure *parent, bool append) {
	return dynamic_cast(::IntegerAttribute *, Create(INTEGERATTRIBUTE, parent, append));
}

::Project *
Creator::Project(::Structure *parent, bool append) {
	return dynamic_cast(::Project *, Create(PROJECT, parent, append));
}

::Reduct *
Creator::Reduct(::Structure *parent, bool append) {
	return dynamic_cast(::Reduct *, Create(REDUCT, parent, append));
}

::Reducts *
Creator::Reducts(::Structure *parent, bool append) {
	return dynamic_cast(::Reducts *, Create(REDUCTS, parent, append));
}

::Rule *
Creator::Rule(::Structure *parent, bool append) {
	return dynamic_cast(::Rule *, Create(RULE, parent, append));
}

::RuleBasedClassification *
Creator::RuleBasedClassification(::Structure *parent, bool append) {
	return dynamic_cast(::RuleBasedClassification *, Create(RULEBASEDCLASSIFICATION, parent, append));
}

::Rules *
Creator::Rules(::Structure *parent, bool append) {
	return dynamic_cast(::Rules *, Create(RULES, parent, append));
}

::StringAttribute *
Creator::StringAttribute(::Structure *parent, bool append) {
	return dynamic_cast(::StringAttribute *, Create(STRINGATTRIBUTE, parent, append));
}

::Structures *
Creator::Structures(::Structure *parent, bool append) {
	return dynamic_cast(::Structures *, Create(STRUCTURES, parent, append));
}

//-------------------------------------------------------------------
// Method........: FloatAttribute
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates and returns a float attribute with a
//                 specified scaling exponent.
// Comments......:
// Revisions.....: Aš 990323: Added a seemingly unnecessary typedef to
//                            increase code compatibility across
//                            compilers/platforms (specifically, egcs
//                            1.1.2 under SunOS 5.5).
//===================================================================

::FloatAttribute *
Creator::FloatAttribute(int exponent, ::Structure *parent, bool append) {

	// Make a convenient typedef. Increases code compatibility across compilers/platforms.
	typedef ::FloatAttribute F;

	// Create a float attribute.
	Handle<F> floatattribute = FloatAttribute(parent, append);

	if (floatattribute == NULL)
		return NULL;

	// Set the scaling exponent.
	if (!floatattribute->SetScalingExponent(exponent)) {
		Message::Error("Failed to set scaling exponent.");
		return NULL;
	}

	return floatattribute.Release();

}

//-------------------------------------------------------------------
// Method........: Reduct
// Author........: Aleksander šhrn
// Date..........:
// Description...: Creates and returns a reduct containing the
//                 attributes specified by the input string.
// Comments......:
// Revisions.....: Aš 990323: Added a seemingly unnecessary typedef to
//                            increase code compatibility across
//                            compilers/platforms (specifically, egcs
//                            1.1.2 under SunOS 5.5).
//===================================================================

::Reduct *
Creator::Reduct(const String &description, const ::DecisionTable *table, bool masked) {

	// Make a convenient typedef. Increases code compatibility across compilers/platforms.
	typedef ::Reduct R;

	// Create an empty reduct.
	Handle<R> reduct = Reduct();

	if (reduct == NULL)
		return NULL;

	// Define a mutable string.
	String tmp = description;

	// Strip away any surrounding curly braces and whitespace.
	tmp.Trim(" \t{");
	tmp.Trim(" \t}");

	// Define the delimiter between attributes.
	const char *delimiter = ",";

	bool first = true;
	bool done  = false;
	bool error = false;

	// Extract all specified attributes and add them to the reduct.
	while (!done) {

		String attribute;

		done = !tmp.GetToken(attribute, delimiter);

		// It could be that the delimiter was not found, i.e. there are less than two attributes.
		if (done) {
			if (first)
				attribute = tmp;
			else
				break;
		}

		// Let the tokenization finish so that subsequent calls to GetToken won't be erroneous.
		if (error)
			continue;

		first = false;

		// Strip away any surrounding (i.e. leading/trailing) whitespace.
		attribute.Trim(" \t");

		// Are we dealing with an empty attribute set?
		if (attribute.IsEmpty())
			break;

		// Determine the index of the specified attribute. Initialize the index as undefined.
		int index = Undefined::Integer();

		bool case_sensitive = false;

		// Try looking up the string as an attribute name, if possible.
		if (table != NULL)
			index = table->GetAttributeIndex(attribute, case_sensitive, masked);

		// If lookup failed, try treating the string as an index itself if possible.
		if (index == Undefined::Integer()) {
			if (attribute.IsInteger()) {
				index = attribute.GetInteger();
				if (index == Undefined::Integer()) {
					Message::Error("Invalid (undefined) attribute index given.");
					error = true;
					continue;
				}
				else if (index < 0) {
					Message::Error("Invalid (negative) attribute index given.");
					error = true;
					continue;
				}
				else if (table != NULL) {
					if (index >= table->GetNoAttributes(masked)) {
						Message::Error("Invalid (too large) attribute index given.");
						error = true;
						continue;
					}
				}
			}
		}

		// If all strategies failed, abort.
		if (index == Undefined::Integer()) {
			Message::Error("Failed to determine index of attribute " + attribute + ".");
			error = true;
			continue;
		}

		// Add the index to the reduct.
		if (!reduct->InsertAttribute(index)) {
			Message::Error("Failed to add index to reduct.");
			error = true;
			continue;
		}

	}

	// An error occurred in the tokenization process.
	if (error)
		return NULL;

	// Return the reduct.
	return reduct.Release();

}

 
