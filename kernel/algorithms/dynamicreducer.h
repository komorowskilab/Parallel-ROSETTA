//-------------------------------------------------------------------
// Author........: Robin Andersson
// Date..........: 040723
// Description...:
// Revisions.....:
//===================================================================

#ifndef __DYNAMICREDUCER_H__
#define __DYNAMICREDUCER_H__

#include <copyright.h>
#include <kernel/algorithms/reducer.h>
#include <kernel/utilities/mathkit.h>

//-------------------------------------------------------------------
// Class.........: DynamicReducer
// Author........: Robin Andersson
// Date..........:
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================
namespace {
#ifndef GCC_STL
using std::hash;
using std::hash_map;
#else
using __gnu_cxx::hash;
using __gnu_cxx::hash_map;
#endif
}

class DynamicReducer : public Reducer {
private:
	//- Type definitions...............................................
	typedef Pair(Handle<Reduct>,int) RedItem;
	typedef Pair(const String, Vector(RedItem)) Entry;

	//Hash structs
	// Hash compare for the hash map of String Vector(RedItem) pairs
	struct Compare {
		bool operator()(const String& a, const String& b) const {
			return a == b;
		}
	};

	// Hash function for the hash map of String Vector(RedItem) pairs
	struct Hash : public std::unary_function<const String&, size_t> {
		size_t operator () (const String& str) const {
			hash<char*> h;
			return h.operator()(str.GetBuffer());
		}
	};

	typedef hash_map<const String, Vector(RedItem), Hash, Compare> ReductMap;
	
	//- Algorithm parameters...........................................
	Handle<Reducer>     reducer_;             // Reduct computation algorithm to use.
	int                 seed_;                // Seed to random number generator.
	int                 no_levels_;           // Number of levels.
	int                 no_samples_;          // Number of samples on each level.
	int                 upper_percentage_;    // Relative size of largest sample.
	int                 lower_percentage_;    // Relative size of smallest sample.
	bool                generalized_;         // Compute generalized dynamic reducts?
	float               threshold_;           // Stability coefficient threshold.

	//- Global help stuff (global means optimized in this case)........
	Vector(int) indices_;                     // The indices of unmasked objects
	int no_unmasked_objects_;                 // The number of unmasked objects

public:
	//- Constructors/destructor........................................
	DynamicReducer();
	virtual ~DynamicReducer();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String      GetParameters() const;
	virtual bool        SetParameter(const String &keyword, const String &value);
	virtual Structure   *Apply(Structure &structure) const;
	virtual bool        AppendReduct(Reducts &reducts) const;
	virtual DynamicReducer	*Clone();

	//- Parameter methods..............................................
	Handle<Reducer>     GetReducer() const {return reducer_;}
	bool                SetReducer(const Handle<Reducer> reducer) {reducer_ = reducer; return true;}

	int                 GetSeed() const {return seed_;}
	bool                SetSeed(int seed) {seed_ = seed; return true;}

	float               GetThreshold() const {return threshold_;}
	bool                SetThreshold(float threshold) {threshold_ = threshold; return true;}

	int                 GetNoLevels() const {return no_levels_;}
	bool                SetNoLevels(int no_levels) {no_levels_ = no_levels; return true;}

	int                 GetNoSamples() const {return no_samples_;}
	bool                SetNoSamples(int no_samples) {no_samples_ = no_samples; return true;}

	int                 GetUpperPercentage() const {return upper_percentage_;}
	bool                SetUpperPercentage(int upper_percentage) {upper_percentage_ = upper_percentage; return true;}

	int                 GetLowerPercentage() const {return lower_percentage_;}
	bool                SetLowerPercentage(int lower_percentage) {lower_percentage_ = lower_percentage; return true;}
	
	bool                Generalized() const {return generalized_;}
	bool                Generalized(bool generalized) {generalized_ = generalized; return true;}

protected:
	//-Internal Methods................................................
	void                UpdateReducts(ReductMap &reduct_map, const Reducts &sample_reducts, bool init = false) const;
	bool                CalculateDynamicReducts(const DecisionTable &table, Reducts &reducts, const ReductMap &reduct_map) const;
	bool                Sample(const DecisionTable &table, DecisionTable &sample, const int sample_size, RNG rng) const;
};

#endif
 
