//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __REDUCTCOSTFILTER_H__
#define __REDUCTCOSTFILTER_H__

#include <copyright.h>

#include <kernel/algorithms/reductfilter.h>
#include <kernel/algorithms/costinformation.h>

#include <kernel/basic/pair.h>

//-------------------------------------------------------------------
// Class.........: ReductCostFilter
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Comments......: Filters away those reducts that have a total cost
//                 associated with them that exceeds some threshold.
//
// Revisions.....: AÛ 990204: For some obscure reason, the ú$@$$ú$ú
//                            compiler suddenly complains about using
//                            an STL (int, String) pair, even though
//                            the code has always compiled OK before...
//                            So I define a helper struct ISPair
//                            instead as a workaround.
//===================================================================

class ReductCostFilter : public ReductFilter {
protected:

	//- Helper stuff for internal use..................................
	struct ISPair {
		int    first;
		String second;
		ISPair() {first = 0;}
		ISPair(int _first, const String &_second) {first = _first; second = _second;}
	};

	typedef Pair(int, float) IFPair;

	struct IFPairCompareSecondDescending {
		bool operator () (const IFPair &x, const IFPair &y) const {
			return x.second > y.second;
		}
	};

	struct ISPairCompareFirstAscending {
		bool operator () (const ISPair &x, const ISPair &y) const {
			return x.first < y.first;
		}
	};

protected:

  //- Parameters.....................................................
	String                    filename_costs_;  // Name of file with attribute costs.
	float                     threshold_;       // Cost threshold.
	float                     default_cost_;    // Default attribute cost.
	String                    filename_log_;    // Name of log file.

  //- Mutable stuff for bookkeeping..................................
	Vector(float)             statistics_;      // All total costs, for summary statistics.
	Vector(IFPair)            rankings_;        // Total costs with indices.
	Vector(ISPair)            reducts_;         // Formatted string representations to go with rankings.

  //- Mutable stuff for execution....................................
	CostInformation           costs_;           // Attribute cost information.

protected:

	//- Methods inherited from Filter..................................
	virtual bool              Remove(const Structures &structures, int i) const;

	//- Local methods..................................................
	bool                      SaveLog(const DecisionTable &table) const;

public:

  //- Constructors/destructor........................................
  ReductCostFilter();
  virtual ~ReductCostFilter();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String            GetParameters() const;
	virtual bool              SetParameter(const String &keyword, const String &value);
	virtual bool              GetOutputFilenames(Vector(String) &filenames) const;
	virtual Structure        *Apply(Structure &structure) const;
	virtual ReductCostFilter	*Clone();

	//- Parameter management...........................................
	const String             &GetCostFilename() const {return filename_costs_;}
	bool                      SetCostFilename(const String &filename) {filename_costs_ = filename; return true;}

	const String             &GetLogFilename() const {return filename_log_;}
	bool                      SetLogFilename(const String &filename) {filename_log_ = filename; return true;}

	float                     GetThreshold() const {return threshold_;}
	bool                      SetThreshold(float threshold) {threshold_ = threshold; return true;}

	float                     GetDefaultCost() const {return default_cost_;}
	bool                      SetDefaultCost(float cost) {default_cost_ = cost; return true;}

};

#endif 
