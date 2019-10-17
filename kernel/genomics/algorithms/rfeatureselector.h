//-------------------------------------------------------------------
// Author........: Jacob Vesterlund
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __RFEATURESELECTOR_H__
#define __RFEATURESELECTOR_H__

#include <copyright.h>

#include <kernel/genomics/algorithms/featureselector.h>

//-------------------------------------------------------------------
// Class.........: RFeatureSelector
// Author........: Jacob Vesterlund
// Date..........:
// Description...: Performs a feature selection using R.
// Revisions.....:
//===================================================================

class RFeatureSelector : public FeatureSelector {
protected:

  //- Parameters.....................................................
	int                attributes_;             
	float              threshold_; 
	 String				 filename_;
	String				rpath_;
	String				 directory_;


public:

  //- Constructors/destructor........................................
  RFeatureSelector();
  virtual ~RFeatureSelector();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String     GetParameters() const;
	virtual bool       SetParameter(const String &keyword, const String &value);
	virtual Structure *Apply(Structure &structure) const;
	virtual bool			 MakeDesignTable(DecisionTable &table, DecisionTable &featuretable) const;
	virtual bool			 MakeClassTable(DecisionTable &table, DecisionTable &featuretable) const;
	virtual RFeatureSelector 	*Clone();
	

	//- Parameter management...........................................
	int                GetAttributes() const {return attributes_;}
	bool               SetAttributes(int attributes) {attributes_ = attributes; return true;}

	float              GetThreshold() const {return threshold_;}
	bool               SetThreshold(float threshold) {float f = threshold; if (f < 0.0) f = 0.0; threshold_ = f; return true;}

	
virtual bool  FormatBStatTable2Ros(const String &infilename,  const String &outfilename) const;	
  virtual bool			 SaveTableAsR(DecisionTable &table, const String &filename) const;
	virtual bool				MakeRFile(const String &filename , const String &infilename, const String &mfilename,const String &dfilename, const String &outfilename, const String &classfilename) const;
	
  const String      &GetFilename() const {return filename_;}
  bool               SetFilename(const String &filename) {filename_ = filename; return true;}


 const String      &GetRPath() const {return rpath_;}
  bool               SetRPath(const String &rpath) {rpath_ = rpath; return true;}


 const String      &GetDirectory() const {return directory_;}
  bool               SetDirectory(const String &directory) {directory_ = directory; return true;}


	// add our get and set functions
};

#endif 
