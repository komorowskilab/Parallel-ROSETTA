//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SAVGENETICREDUCER_H__
#define __SAVGENETICREDUCER_H__

#include <copyright.h>

#include <kernel/algorithms/reducer.h>
#include <kernel/algorithms/costinformation.h>

//-------------------------------------------------------------------
// Class.........: SAVGeneticReducer
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Wrapper around Staal's GA stuff.
// Revisions.....: AÛ 990726: Moved weight stuff to BooleanFunction.
//===================================================================

class SAVGeneticReducer : public Reducer {
public:

	//- Enumerated types...............................................
	enum Variation {VARIATION_SIMPLE, VARIATION_MODIFIED, VARIATION_VARIABLE};
	enum Scaling   {SCALING_BOLTZMANN};

	//- Textual representations........................................
	static String            GetString(SAVGeneticReducer::Variation variation);
	static String            GetString(SAVGeneticReducer::Scaling   scaling);

protected:

	//- Algorithm parameters...........................................
	bool                     scale_;             // Scale fitness function?
	bool                     report_;            // Generate report during calculation?
	bool                     replacement_;       // Sample parent individuals with replacement?
	bool                     elitism_;           // Use elitism?
	Variation                variation_;         // Algorithm varation.
	Scaling                  scaling_;           // Scaling type, if selected.
	long                     seed_;              // RNG seed.
	int                      keepsize_;          // Size of keep list.
	int                      populationsize_;    // Size of population.
	int                      generationgap_;     // Number of generations to wait for improvement.
	bool                     use_fitness_;       // Use average fitness as stopping criterion?
	bool                     use_keeplist_;      // Use change in keeplist as stopping criterion?
	float                    p_crossover_;       // Pr(crossover).
	float                    p_mutation_;        // Pr(mutation).
	float                    p_inversion_;       // Pr(inversion).
	int                      k_crossover_;       // Number of crossover points.
	int                      k_mutation_;        // Number of mutations to perform on a selected individual.
	int                      k_inversion_;       // Number of transpositions for inversion.
	float                    bias_;              // Importance of an individual being in a hitting set.
	bool                     costs_;             // Use cost information?
	String                   costs_filename_;    // Location of cost information.
	CostInformation          costs_info_;        // Cost information, mutable.
  int                      lifetime_max_;      // Maximum lifetime of individual, if VARIABLE variation.
  int                      lifetime_min_;      // Minimum lifetime of individual, if VARIABLE variation.
	float                    temperature_start_; // Start temperature, if BOLTZMANN scaling.
	float                    temperature_stop_;  // Stop temperature, if BOLTZMANN scaling.
	float                    temperature_delta_; // Generational change in temperature, if BOLTZMANN scaling.
	bool                     approximate_;       // Compute approximate solutions?
	float                    hitting_fraction_;  // Minimal hitting fraction, for approximate solutions.
	int                      keep_steps_;        // Number of extra keep lists, for approximate solutions.

protected:

	//- Methods inherited from Reducer.................................
	virtual bool             CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const;
	virtual bool             CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const;

public:

  //- Constructors/destructor........................................
  SAVGeneticReducer();
  virtual ~SAVGeneticReducer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String           GetParameters() const;
	virtual bool             SetParameter(const String &keyword, const String &value);
	virtual Structure       *Apply(Structure &structure) const;
	virtual SAVGeneticReducer	*Clone();

	//- Methods inherited from Reducer.................................
	virtual bool             ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const;

	//- Parameter methods..............................................
	bool                     ScaleFitness() const {return scale_;}
	bool                     ScaleFitness(bool scale) {scale_ = scale; return true;}

	bool                     GenerateReport() const {return report_;}
	bool                     GenerateReport(bool report) {report_ = report; return true;}

	bool                     ReplacementSampling() const {return replacement_;}
	bool                     ReplacementSampling(bool replacement) {replacement_ = replacement; return true;}

	bool                     UseElitism() const {return elitism_;}
	bool                     UseElitism(bool elitism) {elitism_ = elitism; return true;}

	Variation                GetVariation() const {return variation_;}
	bool                     SetVariation(Variation variation) {variation_ = variation; return true;}

	Scaling                  GetScaling() const {return scaling_;}
	bool                     SetScaling(Scaling scaling) {scaling_ = scaling; return true;}

	long                     GetSeed() const {return seed_;}
	bool                     SetSeed(long seed) {seed_ = seed; return true;}

	int                      GetKeepSize() const {return keepsize_;}
	bool                     SetKeepSize(int keepsize) {keepsize_ = keepsize; return true;}

	int                      GetPopulationSize() const {return populationsize_;}
	bool                     SetPopulationSize(int populationsize) {populationsize_ = populationsize; return true;}

	int                      GetGenerationGap() const {return generationgap_;}
	bool                     SetGenerationGap(int generationgap) {generationgap_ = generationgap; return true;}

	bool                     UseFitnessStopping() const {return use_fitness_;}
	bool                     UseFitnessStopping(bool fitness) {use_fitness_ = fitness; return true;}

	bool                     UseKeepListStopping() const {return use_keeplist_;}
	bool                     UseKeepListStopping(bool keeplist) {use_keeplist_ = keeplist; return true;}

	float                    GetCrossoverProbability() const {return p_crossover_;}
	bool                     SetCrossoverProbability(float p) {p_crossover_ = p; return true;}

	float                    GetMutationProbability() const {return p_mutation_;}
	bool                     SetMutationProbability(float p) {p_mutation_ = p; return true;}

	float                    GetInversionProbability() const {return p_inversion_;}
	bool                     SetInversionProbability(float p) {p_inversion_ = p; return true;}

	int                      GetNoCrossoverPoints() const {return k_crossover_;}
	bool                     SetNoCrossoverPoints(int k) {k_crossover_ = k; return true;}

	int                      GetNoMutations() const {return k_mutation_;}
	bool                     SetNoMutations(int k) {k_mutation_ = k; return true;}

	int                      GetNoTranspositions() const {return k_inversion_;}
	bool                     SetNoTranspositions(int k) {k_inversion_ = k; return true;}

	float                    GetBias() const {return bias_;}
	bool                     SetBias(float  bias) {bias_ = bias; return true;}

	bool                     UseCosts() const {return costs_;}
	bool                     UseCosts(bool costs) {costs_ = costs; return true;}

	const String            &GetCostFilename() const {return costs_filename_;}
	bool                     SetCostFilename(const String &filename) {costs_filename_ = filename; return true;}

	int                      GetMinimumLifetime() const {return lifetime_min_;}
	bool                     SetMinimumLifetime(int lifetime) {lifetime_min_ = lifetime; return true;}

	int                      GetMaximumLifetime() const {return lifetime_max_;}
	bool                     SetMaximumLifetime(int lifetime) {lifetime_max_ = lifetime; return true;}

	float                    GetStartTemperature() const {return temperature_start_;}
	bool                     SetStartTemperature(float t) {temperature_start_ = t; return true;}

	float                    GetStopTemperature() const {return temperature_stop_;}
	bool                     SetStopTemperature(float t) {temperature_stop_ = t; return true;}

	float                    GetDeltaTemperature() const {return temperature_delta_;}
	bool                     SetDeltaTemperature(float t) {temperature_delta_ = t; return true;}

	bool                     ApproximateSolutions() const {return approximate_;}
	bool                     ApproximateSolutions(bool approximate) {approximate_ = approximate; return true;}

	float                    GetHittingFraction() const {return hitting_fraction_;}
	bool                     SetHittingFraction(float fraction) {hitting_fraction_ = fraction; return true;}

	int                      GetNoKeepSteps() const {return keep_steps_;}
	bool                     SetNoKeepSteps(int keep_steps) {keep_steps_ = keep_steps; return true;}

};

#endif 
