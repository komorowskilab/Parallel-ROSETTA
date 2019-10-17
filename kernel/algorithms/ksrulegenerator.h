//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KSRULEGENERATOR_H__
#define __KSRULEGENERATOR_H__

#include <copyright.h>

#include <kernel/algorithms/rulegenerator.h>

//-------------------------------------------------------------------
// Class.........: KSRuleGenerator
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

class KSRuleGenerator : public RuleGenerator {
public:

  //- Constructors/destructor........................................
  KSRuleGenerator();
  virtual ~KSRuleGenerator();

  //- Methods inherited from Identifier..............................
	DECLAREIDMETHODS()

  //- Methods inherited from Algorithm...............................
  virtual Structure *Apply(Structure &structure) const;
  virtual KSRuleGenerator *Clone();

};

#endif 
