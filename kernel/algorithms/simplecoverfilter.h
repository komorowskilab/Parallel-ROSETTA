//-------------------------------------------------------------------
// Author........: Ewa Makosa
// Date..........: 01.06.2005
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SIMPLECOVERFILTER_H__
#define __SIMPLECOVERFILTER_H__

#include <copyright.h>
#include <kernel/algorithms/algorithm.h>

#include <kernel/structures/rules.h>
//-------------------------------------------------------------------
// Class.........: SimpleCoverFilter
// Author........: Ewa Makosa
// Date..........:
// Description...: For each object, sets the strongest rules so that it
//					is covered at least threshold_ number of times. 
//					Remaining rules (that weren't used in the "covering"
//					process are filtered away.
//					
// Revisions.....:
//===================================================================
class DecisionTable;
class Discerner;

class SimpleCoverFilter : public Algorithm {

public:

	typedef Vector(bool) UsedRules;
	
private:
	int threshold_;
	bool                    idg_;              // Use indiscernibility definition graph (IDG)?
	String                  filename_idg_;     // IDG location, if relevant.

	void	Filter(const Handle<DecisionTable> table, const Discerner &discerner, Handle<Rules> rules)const;
	void	InitializeUsedRules(int rulesNo, UsedRules &usedRules)const;
	void	RemoveNotUsed(const UsedRules &usedRules, Handle <Rules> rules)const;
	

public:

  //- Constructors/destructor........................................
	SimpleCoverFilter();
	virtual ~SimpleCoverFilter();

	//- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String			GetParameters() const;
	virtual bool			SetParameter(const String &keyword, const String &value);
	virtual bool			IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Structure		*Apply(Structure &structure) const;
	virtual SimpleCoverFilter	*Clone();

	//- Parameter management...............................
	bool	SetThreshold(int threshold){threshold_ = threshold; return true;};
	int		GetThreshold()const{return threshold_;};

	bool            UseIDG() const;
	bool            UseIDG(bool idg);
	
	const String	&GetIDGFilename() const;
	bool            SetIDGFilename(const String &filename);
};
inline bool
SimpleCoverFilter::UseIDG() const {
	return idg_;
}

inline bool
SimpleCoverFilter::UseIDG(bool idg) {
	idg_ = idg;
	return true;
}

//-------------------------------------------------------------------
// Method........: GetIDGFilename/SetIDGFilename
// Author........: Aleksander óhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

inline const String &
SimpleCoverFilter::GetIDGFilename() const {
	return filename_idg_;
}

inline bool
SimpleCoverFilter::SetIDGFilename(const String &filename) {
	filename_idg_ = filename;
	return true;
}

#endif

 
