//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......: In the future, installation of all structural and
//                 algorithmic prototypes (and their associated
//                 dialogs) could be done through dynamic link
//                 libraries (DLLs). This would make everything more
//                 modular and extensible, but I doubt whether I'll
//                 ever get/take the time to do such a restructuring.
// Revisions.....:
//===================================================================

#include <stdafx.h> // Precompiled headers
#include <copyright.h>

#include <common/installer.h>
#include <common/objectmanager.h>
#include <common/modemanager.h>

//-------------------------------------------------------------------
// Structures
//===================================================================

#include <kernel/structures/approximation.h>
#include <kernel/structures/batchclassification.h>
#include <kernel/structures/classification.h>
#include <kernel/structures/decisiontables.h>
#include <kernel/structures/dictionary.h>
#include <kernel/structures/equivalenceclass.h>
#include <kernel/structures/equivalenceclasses.h>
#include <kernel/structures/floatattribute.h>
#include <kernel/structures/generalizeddecision.h>

//Ewa Makosa 03.06.2005
#include <kernel/structures/generalrule.h>
#include <kernel/structures/generalrules.h>

#include <kernel/structures/graph.h>
#include <kernel/structures/integerattribute.h>
#include <kernel/structures/project.h>
#include <kernel/structures/reducts.h>
#include <kernel/structures/rulebasedclassification.h>
#include <kernel/structures/rules.h>
#include <kernel/structures/stringattribute.h>
#include <kernel/structures/textfile.h>
#include <kernel/structures/ksdecisiontable.h>
#include <kernel/structures/ksinformationvector.h>
#include <kernel/structures/ksreduct.h>
#include <kernel/structures/ksrule.h>

//-------------------------------------------------------------------
// Algorithms
//===================================================================

#include <kernel/algorithms/approximator.h>
#include <kernel/algorithms/batchclassifier.h>
#include <kernel/algorithms/binarysplitter.h>
#include <kernel/algorithms/brorthogonalscaler.h>
#include <kernel/algorithms/combinatorialcompleter.h>
#include <kernel/algorithms/conditionedcombinatorialcompleter.h>
#include <kernel/algorithms/conditionedmeancompleter.h>

//Ewa Makosa 03.06.2005
#include <kernel/algorithms/cppgeneralruleexporter.h>

#include <kernel/algorithms/cppruleexporter.h>
#include <kernel/algorithms/cvserialexecutor.h>
#include <kernel/algorithms/dictionaryexporter.h>
#include <kernel/algorithms/dictionaryimporter.h>
#include <kernel/algorithms/discernibilityfunctionexporter.h>
#include <kernel/algorithms/dynamicreducer.h>
#include <kernel/algorithms/entropyscaler.h>
#include <kernel/algorithms/equalfrequencyscaler.h>
#include <kernel/algorithms/reyreducer.h>
#include <kernel/algorithms/holte1rreducer.h>
#include <kernel/algorithms/htmlreporter.h>
#include <kernel/algorithms/indiscernibilitygraphexporter.h>
#include <kernel/algorithms/johnsonreducer.h>
#include <kernel/algorithms/kidnapper.h>
#include <kernel/algorithms/loader.h>
#include <kernel/algorithms/manualreducer.h>
#include <kernel/algorithms/manualscaler.h>
#include <kernel/algorithms/matlabdecisiontableexporter.h>
#include <kernel/algorithms/meancompleter.h>
#include <kernel/algorithms/mydecisiontableexporter.h>
#include <kernel/algorithms/mydecisiontableimporter.h>

// sus
#include <kernel/algorithms/arffdecisiontableimporter.h>
#include <kernel/algorithms/adxdecisiontableimporter.h>
#include <kernel/algorithms/arffdecisiontableexporter.h>
#include <kernel/algorithms/adxdecisiontableexporter.h>

//Ewa Makosa 03.06.2005...................................
#include <kernel/algorithms/mygeneralrulefilter.h>

#include <kernel/algorithms/myposdecisiontableimporter.h>
#include <kernel/algorithms/myreductexporter.h>
#include <kernel/algorithms/myreductfilter.h>
#include <kernel/algorithms/myreductimporter.h>
#include <kernel/algorithms/myreductshortener.h>
#include <kernel/algorithms/myruleexporter.h>
#include <kernel/algorithms/myrulefilter.h>
#include <kernel/algorithms/naivebayesclassifier.h>
#include <kernel/algorithms/naivescaler.h>
#include <kernel/algorithms/objecttrackingvoter.h>
#include <kernel/algorithms/orthogonalfilescaler.h>
#include <kernel/algorithms/parallelexecutor.h>
#include <kernel/algorithms/partitioner.h>
#include <kernel/algorithms/prologdecisiontableexporter.h>

//Ewa Makosa 03.06.2005...................................
#include <kernel/algorithms/prologgeneralruleexporter.h>

#include <kernel/algorithms/prologreductexporter.h>
#include <kernel/algorithms/prologruleexporter.h>
#include <kernel/algorithms/qualityrulefilter.h>
#include <kernel/algorithms/qualityrulefilterloop.h>
#include <kernel/algorithms/reductcostfilter.h>
#include <kernel/algorithms/reductperformancefilter.h>
#include <kernel/algorithms/removalcompleter.h>

//Ewa Makosa 03.06.2005...................................
#include <kernel/algorithms/ruleshortener.h>
#include <kernel/algorithms/rulepairwisegeneralizer.h>
#include <kernel/algorithms/rulegroupgeneralizer.h>

#include <kernel/algorithms/saver.h>
#include <kernel/algorithms/seminaivescaler.h>
#include <kernel/algorithms/serialexecutor.h>

//Ewa Makosa 06.06.2005
#include <kernel/algorithms/simplecoverfilter.h>

#include <kernel/algorithms/standardvoter.h>
#include <kernel/algorithms/structurecreator.h>
#include <kernel/algorithms/valuesplitter.h>
#include <kernel/algorithms/xmldecisiontableexporter.h> 

//Ewa Makosa 03.06.2005...................................
#include <kernel/algorithms/xmlgeneralruleexporter.h> 

#include <kernel/algorithms/xmlreductexporter.h> 
#include <kernel/algorithms/xmlreporter.h> 
#include <kernel/algorithms/xmlruleexporter.h> 

#include <kernel/sav/algorithms/savgeneticreducer.h>

#include <kernel/genomics/algorithms/templatescaler.h>
#include <kernel/genomics/algorithms/cvclassifier.h>
#include <kernel/genomics/algorithms/functionalclassifier.h>


// -N
#include <kernel/genomics/algorithms/mcfeatureselector.h>

// Jacob Vesterlund:
#include <kernel/genomics/algorithms/rfeatureselector.h>
#include <kernel/genomics/algorithms/ftest.h>
#include <kernel/genomics/algorithms/ttest.h>
#include <kernel/genomics/algorithms/manualfeatureselector.h>
#include <kernel/genomics/algorithms/pca.h>
//#include <kernel/genomics/algorithms/permutationtest.h>
#include <kernel/genomics/algorithms/loadfeatureselection.h>
#include <kernel/genomics/algorithms/loocvserialexecutor.h>

#if defined(_ROSGUI)
#include <frontend/algorithms/odbcdecisiontableimporter.h>
#endif

#include <kernel/algorithms/ksrulegenerator.h>

//-------------------------------------------------------------------
// Algorithm dialogs.
//===================================================================

#if defined(_ROSGUI)
#include <frontend/dialogs/algorithmdialogs/dialogapproximator.h>
#include <frontend/dialogs/algorithmdialogs/dialogattributeselector.h>
#include <frontend/dialogs/algorithmdialogs/dialogbatchclassifier.h>
#include <frontend/dialogs/algorithmdialogs/dialogbinarysplitter.h>
#include <frontend/dialogs/algorithmdialogs/dialogbrorthogonalscaler.h>
#include <frontend/dialogs/algorithmdialogs/dialogcvserialexecutor.h>
#include <frontend/dialogs/algorithmdialogs/dialogdiscernibilityfunctionexporter.h>
#include <frontend/dialogs/algorithmdialogs/dialogdynamicreducer.h>
#include <frontend/dialogs/algorithmdialogs/dialogequalfrequencyscaler.h>
#include <frontend/dialogs/algorithmdialogs/dialogexecutor.h>
#include <frontend/dialogs/algorithmdialogs/dialogfilename.h>
#include <frontend/dialogs/algorithmdialogs/dialogfilescaler.h>
#include <frontend/dialogs/algorithmdialogs/dialogreyreducer.h>
#include <frontend/dialogs/algorithmdialogs/dialogindiscernibilitygraphexporter.h>
#include <frontend/dialogs/algorithmdialogs/dialogjohnsonreducer.h>

//Ewa Makosa
#include <frontend/dialogs/algorithmdialogs/dialoggeneralrulepruner.h>

#include <frontend/dialogs/algorithmdialogs/dialogmanualscaler.h>
#include <frontend/dialogs/algorithmdialogs/dialogmyreductshortener.h>
#include <frontend/dialogs/algorithmdialogs/dialognaivebayesclassifier.h>
#include <frontend/dialogs/algorithmdialogs/dialogorthogonalscaler.h>
#include <frontend/dialogs/algorithmdialogs/dialogodbcdecisiontableimporter.h>
#include <frontend/dialogs/algorithmdialogs/dialogparametersgeneral.h>
#include <frontend/dialogs/algorithmdialogs/dialogqualityrulefilter.h>
#include <frontend/dialogs/algorithmdialogs/dialogqualityrulefilterloop.h>
#include <frontend/dialogs/algorithmdialogs/dialogreductcostfilter.h>
#include <frontend/dialogs/algorithmdialogs/dialogreductperformancepruner.h>
#include <frontend/dialogs/algorithmdialogs/dialogreductpruner.h>
#include <frontend/dialogs/algorithmdialogs/dialogrulepruner.h>

//Ewa Makosa
#include <frontend/dialogs/algorithmdialogs/dialogruleshortener.h>
#include <frontend/dialogs/algorithmdialogs/dialogrulegroupgeneralizer.h>
#include <frontend/dialogs/algorithmdialogs/dialogrulepairwisegeneralizer.h>

#include <frontend/dialogs/algorithmdialogs/dialogsavgeneticreducer.h>
#include <frontend/dialogs/algorithmdialogs/dialogstandardvoter.h>
#include <frontend/dialogs/algorithmdialogs/dialogvoter.h>

// Torgeir 18.06.01 ---------------------------
#include <frontend/dialogs/algorithmdialogs/dialogfunctionalclassifier.h>
#include <frontend/dialogs/algorithmdialogs/dialogrulegenerator.h>

// Jacob Vesterlund:
#include <frontend/dialogs/algorithmdialogs/dialogmanualfeatureselector.h>
#include <frontend/dialogs/algorithmdialogs/dialogpca.h>
#include <frontend/dialogs/algorithmdialogs/dialogloocvserialexecutor.h>
#include <frontend/dialogs/algorithmdialogs/dialogloadfeatureselection.h>
#include <frontend/dialogs/algorithmdialogs/dialogrfeatureselector.h>
#include <frontend/dialogs/algorithmdialogs/dialogttest.h>
#include <frontend/dialogs/algorithmdialogs/dialogftest.h>
//#include <frontend/dialogs/algorithmdialogs/dialogpermutationtest.h>

#endif

//-------------------------------------------------------------------
// Methods for class Installer.
//===================================================================

//-------------------------------------------------------------------
// Method........: Install
// Author........: Aleksander šhrn
// Date..........:
// Description...:
// Comments......:
// Revisions.....:
//===================================================================

bool
Installer::Install() {
	return InstallStructures() && InstallAlgorithms();
}

//-------------------------------------------------------------------
// Method........: InstallStructures
// Author........: Aleksander šhrn
// Date..........:
// Description...: Installs all structures.
// Comments......: Note that the order of installation may be of
//                 importance if structures are implemented in terms
//                 of each other.
//
//                 Also, the installation order bears importance with
//                 respect to what is the default object for creation
//                 in case of inexact matches in the Creator::Create(Id)
//                 method.
//
//                 A rule of thumb may be to install in order of increasing
//                 specialization and frequency of use.
//
//                 See comments about DLLs at the top of this file.
// Revisions.....:
//===================================================================

bool
Installer::InstallStructures() {
	
	// Are the structures already installed?
	if (ObjectManager::GetNoStructures() != 0)
		return false;
	
	INSTALLSTRUCTURE(new GeneralizedDecision())
		INSTALLSTRUCTURE(new Classification())
		INSTALLSTRUCTURE(new RuleBasedClassification())
		INSTALLSTRUCTURE(new EquivalenceClass())
		INSTALLSTRUCTURE(new Graph())
		INSTALLSTRUCTURE(new Annotation())
		INSTALLSTRUCTURE(new ParentStructure())
		INSTALLSTRUCTURE(new Structures())
		INSTALLSTRUCTURE(new EquivalenceClasses())
		INSTALLSTRUCTURE(new Approximation())
		INSTALLSTRUCTURE(new Project())
		INSTALLSTRUCTURE(new KSDecisionTable())
		INSTALLSTRUCTURE(new KSInformationVector())
		INSTALLSTRUCTURE(new KSReduct())
		INSTALLSTRUCTURE(new BatchClassification())
		INSTALLSTRUCTURE(new IntegerAttribute())
		INSTALLSTRUCTURE(new FloatAttribute())
		INSTALLSTRUCTURE(new StringAttribute())
		INSTALLSTRUCTURE(new Dictionary())
		INSTALLSTRUCTURE(new Reducts())
		INSTALLSTRUCTURE(new Rules())
		
		// Author........: Ewa Makosa
		INSTALLSTRUCTURE(new GeneralRules())
		INSTALLSTRUCTURE(new GeneralRule())

		INSTALLSTRUCTURE(new KSRule())
		INSTALLSTRUCTURE(new DecisionTables())
		INSTALLSTRUCTURE(new TextFile())
		
		return true;
	
}

//-------------------------------------------------------------------
// Method........: InstallAlgorithms
// Author........: Aleksander šhrn
// Date..........:
// Description...: Installs all algorithms. This is also where the
//                 associations between algorithms and parameter
//                 dialogs are defined.
//
// Comments......: Cf. comments for the InstallStructures method.
//                 See comments about DLLs at the top of this file.
// Revisions.....:
//===================================================================

bool
Installer::InstallAlgorithms() {
	
	// Are the algorithms already installed?
	if (ObjectManager::GetNoAlgorithms() != 0)
		return false;
		
		//-----------------------------------------------------------------
		// Reducers.
		//=================================================================
		
			INSTALLALGORITHM(new SAVGeneticReducer(),                 new CDialogSAVGeneticReducer())
			INSTALLALGORITHM(new JohnsonReducer(),                    new CDialogJohnsonReducer())
			INSTALLALGORITHM(new Holte1RReducer(),                    NULL)
			INSTALLALGORITHM(new ManualReducer(),                     new CDialogAttributeSelector())
			// Robin 040723--------------
			INSTALLALGORITHM(new DynamicReducer(),                    new CDialogDynamicReducer())
			INSTALLALGORITHM(new ReyReducer(),						  new CDialogReyReducer())
			// --------------------------
			
			//-----------------------------------------------------------------
			// Rule generators.
			//=================================================================
			
			INSTALLALGORITHM(new KSRuleGenerator(),                   new CDialogRuleGenerator())

			
			//-----------------------------------------------------------------
			// Discretization.
			//=================================================================
			
			INSTALLALGORITHM(new BROrthogonalScaler(),                new CDialogBROrthogonalScaler())
			INSTALLALGORITHM(new ManualScaler(),                      new CDialogManualScaler())
			INSTALLALGORITHM(new EntropyScaler(),                     new CDialogOrthogonalScaler())
			INSTALLALGORITHM(new EqualFrequencyScaler(),              new CDialogEqualFrequencyScaler())
			INSTALLALGORITHM(new NaiveScaler(),                       new CDialogOrthogonalScaler())
			INSTALLALGORITHM(new SemiNaiveScaler(),                   new CDialogOrthogonalScaler())
			INSTALLALGORITHM(new OrthogonalFileScaler(),              new CDialogFileScaler())
					
			//-----------------------------------------------------------------
			// Classifiers
			//=================================================================
			
			INSTALLALGORITHM(new BatchClassifier(),                   new CDialogBatchClassifier())
			INSTALLALGORITHM(new StandardVoter(),                     new CDialogStandardVoter())
			INSTALLALGORITHM(new ObjectTrackingVoter(),               new CDialogVoter())
			INSTALLALGORITHM(new NaiveBayesClassifier(),              new CDialogNaiveBayesClassifier())
			
			//-----------------------------------------------------------------
			// I/O.
			//=================================================================
			
			INSTALLALGORITHM(new MyDecisionTableImporter(),           new CDialogFilename())

			// sus
			INSTALLALGORITHM(new ArffDecisionTableImporter(),           new CDialogFilename())
			INSTALLALGORITHM(new AdxDecisionTableImporter(),           new CDialogFilename())
			INSTALLALGORITHM(new ArffDecisionTableExporter(),           new CDialogFilename())
			INSTALLALGORITHM(new AdxDecisionTableExporter(),           new CDialogFilename())

			INSTALLALGORITHM(new MyPOSDecisionTableImporter(),        new CDialogFilename())
#if defined(_ROSGUI)
			INSTALLALGORITHM(new ODBCDecisionTableImporter(),         new CDialogODBCDecisionTableImporter())
#endif
			INSTALLALGORITHM(new DictionaryExporter(),                new CDialogFilename())
			INSTALLALGORITHM(new DictionaryImporter(),                new CDialogFilename())
			INSTALLALGORITHM(new MyReductImporter(),                  new CDialogFilename())
			INSTALLALGORITHM(new PrologRuleExporter(),                new CDialogFilename())

			//Ewa Makosa 03.06.2005
			INSTALLALGORITHM(new CPPGeneralRuleExporter(),            new CDialogFilename())
			INSTALLALGORITHM(new PrologGeneralRuleExporter(),         new CDialogFilename())
			
			INSTALLALGORITHM(new CPPRuleExporter(),                   new CDialogFilename())
			INSTALLALGORITHM(new MyRuleExporter(),                    new CDialogFilename())
			INSTALLALGORITHM(new PrologDecisionTableExporter(),       new CDialogFilename())
			INSTALLALGORITHM(new MatlabDecisionTableExporter(),       new CDialogFilename())
			INSTALLALGORITHM(new MyDecisionTableExporter(),           new CDialogFilename())
			INSTALLALGORITHM(new IndiscernibilityGraphExporter(),     new CDialogIndiscernibilityGraphExporter())
			INSTALLALGORITHM(new DiscernibilityFunctionExporter(),    new CDialogDiscernibilityFunctionExporter())
			INSTALLALGORITHM(new PrologReductExporter(),              new CDialogFilename())
			INSTALLALGORITHM(new MyReductExporter(),                  new CDialogFilename())
			INSTALLALGORITHM(new HTMLReporter(),                      new CDialogFilename())
			INSTALLALGORITHM(new XMLRuleExporter(),                   new CDialogFilename())
			//Ewa Makosa 03.06.2005			
			INSTALLALGORITHM(new XMLGeneralRuleExporter(),            new CDialogFilename())

			INSTALLALGORITHM(new XMLDecisionTableExporter(),          new CDialogFilename())
			INSTALLALGORITHM(new XMLReductExporter(),                 new CDialogFilename())
			INSTALLALGORITHM(new XMLReporter(),                       new CDialogFilename())
			
			//-----------------------------------------------------------------
			// Table splitters.
			//=================================================================
			
			INSTALLALGORITHM(new BinarySplitter(),                    new CDialogBinarySplitter())
			//INSTALLALGORITHM(new ValueSplitter(),                     new CDialogParametersGeneral())
			
			//-----------------------------------------------------------------
			// Filters.
			//=================================================================
			
			INSTALLALGORITHM(new MyReductFilter(),                    new CDialogReductPruner())
			INSTALLALGORITHM(new ReductCostFilter(),                  new CDialogReductCostFilter())
			INSTALLALGORITHM(new ReductPerformanceFilter(),           new CDialogReductPerformancePruner())
			INSTALLALGORITHM(new MyReductShortener(),                 new CDialogMyReductShortener())
			//Ewa Makosa 03.06.2005
			INSTALLALGORITHM(new MyGeneralRuleFilter(),               new CDialogGeneralRulePruner())
			
			INSTALLALGORITHM(new MyRuleFilter(),                      new CDialogRulePruner())
			INSTALLALGORITHM(new QualityRuleFilter(),                 new CDialogQualityRuleFilter())
			INSTALLALGORITHM(new QualityRuleFilterLoop(),             new CDialogQualityRuleFilterLoop())
			
			//-----------------------------------------------------------------
			// Tuners
			// Ewa Makosa 03.06.2005 ---------------------------
			//=================================================================			

			INSTALLALGORITHM(new RuleShortener(),			new CDialogRuleShortener())
			INSTALLALGORITHM(new RulePairwiseGeneralizer(),		new CDialogRulePairwiseGeneralizer())
			INSTALLALGORITHM(new RuleGroupGeneralizer(),		new CDialogRuleGroupGeneralizer())
			INSTALLALGORITHM(new SimpleCoverFilter(),		NULL)
			//-----------------------------------------------------------------
			// Miscellaneous.
			//=================================================================
			
			INSTALLALGORITHM(new Partitioner(),                       new CDialogAttributeSelector())
			INSTALLALGORITHM(new Approximator(),                      new CDialogApproximator())
			//INSTALLALGORITHM(new Anonymizer(),                        new CDialogAnonymizer())
			
			//-----------------------------------------------------------------
			// Executors.
			//=================================================================
			
			INSTALLALGORITHM(new SerialExecutor(),                    new CDialogExecutor())
			INSTALLALGORITHM(new CVSerialExecutor(),                  new CDialogCVSerialExecutor())
			INSTALLALGORITHM(new ParallelExecutor(),                  new CDialogExecutor())
			
			//-----------------------------------------------------------------
			// Completers.
			//=================================================================
			
			INSTALLALGORITHM(new RemovalCompleter(),                  NULL)
			INSTALLALGORITHM(new MeanCompleter(),                     NULL)
			INSTALLALGORITHM(new ConditionedMeanCompleter(),          NULL)
			INSTALLALGORITHM(new CombinatorialCompleter(),            NULL)
			INSTALLALGORITHM(new ConditionedCombinatorialCompleter(), NULL)
			
			//-----------------------------------------------------------------
			// Script algorithms.
			//=================================================================
			
			INSTALLALGORITHM(new Saver(),                             new CDialogFilename())
			INSTALLALGORITHM(new Loader(),                            new CDialogFilename())
			INSTALLALGORITHM(new StructureCreator(),                  NULL)
			INSTALLALGORITHM(new Kidnapper(),                         NULL)
	

		//-----------------------------------------------------------------
		// Genomic Rosetta Algorithms.
		//=================================================================
		
		// Torgeir 28.02.01 ---------------------------
		//-----------------------------------------------------------------
		INSTALLALGORITHM(new TemplateScaler(),						  NULL)
		INSTALLALGORITHM(new CVClassifier(),					      NULL)
		INSTALLALGORITHM(new FunctionalClassifier(),			      new CDialogFunctionalClassifier())
		//-----------------------------------------------------------------

		// -N
		//-----------------------------------------------------------------
		INSTALLALGORITHM(new MCFeatureSelector(),			NULL)
		//-----------------------------------------------------------------
	

		// Jacob Vesterlund
		//-----------------------------------------------------------------
		INSTALLALGORITHM(new LoadFeatureSelection(),				  new CDialogLoadFeatureSelection())
		INSTALLALGORITHM(new ManualFeatureSelector(),				  new CDialogManualFeatureSelector())
		INSTALLALGORITHM(new RFeatureSelector(),					  new CDialogRFeatureSelector())
		INSTALLALGORITHM(new FTest(),								  new CDialogFTest())
		INSTALLALGORITHM(new TTest(),								  new CDialogTTest())
		INSTALLALGORITHM(new PrincipalComponentAnalysis(),			  new CDialogPrincipalComponentAnalysis())
		INSTALLALGORITHM(new LeaveOneOutCVSerialExecutor(),           new CDialogLeaveOneOutCVSerialExecutor())
		//INSTALLALGORITHM(new PermutationTest(),                       new CDialogPermutationTest())
		//-----------------------------------------------------------------


#if defined(_ROSGUI)
		
		//Define the genomic algorithms: An algorithm is per default a "Standard Rosetta"
		//algorithm. The algorithms defined here to be part of the "Genomic Rosetta" mode
		//will only appear in the project tree of Genomic Rosetta.

		//Expression analysis
		GENOMICALGORITHM(CVCLASSIFIER)
		GENOMICALGORITHM(TEMPLATESCALER)
		GENOMICALGORITHM(FUNCTIONALCLASSIFIER)

		//Feature analysis
		GENOMICALGORITHM(FTEST)
		GENOMICALGORITHM(LOADFEATURESELECTION)
		GENOMICALGORITHM(LOOCVSERIALEXECUTOR)
		GENOMICALGORITHM(MANUALFEATURESELECTOR)
		GENOMICALGORITHM(PRINCIPALCOMPONENTANALYSIS)
		GENOMICALGORITHM(PERMCVSERIALEXECUTOR)
		//GENOMICALGORITHM(PERMLOOCVSERIALEXECUTOR)
		//GENOMICALGORITHM(PERMUTATIONTEST)
		GENOMICALGORITHM(RFEATURESELECTOR)
		GENOMICALGORITHM(TTEST)

		//Connect the genomic algorithm to correct node in the project tree. Genomic algorithms
		//that the user are allowed to interact with through dialogs must be defined here
		//in order to be visible in the project tree.

		CONNECTTONODE(FUNCTIONALCLASSIFIER, "Expression Analysis")
		
		CONNECTTONODE(FTEST, "Feature Selection")
		CONNECTTONODE(LOADFEATURESELECTION, "Feature Selection")
		CONNECTTONODE(LOOCVSERIALEXECUTOR, "Feature Selection")
		CONNECTTONODE(MANUALFEATURESELECTOR, "Feature Selection")
		CONNECTTONODE(PRINCIPALCOMPONENTANALYSIS, "Feature Selection")
		//CONNECTTONODE(PERMUTATIONTEST, "Feature Selection")
		CONNECTTONODE(RFEATURESELECTOR, "Feature Selection")
		CONNECTTONODE(TTEST, "Feature Selection")

#endif
	
	return true;
	
}
 
