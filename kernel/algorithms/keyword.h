//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........:
// Description...:
// Revisions.....:
//===================================================================

#ifndef __KEYWORD_H__
#define __KEYWORD_H__

#include <copyright.h>

#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Class.........: Keyword
// Author........: Aleksander Ûhrn
// Date..........:
// Description...: Scope around keywords used for parameter setting.
// Comments......: Alphabetize for clarity.  Note the case.
// Revisions.....:
//===================================================================

class Keyword {
public:

	//- Symbols........................................................
	static String Assignment()      {return String("=");               }
	static String MapsTo()          {return String("->");              }
	static String Separator()       {return String("; ");              }
	static String Dot()             {return String(".");               }

	//- Keywords.......................................................
	static String Accuracy()        {return String("ACCURACY");        }
	static String Algorithm()       {return String("ALGORITHM");       }
	static String And()             {return String("AND");             }
	static String Append()          {return String("APPEND");          }
	static String Approximate()     {return String("APPROXIMATE");     }
	static String APSP()            {return String("APSP");            }
	static String Attribute()       {return String("ATTRIBUTE");       }
	
	// Ewa Makosa 03.06.2005 ---------------------------
	static String AttrDifference()	{return String("ATTRDIFFERENCE");  }	

	static String Attributes()      {return String("ATTRIBUTES");      }
	static String BatchClassifier() {return String("BATCHCLASSIFIER"); }
	static String Bias()            {return String("BIAS");            }
	static String BRT()             {return String("BRT");             }
	static String Calibration()     {return String("CALIBRATION");     }
	static String Cardinality()     {return String("CARDINALITY");     }
	static String Certainty()       {return String("CERTAINTY");       }
	static String Class()           {return String("CLASS");           }
	static String Classifier()      {return String("CLASSIFIER");      }
	static String Commands()        {return String("COMMANDS");        }
	static String Completer()       {return String("COMPLETER");       }
	static String Condition()       {return String("CONDITION");       }
	static String Connect()         {return String("CONNECT");         }
	static String Connective()      {return String("CONNECTIVE");      }
	static String Cost()            {return String("COST");            }
	static String Coverage()        {return String("COVERAGE");        }
	static String Crossover()       {return String("CROSSOVER");       }
	static String Data()            {return String("DATA");            }
	static String Decision()        {return String("DECISION");        }
	static String DecisionTable()   {return String("DECISIONTABLE");   }
	static String Default()         {return String("DEFAULT");         }
	static String Degree()          {return String("DEGREE");          }
	static String Delta()           {return String("DELTA");           }
	static String Dictionary()      {return String("DICTIONARY");      }
	static String Discernibility()  {return String("DISCERNIBILITY");  }
	static String Dominate()        {return String("DOMINATE");        }
	static String Elitism()         {return String("ELITISM");         }
	static String Duplicate()       {return String("DUPLICATE");       }
	static String Factor()          {return String("FACTOR");          }
	static String Fallback()        {return String("FALLBACK");        }
	static String Filename()        {return String("FILENAME");        }
	static String Filtering()       {return String("FILTERING");       }
	static String Fitness()         {return String("FITNESS");         }
	static String Fraction()        {return String("FRACTION");        }
	static String Gap()             {return String("GAP");             }
	static String Generation()      {return String("GENERATION");      }
	static String Generalized()     {return String("GENERALIZED");     }
	static String Groups()          {return String("GROUPS");          }
	static String IDG()             {return String("IDG");             }
	static String IncludeWhole()    {return String("INCLUDEWHOLE");    }
	static String Index()           {return String("INDEX");           }
	static String Intervals()       {return String("INTERVALS");       }
	static String Invert()          {return String("INVERT");          }
	static String Inversion()       {return String("INVERSION");       }
	static String Keep()            {return String("KEEP");            }
	static String Length()          {return String("LENGTH");          }
	static String Levels()          {return String("LEVELS");          }
	static String LHS()             {return String("LHS");             }
	static String Lifetime()        {return String("LIFETIME");        }
	static String Log()             {return String("LOG");             }
	static String Lower()           {return String("LOWER");           }
	static String Mask()            {return String("MASK");            }
	static String Mode()            {return String("MODE");            }
	static String Modulo()          {return String("MODULO");          }
	static String Multiple()        {return String("MULTIPLE");        }
	static String Mutation()        {return String("MUTATION");        }
	static String Name()            {return String("NAME");            }
	static String Names()           {return String("NAMES");           }
	static String Normalization()   {return String("NORMALIZATION");   }
	static String Number()          {return String("NUMBER");          }
	static String Object()          {return String("OBJECT");          }
	static String Objects()         {return String("OBJECTS");         }
	static String Output()          {return String("OUTPUT");          }
	static String Percentage()      {return String("PERCENTAGE");      }
	static String Population()      {return String("POPULATION");      }
	static String Precision()       {return String("PRECISION");       }
	static String Precompute()      {return String("PRECOMPUTE");      }
	static String Primary()         {return String("PRIMARY");         }
	static String Probability()     {return String("PROBABILITY");     }
	static String Ratio()           {return String("RATIO");           }
	static String Reducer()         {return String("REDUCER");         }
	static String Reduct()          {return String("REDUCT");          }
	static String Reducts()         {return String("REDUCTS");         }
	static String Reflexive()       {return String("REFLEXIVE");       }
	static String Remove()          {return String("REMOVE");          }
	static String Replace()         {return String("REPLACE");         }
	static String Report()          {return String("REPORT");          }
	static String Resolution()      {return String("RESOLUTION");      }
	static String RHS()             {return String("RHS");             }
	static String ROC()             {return String("ROC");             }
	static String RuleGenerator()   {return String("RULEGENERATOR");   }
	static String Rules()           {return String("RULES");           }
	static String Samples()         {return String("SAMPLES");         }
	static String Scaling()         {return String("SCALING");         }
	static String Secondary()       {return String("SECONDARY");       }
	static String Seed()            {return String("SEED");            }
	static String Selection()       {return String("SELECTION");       }
	static String Shortening()      {return String("SHORTENING");      }
	static String Simplify()        {return String("SIMPLIFY");        }
	static String Size()            {return String("SIZE");            }
	static String Speed()           {return String("SPEED");           }
	static String Specific()        {return String("SPECIFIC");        }
	static String Stability()       {return String("STABILITY");       }
	static String Stop()            {return String("STOP");            }
	static String Support()         {return String("SUPPORT");         }
	static String Temperature()     {return String("TEMPERATURE");     }
	static String Threshold()       {return String("THRESHOLD");       }
	static String Type()            {return String("TYPE");            }
	static String Undefined()       {return String("UNDEFINED");       }
	static String Upper()           {return String("UPPER");           }
	static String Value()           {return String("VALUE");           }
	static String Verbose()         {return String("VERBOSE");         }
	static String Voting()          {return String("VOTING");          }
	static String TemplateLength()	        {return String("TEMPLATELENGTH");	         }
	static String ConstantTemplateLength()	{return String("CONSTANTTEMPLATELENGTH");	 }
	static String CreaseThr()				{return String("CREASETHR");			     }
	static String ConstantThr()			    {return String("CONSTANTTHR");		         }
	static String StartEndThr()			    {return String("STARTENDTHR");		         }
	static String ToleranceThr()		    {return String("TOLERANCETHR");		         }
	static String Supervised()			    {return String("SUPERVISED");		         }
	static String IterativeLog()		    {return String("ITERATIVELOG");		         }
	static String Iterate()					{return String("ITERATE");				     }
	static String Weight()					{return String("WEIGHT");					 }
	static String CV()                      {return String("CV");                        }
	static String Randomization()			{return String("RANDOMIZATION");		     }
	static String RulePruning()				{return String("RULEPRUNING");			     }
	static String To()                      {return String("TO");                        }
	static String Step()                    {return String("STEP");                      }
	
	static String Bootstrap()				{return String("BOOTSTRAP");				 }
	static String Straps()					{return String("STRAPS");					 }
	static String PThreshold()				{return String("PTHRESHOLD");				 }
	static String Generate()				{return String("GENERATE");					 }
	static String Directory()				{return String("DIRECTORY");				 }
	static String Permutations()			{return String("PERMUTATIONS");				 }
	static String Path()					{return String("PATH");						 }
	static String Folds()					{return String("FOLDS");					 }
	static String SaveFeatureSelection()    {return String("SAVEFEATURESELECTION");      }
	static String SaveFeatureSelectionFilename() {return String("SAVEFEATURESELECTIONFILENAME");      }

	//-N 01/08/13 Used for loading and saving classifier structures.
	static String Load()			{return String("LOAD");}
	static String Save()			{return String("SAVE");}
	static String SaveAndExit()		{return String("SAVEANDEXIT");}

	//-N 22/08/13 Keywords for Random Reducts.
	static String Features()		{return String("FEATURES");}
	static String BackgroundLoops()		{return String("BACKGROUNDLOOPS");}
	static String Test()			{return String("TEST");}
	static String SignificanceCutOff()	{return String("SIGNIFICANCECUTOFF");}
	static String Projections()		{return String("PROJECTIONS");}
	static String ProjectionSize()		{return String("PROJECTIONSIZE");}
	static String Classifiers()		{return String("CLASSIFIERS");}
	static String Criterion()		{return String("CRITERION");}

	//-N For setting the upper limit on number of threads allowed.
	static String MaxThreads()		{return String("MAXTHREADS");}	
	static String DynamicThreads()		{return String("DYNAMICTHREADS");}
	static String NestedThreads()		{return String("NESTEDTHREADS");}


	//-N 24/11/14 Keywords for MyRuleExporter to include Odds Ratio and Risk Ratio.
	static String WithOR()			{return String("WITHOR");}
	static String WithRR()			{return String("WITHRR");}
	static String WithCI()			{return String("WITHCI");}	//20/02/15 - N
	static String WithORAlpha()		{return String("WITHORALPHA");}	//20/02/15 - N
	


	//- Obsolete (for backwards compatibility).........................
	static String BCDecisionTable() {return String("BCDECISIONTABLE"); }
	static String LLThreshold()     {return String("LLTHRESHOLD");     }
	static String LPercentage()     {return String("LPERCENTAGE");     }
	static String LPThreshold()     {return String("LPTHRESHOLD");     }
	static String LSThreshold()     {return String("LSTHRESHOLD");     }
	static String RGDecisionTable() {return String("RGDECISIONTABLE"); }
	static String Tolerance()       {return String("TOLERANCE");       }
	static String Types()           {return String("TYPES");           }
	static String ULThreshold()     {return String("ULTHRESHOLD");     }
	static String UPercentage()     {return String("UPERCENTAGE");     }
	static String UPThreshold()     {return String("UPTHRESHOLD");     }
	static String USThreshold()     {return String("USTHRESHOLD");     }
	static String UseDecision()     {return String("USEDECISION");     }
	static String UseDictionary()   {return String("USEDICTIONARY");   }

};

#endif
 
