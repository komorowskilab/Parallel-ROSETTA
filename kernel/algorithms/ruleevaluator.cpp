//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/ruleevaluator.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/rule.h>

#include <kernel/system/math.h>

//-------------------------------------------------------------------
// Methods for class RuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

RuleEvaluator::RuleEvaluator() {
}

RuleEvaluator::RuleEvaluator(const DecisionTable &table) {

	bool masked = true;

	// Get stuff for computing coverage.
	if (!table.GetValueSet(decisions_, cardinalities_, table.GetDecisionAttribute(masked), masked)) {
		decisions_.erase(decisions_.begin(), decisions_.end());
		cardinalities_.erase(cardinalities_.begin(), cardinalities_.end());
		Message::Error("Call to DecisionTable::GetValueSet failed.");
	}

	// Get |U|.
	table_cardinality_ = table.GetNoObjects(masked);

}

RuleEvaluator::~RuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: GetAccuracy
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
RuleEvaluator::GetAccuracy(const Rule &rule, int i) const {
	return rule.GetAccuracy(i);
}

//-------------------------------------------------------------------
// Method........: GetCoverage
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
RuleEvaluator::GetCoverage(const Rule &rule, int i) const {
	return rule.GetCoverage(i, decisions_, cardinalities_);
}

//-------------------------------------------------------------------
// Method........: GetContingencyTable
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...: Computes the elements of the contingency table for a
//                 given rule and a given RHS position.
//
// Comments......: Values for a rule R = alpha -> beta[i] are:
//
//                   a = support(alpha,     beta)
//                   b = support(alpha,     not beta)
//                   c = support(not alpha, beta)
//                   d = support(not alpha, not beta)
//
//                   B = support(beta)  = a + c
//                   A = support(alpha) = a + b
//                   K = a + b + c + d
//
//                             | beta  | not beta |
//                   ------------------------------------
//                       alpha | a     | b        | A
//                   not alpha | c     | d        | K - A
//									 ------------------------------------
//                             | B     | K - B    | K
//
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

bool
RuleEvaluator::GetContingencyTable(const Rule &rule, int position, int &a, int &b, int &c, int &d, int &B, int &A, int &K) const {

	// Compute table entries.
	K = table_cardinality_;
	a = rule.GetSupport(position);
	A = rule.GetSupport();
	b = A - a;
	B = static_cast(int, static_cast(float, a) / GetCoverage(rule, position));
	c = B - a;
	d = K - a - b - c;

	return true;

}

//-------------------------------------------------------------------
// Method........: EvaluateDominating
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
RuleEvaluator::EvaluateDominating(const Rule &rule) const {

	int support, position;

	rule.GetDominatingDecisionValue(support, position);

	// Make sure that there is at least one decision value.
	if (position == Undefined::Integer())
		return Undefined::Float();

	return Evaluate(rule, position);

}

//-------------------------------------------------------------------
// Method........: EvaluateBest
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
RuleEvaluator::EvaluateBest(const Rule &rule) const {

	int   i, no_decisions = rule.GetNoDecisionValues();
	float best_quality = Undefined::Float();

	for (i = 0; i < no_decisions; i++) {

		// Evaluate quality wrt current decision value.
		float quality = Evaluate(rule, i);

		// Best ever?
		if (quality != Undefined::Float()) {
			if ((best_quality == Undefined::Float()) || (quality > best_quality))
				best_quality = quality;
		}

	}

	return best_quality;

}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: A rule may have several decision values. For each
//                 decision value, a quality measure may be
//                 associated. Due to legacy issues wrt rule
//                 representation, a rule has to be "complete" in that
//                 all decision values have to be listed in a rule,
//                 hence a rule cannot be "split".
//
//                 Two alternatives are obvious:
//
//                 (1) Return the best quality across all decision
//                     values.
//                 (2) Return the quality for the dominating decision.
//
//                 Alternative (1) is the default, but can be
//                 overloaded. Some quality measures assume
//                 alternative (2), though. (See Bruha paper, page 116).
//
// Revisions.....:
//===================================================================

float
RuleEvaluator::Evaluate(const Rule &rule) const {
	return EvaluateBest(rule);
}

//-------------------------------------------------------------------
// Methods for class MichalskiRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

MichalskiRuleEvaluator::MichalskiRuleEvaluator() {
	w1_ = 0.5;
	w2_ = 0.5;
}

MichalskiRuleEvaluator::MichalskiRuleEvaluator(const DecisionTable &table, float w1, float w2) : RuleEvaluator(table) {
	w1_ = w1;
	w2_ = w2;
}

MichalskiRuleEvaluator::~MichalskiRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
MichalskiRuleEvaluator::Evaluate(const Rule &rule, int i) const {
	return (w1_ * GetAccuracy(rule, i)) + (w2_ * GetCoverage(rule, i));
}

//-------------------------------------------------------------------
// Methods for class TorgoRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

TorgoRuleEvaluator::TorgoRuleEvaluator() {
}

TorgoRuleEvaluator::TorgoRuleEvaluator(const DecisionTable &table) : RuleEvaluator(table) {
}

TorgoRuleEvaluator::~TorgoRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
TorgoRuleEvaluator::Evaluate(const Rule &rule, int i) const {

	float accuracy = GetAccuracy(rule, i);

	float w1 = 0.5f + 0.25f * accuracy;
	float w2 = 0.5f - 0.25f * accuracy;

	return (w1 * accuracy) + (w2 * GetCoverage(rule, i));

}

//-------------------------------------------------------------------
// Methods for class BrazdilRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

BrazdilRuleEvaluator::BrazdilRuleEvaluator() {
}

BrazdilRuleEvaluator::BrazdilRuleEvaluator(const DecisionTable &table) : RuleEvaluator(table) {
}

BrazdilRuleEvaluator::~BrazdilRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
BrazdilRuleEvaluator::Evaluate(const Rule &rule, int i) const {
	return GetAccuracy(rule, i) * exp(GetCoverage(rule, i) - 1);
}


//-------------------------------------------------------------------
// Methods for class PearsonRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

PearsonRuleEvaluator::PearsonRuleEvaluator() {
}

PearsonRuleEvaluator::PearsonRuleEvaluator(const DecisionTable &table) : RuleEvaluator(table) {
}

PearsonRuleEvaluator::~PearsonRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

float
PearsonRuleEvaluator::Evaluate(const Rule &rule, int i) const {

	int a, b, c, d, B, A, K;

	// Compute table entries.
	if (!GetContingencyTable(rule, i, a, b, c, d, B, A, K))
		return Undefined::Float();

	// Catch division by zero.
	if (B == 0 || B == K || A == 0 || A == K)
		return Undefined::Float();

	int ad = a * d;
	int bc = b * c;

	return static_cast(float, (ad - bc) * (ad - bc)) / (B * (K - B) * A * (K - A));

}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: See RuleEvaluator::Evaluate and Bruha paper
//                 (page 116).
// Revisions.....:
//===================================================================

float
PearsonRuleEvaluator::Evaluate(const Rule &rule) const {
	return EvaluateDominating(rule);
}

//-------------------------------------------------------------------
// Methods for class G2RuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

G2RuleEvaluator::G2RuleEvaluator() {
}

G2RuleEvaluator::G2RuleEvaluator(const DecisionTable &table) : RuleEvaluator(table) {
}

G2RuleEvaluator::~G2RuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

float
G2RuleEvaluator::Evaluate(const Rule &rule, int i) const {

	int a, b, c, d, B, A, K;

	// Compute table entries.
	if (!GetContingencyTable(rule, i, a, b, c, d, B, A, K))
		return Undefined::Float();

	// Verify measure applicability.
	if (K == 0 || a == 0 || A == 0 || B == 0 || b == 0 || B == K)
		return Undefined::Float();

	return 2 * (a * log(static_cast(float, (a * K)) / (A * B)) +
		          b * log(static_cast(float, (b * K)) / (A * (K - B))));

}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: See RuleEvaluator::Evaluate and Bruha paper
//                 (page 116).
// Revisions.....:
//===================================================================

float
G2RuleEvaluator::Evaluate(const Rule &rule) const {
	return EvaluateDominating(rule);
}

//-------------------------------------------------------------------
// Methods for class JRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Thomas ×gotnes/Aleksander šhrn
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

JRuleEvaluator::JRuleEvaluator() {
}

JRuleEvaluator::JRuleEvaluator(const DecisionTable &table) : G2RuleEvaluator(table) {
}

JRuleEvaluator::~JRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
JRuleEvaluator::Evaluate(const Rule &rule, int i) const {

	// Compute G2 quality.
	float quality = G2RuleEvaluator::Evaluate(rule, i);

	int K = table_cardinality_;

	if (quality == Undefined::Float() || K == 0)
		return Undefined::Float();

	return quality / (2 * K);

}

//-------------------------------------------------------------------
// Methods for class CohenRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

CohenRuleEvaluator::CohenRuleEvaluator() {
}

CohenRuleEvaluator::CohenRuleEvaluator(const DecisionTable &table) : RuleEvaluator(table) {
}

CohenRuleEvaluator::~CohenRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

float
CohenRuleEvaluator::Evaluate(const Rule &rule, int i) const {

	int a, b, c, d, B, A, K;

	// Compute table entries.
	if (!GetContingencyTable(rule, i, a, b, c, d, B, A, K))
		return Undefined::Float();

	int term = (A * B) - ((K - A) * (K - B));

	// Catch division by zero.
	if ((K * K) == term)
		return Undefined::Float();

	return static_cast(float, (K * a) + (K * d) - term) / ((K * K) - term);

}

//-------------------------------------------------------------------
// Methods for class ColemanRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....: Aš 981217: Inherit from CohenRuleEvaluator for
//                            C1 normalization purposes.
//===================================================================

ColemanRuleEvaluator::ColemanRuleEvaluator() {
}

ColemanRuleEvaluator::ColemanRuleEvaluator(const DecisionTable &table, Normalization normalization) : CohenRuleEvaluator(table) {
	normalization_ = normalization;
}

ColemanRuleEvaluator::~ColemanRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

float
ColemanRuleEvaluator::Evaluate(const Rule &rule, int i) const {

	// Evaluate non-normalized Coleman quality.
	float quality = EvaluateUnnormalized(rule, i);

	if (quality == Undefined::Float())
		return Undefined::Float();

	float c;

	// Normalize?
	switch (normalization_) {
		case NORMALIZATION_NONE: return quality;
		case NORMALIZATION_C1:   c = CohenRuleEvaluator::Evaluate(rule, i);
			                       if (c != Undefined::Float())
														   return quality * (2.0 + c) / 3.0;
													   return Undefined::Float();
		case NORMALIZATION_C2:   c = GetCoverage(rule, i);
			                       if (c != Undefined::Float())
														   return quality * (1.0 + c) / 3.0;
													   return Undefined::Float();
	}

	return Undefined::Float();

}

//-------------------------------------------------------------------
// Method........: EvaluateUnnormalized
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

float
ColemanRuleEvaluator::EvaluateUnnormalized(const Rule &rule, int i) const {

	int a, b, c, d, B, A, K;

	// Compute table entries.
	if (!GetContingencyTable(rule, i, a, b, c, d, B, A, K))
		return Undefined::Float();

	// Verify measure applicability.
	if (A == 0 || (K - B) == 0)
		return Undefined::Float();

	return static_cast(float, (K * a) - (A * B)) / (A * (K - B));

}

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Thomas ×gotnes
// Date..........: 981030
// Description...:
// Comments......:
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

String
ColemanRuleEvaluator::GetString(Normalization normalization) {

	switch (normalization) {
		case NORMALIZATION_NONE: return "None";
		case NORMALIZATION_C1:   return "C1";
		case NORMALIZATION_C2:   return "C2";
		default:                 return Undefined::String();
	}

}

//-------------------------------------------------------------------
// Methods for class KononenkoRuleEvaluator.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructors/destructor
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

KononenkoRuleEvaluator::KononenkoRuleEvaluator() {
}

KononenkoRuleEvaluator::KononenkoRuleEvaluator(const DecisionTable &table) : RuleEvaluator(table) {
}

KononenkoRuleEvaluator::~KononenkoRuleEvaluator() {
}

//-------------------------------------------------------------------
// Method........: Evaluate
// Author........: Thomas ×gotnes
// Date..........: 981029
// Description...:
// Comments......: log2(x) = ln(x) / ln(2)
//
//                 Bruha in his paper (page 118) mentions a necessary
//                 condition for the Kononenko quality formula to
//                 be valid, This is currently not tested for!
//
// Revisions.....: Aš 981114: Minor stuff.
//===================================================================

float
KononenkoRuleEvaluator::Evaluate(const Rule &rule, int i) const {

	int a, b, c, d, B, A, K;

	// Compute table entries.
	if (!GetContingencyTable(rule, i, a, b, c, d, B, A, K))
		return Undefined::Float();

	float accuracy = GetAccuracy(rule, i);

	// Verify measure applicability.
	if ((a + c) == 0 || K == 0 || accuracy == 0.0 || accuracy == Undefined::Float())
		return Undefined::Float();

#ifndef OBSOLETE
	double log2 = log(2);
#else
	double log2 = log((double)2);
#endif

	return -(log(static_cast(float, (a + c)) / K) / log2) +
		      (log(accuracy) / log2);

}
 
