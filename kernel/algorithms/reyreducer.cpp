//-------------------------------------------------------------------
// Author........: Robin Andersson
// Date..........: 040810
// Description...:
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>
// DEBUG
#include <kernel/utilities/iokit.h>

#include <kernel/algorithms/reyreducer.h>
#include <kernel/algorithms/keyword.h>

#include <kernel/structures/decisiontable.h>
#include <kernel/structures/discernibilitymatrix.h>

#include <kernel/basic/message.h>

#include <kernel/utilities/mathkit.h>
//#include <kernel/basic/bits.h>
//DEBUG:
#include <fstream>

//-------------------------------------------------------------------
// Static helpers.
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticCountOnes
// Author........: Robin Andersson
// Date..........:
// Description...: Static helper that computes the number of positive
//                 bits in a bitvector.
// Comments......: 
// Revisions.....:
//===================================================================

static int
StaticCountOnes(const Bits &bits) {

	int count = 0;

	// Iterate over the bits in the bitvector
	for (int i = 0; i < bits.GetSize(); i++) {
		if (bits[i])
			count++;
	}
	return count;
}

//-------------------------------------------------------------------
// Methods for class ReyReducer.
//===================================================================

//-------------------------------------------------------------------
// Constructors/destructor.
//===================================================================

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

ReyReducer::ReyReducer() {
	//no_objs_ = 0;
}

ReyReducer::~ReyReducer() {
}

//-------------------------------------------------------------------
// Methods inherited from Identifier.
//===================================================================

IMPLEMENTIDMETHODS(ReyReducer, REYREDUCER, Reducer)

//-------------------------------------------------------------------
// Methods inherited from Algorithm.
//===================================================================

//-------------------------------------------------------------------
// Method........: GetParameters
// Author........: Robin Andersson
// Date..........:
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

String
ReyReducer::GetParameters() const {

	String parameters = Reducer::GetParameters();

	return parameters;
}

//-------------------------------------------------------------------
// Method........: SetParameter
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
ReyReducer::SetParameter(const String &keyword, const String &value) {

	return Reducer::SetParameter(keyword, value);
}

//-------------------------------------------------------------------
// Method........: Apply
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

Structure *
ReyReducer::Apply(Structure &structure) const {

	//Handle<DecisionTable> table = dynamic_cast(DecisionTable *, &structure);

	// Global count of objects
	//const_cast(ReyReducer*, this)->no_objs_ = table->GetNoObjects(true);
	
	return Reducer::Apply(structure);
}

//-------------------------------------------------------------------
// Methods inherited from Reducer.
//===================================================================

//-------------------------------------------------------------------
// Method........: ComputePrimeImplicants
// Author........: Robin Andersson
// Date..........: 040810
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
//                 For more information on transversal hypergraphs
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
// Comments......: 
// Revisions.....: 040905: RA
//                 Added support for approximate solutions
//				   050316: RA
//                 Found and corrected some bugs in the original algorithm described in 
//                 the paper of Hacid.
//                 050316: RA
//                 Removed support for approximate solutions 
//===================================================================

bool
ReyReducer::ComputePrimeImplicants(const DiscernibilityFunction &function, BooleanSOPFunction &primeimplicants, Vector(int) &supports) const {

	Message message;
	int no_sums = function.GetNoSums();
	int no_attributes = function.GetArity(false);

	primeimplicants.RemoveAllProducts();
	supports.erase(supports.begin(), supports.end());
	
	// Empty function?
	if (no_sums == 0)
		return true;
	
	int i,j;
	
	// hyperedges with arity (number of bits set to one) > 1
	Vector(Bits) hyperedges;
	
	// the partial transversals
	BitsSet transversals;
	
	int hyper_index = 0;

	// Fill transversals with a transversal of the edges (sums) of the discernibility 
	// function that have arity = 1, they are together a partial transversal.
	Bits init_transversal(no_attributes, false);
	for (i = no_sums-1; i >= 0; i--) {
		
		const Bits edge = *function.GetSum(i);

		int count = StaticCountOnes(edge);

		// If only one attribute is covered by the hyperedge, 
		// then use it for building a reduct
		if (count == 1)
			init_transversal = init_transversal | edge;

		else if (count > 1) {
			hyperedges.push_back(edge);
			hyper_index++;
		}
	}
	if (StaticCountOnes(init_transversal) > 0)
		transversals.insert(init_transversal);

	// No 1-arity transversal added?
	if (transversals.size() == 0 && hyperedges.size() > 1){

		// Fill transversals with the 1 attribute transversals of
		// the first hyperedge
		Bits edge = GetNextElement(hyperedges);

		for (i = 0; i < edge.GetSize(); i++) {

			if (edge[i]){
				Bits new_edge(no_attributes, false);
				new_edge.SetState(i, true);
				transversals.insert(new_edge);
			}
		}
	}

	// helpers
	BitsPairSet transversals_1_p;           // 1-persistings
	BitsPairSet transversals_not_p;	        // not-persistings
	BitsSet transversals_n_p;		        // n-persistings

	// message progress stuff
	int init_no_edges = hyperedges.size();
	int edges_done = 0;

	if (!message.Progress("Computing reducts...", edges_done, init_no_edges)) {
		Message::Error("Algorithm aborting...");
		return false;
	}

	// Iterate over all hyperedges with arity > 1
	while (hyperedges.size() > 0) {

		Bits edge = GetNextElement(hyperedges);

		// Iterate over all partial minimal transversals
		while (!transversals.empty()) {

			Bits transversal = GetNextElement(transversals);

			Bits intersection = edge & transversal;

			// Calculate the number of common attributes in the
			// partial minimal transversal and the hyperedge
			int count = StaticCountOnes(intersection);

			// More than 1 commmon attribute? -> n-persisting
			if (count > 1) {
				// Remember the partial transversal as a n-persisting
				transversals_n_p.insert(transversal);
			}
			// Exactly 1 common attribute? -> 1-persisting
			else if (count == 1){
				// Remember both the partial transversal and 
				// the common attribute
				BitsPair p(transversal,intersection);
				transversals_1_p.insert(p);
			}
			// No common attribute? -> not-persisting
			else {
				// Remember both the partial transversal and
				// the hyperedge
				BitsPair p(transversal,edge);
				transversals_not_p.insert(p);
			}
		}

		// Iterate over all 1-persistings
		BitsPairSet::const_iterator tr_1_p_pair = transversals_1_p.begin();
		while (tr_1_p_pair != transversals_1_p.end()) {
			
			// Which attributes in the transversal are not present in the hyperedge?
			Bits setdiff = tr_1_p_pair->first - tr_1_p_pair->second;
			
			Vector(BitsPair) replaces;
		
			bool increment;
			
			// Modify the not-persistings by removing the attributes from 
			// the hyperedge that are not common attributes of the transversal
			// and the hyperedge IF the set difference is a proper subset of
			// the transversal.
			BitsPairSet::iterator tr_not_p_pair = transversals_not_p.begin();
			while (tr_not_p_pair != transversals_not_p.end()) {
				
				increment = true;

				if (setdiff < tr_not_p_pair->first) {
					
					BitsPair replace = *tr_not_p_pair;
					replace.second = replace.second - tr_1_p_pair->second;
					replaces.push_back(replace);
					BitsPairSet::iterator tmp_iter = tr_not_p_pair;
					tr_not_p_pair++;
					transversals_not_p.erase(tmp_iter);
					increment = false;
				}

				if (increment)
					tr_not_p_pair++;
			}
			
			for (int r = 0; r < replaces.size(); r++)
				transversals_not_p.insert(replaces[r]);

			tr_1_p_pair++;
		}

		
		// update partial minimum transversals as the n-persistings
		transversals = transversals_n_p;

		tr_1_p_pair = transversals_1_p.begin();
		while (tr_1_p_pair != transversals_1_p.end()) {

			transversals.insert(tr_1_p_pair->first);
			tr_1_p_pair++;
		}
		
		// Construct new partial transversals from the nodes in the not-persistings
		BitsPairSet::const_iterator tr_not_p_pair = transversals_not_p.begin();
		while (tr_not_p_pair != transversals_not_p.end()) {

			for (j = 0; j < tr_not_p_pair->second.GetSize(); j++) {

				if (tr_not_p_pair->second[j]) {

					Bits transversal = tr_not_p_pair->first;
					transversal.SetState(j,true);
					transversals.insert(transversal);
				}
			}
			tr_not_p_pair++;
		}

		// Clear helpstructures
#ifdef WIN32
		transversals_1_p.clear();
		transversals_n_p.clear();
		transversals_not_p.clear();
#endif
#ifdef LINUX
		transversals_1_p.erase(transversals_1_p.begin(), transversals_1_p.end());
		transversals_n_p.erase(transversals_n_p.begin(), transversals_n_p.end());
		transversals_not_p.erase(transversals_not_p.begin(), transversals_not_p.end());
#endif
		
		edges_done++;
		if (!message.Progress("Computing reducts...", edges_done, init_no_edges)) {
			Message::Error("Algorithm aborting...");
			return false;
		}
	}
	
	bool copy = true;
	bool weight = 1.0f;

	supports.reserve(transversals.size());
	primeimplicants.Reserve(transversals.size());
	
	// Build primeimplicants from the minimal transversals
	BitsSet::const_iterator iter = transversals.begin();
	while (iter != transversals.end()) {
	
		Bits bits = *iter;
		primeimplicants.AppendProduct(&bits, copy, weight);
		supports.push_back(100);

		iter++;
	}

	return true;
}

//-------------------------------------------------------------------
// Methods inherited from Reducer.
//===================================================================

//-------------------------------------------------------------------
// Method........: CreateFunction
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

bool
ReyReducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, const DecisionTable &table, bool masked, const Discerner &discerner) const {

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
// Author........: Robin Andersson
// Date..........:
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

bool
ReyReducer::CreateFunction(DiscernibilityFunction &function, const DiscernibilityMatrix &matrix, int object_no, const DecisionTable &table, bool masked, const Discerner &discerner, const GeneralizedDecision::Handles *decisions) const {

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

ReyReducer *
ReyReducer::Clone() {
	return new ReyReducer;
}
