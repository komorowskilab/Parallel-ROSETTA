// ontext.h: interface for the Context class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ONTEXT_H__F2ED4F80_3DDE_11D2_AF91_000000000000__INCLUDED_)
#define AFX_ONTEXT_H__F2ED4F80_3DDE_11D2_AF91_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../ea/EABitset.h"

#define _SAVHACK

struct Parameters{
  enum algtypes { SIMPLE, MOD, VAP }; // Algorithm types
  enum scaletypes { BOLTZ };    // fitness scaling types

#ifdef _SAVHACK
	bool is_interruptible;
#endif


  // choices
  bool do_scale;         // fitness function scale toggle
  bool report_gen;       // toggles generational output, should be false.
  bool replacement;      // parents sampled with replacement ?
  bool elite;            // use elitism ? 
  bool debug;            // should be off.

  /////////////    global

  algtypes alg_type;     // agorithm type
  scaletypes scale_type; // only in effect if do_scale == true
  long rand_seed;        // random seed
  int keep_size;         // size of keep list > 0
  int population_size;   // population size   > 0, acts as start size for VAP
	float hit_fraction;    // the fraction of total sets weights to have to be kept in the highscore list, (0.0,1.0)
	int keep_steps;        // when hit_fraction (hf)< 1.0, use a separate keep list extra
	                       // for each (hf + (1 - hf)/keep_step) from hit_fraction to 1.0
	                       // >= 0

  // Stopping criteria
  int gen_gap;           // how many generations to wait for improvement
  bool use_fs;           // use avg fitness as criteria ?
  bool use_ks;           // use change in keep list as criteria ? 

  // genetic operations
  float pcrs;  // fraction of individuals that should undergo crossover
  float pmut;  // ditto for mutation
  float pinv;  // ditto for inversion
  int crs_k;    // number of crossover points
  int mut_k;    // number of mutations to perform on a selected individual
  int inv_k;    // number of transposistions for inversion

  // fitness function stuff
  float bias; // importance of an individual being a hitting set, 0 < bias < 1
  Vector(float) costs;  // vector of positive costs pr element.
	Vector(float) set_weights;  // the weight of each set to be hit

  ///////////////    Specifics

  // Only for alg_type == VAP 
  int reaper_max_lt;  // maximum lifetime of individual > min_lt
  int reaper_min_lt;  // minimum lifetime > 0


  // Only for do_scal == true, scale_type == BOLTZ
  // used for simulated annealing effect scaling 
  float boltz_start_tmp;  // start temperature > end_tmp
  float boltz_end_tmp;    // end temperature > 0
  float boltz_delta;      // generational change in temperature
                           // < start - end, > 0 


  Parameters(){
    alg_type = MOD;
    pcrs = 0.3f;
    pmut = 0.05f;
    pinv = 0.05f;
    bias = 0.4f;
    rand_seed = 12345;
    scale_type = BOLTZ;
    reaper_max_lt = 6;
    reaper_min_lt = 1;
    boltz_start_tmp = 6.45f;
    boltz_end_tmp = 1.45f;
    boltz_delta = 0.02f;
    do_scale = true;
    gen_gap = 30;
    keep_size = 256;
    population_size = 70;
    report_gen = false;
    use_ks = true;
    use_fs = true;
    crs_k = 1;
    mut_k = 1;
    inv_k = 1;
    replacement = true;
    elite = true;
    debug = false;
		hit_fraction = 1.0;
		keep_steps = 0;
#ifdef _SAVHACK
		is_interruptible = true;
#endif

  }
};



class Context  
{
public:
  //typedef Vector(Vector(int)) etosmap;
	typedef Vector(Bits) etosmap;
  double covers(EABitset * b);
  Bits elements_used;
  bool keep_changed;
  int nsets; // number of sets
	double sum_set_weights;
	double keep_limit;   // = p.hit_fraction * sum_sets_weights
  int nelems;          // number of elements in all sets
  int elems_used;            // cardinality of universe
  Parameters &parameters;
  const Vector(const Bits*) &sets;
  Vector(int) setsize;
  Vector(int) real_index;
  etosmap etos; // map elements to the set of 
                     // indices of sets that this element is in
  Context(const Vector(const Bits*) &bits, Parameters &p);
  virtual ~Context();
};



#endif // !defined(AFX_ONTEXT_H__F2ED4F80_3DDE_11D2_AF91_000000000000__INCLUDED_)
 
