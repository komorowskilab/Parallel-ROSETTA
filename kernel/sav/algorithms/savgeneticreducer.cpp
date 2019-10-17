//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/sav/algorithms/savgeneticreducer.h>

#include <kernel/sav/library/hits/Hits.h>

#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>

#include <kernel/utilities/mathkit.h>

#include <kernel/basic/message.h>

//-------------------------------------------------------------------
// Methods for class SAVGeneticReducer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Aleksander šhrn/Staal Vinterbo
// Date..........:
// Description...: Sets sensible default parameter values.
// Comments......:
// Revisions.....:
//===================================================================

SAVGeneticReducer::SAVGeneticReducer() {
	ScaleFitness(true);
	GenerateReport(false);
	ReplacementSampling(true);
	UseElitism(true);
	SetVariation(VARIATION_MODIFIED);
	SetScaling(SCALING_BOLTZMANN);
	SetSeed(12345);
	SetKeepSize(256);
	SetPopulationSize(70);
	SetGenerationGap(30);
	UseFitnessStopping(true);
	UseKeepListStopping(true);
	SetCrossoverProbability(0.3f);
	SetMutationProbability(0.05f);
	SetInversionProbability(0.05f);
	SetNoCrossoverPoints(1);
	SetNoMutations(1);
	SetNoTranspositions(1);
	SetBias(0.4f);
	UseCosts(false);
	SetCostFilename(Undefined::String());
	SetMinimumLifetime(1);
	SetMaximumLifetime(6);
	SetStartTemperature(6.45f);
	SetStopTemperature(1.45f);
	SetDeltaTemperature(0.02f);
	ApproximateSolutions(false);
	SetHittingFraction(0.8f);
	SetNoKeepSteps(3);
}

SAVGeneticReducer::~SAVGeneticReducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(SAVGeneticReducer, SAVGENETICREDUCER, Reducer)

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
SAVGeneticReducer::GetParameters() const {

	String parameters = Reducer::GetParameters() + Keyword::Separator();

	// Algorithm variation.
	parameters += Keyword::Algorithm();
	parameters += Keyword::Assignment();
	parameters += GetString(GetVariation());

	if (GetVariation() == VARIATION_VARIABLE) {

		// Lifetime, min.
		parameters += Keyword::Separator();
		parameters += Keyword::Lifetime() + Keyword::Dot() + Keyword::Lower();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetMinimumLifetime());

		parameters += Keyword::Separator();

		// Lifetime, max.
		parameters += Keyword::Lifetime() + Keyword::Dot() + Keyword::Upper();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetMaximumLifetime());

	}

	parameters += Keyword::Separator();

	// Scale fitness.
	parameters += Keyword::Scaling();
	parameters += Keyword::Assignment();
	parameters += String::Format(ScaleFitness());

	if (ScaleFitness()) {

		// Scaling type.
		parameters += Keyword::Separator();
		parameters += Keyword::Scaling() + Keyword::Dot() + Keyword::Type();
		parameters += Keyword::Assignment();
		parameters += GetString(GetScaling());

		if (GetScaling() == SCALING_BOLTZMANN) {

			// Temperature, start.
			parameters += Keyword::Separator();
			parameters += Keyword::Temperature() + Keyword::Dot() + Keyword::Upper();
			parameters += Keyword::Assignment();
			parameters += String::Format(GetStartTemperature());

			parameters += Keyword::Separator();

			// Temperature, stop.
			parameters += Keyword::Temperature() + Keyword::Dot() + Keyword::Lower();
			parameters += Keyword::Assignment();
			parameters += String::Format(GetStopTemperature());

			parameters += Keyword::Separator();

			// Temperature, delta.
			parameters += Keyword::Temperature() + Keyword::Dot() + Keyword::Delta();
			parameters += Keyword::Assignment();
			parameters += String::Format(GetDeltaTemperature());

		}

	}

	parameters += Keyword::Separator();

	// Report.
	parameters += Keyword::Report();
	parameters += Keyword::Assignment();
	parameters += String::Format(GenerateReport());

	parameters += Keyword::Separator();

	// Replacement sampling.
	parameters += Keyword::Replace();
	parameters += Keyword::Assignment();
	parameters += String::Format(ReplacementSampling());

	parameters += Keyword::Separator();

	// Elitism.
	parameters += Keyword::Elitism();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseElitism());

	parameters += Keyword::Separator();

	// Seed.
	parameters += Keyword::Seed();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetSeed());

	parameters += Keyword::Separator();

	// Size, keeplist.
	parameters += Keyword::Size() + Keyword::Dot() + Keyword::Keep();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetKeepSize());

	parameters += Keyword::Separator();

	// Size, population.
	parameters += Keyword::Size() + Keyword::Dot() + Keyword::Population();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetPopulationSize());

	parameters += Keyword::Separator();

	// Gap, generation.
	parameters += Keyword::Gap() + Keyword::Dot() + Keyword::Generation();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetGenerationGap());

	parameters += Keyword::Separator();

	// Stopping, fitness.
	parameters += Keyword::Stop() + Keyword::Dot() + Keyword::Fitness();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseFitnessStopping());

	parameters += Keyword::Separator();

	// Stopping, keeplist.
	parameters += Keyword::Stop() + Keyword::Dot() + Keyword::Keep();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseKeepListStopping());

	parameters += Keyword::Separator();

	// Probability, crossover.
	parameters += Keyword::Probability() + Keyword::Dot() + Keyword::Crossover();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetCrossoverProbability());

	parameters += Keyword::Separator();

	// Probability, mutation.
	parameters += Keyword::Probability() + Keyword::Dot() + Keyword::Mutation();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetMutationProbability());

	parameters += Keyword::Separator();

	// Probability, inversion.
	parameters += Keyword::Probability() + Keyword::Dot() + Keyword::Inversion();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetInversionProbability());

	parameters += Keyword::Separator();

	// Number, crossover.
	parameters += Keyword::Number() + Keyword::Dot() + Keyword::Crossover();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoCrossoverPoints());

	parameters += Keyword::Separator();

	// Number, mutation.
	parameters += Keyword::Number() + Keyword::Dot() + Keyword::Mutation();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoMutations());

	parameters += Keyword::Separator();

	// Number, inversions/transpositions.
	parameters += Keyword::Number() + Keyword::Dot() + Keyword::Inversion();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetNoTranspositions());

	parameters += Keyword::Separator();

	// Bias.
	parameters += Keyword::Bias();
	parameters += Keyword::Assignment();
	parameters += String::Format(GetBias());

	parameters += Keyword::Separator();

	// Costs.
	parameters += Keyword::Cost();
	parameters += Keyword::Assignment();
	parameters += String::Format(UseCosts());

	if (UseCosts()) {

		// Costs, filename.
		parameters += Keyword::Separator();
		parameters += Keyword::Cost() + Keyword::Dot() + Keyword::Filename();
		parameters += Keyword::Assignment();
		parameters += GetCostFilename();

	}

	parameters += Keyword::Separator();

	// Approximate solutions.
	parameters += Keyword::Approximate();
	parameters += Keyword::Assignment();
	parameters += String::Format(ApproximateSolutions());

	if (ApproximateSolutions()) {

		// Hitting fraction.
		parameters += Keyword::Separator();
		parameters += Keyword::Fraction();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetHittingFraction());

		// Keep steps.
		parameters += Keyword::Separator();
		parameters += Keyword::Keep() + Keyword::Dot() + Keyword::Levels();
		parameters += Keyword::Assignment();
		parameters += String::Format(GetNoKeepSteps());

	}

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
SAVGeneticReducer::SetParameter(const String &keyword, const String &value) {

	if (Reducer::SetParameter(keyword, value))
		return true;

	// Algorithm variation.
	if (keyword == Keyword::Algorithm()) {
		if (value == GetString(VARIATION_SIMPLE))
			return SetVariation(VARIATION_SIMPLE);
		if (value == GetString(VARIATION_MODIFIED))
			return SetVariation(VARIATION_MODIFIED);
		if (value == GetString(VARIATION_VARIABLE))
			return SetVariation(VARIATION_VARIABLE);
		return false;
	}

	// Lifetime, min.
	if (keyword == Keyword::Lifetime() + Keyword::Dot() + Keyword::Lower() && value.IsInteger())
		return SetMinimumLifetime(value.GetInteger());

	// Lifetime, min.
	if (keyword == Keyword::Lifetime() + Keyword::Dot() + Keyword::Upper() && value.IsInteger())
		return SetMaximumLifetime(value.GetInteger());

	// Scale fitness.
	if (keyword == Keyword::Scaling() && value.IsBoolean())
		return ScaleFitness(value.GetBoolean());

	// Scaling type.
	if (keyword == Keyword::Scaling() + Keyword::Dot() + Keyword::Type()) {
		if (value == GetString(SCALING_BOLTZMANN))
			return SetScaling(SCALING_BOLTZMANN);
		return false;
	}

	// Temperature, start.
	if (keyword == Keyword::Temperature() + Keyword::Dot() + Keyword::Upper() && value.IsFloat())
		return SetStartTemperature(value.GetFloat());

	// Temperature, stop.
	if (keyword == Keyword::Temperature() + Keyword::Dot() + Keyword::Lower() && value.IsFloat())
		return SetStopTemperature(value.GetFloat());

	// Temperature, delta.
	if (keyword == Keyword::Temperature() + Keyword::Dot() + Keyword::Delta() && value.IsFloat())
		return SetDeltaTemperature(value.GetFloat());

	// Report.
	if (keyword == Keyword::Report() && value.IsBoolean())
		return GenerateReport(value.GetBoolean());

	// Replacement sampling.
	if (keyword == Keyword::Replace() && value.IsBoolean())
		return ReplacementSampling(value.GetBoolean());

	// Elitism.
	if (keyword == Keyword::Elitism() && value.IsBoolean())
		return UseElitism(value.GetBoolean());

	// Seed.
	if (keyword == Keyword::Seed() && value.IsInteger())
		return SetSeed(value.GetInteger());

	// Size, keeplist.
	if (keyword == Keyword::Size() + Keyword::Dot() + Keyword::Keep() && value.IsInteger())
		return SetKeepSize(value.GetInteger());

	// Size, population.
	if (keyword == Keyword::Size() + Keyword::Dot() + Keyword::Population() && value.IsInteger())
		return SetPopulationSize(value.GetInteger());

	// Gap, generation.
	if (keyword == Keyword::Gap() + Keyword::Dot() + Keyword::Generation() && value.IsInteger())
		return SetGenerationGap(value.GetInteger());

	// Stopping, fitness.
	if (keyword == Keyword::Stop() + Keyword::Dot() + Keyword::Fitness() && value.IsBoolean())
		return UseFitnessStopping(value.GetBoolean());

	// Stopping, keeplist.
	if (keyword == Keyword::Stop() + Keyword::Dot() + Keyword::Keep() && value.IsBoolean())
		return UseKeepListStopping(value.GetBoolean());

	// Probability, crossover.
	if (keyword == Keyword::Probability() + Keyword::Dot() + Keyword::Crossover() && value.IsFloat())
		return SetCrossoverProbability(value.GetFloat());

	// Probability, mutation.
	if (keyword == Keyword::Probability() + Keyword::Dot() + Keyword::Mutation() && value.IsFloat())
		return SetMutationProbability(value.GetFloat());

	// Probability, inversion.
	if (keyword == Keyword::Probability() + Keyword::Dot() + Keyword::Inversion() && value.IsFloat())
		return SetInversionProbability(value.GetFloat());

	// Number, crossover.
	if (keyword == Keyword::Number() + Keyword::Dot() + Keyword::Crossover() && value.IsInteger())
		return SetNoCrossoverPoints(value.GetInteger());

	// Number, mutation.
	if (keyword == Keyword::Number() + Keyword::Dot() + Keyword::Mutation() && value.IsInteger())
		return SetNoMutations(value.GetInteger());

	// Number, inversions/transpositions.
	if (keyword == Keyword::Number() + Keyword::Dot() + Keyword::Inversion() && value.IsInteger())
		return SetNoTranspositions(value.GetInteger());

	// Bias.
	if (keyword == Keyword::Bias() && value.IsFloat())
		return SetBias(value.GetFloat());

	// Costs.
	if (keyword == Keyword::Cost() && value.IsBoolean())
		return UseCosts(value.GetBoolean());

	// Costs, filename.
	if (keyword == Keyword::Cost() + Keyword::Dot() + Keyword::Filename())
		return SetCostFilename(value);

	// Approximate solutions.
	if (keyword == Keyword::Approximate() && value.IsBoolean())
		return ApproximateSolutions(value.GetBoolean());

	// Hitting fraction.
	if (keyword == Keyword::Fraction() && value.IsFloat())
		return SetHittingFraction(value.GetFloat());

	// Keep steps.
	if (keyword == Keyword::Keep() + Keyword::Dot() + Keyword::Levels() && value.IsInteger())
		return SetNoKeepSteps(value.GetInteger());

	return false;

}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
SAVGeneticReducer::Apply(Structure &structure) const {

	// Read cost information, if relevant.
	if (UseCosts()) {
		if (!structure.IsA(DECISIONTABLE))
			return NULL;
		Handle<DecisionTable>     table = dynamic_cast(DecisionTable *, &structure);
		Handle<SAVGeneticReducer> self  = const_cast(SAVGeneticReducer *, this);
		if (!self->costs_info_.Load(GetCostFilename(), *table, 1.0 /* default cost */)) {
			Message::Error("Failed to load cost information.");
			return NULL;
		}
	}

	return Reducer::Apply(structure);

}

//-------------------------------------------------------------------
// Methods inherited from Reducer.
//===================================================================

//-------------------------------------------------------------------
// Method........: CreateFunction
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....: Aš 990226: Added PrecomputeMatrix() stuff.
//                 Aš 990629: Added BRT stuff.
//                 Aš 990726: Moved weight stuff to BooleanFunction.
//===================================================================

bool
SAVGeneticReducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const {

	bool  copy      = false;
	bool  reduce    = true;
	bool  simplify  = true;
	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();
	bool  weighted  = ApproximateSolutions();

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
//                 Aš 990726: Moved weight stuff to BooleanFunction.
//===================================================================

bool
SAVGeneticReducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const {

	bool  copy      = false;
	bool  reduce    = true;
	bool  simplify  = true;
	float precision = UseBRT() ? GetBRTPrecision() : Undefined::Float();
	bool  weighted  = ApproximateSolutions();

	if (PrecomputeMatrix())
		return function.Create(matrix, object_no, copy, reduce, simplify, weighted);
	else
		return function.CreateSimplified(table, object_no, masked, ModuloDecision(), discerner, decisions, precision, &matrix, weighted);

}

//-------------------------------------------------------------------
// Method........: ComputePrimeImplicants
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
SAVGeneticReducer::ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const {

	int i, j;

	// Set SAV library parameters.
	Parameters parameters;
	switch (GetVariation()) {
		case VARIATION_SIMPLE:   parameters.alg_type  = Parameters::SIMPLE; break;
		case VARIATION_MODIFIED: parameters.alg_type  = Parameters::MOD;    break;
		case VARIATION_VARIABLE: parameters.alg_type  = Parameters::VAP;    break;
		default:                 return false;
	}
  parameters.pcrs                                 = GetCrossoverProbability();
  parameters.pmut                                 = GetMutationProbability();
  parameters.pinv                                 = GetInversionProbability();
  parameters.bias                                 = GetBias();
	parameters.rand_seed                            = GetSeed();
	switch (GetScaling()) {
		case SCALING_BOLTZMANN: parameters.scale_type = Parameters::BOLTZ; break;
		default:                return false;
	}
  parameters.reaper_max_lt                        = GetMaximumLifetime();
  parameters.reaper_min_lt                        = GetMinimumLifetime();
  parameters.boltz_start_tmp                      = GetStartTemperature();
  parameters.boltz_end_tmp                        = GetStopTemperature();
  parameters.boltz_delta                          = GetDeltaTemperature();
  parameters.do_scale                             = ScaleFitness();
  parameters.gen_gap                              = GetGenerationGap();
  parameters.keep_size                            = GetKeepSize();
  parameters.population_size                      = GetPopulationSize();
  parameters.report_gen                           = GenerateReport();
  parameters.use_ks                               = UseKeepListStopping();
  parameters.use_fs                               = UseFitnessStopping();
  parameters.crs_k                                = GetNoCrossoverPoints();
  parameters.mut_k                                = GetNoMutations();
  parameters.inv_k                                = GetNoTranspositions();
  parameters.replacement                          = ReplacementSampling();
  parameters.elite                                = UseElitism();
#ifdef _DEBUG
  parameters.debug                                = true;
#else
  parameters.debug                                = false;
#endif
#ifdef _SAVHACK
	parameters.is_interruptible                     = GetDiscernibility() == Reduct::DISCERNIBILITY_FULL;
#endif
	if (UseCosts()) {
		parameters.costs                              = costs_info_.GetCosts();
	}

	int no_components = function.GetNoComponents();

	if (ApproximateSolutions()) {
	  parameters.hit_fraction                       = GetHittingFraction();
	  parameters.keep_steps                         = GetNoKeepSteps();

		// Extract the weights associated with the function's components.
		parameters.set_weights.erase(parameters.set_weights.begin(), parameters.set_weights.end());
		parameters.set_weights.reserve(no_components);
		for (i = 0; i < no_components; i++)
			parameters.set_weights.push_back(function.GetWeight(i));

	}
	else {
	  parameters.hit_fraction                       = 1.0;
	  parameters.keep_steps                         = 0;
	}

#ifdef _DEBUG
	Message::Debug("Invoking SAV routines...");
#endif

	Vector(const Bits *) components;
	components.reserve(no_components);

	// Prepare SAV input.
	for (i = 0; i < no_components; i++)
		components.push_back(function.GetComponent(i));

	// Call SAV library function.
	Hits hits(components, parameters);
	Vector(Vector(Bits)) solutions;
	Vector(Vector(HittingSetInfo)) info;
	if (!hits.hard(solutions, info, true /* simplify */)) {
		Message::Error("Call to SAV routine failed.");
		return false;
	}
#ifdef _DEBUG
	if (solutions.size() != info.size()) {
		Message::Error("SAV routine returned inconsistent data.");
		return false;
	}
#endif

#ifdef _DEBUG
	Message::Debug("Interpreting SAV output...");
#endif

	int no_reducts = 0;

	// Compute total number of returned reducts.
	for (i = solutions.size() - 1; i >= 0; i--)
		no_reducts += solutions[i].size();

	// Initialize return structures.
	supports.erase(supports.begin(), supports.end());
	supports.reserve(no_reducts);

	primeimplicants.RemoveAllProducts();
	primeimplicants.Reserve(no_reducts);

	// Convert SAV output to function format.
	for (i = 0; i < solutions.size(); i++) {
#ifdef _DEBUG
		if (solutions[i].size() != info[i].size()) {
			Message::Error("SAV routine returned inconsistent data.");
			return false;
		}
#endif
		for (j = 0; j < solutions[i].size(); j++) {
			primeimplicants.AppendProduct(&(solutions[i][j]), true, 1.0);
			supports.push_back(MathKit::Round(100 * info[i][j].cover));
		}
	}

	return true;

}

//-------------------------------------------------------------------
// Local methods.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetString
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

String
SAVGeneticReducer::GetString(SAVGeneticReducer::Variation variation) {

	switch (variation) {
		case VARIATION_SIMPLE:   return "Simple";   break;
		case VARIATION_MODIFIED: return "Modified"; break;
		case VARIATION_VARIABLE: return "Variable"; break;
		default:                 return Undefined::String();
	}

}

String
SAVGeneticReducer::GetString(SAVGeneticReducer::Scaling scaling) {

	switch (scaling) {
		case SCALING_BOLTZMANN: return "Boltzmann"; break;
		default:                return Undefined::String();
	}

}
 
SAVGeneticReducer *
SAVGeneticReducer::Clone() {
	return new SAVGeneticReducer;
}
