//-------------------------------------------------------------------
// Author........: Robin Andersson
// Date..........: 040810
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REYREDUCER_H__
#define __REYREDUCER_H__

#include <copyright.h>

#include <kernel/algorithms/reducer.h>

//-------------------------------------------------------------------
// Class.........: ReyReducer
// Author........: Robin Andersson
// Date..........:
// Description...: Implements an algorithm for computing the
//                 minimal transversals of a hypergraph (i.e. the 
//                 transversal hypergraph), presented in:
//                 
//                 Rey C, Toumani F, Hacid M-S, Leger A (2003) An algorithm
//                 and a prototype for the dynamic discovery of e-services.
//                 Technical Report RR05-03, LIMOS, Clemont-Ferrand, France.
//                 http://www.isima.fr/limos/publications.htm
//
//                 A monotone boolean function constructed from the
//                 discernibility matrix of a decision table can be
//                 represented by a hypergraph whose edges are sets
//                 of literals (i.e. sums in the function).
//                 A minimal transversal of a hypergraph is a collection
//                 of nodes such that all edges are covered by at least
//                 one node. This is equivalent to a prime implicant
//                 of a monotone boolean function (i.e. a reduct).
//                 Hence, computing the set of minimal transversals
//                 of such a hypergraph corresponds to the calculation
//                 of all reducts of a decision table.
//                 
//                 For more information on transversals hypergraphs
//                 consult the following article:
//                 
//                 @misc{ eiter02hypergraph,
//						  author = "T. Eiter and G. Gottlob",
//						  title = "Hypergraph transversal computation and related problems in logic and AI",
//						  text = "T. Eiter and G. Gottlob. Hypergraph transversal computation and related
//					              problems in logic and AI. In Proc. 8th European Conference on Logics in
//                                Artificial Intelligence (JELIA 2002), LNCS 2424, pp. 549--564. Springer,
//                                2002.",
//                        year = "2002",
//                        url = "citeseer.ist.psu.edu/eiter02hypergraph.html" }
// Revisions.....: See reyreducer.cpp
//===================================================================
namespace {
#ifndef GCC_STL
using std::hash_set;
#else
using __gnu_cxx::hash_set;

#endif
}

class ReyReducer : public Reducer {
	
	//- Typedefs and structs
public:
	typedef Pair(Bits,Bits) BitsPair;

protected:

	// Hash compare for the hash_set of Bits
	struct bitscompare {
		bool operator()(const Bits& a, const Bits& b) const {
			return a.Format() == b.Format();
		}
	};

	// Hash compare for the hash_set of BitsPair
	struct bitspaircompare {
		bool operator()(const BitsPair& a, const BitsPair& b) const {
			if (a.first.Format() == b.first.Format())
				return a.second.Format() == b.second.Format();
			else return false;
		}
	};

public:

	// Hash function for the hash_set of Bits
	struct BitsHash : public std::unary_function<const Bits&, size_t> {
		size_t operator () (const Bits& bits) const {
			hash_set<char*>::hasher h;
			return h.operator()(bits.Format().GetBuffer());
		}
	};

	// Hash function for the hash_set of BitsPair
	struct BitsPairHash : public std::unary_function<const BitsPair&, size_t> {
		size_t operator () (const BitsPair& bits_pair) const {
			hash_set<char*>::hasher h;
			String bits_pair_string = bits_pair.first.Format();
			bits_pair_string += bits_pair.second.Format();
			return h.operator()(bits_pair_string.GetBuffer());
		}
	};
	
	// typedefs for convenience
	typedef hash_set<Bits, BitsHash, bitscompare> BitsSet;
	typedef hash_set<BitsPair, BitsPairHash, bitspaircompare> BitsPairSet;

protected:

	//- Parameters

	//- Global variables...............................................

protected:

	//- Methods inherited from Reducer.................................
	virtual bool       CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const;
	virtual bool       CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const;

public:

  //- Constructors/destructor........................................
  ReyReducer();
  virtual ~ReyReducer();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual ReyReducer	*Clone();

	//- Methods inherited from Reducer.................................
	virtual bool       ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const;

	//- Parameter methods..............................................

};

//-------------------------------------------------------------------
// Inline methods
//===================================================================

//-------------------------------------------------------------------
// Method........: UnionInsert
// Author........: Robin Andersson
// Date..........:
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

inline void
UnionInsert(ReyReducer::BitsPairSet &bits_pair_set, ReyReducer::BitsPair pair) {

	bool replaced = false;

	ReyReducer::BitsPairSet::const_iterator iter = bits_pair_set.begin();
	while (iter != bits_pair_set.end()) {
		
		if (pair.second == iter->second) {
			replaced = true;
			ReyReducer::BitsPair replace = *iter;
			replace.first = replace.first | pair.first;
			bits_pair_set.erase(*iter);
			bits_pair_set.insert(replace);
			break;
		}
		iter++;
	}
			
	if (!replaced)
		bits_pair_set.insert(pair);
}


//-------------------------------------------------------------------
// Method........: GetNextElement
// Author........: Robin Andersson
// Date..........:
// Description...: Removes and returns an element from the data structure
// Comments......:
// Revisions.....:
//===================================================================

// For vector of Bits
inline Bits
GetNextElement(Vector(Bits) &v) {

	Bits bits = v[v.size() - 1];
	v.pop_back();
	return bits;
}

// For hash_set of Bits
inline Bits
GetNextElement(ReyReducer::BitsSet &bits_set) {
	ReyReducer::BitsSet::iterator i = bits_set.begin();
	Bits b = *i;
	bits_set.erase(i);
	return b;
}

#endif
 
