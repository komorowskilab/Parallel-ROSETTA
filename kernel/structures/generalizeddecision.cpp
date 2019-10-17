//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/structures/generalizeddecision.h>

#include <kernel/utilities/iokit.h>
#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Methods for class GeneralizedDecision.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

GeneralizedDecision::GeneralizedDecision(const GeneralizedDecision &in) : Structure(in) {
	decisions_ = in.decisions_;
	supports_  = in.supports_;
}

GeneralizedDecision::GeneralizedDecision() {
}

GeneralizedDecision::~GeneralizedDecision() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(GeneralizedDecision, GENERALIZEDDECISION, Structure)

//-------------------------------------------------------------------
// Methods inherited from Persistent.
//===================================================================

//-------------------------------------------------------------------
// Method........: Load
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::Load(ifstream &stream) {

	Clear();

	int i, no_decisions, decision, support;

	if (!IOKit::Load(stream, no_decisions))
		return false;

	if (!Reserve(no_decisions))
		return false;

	for (i = 0; i < no_decisions; i++) {
		if (!IOKit::Load(stream, decision) || !IOKit::Load(stream, support))
			return false;
		if (!AppendDecision(decision, support))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: Save
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::Save(ofstream &stream) const {

	int i, no_decisions = GetNoDecisions();

	if (!IOKit::Save(stream, no_decisions))
		return false;

	for (i = 0; i < no_decisions; i++) {
		if (!IOKit::Save(stream, GetDecision(i)) || !IOKit::Save(stream, GetSupport(i)))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Methods inherited from Structure.
//===================================================================

//-------------------------------------------------------------------
// Method........: Clear
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

void
GeneralizedDecision::Clear() {

	if (!decisions_.empty())
		decisions_.erase(decisions_.begin(), decisions_.end());

	if (!supports_.empty())
		supports_.erase(supports_.begin(), supports_.end());

}

//-------------------------------------------------------------------
// Method........: Duplicate
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
GeneralizedDecision::Duplicate() const {
	return new GeneralizedDecision(*this);
}

//-------------------------------------------------------------------
// New virtual methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetNoDecisions
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
GeneralizedDecision::GetNoDecisions() const {
	return decisions_.size();
}

//-------------------------------------------------------------------
// Method........: GetDecision
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
GeneralizedDecision::GetDecision(int position_no) const {
	return decisions_[position_no];
}

//-------------------------------------------------------------------
// Method........: GetSupport
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

int
GeneralizedDecision::GetSupport(int position_no) const {
	return supports_[position_no];
}

//-------------------------------------------------------------------
// Method........: InsertDecision
// Author........:
// Date..........:
// Description...:
// Comments......: To do: Check for membership to avoid duplicates.
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::InsertDecision(int position_no, int decision, int support) {

	decisions_.insert(decisions_.begin() + position_no, decision);
	supports_.insert(supports_.begin() + position_no, support);

	return true;

}

//-------------------------------------------------------------------
// Method........: AppendDecision
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::AppendDecision(int decision, int support) {
	return InsertDecision(GetNoDecisions(), decision, support);
}

//-------------------------------------------------------------------
// Method........: RemoveDecision
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::RemoveDecision(int position_no) {

	decisions_.erase(decisions_.begin() + position_no);
	supports_.erase(supports_.begin() + position_no);

	return true;

}

//-------------------------------------------------------------------
// Method........: GetDominatingDecision
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns the dominating decision value. Also
//                 returns (in-place) the index/position of the
//                 dominating decision.
//
// Comments......: Ties are resolved arbitrarily,
// Revisions.....:
//===================================================================

int
GeneralizedDecision::GetDominatingDecision(int &position_no) const {

	int i, no_decisions = decisions_.size();

	if (no_decisions == 0) {
		position_no = Undefined::Integer();
		return Undefined::Integer();
	}

	int position_max = 0;
	int support_max  = supports_[0];

	for (i = 1; i < no_decisions; i++) {
		if (supports_[i] > support_max) {
			support_max  = supports_[i];
			position_max = i;
		}
	}

	position_no = position_max;

	return decisions_[position_no];

}

//-------------------------------------------------------------------
// Method........: GetProbability
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
GeneralizedDecision::GetProbability(int position_no) const {

	int i, sum = 0;

	for (i = decisions_.size() - 1; i >= 0; i--)
		sum += supports_[i];

	return static_cast(float, supports_[position_no]) / sum;

}

//-------------------------------------------------------------------
// Method........: Reserve
// Author........:
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::Reserve(int size) {

	decisions_.reserve(size);
	supports_.reserve(size);

	return true;

}

//-------------------------------------------------------------------
// Method........: HasEqualDecisions
// Author........:
// Date..........:
// Description...: Checks if this generalized decision is "approximately
//                 equal" to the other one.
//
//                 Similar to HasEqualDecisions(), but compares
//                 "filtered" sets of decision values, where the
//                 filtering is done on the basis of the distributions
//                 of decision values and the supplied precision
//                 threshold.
//
//                 This effectively implements a simple form of
//                 boundary region thinning (BRT).
//
//                 The precision level should normally lie in the
//                 range [0, 0.5]. If the precision level is 0, then
//                 an "ordinary" comparison takes place, ignoring support
//                 counts.
//
// Comments......: Assumes same ordering of decisions.
//
//                 To increase efficiency, consider caching the
//                 filtered set of decision values.
//
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::HasEqualDecisions(const GeneralizedDecision &in, float precision) const {

  if (&in == this)
    return true;

	// Is "filtering" clearly not needed?
	if (precision <= 0.0 || precision >= 1.0)
		return (decisions_ == in.decisions_);

	// Keep static variables to reduce allocation/deallocation overhead. Overkill? - sus - removed static for multithreading
	Vector(int) filtered0;
	Vector(int) filtered1;

	// Declare iteration structures.
	      Vector(int) *filtered[2]   = {&filtered0, &filtered1};
	const Vector(int) *unfiltered[2] = {&decisions_, &(in.decisions_)};
	const Vector(int) *supports[2]   = {&supports_, &(in.supports_)};
	const Vector(int) *used[2]       = {NULL, NULL};

	int i;

	// Find out which decision sets to use.
	for (i = 0; i < 2; i++) {

		// Optimization: No need to filter singletons.
		if (unfiltered[i]->size() == 1) {
			used[i] = unfiltered[i];
			continue;
		}

		// If needed, clear previous contents of static stuff.
		if (!(filtered[i]->empty()))
			filtered[i]->erase(filtered[i]->begin(), filtered[i]->end());

		filtered[i]->reserve(unfiltered[i]->size());

		// Compute normalization factor.
		int sum = MathKit::Sum(*(supports[i]));

		Vector(int)::const_iterator itd = unfiltered[i]->begin();
		Vector(int)::const_iterator its = supports[i]->begin();

		// Do filtering.
		while (its != (supports[i]->end())) {
			if ((static_cast(float, *its) / sum) >= precision)
				filtered[i]->push_back(*itd);
			itd++;
			its++;
		}

		// Did any decision values survive the filtering? If not, revert to the unfiltered set.
		if (!(filtered[i]->empty()))
			used[i] = filtered[i];
		else
			used[i] = unfiltered[i];

	}

	// Compare decision sets.
	return (*(used[0]) == *(used[1]));

}

//-------------------------------------------------------------------
// Method........: HasEqualSupports
// Author........:
// Date..........:
// Description...: Compares support counts.
// Comments......: Assumes same ordering of decisions.
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::HasEqualSupports(const GeneralizedDecision &in) const {

  if (&in == this)
    return true;

	return (supports_ == in.supports_);

}

//-------------------------------------------------------------------
// Operators.
//===================================================================

//-------------------------------------------------------------------
// Operator......: =
// Author........:
// Date..........:
// Description...: Assignment operator.
// Comments......:
// Revisions.....:
//===================================================================

GeneralizedDecision &
GeneralizedDecision::operator=(const GeneralizedDecision &in) {

	decisions_ = in.decisions_;
	supports_  = in.supports_;

  return *this;

}

//-------------------------------------------------------------------
// Operator......: ==
// Author........:
// Date..........:
// Description...: Equality operator. Compares both decisions and
//                 support counts. See also HasEqualDecisions and
//                 HasEqualSupports methods.
//
// Comments......: Assumes same ordering of decisions.
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::operator==(const GeneralizedDecision &in) const {

  if (&in == this)
    return true;

	if (decisions_ != in.decisions_)
		return false;

	if (supports_ != in.supports_)
		return false;

  return true;

}

//-------------------------------------------------------------------
// Operator......: !=
// Author........:
// Date..........:
// Description...: Inequality operator.
// Comments......:
// Revisions.....:
//===================================================================

bool
GeneralizedDecision::operator!=(const GeneralizedDecision &in) const {
  return !(*this == in);
}

 
