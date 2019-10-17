// Hits.cpp: implementation of the Hits class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h> // Added 980901 by Aleksander Ührn.

#include "Hits.h"
#include "HitsMessage.h"
#include <kernel/system/assert.h>
#include "HitsKeeper.h"

void simplify(const Vector(Bits) &in, 
              Vector(Bits) &out, 
              const Vector(HittingSetInfo) &w_in, 
              Vector(HittingSetInfo) &w_out);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



Hits::Hits(const Vector(const Bits*) &bits, Parameters &parameters)
  : sets(bits), 
    params(parameters) 
{

  stat_initialized = 0; // Added 990317 by AÜ/SAV.

  if (bits.empty()) { // Added 990317 by AÜ/SAV.
    Message::Warning("Empty function passed to GA.", false);
    stat_initialized = 1;
    return;
  }

  HitsMessage::warn("Hits::Hits, initializing...\n");
  Rand::setSeed(params.rand_seed);
  context = new Context(sets, params);
  stats = new Stats();

  fitness = new HitsFitness(*context);
  //	keeper = new Keeper(params.keep_size);
  keeper = new HitsKeeper(*context);
  //	HitsCriteria * crit = new HitsCriteria(*context); // deleted by keeper
  //	keeper->crit = crit;

  init = new HitsInit(*context);
  eval = new Evaluator(fitness, *stats);
  stop = new HistStop(*context, params.gen_gap, params.use_fs, params.use_ks);
	
		
  sampler = new AbstractSampler();

  crsop = new CrossoverOperator(params.crs_k);
  invop = new InversionOperator(params.inv_k);
  mutop = new MutateOperator(params.mut_k);
  clnop = new CloneOperator();

  binop = new BinaryOperation(*crsop, params.pcrs);
  unop1 = new UnaryOperation(*mutop, params.pmut);
  unop2 = new UnaryOperation(*invop, params.pinv);
  unop3 = new UnaryOperation(*clnop, 1.0);
  elite = new EliteOperation();

  parents_selector = new ParentsSelector(*sampler, 
                                         params.pcrs + params.pmut + params.pinv, params.replacement);
	
  switch(params.alg_type){
  case Parameters::SIMPLE:
    recombinator = new RecombineReplaceParents();
    reaper = NULL;
  case Parameters::MOD:
    recombinator = new RecombineReplaceSample(*sampler);
    reaper = NULL;
    break;
  case Parameters::VAP:
    recombinator = new RecombineAdd();
    reaper = new Reaper(params.reaper_min_lt, params.reaper_max_lt);
    break;
  }
	
  switch(params.scale_type){
    // Removed 980903 by Aleksander Ührn.
    //case Parameters::NONE:
    //	scaler = NULL;
    //	break;
  case Parameters::BOLTZ:
    scaler = new Boltzscaler(params.boltz_start_tmp, 
                             params.boltz_end_tmp, params.boltz_delta);
    break;
  default:
    scaler = NULL; // Added 980903 by Aleksander Ührn.
  }

  if(elite != NULL)
    ops.push_back(elite);
  if(scaler != NULL)
    ops.push_back(scaler);
  if(reaper != NULL){
    ops.push_back(reaper);
    ops.push_back(stats); // need to recompute statistics after
    // population change
  }
  ops.push_back(parents_selector);
  ops.push_back(binop);
  ops.push_back(unop1);
  ops.push_back(unop2);
  ops.push_back(unop3);
  ops.push_back(recombinator);

  engine = new HitsEngine(*init, 
                          *eval,
                          ops, 
                          *stop,
                          *keeper,
                          *context);
  stat_initialized = 2;// Added 990317 by AÜ/SAV.
	
}

Hits::~Hits()
{

  //delete stuff
  if(stat_initialized != 2) { // Added 990317 by AÜ/SAV.
    return;
  }
  delete keeper ;
  delete engine;
  delete scaler;
  delete recombinator;
  delete parents_selector;
  delete context; 
  delete stats ;

  delete fitness ;

  delete init ;
  delete eval ;
  delete stop ;
		
  delete sampler ;
  delete crsop ;
  delete invop ;
  delete mutop ;
  delete clnop ;

  delete binop ;
  delete unop1 ;
  delete unop2 ;
  delete unop3 ;
  delete elite ;
}

bool Hits::hard(Vector(Bits) &solution, Vector(HittingSetInfo) &info, bool simp)
{
  HitsMessage::warn("Hits::hard, start...\n");

  if (stat_initialized == 0) // Added 990317 by AÜ/SAV.
    return false;
  if (stat_initialized == 1) // Added 990317 by AÜ/SAV.
    return true;


  if(context->parameters.report_gen)
    HitsMessage::warn("gen avg min max chg psz ect sec low ksz"); 
  engine->run();
  Vector(Individual*) res;
  keeper->kept(res);
  solution.reserve(res.size());
  for(unsigned int i = 0; i < res.size(); i++){
    Bits bit(context->nelems, false);
    EABitset * ebit = (EABitset *)res[i];
    //assert(ebit->size() == context->nelems); ???
    // copy into bitset, but use inversion info...
    for(unsigned int j = 0; j < ebit->size(); j++) 
      bit.SetState(context->real_index[j], ebit->get(j));
    solution.push_back(bit);
    HittingSetInfo dummy(ebit->term1, ebit->cover, ebit->cover);
    info.push_back(dummy);

  }

  if(simp){
    Vector(Bits) out;
    Vector(HittingSetInfo) info_out;
    simplify(solution, out, info, info_out);
    solution = out;
    info = info_out;			
  }
  return true;
}


bool Hits::hard(Vector ( Vector ( Bits ) ) & solutions, Vector(Vector(HittingSetInfo))  &info, bool simp)
{
  HitsMessage::warn("Hits::hard', start...\n");

  if (stat_initialized == 0) // Added 990317 by AÜ/SAV.
    return false;
  if (stat_initialized == 1) // Added 990317 by AÜ/SAV.
    return true;

  if(context->parameters.report_gen)
    HitsMessage::warn("gen avg min max chg psz ect sec low ksz"); 
  engine->run();
  Vector(Vector(Individual *)) res;
  ((HitsKeeper*)keeper)->keptList(res);
  int i;
  for(i = 0; i < res.size(); i++){
    while(solutions.size() <= i){
      Vector(Bits) dummy;
      solutions.push_back(dummy);
    }
    while(info.size() <= i){
      Vector(HittingSetInfo) dummy;
      info.push_back(dummy);
    }
    Vector(Individual *) * dres = &res[i];
    for(int j = 0; j < dres->size(); j++){
      Bits bit(context->nelems, false);
      EABitset * ebit = (EABitset *)(*dres)[j];
      for(unsigned int k = 0; k < ebit->size(); k++) 
        bit.SetState(context->real_index[k], ebit->get(k));
      solutions[i].push_back(bit);
      HittingSetInfo dummy(ebit->fitness, ebit->cover/context->sum_set_weights, ebit->cover/context->sum_set_weights);
      info[i].push_back(dummy);
    }
  }

  for(i = 0; simp && i < solutions.size(); i++){
    if(solutions[i].size() > 0){
      Vector(Bits) out;
      Vector(HittingSetInfo) info_out;
      simplify(solutions[i], out, info[i], info_out);
      solutions[i] = out;
      info[i] = info_out;
    }
  }

  return true;
}

struct iltbsp
{
  const Vector(Bits) &v;
  bool operator()(int s1, int s2) const
  {
    return (v[s1].Count(true) < v[s2].Count(true));
  }
  iltbsp(const Vector(Bits) &v_in) : v(v_in) {}
};

void simplify(const Vector(Bits) &in, 
              Vector(Bits) &out, 
              const Vector(HittingSetInfo) &w_in, 
              Vector(HittingSetInfo) &w_out)
{

  // create index vector
  Vector(int) index;
  index.reserve(in.size());
  int i;
  for(i = 0; i < in.size(); i++)
    index.push_back(i);

  // sort indexes on size of sets they index
  iltbsp cmp(in);
  std::sort(index.begin(), index.end(), cmp);

  // remove duplicates and supersets in one operation, aggregating w values
  Bits discard(in.size(), false);
  for(i = 0; i < in.size(); i++){
    if(discard.GetState(index[i]))
      continue;
    HittingSetInfo f_tmp = w_in[index[i]];
    for(int j = i + 1; j < in.size(); j++){
      if(in[index[i]] <= in[index[j]]){
        discard.SetState(index[j], true);
        f_tmp.acc_cover += w_in[index[j]].cover;
      }
    }
    out.push_back(in[index[i]]);
    w_out.push_back(f_tmp);
  }

  // done
}
 
