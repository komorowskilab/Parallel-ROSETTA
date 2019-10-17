//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 960307
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/algorithms/reducer.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/discernibilitymatrix.h>

#include <kernel/utilities/discerner.h>
#include <kernel/utilities/creator.h>

#include <kernel/basic/message.h>

// -N 19/08/13
#include "omp.h"
#include <kernel/algorithms/executor.h>
#include <kernel/utilities/threadcontrol.h>

#include <cmath>
//-------------------------------------------------------------------
// Methods for class Reducer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

Reducer::Reducer() {
  discernibility_  = Reduct::DISCERNIBILITY_FULL;
	modulo_decision_ = true;
	idg_             = false;
	filename_idg_    = Undefined::String();
	precompute_      = false;
	brt_             = false;
	precision_brt_   = 0.1f;
}

Reducer::~Reducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(Reducer, REDUCER, Algorithm)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
Reducer::GetParameters() const {

	String parameters;

	// Discernibility.
	parameters += Keyword::Discernibility();
	parameters += Keyword::Assignment();
	parameters += Reduct::GetString(GetDiscernibility());

	parameters += Keyword::Separator();

	// Modulo decision?
	parameters += Keyword::Modulo() + Keyword::Dot() + Keyword::Decision();
	parameters += Keyword::Assignment();
	parameters += String::Format(ModuloDecision());

	// Use BRT?
	if (ModuloDecision()) {
		parameters += Keyword::Separator();
		parameters += Keyword::BRT();
		parameters += Keyword::Assignment();
		parameters += String::Format(UseBRT());
	}

	// BRT precision.
	if (ModuloDecision() && UseBRT()) {
		parameters += Keyword::Separator();
		parameters += Keyword::BRT() + Keyword::Dot() + Keyword::Precision();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetBRTPrecision());
	}

	// Object selection.
	if (GetDiscernibility() == Reduct::DISCERNIBILITY_OBJECT) {
		parameters += Keyword::Separator();
		parameters += GetObjectSelector().GetParameters();
	}

	parameters += Keyword::Separator();

	// Use IDG?
	parameters += Keyword::IDG();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseIDG());

	// IDG filename.
	if (UseIDG()) {
		parameters += Keyword::Separator();
		parameters += Keyword::IDG() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetIDGFilename();
	}

	parameters += Keyword::Separator();

	// Precompute entries?
	parameters += Keyword::Precompute();
	parameters += Keyword::Assignment();
	parameters += String::Format(PrecomputeMatrix());

	return parameters;

}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducer::SetParameter(const String &keyword, const String &value) {

	// Discernibility.
	if (keyword == Keyword::Discernibility()) {
		if (value == Reduct::GetString(Reduct::DISCERNIBILITY_FULL))
			return SetDiscernibility(Reduct::DISCERNIBILITY_FULL);
		if (value == Reduct::GetString(Reduct::DISCERNIBILITY_OBJECT))
			return SetDiscernibility(Reduct::DISCERNIBILITY_OBJECT);

		// For backwards compatibility.
		String copy(value);
		copy.ToLowercase();
		if (copy == "object related")
			return SetDiscernibility(Reduct::DISCERNIBILITY_OBJECT);

		return false;
	}

	// Modulo decision.
	if ((keyword == Keyword::Modulo() + Keyword::Dot() + Keyword::Decision()) && value.IsBoolean())
		return ModuloDecision(value.GetBoolean());

	// Modulo decision (backwards compatibility).
	if (keyword == Keyword::UseDecision() && value.IsBoolean())
		return ModuloDecision(value.GetBoolean());

	// BRT.
	if (keyword == Keyword::BRT() && value.IsBoolean())
		return UseBRT(value.GetBoolean());

	// BRT precision.
	if ((keyword == Keyword::BRT() + Keyword::Dot() + Keyword::Precision()) && value.IsFloat())
		return SetBRTPrecision(value.GetFloat());

	// IDG.
	if (keyword == Keyword::IDG() && value.IsBoolean())
		return UseIDG(value.GetBoolean());

	// IDG filename.
	if (keyword == Keyword::IDG() + Keyword::Dot() + Keyword::Filename())
		return SetIDGFilename(value);

	// Precompute disc. matrix?
	if (keyword == Keyword::Precompute() && value.IsBoolean())
		return PrecomputeMatrix(value.GetBoolean());

	// Object selection?
	if (GetObjectSelector().SetParameter(keyword, value))
		return true;

	// Unknown keyword/value.
	return false;

}

//-------------------------------------------------------------------
// Method........: IsApplicable
// Author........: Aleksander šhrn
// Date..........:
// Description...: Returns true if the algorithm is applicable to the
//                 structure, false otherwise.
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducer::IsApplicable(const Structure &structure, bool /*warn*/) const {
	return structure.IsA(DECISIONTABLE);
}
//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Note that the whole approach of computing the
//                 full discernibility matrix (or rows of it) is not
//                 optimal wrt memory considerations. For large
//                 datasets or if memory is scarce, then one should
//                 consider doing all the absorption on-the-fly and not
//                 actually store the matrix entries before building and
//                 simplifying the function.
//
// Revisions.....: Aš 981026: For reducts relative to objects, compute
//                            prime implicants of representatives only.
//                 Aš 990226: Added "precompute matrix" stuff.
//                            For large datasets, it might consume too
//                            much memory to compute all entries.
//		   -N 01/09/13 Added threading. Moved stuff around to allow
//			for threading. Added criticals to the deeper layers.
//			Added flush and spinouts.
//		   -N 25/10/13 Added single directive to failed reduct pattern
//			computation. Fixes a bug where copies of the patterns
//			would be added to the reduct set.
//===================================================================

Structure *
Reducer::Apply(Structure &structure) const {

	Message message;

	// Is the input structure of right type?
	if (!IsApplicable(structure))
		return NULL;

	// Cast to verified type.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	bool masked = true;

	// Special case: Is the table empty?
	if ((table->GetNoObjects(masked) == 0) || (table->GetNoAttributes(masked) == 0)) {
		Message::Error("The decision table is empty.");
		return NULL;
	}

	// Create a reduct set where the computed reducts are stored.
	Handle<Reducts> reducts = Creator::Reducts();

	// Special case: Are all condition attributes disabled?
	if (table->GetNoAttributes(masked) == 1 && table->GetDecisionAttribute(masked) == 0) {
		if (!AppendReduct(*reducts))
			return NULL;
		return reducts.Release();
	}



	bool failure = false;
	bool uselock = false;
	#ifdef _OPENMP
		//This will return true if the process is already running in parallel. If that is
		//the case, we don't need to use locks here as the data has already been separated in the threads
		//unless the nested flag is set. 
		//The logical set is lock = !p OR (p AND n). -N
/*		
		if (ThreadControl::GetInParallel() && ThreadControl::GetNestedThreads()) {
			//Set and reserve threads. -N
			ThreadControl::GetSplitSize();
		}
*/		
		ThreadControl::GetSplitSize();

	#endif
	#pragma omp parallel \
	firstprivate(uselock, table) \
	shared(failure, message, masked, reducts) \
	default(none)
	{
		//Add thread to the counter. -N
		ThreadControl::AddThreadToCount();
		//Okay, this was way easier. -N
		uselock = (ThreadControl::GetCurrentTeamSize() > 1);

		//Moved in from serial region. -N
		Discerner discerner;

		// Initialize discerner.
		if (UseIDG()) {
			if (!discerner.LoadIDGs(GetIDGFilename(), *table, masked)) {
				Message::Error("Failed to load IDGs from " + GetIDGFilename() + ".");
	//			return NULL;
				failure = true;
			}
		}


		DiscernibilityMatrix         matrix;
		GeneralizedDecision::Handles decisions;
		Vector(int)                  selected;

		float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();

		// Create discernibility matrix.
		if (GetDiscernibility() == Reduct::DISCERNIBILITY_FULL) {
			if (PrecomputeMatrix()) {
				message.Notify("Computing discernibility matrix...");
				if (!matrix.Create(*table, masked, ModuloDecision(), discerner, precision))
//					return NULL;
					failure = true;
			}
			else {
				message.Notify("Initializing discernibility matrix...");
				if (!matrix.Initialize(*table, masked, ModuloDecision()))
//					return NULL;
					failure = true;
			}
		}
		else {
			if (!GetObjectSelector().Select(selected, *table, masked)) {
				Message::Error("Failed to perform object selection.");
//				return NULL;
				failure = true;
			}
			if (selected.empty()) {
				Message::Error("No objects match selection criteria.");
//				return NULL;
				failure = true;
			}
			if (PrecomputeMatrix()) {
				message.Notify("Computing discernibility matrix...");
				if (!matrix.Create(*table, selected, masked, ModuloDecision(), discerner, precision))
//					return NULL;
					failure = true;
			}
			else {
				message.Notify("Initializing discernibility matrix...");
				if (!matrix.Initialize(*table, masked, ModuloDecision()))
//					return NULL;
					failure = true;
				message.Notify("Computing generalized decisions...");
				if (ModuloDecision() && !table->GetGeneralizedDecisions(decisions, discerner, masked))
//					return NULL;
					failure = true;
			}
		}



		DiscernibilityFunction function;
		BooleanSOPFunction     primeimplicants;
		Vector(int)            supports;
	
	
		// Create discernibility function(s) and compute prime implicants.
		if (GetDiscernibility() == Reduct::DISCERNIBILITY_FULL) {
			//This area is executed by one thread only. Since the data here is shared,
			//allowing all threads to perform this section would result in copies of the
			//same patterns. -N
			#pragma omp single
			{
				message.Notify("Building discernibility function...");
				if (!CreateFunction(function, matrix, *table, masked, discerner))
//					return NULL;
					failure = true;
				message.Notify("Computing prime implicants...");
				if (!ComputePrimeImplicants(function, primeimplicants, supports))
//					return NULL;
					failure = true;
				message.Notify("Updating reduct set...");
				if (!AppendReducts(primeimplicants, supports, *reducts))
//					return NULL;
					failure = true;
			}
		}
		else if (!failure) {
			message.Notify("Computing reducts...");	
			
			Bits computed(matrix.GetDimension(true), false);
			#pragma omp for schedule(dynamic)
			for (int i = 0; i < selected.size(); i++) {
	
				#pragma omp flush(failure)
				if (!failure) {

#ifdef X86_64
					if (!message.Progress("Computing reducts for object " + String::Format((int)i + 1) + " of " + String::Format((int)selected.size()) + "...", i, selected.size()))
#else
					if (!message.Progress("Computing reducts for object " + String::Format(i + 1) + " of " + String::Format(selected.size()) + "...", i, selected.size()))

#endif
						failure = true;
					if (computed.GetState(matrix.GetEquivalenceClass(selected[i])))
						continue;
					Message::Debug("Building discernibility function...");
//					#pragma omp critical(reducts)
//					{
					if (!CreateFunction(function, matrix, selected[i], *table, masked, discerner, ModuloDecision() ? &decisions : NULL)) {
//						return NULL;
						failure = true;
						continue;
					}
//					}
					Message::Debug("Computing prime implicants...");
					bool ret;
					ret = ComputePrimeImplicants(function, primeimplicants, supports);
					if (!ret) { 
//						return NULL;
						failure = true;
						continue;
					}
					Message::Debug("Updating reduct and rule/pattern sets...");

					ret = AppendReducts(primeimplicants, *table, selected[i], masked, supports, *reducts, uselock);

					if (!ret)  {
//						return NULL;
						failure = true;
						continue;
					}
					computed.SetState(matrix.GetEquivalenceClass(selected[i]), true);
				}
			}
//			#pragma omp critical(reducts)
//			{
				reducts->PostCreation(*table, discerner, ModuloDecision(), uselock);
//			}
		}
		//Remove thread from the counter. -N
		ThreadControl::RemoveThreadFromCount();
	} // Parallel end

	return failure ? NULL : reducts.Release();
	
	/*

	DiscernibilityMatrix         matrix;
	GeneralizedDecision::Handles decisions;
	Vector(int)                  selected;

	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();

	// Create discernibility matrix.
	if (GetDiscernibility() == Reduct::DISCERNIBILITY_FULL) {
		if (PrecomputeMatrix()) {
			message.Notify("Computing discernibility matrix...");
			if (!matrix.Create(*table, masked, ModuloDecision(), discerner, precision))
				return NULL;
		}
		else {
			message.Notify("Initializing discernibility matrix...");
			if (!matrix.Initialize(*table, masked, ModuloDecision()))
				return NULL;
		}
	}
	else {
		if (!GetObjectSelector().Select(selected, *table, masked)) {
			Message::Error("Failed to perform object selection.");
			return NULL;
		}
		if (selected.empty()) {
			Message::Error("No objects match selection criteria.");
			return NULL;
		}
		if (PrecomputeMatrix()) {
			message.Notify("Computing discernibility matrix...");
			if (!matrix.Create(*table, selected, masked, ModuloDecision(), discerner, precision))
				return NULL;
		}
		else {
			message.Notify("Initializing discernibility matrix...");
			if (!matrix.Initialize(*table, masked, ModuloDecision()))
				return NULL;
			message.Notify("Computing generalized decisions...");
			if (ModuloDecision() && !table->GetGeneralizedDecisions(decisions, discerner, masked))
				return NULL;
		}
	}
	DiscernibilityFunction function;
	BooleanSOPFunction     primeimplicants;
	Vector(int)            supports;

	int i;

	// Create discernibility function(s) and compute prime implicants.
	if (GetDiscernibility() == Reduct::DISCERNIBILITY_FULL) {
		message.Notify("Building discernibility function...");
		if (!CreateFunction(function, matrix, *table, masked, discerner))
			return NULL;
		message.Notify("Computing prime implicants...");
		if (!ComputePrimeImplicants(function, primeimplicants, supports))
			return NULL;
		message.Notify("Updating reduct set...");
		if (!AppendReducts(primeimplicants, supports, *reducts))
			return NULL;
	}
	else {
		message.Notify("Computing reducts...");
		Bits computed(matrix.GetDimension(true), false);
		for (i = 0; i < selected.size(); i++) {
#ifdef X86_64
			if (!message.Progress("Computing reducts for object " + String::Format((int)i + 1) + " of " + String::Format((int)selected.size()) + "...", i, selected.size()))
#else
			if (!message.Progress("Computing reducts for object " + String::Format(i + 1) + " of " + String::Format(selected.size()) + "...", i, selected.size()))

#endif
				break;
			if (computed.GetState(matrix.GetEquivalenceClass(selected[i])))
				continue;
			Message::Debug("Building discernibility function...");
			if (!CreateFunction(function, matrix, selected[i], *table, masked, discerner, ModuloDecision() ? &decisions : NULL))
				return NULL;
			Message::Debug("Computing prime implicants...");
			bool ret;
			ret = ComputePrimeImplicants(function, primeimplicants, supports);
			if (!ret) 
				return NULL;
			Message::Debug("Updating reduct and rule/pattern sets...");
			ret = AppendReducts(primeimplicants, *table, selected[i], masked, supports, *reducts);
			if (!ret) 
				return NULL;
			computed.SetState(matrix.GetEquivalenceClass(selected[i]), true);
		}
		
		reducts->PostCreation(*table, discerner, ModuloDecision());
	}

	return reducts.Release();
	*/	
}


//-------------------------------------------------------------------
// New methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: CreateFunction
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990226: Added PrecomputeMatrix() stuff.
//                 Aš 990629: Added BRT stuff.
//===================================================================

bool
Reducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const {

	bool  copy      = false;
	bool  reduce    = true;
	bool  simplify  = true;
	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();
	bool  weighted  = false;

	const GeneralizedDecision::Handles *decisions = NULL;

	if (PrecomputeMatrix())
		return function.Create(matrix, copy, reduce, simplify, weighted);
	else
		return function.CreateSimplified(table, masked, ModuloDecision(), discerner, decisions, precision, &matrix, weighted);

}

//-------------------------------------------------------------------
// Method........: CreateFunction
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990226: Added PrecomputeMatrix() stuff.
//                 Aš 990629: Added BRT stuff.
//===================================================================

bool
Reducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const {

	bool  copy      = false;
	bool  reduce    = true;
	bool  simplify  = true;
	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();
	bool  weighted  = false;

	if (PrecomputeMatrix())
		return function.Create(matrix, object_no, copy, reduce, simplify, weighted);
	else
		return function.CreateSimplified(table, object_no, masked, ModuloDecision(), discerner, decisions, precision, &matrix, weighted);

}

//-------------------------------------------------------------------
// Method........: ComputePrimeImplicants
// Author........: Aleksander šhrn
// Date..........:
// Description...: Computes the prime implicants of the supplied
//                 discernibility function, and returns them (in-place)
//                 encoded as a Boolean SOP function. Also returned
//                 (in-place) is a vector of supports/strengths for
//                 each reduct, in case the reducts are approximate
//                 and non-exact.
//
//                 This method should be overloaded. The default
//                 implementation does nothing.
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducer::ComputePrimeImplicants(const DiscernibilityFunction &/*function*/, BooleanSOPFunction &/*primeimplicants*/, Vector(int) &/*supports*/) const {
	return false;
}

//-------------------------------------------------------------------
// Method........: AppendReduct
// Author........: Aleksander šhrn
// Date..........:
// Description...: Appends a single reduct that is either empty or
//                 contains the decision attribute. Typically relevant
//                 if all condition attributes in the input table are
//                 masked away.
//
// Comments......: Called from Apply as a special case.
// Revisions.....:
//===================================================================

bool
Reducer::AppendReduct(Reducts &reducts) const {

	// Create an empty reduct.
	Handle<Reduct> reduct = Creator::Reduct();

	// Initialize support.
	reduct->SetSupport(1);

	// Initialize disc. type. For now, set full discernibility regardless of specified
	// type so that rules can be generated afterwards. Should perhaps issue a warning.
	reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);

	reduct->IsModuloDecision(ModuloDecision());

	// Initialize reduct contents.
	if (!ModuloDecision())
		reduct->InsertAttribute(0);

	// Append reduct.
	return reducts.AppendStructure(reduct.GetPointer());

}

//-------------------------------------------------------------------
// Method........: AppendReducts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Interprets each product term of the SOP function
//                 as a reduct, and appends these to the reduct set.
//
// Comments......: For full discernibility reducts.
//
//                 This method is should only be called once, and
//                 the prime implicants should be unique. Otherwise,
//                 the appending to the Reducts set may go haywire,
//                 in case it originates from the RSES library...
//
// Revisions.....:
//===================================================================

bool
Reducer::AppendReducts(const BooleanSOPFunction &primeimplicants, const Vector(int) &supports, Reducts &reducts) const {

	int no_reducts    = primeimplicants.GetNoProducts();
	int no_attributes = primeimplicants.GetArity(false);

#ifdef _DEBUG
	if (no_reducts != supports.size()) {
		Message::Error("Illegal support vector passed to AppendReducts method.");
		return false;
	}
#endif

	int i, j;

	for (i = 0; i < no_reducts; i++) {

		Handle<Reduct> reduct = Creator::Reduct();

		// Set basic reduct properties.
		reduct->SetSupport(supports[i]);
		reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);
		reduct->IsModuloDecision(ModuloDecision());

		// Fill reduct.
		for (j = 0; j < no_attributes; j++) {
			if ((*(primeimplicants.GetProduct(i)))[j]) {
				if (!reduct->InsertAttribute(j))
					return false;
			}
		}

		// Append reduct to set.
		if (!reducts.AppendStructure(reduct.GetPointer()))
			return false;

	}

	// Handle special case in a graceful manner.
	if (no_reducts == 0) {
		Handle<Reduct> reduct = Creator::Reduct();
		reduct->SetSupport(1);
		reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_FULL);
		reduct->IsModuloDecision(ModuloDecision());
		if (!reducts.AppendStructure(reduct.GetPointer()))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: AppendReducts
// Author........: Aleksander šhrn
// Date..........:
// Description...: Interprets each product term of the SOP function
//                 as a reduct, and appends these to the reduct set.
//
// Comments......: For reducts relative to a particular object.
// Revisions.....:
//===================================================================

bool
Reducer::AppendReducts(const BooleanSOPFunction &primeimplicants, const DecisionTable &table, int object_no, bool masked, const Vector(int) &supports, Reducts &reducts, bool lock) const {

	int no_reducts    = primeimplicants.GetNoProducts();
	int no_attributes = primeimplicants.GetArity(false);

#ifdef _DEBUG
	if (no_reducts != supports.size()) {
		Message::Error("Illegal support vector passed to AppendReducts method.");
		return false;
	}
#endif

	bool verify = false;

	int i, j;

	for (i = 0; i < no_reducts; i++) {

		Handle<Reduct> reduct = Creator::Reduct();

		// Set basic reduct properties.
		reduct->SetObject(object_no);
		reduct->SetSupport(supports[i]);
		reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_OBJECT);
		reduct->IsModuloDecision(ModuloDecision());

		// Fill reduct.
		for (j = 0; j < no_attributes; j++) {
			if ((*(primeimplicants.GetProduct(i)))[j]) {
				if (!reduct->InsertAttribute(j))
					return false;
			}
		}

		// Append reduct to set.
		// Only if the reduct contains attributes. -N
		if (reduct->GetNoAttributes() > 0)
			if (!reducts.AppendReduct(reduct.GetPointer(), table, object_no, masked, ModuloDecision(), verify, lock))
				return false;

	}

	// Handle special case in a graceful manner.
	if (no_reducts == 0) {
		Handle<Reduct> reduct = Creator::Reduct();
		reduct->SetObject(object_no);
		reduct->SetSupport(1);
		reduct->SetDiscernibilityType(Reduct::DISCERNIBILITY_OBJECT);
		reduct->IsModuloDecision(ModuloDecision());
		if (!reducts.AppendReduct(reduct.GetPointer(), table, object_no, masked, ModuloDecision(), verify, lock))
			return false;
	}

	return true;

}

//-------------------------------------------------------------------
// Method........: UseIDG
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducer::UseIDG() const {
	return idg_;
}

bool
Reducer::UseIDG(bool idg) {
	idg_ = idg;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetIDGFilename/SetIDGFilename
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

const String &
Reducer::GetIDGFilename() const {
	return filename_idg_;
}

bool
Reducer::SetIDGFilename(const String &filename) {
	filename_idg_ = filename;
	return true;
}

//-------------------------------------------------------------------
// Method........: UseBRT
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducer::UseBRT() const {
	return brt_;
}

bool
Reducer::UseBRT(bool brt) {
	brt_ = brt;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetBRTPrecision/SetBRTPrecision
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

float
Reducer::GetBRTPrecision() const {
	return precision_brt_;
}

bool
Reducer::SetBRTPrecision(float precision) {
	precision_brt_ = precision;
	return (precision_brt_ >= 0.0 && precision_brt_ <= 1.0);
}

//-------------------------------------------------------------------
// Method........: PrecomputeMatrix
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Reducer::PrecomputeMatrix() const {
	return precompute_;
}

bool
Reducer::PrecomputeMatrix(bool precompute) {
	precompute_ = precompute;
	return true;
}

//-------------------------------------------------------------------
// Method........: IsDynamicComponent
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: Default implementation.
// Revisions.....:
//===================================================================

bool
Reducer::IsDynamicComponent() const {
	return false;
}

bool
Reducer::IsDynamicComponent(bool /*is_dynamic*/) {
	return false;
}
