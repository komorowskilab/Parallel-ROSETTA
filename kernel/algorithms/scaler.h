//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 960420
// Description...:
// Revisions.....:
//===================================================================

#ifndef __SCALER_H__
#define __SCALER_H__

#include <copyright.h>

#include <kernel/algorithms/algorithm.h>

#include <kernel/structures/decisiontable.h>

//-------------------------------------------------------------------
// Class prototypes.
//===================================================================

class Attribute;
class FloatAttribute;
class IntegerAttribute;
class StringAttribute;

//-------------------------------------------------------------------
// Class.........: Scaler
// Author........: Aleksander Ûhrn
// Date..........: 960420
// Description...:
// Revisions.....:
//===================================================================

class Scaler : public Algorithm {
public:

	//- Enumerated types...............................................
	enum Mode {MODE_DISCARD, MODE_SAVE, MODE_LOAD};

	//- Textual representations........................................
	static String            GetString(Scaler::Mode mode);

protected:

	//- Parameters.....................................................
	Scaler::Mode             mode_;     // Scaling mode.
	String                   filename_; // Filename where scaling information is to be stored, for later reconstruction.
	bool                     mask_;     // Mask away symbolic attributes prior to scaling?

protected:

	//- Masking methods................................................
	virtual bool             GetAttributeMasks(const DecisionTable &table, Vector(DecisionTable::Mask) &masks) const;
	virtual bool             SetAttributeMasks(DecisionTable &table, const Vector(DecisionTable::Mask) &masks) const;

	virtual bool             GetTemporaryAttributeMasks(const DecisionTable &table, Vector(DecisionTable::Mask) &masks) const;

public:

  //- Constructors/destructor........................................
  Scaler();
  virtual ~Scaler();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

	//- Methods inherited from Algorithm...............................
	virtual String           GetParameters() const;
	virtual bool             GetOutputFilenames(Vector(String) &filenames) const;
	virtual bool             SetParameter(const String &keyword, const String &value);
	virtual bool             IsApplicable(const Structure &structure, bool warn = true) const;
	virtual Scaler				*Clone() = 0;

	//- Parameter methods..............................................
	Scaler::Mode             GetMode() const {return mode_;}
	virtual bool             SetMode(Scaler::Mode mode);

	const String            &GetFilename() const {return filename_;}
	bool                     SetFilename(const String &filename) {filename_ = filename; return true;}

	bool                     MaskSymbolic() const {return mask_;}
	bool                     MaskSymbolic(bool mask) {mask_ = mask; return true;}

};

#endif 
