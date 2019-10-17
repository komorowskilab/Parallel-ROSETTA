//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/xmlgeneralruleexporter.h>

#include <kernel/structures/generalrules.h>


#include <kernel/utilities/xmlkit.h>
#include <kernel/utilities/systemkit.h>

#include <kernel/system/fstream.h>
#include <kernel/system/iostream.h>

//-------------------------------------------------------------------
// Methods for class XMLGeneralRuleExporter.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

XMLGeneralRuleExporter::XMLGeneralRuleExporter() {
}

XMLGeneralRuleExporter::~XMLGeneralRuleExporter() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(XMLGeneralRuleExporter, XMLGENERALRULEEXPORTER, RuleExporter)

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: EwaMakosa
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
XMLGeneralRuleExporter::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(GENERALRULES);
}


//-------------------------------------------------------------------
// Methods inherited from RuleExporter.
//===================================================================

//-------------------------------------------------------------------
// Method........: ExportRule
// Author........: Aleksander šhrn
// Date..........:
// Description...: Overloaded to deal with this particular rule syntax.
// Comments......:
// Revisions.....:
//===================================================================

bool
XMLGeneralRuleExporter::ExportRule(ofstream &stream, int /*rule_no*/, const GeneralRule &rule) const {

	int i;

	// Define indentation.
	String indent(' ', 2);
	String start(indent);

	// Get decision table.
	Handle<DecisionTable> table = const_cast(DecisionTable *, GetDecisionTable());

	bool masked = true;

	// Get rule dimensions.
	int no_conditions = rule.GetNoConditionDescriptors();
	int no_decisions  = rule.GetNoDecisionValues();

	// Output opening tag.
	stream << start << "<rule>" << endl;

	// Output <if> tag.
	stream << start << indent << "<if support=\"" << rule.GetSupport() << "\"";

	if (!table.IsNULL())
		stream << " coverage=\"" << rule.GetCoverage(table->GetNoObjects(masked)) << "\"";

	stream << ">" << endl;
	stream << start << indent << indent << "<and>" << endl;

	for (i = 0; i < no_conditions; i++) {

		// Get (attribute, value) pair.
		int attribute_no = rule.GetConditionAttribute(i);
		
		GeneralRule::ValueSet::const_iterator value_it = rule.GetConditionValueIterator(i);
		
		int no_values = rule.GetNoConditionValues(i);
			
		int v;
		stream << start << indent << indent << indent << "<or>" << endl;
		for (v = 0; v < no_values; v++){
			
			int value = *value_it;

			// Lookup their names.
			String caname = (table == NULL) ? "a" + String::Format(attribute_no) : XMLKit::Escape(table->GetAttributeName(attribute_no, masked));
			String cvname = (table == NULL) ? String::Format(value) : XMLKit::Escape(table->GetDictionaryEntry(attribute_no, value, masked));
			stream << start << indent << indent << indent << indent <<"<descriptor attribute=\"" << caname << "\" value=\"" << cvname << "\"/>" << endl;
	
			value_it++;

				
		}
		stream << start << indent << indent << indent << "</or>" << endl;

	}

	stream << start << indent << indent << "</and>" << endl;
	stream << start << indent << "</if>" << endl;

	// Output <then> tag.
	if (no_decisions > 0) {

		// Get decision attribute.
		int decision_attribute = rule.GetDecisionAttribute();

		String daname;

		// Get name of decision attribute.
		if (decision_attribute == Undefined::Integer())
			daname = Undefined::String();
		else if (table == NULL)
			daname = String::Format(decision_attribute);
		else
			daname = XMLKit::Escape(table->GetAttributeName(decision_attribute, masked));

		stream << start << indent << "<then>" << endl;
		stream << start << indent << indent << "<or>" << endl;

		for (i = 0; i < no_decisions; i++) {

			// Get decision value.
			int decision = rule.GetDecisionValue(i);

			// Get name of decision value.
			String dvname = (table == NULL) ? String::Format(decision) : XMLKit::Escape(table->GetDictionaryEntry(decision_attribute, decision, masked));
	
			// Get quantitative information.
			int    support   = rule.GetSupport(i);
			float  accuracy  = rule.GetAccuracy(i);
			float  coverage  = rule.GetCoverage(i, decisions_, cardinalities_);
			double stability = rule.GetStability(i);

			stream << start << indent << indent << indent << "<decision support=\"" << support << "\" accuracy=\"" << accuracy << "\" coverage=\"" << coverage << "\"";
			if (stability != 1.0) {
				stream << " stability=\"" << stability << "\"";
			}
			stream << ">" << endl;
			stream << start << indent << indent << indent << indent << "<descriptor attribute=\"" << daname << "\" value=\"" << dvname << "\"/>" << endl;
			stream << start << indent << indent << indent << "</decision>" << endl;

		}

		stream << start << indent << indent << "</or>" << endl;
		stream << start << indent << "</then>" << endl;

	}

	// Output closing tag.
	stream << start << "</rule>" << endl;

	return true;

}

XMLGeneralRuleExporter *
XMLGeneralRuleExporter::Clone() {
   return new XMLGeneralRuleExporter;
} 
