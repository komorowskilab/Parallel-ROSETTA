//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Revisions.....: 
//===================================================================

#include <stdafx.h> // Precompiled headers.
#include <copyright.h>

#include <kernel/basic/ids.h>
#include <kernel/basic/idholder.h>

//-------------------------------------------------------------------
// Identifier implementations
//===================================================================

IMPLEMENTID(ALGORITHM,                         "Algorithm",                         "Algorithm")
IMPLEMENTID(ANNOTATEDSTRUCTURE,                "AnnotatedStructure",                "Annotated structure")
IMPLEMENTID(ANNOTATION,                        "Annotation",                        "Annotation");
IMPLEMENTID(ANONYMIZER,                        "Anonymizer",                        "Anonymize")
IMPLEMENTID(APPROXIMATION,                     "Approximation",                     "Approximation")
IMPLEMENTID(APPROXIMATOR,                      "Approximator",                      "Approximate decision class")
IMPLEMENTID(ATTRIBUTE,                         "Attribute",                         "Attribute")
IMPLEMENTID(BATCHCLASSIFICATION,               "BatchClassification",               "Batch classification");
IMPLEMENTID(BATCHCLASSIFIER,                   "BatchClassifier",                   "Batch classifier")
IMPLEMENTID(BINARYOUTCOMECURVE,                "BinaryOutcomeCurve",                "Binary outcome curve")
IMPLEMENTID(BINARYSPLITTER,                    "BinarySplitter",                    "Split in two")
IMPLEMENTID(BOOLEANFUNCTION,                   "BooleanFunction",                   "Boolean function")
IMPLEMENTID(BOOLEANPOSFUNCTION,                "BooleanPOSFunction",                "Boolean POS function")
IMPLEMENTID(BOOLEANSOPFUNCTION,                "BooleanSOPFunction",                "Boolean SOP function")
IMPLEMENTID(BRORTHOGONALSCALER,                "BROrthogonalScaler",                "Boolean reasoning algorithm")
IMPLEMENTID(CALIBRATIONCURVE,                  "CalibrationCurve",                  "Calibration curve")
IMPLEMENTID(CLASSIFICATION,                    "Classification",                    "Classification")
IMPLEMENTID(CLASSIFIER,                        "Classifier",                        "Classifier")
IMPLEMENTID(COMBINATORIALCOMPLETER,            "CombinatorialCompleter",            "Combinatorial completion")
IMPLEMENTID(COMPLETER,                         "Completer",                         "Completer")
IMPLEMENTID(CONDITIONEDCOMBINATORIALCOMPLETER, "ConditionedCombinatorialCompleter", "Conditioned combinatorial completion")
IMPLEMENTID(CONDITIONEDCOMPLETER,              "ConditionedCompleter",              "Conditioned completer")
IMPLEMENTID(CONDITIONEDMEANCOMPLETER,          "ConditionedMeanCompleter",          "Conditioned mean/mode fill")
IMPLEMENTID(CONFUSIONMATRIX,                   "ConfusionMatrix",                   "Confusion matrix");

//Ewa Makosa
IMPLEMENTID(CPPGENERALRULEEXPORTER,            "CPPGeneralRuleExporter",            "C++ general format");

IMPLEMENTID(CPPRULEEXPORTER,                   "CPPRuleExporter",                   "C++ format");
IMPLEMENTID(CVSERIALEXECUTOR,                  "CVSerialExecutor",                  "Pipeline script with CV")
IMPLEMENTID(DECISIONTABLE,                     "DecisionTable",                     "Decision table")
IMPLEMENTID(DECISIONTABLEEXPORTER,             "DecisionTableExporter",             "Decision table exporter")
IMPLEMENTID(DECISIONTABLEIMPORTER,             "DecisionTableImporter",             "Decision table importer")
IMPLEMENTID(DECISIONTABLES,                    "DecisionTables",                    "Decision tables")
IMPLEMENTID(DICTIONARY,                        "Dictionary",                        "Dictionary")
IMPLEMENTID(DICTIONARYEXPORTER,                "DictionaryExporter",                "Export dictionary")
IMPLEMENTID(DICTIONARYIMPORTER,                "DictionaryImporter",                "Import dictionary")
IMPLEMENTID(DISCERNIBILITYEXPORTER,            "DiscernibilityExporter",            "Discernibility exporter")
IMPLEMENTID(DISCERNIBILITYFUNCTION,            "DiscernibilityFunction",            "Discernibility function")
IMPLEMENTID(DISCERNIBILITYFUNCTIONEXPORTER,    "DiscernibilityFunctionExporter",    "Discernibility functions")
IMPLEMENTID(DISCERNIBILITYMATRIX,              "DiscernibilityMatrix",              "Discernibility matrix")
IMPLEMENTID(DYNAMICREDUCER,                    "DynamicReducer",                    "Dynamic reducts")
IMPLEMENTID(ENTROPYSCALER,                     "EntropyScaler",                     "Entropy/MDL algorithm")
IMPLEMENTID(EQUALFREQUENCYSCALER,              "EqualFrequencyScaler",              "Equal frequency binning")
IMPLEMENTID(EQUIVALENCECLASS,                  "EquivalenceClass",                  "Equivalence class")
IMPLEMENTID(EQUIVALENCECLASSES,                "EquivalenceClasses",                "Equivalence classes")
IMPLEMENTID(EXECUTOR,                          "Executor",                          "Execute")
IMPLEMENTID(EXPORTER,                          "Exporter",                          "Exporter")
IMPLEMENTID(FILTER,                            "Filter",                            "Filter")
IMPLEMENTID(FLOATATTRIBUTE,                    "FloatAttribute",                    "Float attribute")
IMPLEMENTID(GENERALIZEDDECISION,               "GeneralizedDecision",               "Generalized decision")

//Ewa Makosa 03.06.2005
IMPLEMENTID(GENERALRULE,                       "GeneralRule",                       "General Rule")
IMPLEMENTID(GENERALRULEEXPORTER,               "GeneralRuleExporter",               "General rule exporter")
IMPLEMENTID(GENERALRULES,                      "GeneralRules",                      "General Rules")

IMPLEMENTID(GRAPH,                             "Graph",                             "Graph")
IMPLEMENTID(REYREDUCER,						   "ReyReducer",						"Rey's algorithm")
IMPLEMENTID(HISTORY,                           "History",                           "History")
IMPLEMENTID(HISTORYENTRY,                      "HistoryEntry",                      "History entry")
IMPLEMENTID(HOLTE1RREDUCER,                    "Holte1RReducer",                    "Holte's 1R")
IMPLEMENTID(HTMLREPORTER,                      "HTMLReporter",                      "HTML format")
IMPLEMENTID(IDENTIFIER,                        "Identifier",                        "Identifier")
IMPLEMENTID(IMPORTER,                          "Importer",                          "Importer")
IMPLEMENTID(INDISCERNIBILITYGRAPH,             "IndiscernibilityGraph",             "Indiscernibility graph")
IMPLEMENTID(INDISCERNIBILITYGRAPHEXPORTER,     "IndiscernibilityGraphExporter",     "Indiscernibility graph")
IMPLEMENTID(INFORMATIONVECTOR,                 "InformationVector",                 "Information vector")
IMPLEMENTID(INTEGERATTRIBUTE,                  "IntegerAttribute",                  "Integer attribute")
IMPLEMENTID(JOHNSONREDUCER,                    "JohnsonReducer",                    "Johnson's algorithm")
IMPLEMENTID(KIDNAPPER,                         "Kidnapper",                         "GetChild")
IMPLEMENTID(KSDECISIONTABLE,                   "KSDecisionTable",                   "Decision table")
IMPLEMENTID(KSINFORMATIONVECTOR,               "KSInformationVector",               "Information vector")
IMPLEMENTID(KSREDUCT,                          "KSReduct",                          "Reduct")
IMPLEMENTID(KSRULE,                            "KSRule",                            "Rule")
IMPLEMENTID(KSRULEGENERATOR,                   "KSRuleGenerator",                   "Rule generator")
IMPLEMENTID(LOADER,                            "Loader",                            "Load")
IMPLEMENTID(MANUALREDUCER,                     "ManualReducer",                     "Manual reducer")
IMPLEMENTID(MANUALSCALER,                      "ManualScaler",                      "Manual discretization")
IMPLEMENTID(MATLABDECISIONTABLEEXPORTER,       "MatlabDecisionTableExporter",       "Matlab format")
IMPLEMENTID(MEANCOMPLETER,                     "MeanCompleter",                     "Mean/mode fill")
IMPLEMENTID(MYDECISIONTABLEEXPORTER,           "MyDecisionTableExporter",           "Plain format")
IMPLEMENTID(MYDECISIONTABLEIMPORTER,           "MyDecisionTableImporter",           "Plain format")

// sus
IMPLEMENTID(ARFFDECISIONTABLEIMPORTER,           "ArffDecisionTableImporter",           "WEKA format")
IMPLEMENTID(ADXDECISIONTABLEIMPORTER,           "AdxDecisionTableImporter",           "dmLab format")
IMPLEMENTID(ARFFDECISIONTABLEEXPORTER,           "ArffDecisionTableExporter",           "WEKA format")
IMPLEMENTID(ADXDECISIONTABLEEXPORTER,           "AdxDecisionTableExporter",           "dmLab format")

//Ewa Makosa 03.06.2005
IMPLEMENTID(MYGENERALRULEFILTER,               "MyGeneralRuleFilter",               "Basic general filtering")

IMPLEMENTID(MYPOSDECISIONTABLEIMPORTER,        "MyPOSDecisionTableImporter",        "POS format")
IMPLEMENTID(MYREDUCTEXPORTER,                  "MyReductExporter",                  "Plain format")
IMPLEMENTID(MYREDUCTFILTER,                    "MyReductFilter",                    "Basic filtering")
IMPLEMENTID(MYREDUCTIMPORTER,                  "MyReductImporter",                  "Import reducts")
IMPLEMENTID(MYREDUCTSHORTENER,                 "MyReductShortener",                 "Basic shortening")
IMPLEMENTID(MYRULEEXPORTER,                    "MyRuleExporter",                    "Plain format")
IMPLEMENTID(MYRULEFILTER,                      "MyRuleFilter",                      "Basic filtering")
IMPLEMENTID(NAIVEBAYESCLASSIFIER,              "NaiveBayesClassifier",              "Naive Bayes")
IMPLEMENTID(NAIVESCALER,                       "NaiveScaler",                       "Naive algorithm")
IMPLEMENTID(OBJECTTRACKINGVOTER,               "ObjectTrackingVoter",               "Voting with object tracking")
IMPLEMENTID(ODBCDECISIONTABLEIMPORTER,         "ODBCDecisionTableImporter",         "Decision table importer (ODBC)")
IMPLEMENTID(ORTHOGONALFILESCALER,              "OrthogonalFileScaler",              "From file with cuts")
IMPLEMENTID(ORTHOGONALSCALER,                  "OrthogonalScaler",                  "Orthogonal scaler")
IMPLEMENTID(PARALLELEXECUTOR,                  "ParallelExecutor",                  "Batch script")
IMPLEMENTID(PARENTSTRUCTURE,                   "ParentStructure",                   "Parent structure")
IMPLEMENTID(PARTITIONER,                       "Partitioner",                       "Partition")
IMPLEMENTID(PERSISTENT,                        "Persistent",                        "Persistent");
IMPLEMENTID(PROJECT,                           "Project",                           "Project")
IMPLEMENTID(PROLOGDECISIONTABLEEXPORTER,       "PrologDecisionTableExporter",       "Prolog format")

//Ewa Makosa 03.06.2005
IMPLEMENTID(PROLOGGENERALRULEEXPORTER,         "PrologGeneralRuleExporter",         "Prolog general format")

IMPLEMENTID(PROLOGREDUCTEXPORTER,              "PrologReductExporter",              "Prolog format")
IMPLEMENTID(PROLOGRULEEXPORTER,                "PrologRuleExporter",                "Prolog format")
IMPLEMENTID(QUALITYRULEFILTER,                 "QualityRuleFilter",                 "Quality filtering")
IMPLEMENTID(QUALITYRULEFILTERLOOP,             "QualityRuleFilterLoop",             "Quality filtering loop")
IMPLEMENTID(REDUCER,                           "Reducer",                           "Reducer")
IMPLEMENTID(REDUCT,                            "Reduct",                            "Reduct")
IMPLEMENTID(REDUCTCOSTFILTER,                  "ReductCostFilter",                  "Cost filtering")
IMPLEMENTID(REDUCTEXPORTER,                    "ReductExporter",                    "Reduct exporter")
IMPLEMENTID(REDUCTFILTER,                      "ReductFilter",                      "Reduct filter")
IMPLEMENTID(REDUCTIMPORTER,                    "ReductImporter",                    "Reduct importer")
IMPLEMENTID(REDUCTPERFORMANCEFILTER,           "ReductPerformanceFilter",           "Performance filtering")
IMPLEMENTID(REDUCTS,                           "Reducts",                           "Reducts")
IMPLEMENTID(REDUCTSHORTENER,                   "ReductShortener",                   "Reduct shortener")
IMPLEMENTID(REMOVALCOMPLETER,                  "RemovalCompleter",                  "Remove incompletes")
IMPLEMENTID(REPORTER,                          "Reporter",                          "Report generator")
IMPLEMENTID(ROCCURVE,                          "ROCCurve",                          "ROC curve")
IMPLEMENTID(RULE,                              "Rule",                              "Rule")
IMPLEMENTID(RULEBASEDCLASSIFICATION,           "RuleBasedClassification",           "Rule-based classification")
IMPLEMENTID(RULEBASEDCLASSIFIER,               "RuleBasedClassifier",               "Rule-based classifier")
IMPLEMENTID(RULEEXPORTER,                      "RuleExporter",                      "Rule exporter")
IMPLEMENTID(RULEFILTER,                        "RuleFilter",                        "Rule filter")

// Ewa Makosa 03.06.2005 --------------------------
IMPLEMENTID(RULEGENERALIZER,                   "RuleGeneralizer",                  "Rule generalizing")
// Ewa Makosa 03.06.2005 --------------------------

IMPLEMENTID(RULEGENERATOR,                     "RuleGenerator",                     "Rule generator")

// Ewa Makosa 03.06.2005 ---------------------------
IMPLEMENTID(RULEGROUPGENERALIZER,              "RuleGroupGeneralizer",             "Rule group generalizing")
IMPLEMENTID(RULEPAIRWISEGENERALIZER,	       "RulePairwiseGeneralizer",	   "Rule pairwise generalizing")
// Ewa Makosa 03.06.2005 ---------------------------

IMPLEMENTID(RULES,                             "Rules",                             "Rules")

// Ewa Makosa 03.06.2005 ---------------------------
IMPLEMENTID(RULESHORTENER,                     "RuleShortener",                     "Rule shortening")
IMPLEMENTID(RULETUNER,                         "RuleTuner",                         "Rule tuner")
// Ewa Makosa 03.06.2005 ---------------------------

IMPLEMENTID(SAVER,                             "Saver",                             "Save")
IMPLEMENTID(SAVGENETICREDUCER,                 "SAVGeneticReducer",                 "Genetic algorithm")
IMPLEMENTID(SCALER,                            "Scaler",                            "Scaler")
IMPLEMENTID(SCRIPTALGORITHM,                   "ScriptAlgorithm",                   "Script algorithm")
IMPLEMENTID(SEMINAIVESCALER,                   "SemiNaiveScaler",                   "Semi-naive algorithm")
IMPLEMENTID(SERIALEXECUTOR,                    "SerialExecutor",                    "Pipeline script")
IMPLEMENTID(SERIALEXECUTORLOOP,                "SerialExecutorLoop",                "Looped serial execution")

//Ewa Makosa 06.06.2005
IMPLEMENTID(SIMPLECOVERFILTER,				   "SimpleCoverFilter",					"Naive coverage filtering algorithm")

IMPLEMENTID(SPLITTER,                          "Splitter",                          "Splitter")
IMPLEMENTID(STANDARDVOTER,                     "StandardVoter",                     "Standard voting")
IMPLEMENTID(STRINGATTRIBUTE,                   "StringAttribute",                   "String attribute")
IMPLEMENTID(STRUCTURE,                         "Structure",                         "General structure.");
IMPLEMENTID(STRUCTURECREATOR,                  "StructureCreator",                  "Create")
IMPLEMENTID(STRUCTURES,                        "Structures",                        "Structures")
IMPLEMENTID(TEXTFILE,                          "TextFile",                          "Textfile")
IMPLEMENTID(VALUESPLITTER,                     "ValueSplitter",                     "Value splitter")
IMPLEMENTID(VOTER,                             "Voter",                             "Voting")
IMPLEMENTID(XMLDECISIONTABLEEXPORTER,          "XMLDecisionTableExporter",          "XML format") 

//Ewa Makosa 03.06.2005
IMPLEMENTID(XMLGENERALRULEEXPORTER,            "XMLGeneralRuleExporter",            "XML general format") 

IMPLEMENTID(XMLREDUCTEXPORTER,                 "XMLReductExporter",                 "XML format") 
IMPLEMENTID(XMLREPORTER,                       "XMLReporter",                       "XML format") 
IMPLEMENTID(XMLRULEEXPORTER,                   "XMLRuleExporter",                   "XML format") 

// Torgeir 28.02.01 ---------------------------
IMPLEMENTID(TEMPLATESCALER,						"TemplateScaler",					"Template scaler")
IMPLEMENTID(ITERATIVECLASSIFER,					"IterativeClassifier",	       		"Iterative classifier")
IMPLEMENTID(CVCLASSIFIER,						"CVClassifier",	       				"CV classifier")
IMPLEMENTID(FULLREDUCTS,						"FullReducts",						"Full Reducts")
IMPLEMENTID(FUNCTIONALCLASSIFIER,				"FunctionalClassifier",				"Functional Classifier")
// Torgeir 28.02.01 ---------------------------

IMPLEMENTID(FEATURESELECTOR,                   "FeatureSelector",                   "Feature selector")
// -N 22/08/13 mcfeatureselector (RR) ---------
IMPLEMENTID(MCFEATURESELECTOR,			"MCFeatureSelector",			"Random Reducts feature selector")
IMPLEMENTID(RFEATURESELECTOR,                  "RFeatureSelector",                  "Perform Feature Selection with R")
IMPLEMENTID(TTEST,                             "TTest",                             "Perform T-Test")
IMPLEMENTID(FTEST,                             "FTest",                             "Perform F-Test")
IMPLEMENTID(MANUALFEATURESELECTOR,             "ManualFeatureSelector",             "Manual Feature Selection")
IMPLEMENTID(PRINCIPALCOMPONENTANALYSIS,		   "PrincipalComponentAnalysis",		"Principal Component Analysis")
IMPLEMENTID(LOADFEATURESELECTION,              "LoadFeatureSelection",				"Load From File")
IMPLEMENTID(LOOCVSERIALEXECUTOR,               "LeaveOutOneCVSerialExecutor",       "Pipeline script with leave-one-out CV")
IMPLEMENTID(PERMUTATIONTEST,				   "PermutationTest",					"Perform Permutation Test")
IMPLEMENTID(PERMLOOCVSERIALEXECUTOR,           "PermLeaveOutOneCVSerialExecutor",   "Pipeline script with leave-one-out CV for permutation")
IMPLEMENTID(PERMCVSERIALEXECUTOR,			   "PermCVSerialExecutor",				"Pipeline script with CV for permutation")


 
