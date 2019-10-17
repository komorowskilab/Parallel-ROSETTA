// dialogsavgeneticparameters.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogsavgeneticparameters.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogSAVGeneticParameters dialog


CDialogSAVGeneticParameters::CDialogSAVGeneticParameters(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogSAVGeneticParameters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSAVGeneticParameters)
	m_BoltzmannScaling = FALSE;
	m_Costs = FALSE;
	m_CostFile = _T("");
	m_Elitism = FALSE;
	m_Replacement = FALSE;
	m_StopFitness = FALSE;
	m_StopKeep = FALSE;
	m_Bias = 0.0f;
	m_GenerationGap = 0;
	m_HitFraction = 0.0f;
	m_LifetimeMax = 0;
	m_LifetimeMin = 0;
	m_NCrossover = 0;
	m_NInversion = 0;
	m_NMutation = 0;
	m_PCrossover = 0.0f;
	m_PInversion = 0.0f;
	m_PMutation = 0.0f;
	m_Seed = 0;
	m_SizeKeep = 0;
	m_SizePopulation = 0;
	m_TempDelta = 0.0f;
	m_TempStart = 0.0f;
	m_TempStop = 0.0f;
	m_KeepSteps = 0;
	m_ApproximateSolutions = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogSAVGeneticParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSAVGeneticParameters)
	DDX_Control(pDX, IDC_CHECK_APPROXIMATE, m_CheckApproximateSolutions);
	DDX_Control(pDX, IDC_EDIT_KEEPSTEPS, m_EditKeepSteps);
	DDX_Control(pDX, IDC_EDIT_TEMPERATURE_STOP, m_EditTemperatureStop);
	DDX_Control(pDX, IDC_EDIT_TEMPERATURE_START, m_EditTemperatureStart);
	DDX_Control(pDX, IDC_EDIT_TEMPERATURE_DELTA, m_EditTemperatureDelta);
	DDX_Control(pDX, IDC_EDIT_LIFETIME_MIN, m_EditLifetimeMin);
	DDX_Control(pDX, IDC_EDIT_LIFETIME_MAX, m_EditLifetimeMax);
	DDX_Control(pDX, IDC_EDIT_HITFRACTION, m_EditHitFraction);
	DDX_Control(pDX, IDC_EDIT_COSTFILE, m_EditCostFile);
	DDX_Control(pDX, IDC_CHECK_COSTS, m_CheckCosts);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Check(pDX, IDC_CHECK_BOLTZMANNSCALING, m_BoltzmannScaling);
	DDX_Check(pDX, IDC_CHECK_COSTS, m_Costs);
	DDX_Text(pDX, IDC_EDIT_COSTFILE, m_CostFile);
	DDX_Check(pDX, IDC_CHECK_ELITISM, m_Elitism);
	DDX_Check(pDX, IDC_CHECK_REPLACEMENT, m_Replacement);
	DDX_Check(pDX, IDC_CHECK_STOP_FITNESS, m_StopFitness);
	DDX_Check(pDX, IDC_CHECK_STOP_KEEP, m_StopKeep);
	DDX_Text(pDX, IDC_EDIT_BIAS, m_Bias);
	DDV_MinMaxFloat(pDX, m_Bias, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_GENERATIONGAP, m_GenerationGap);
	DDV_MinMaxInt(pDX, m_GenerationGap, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_HITFRACTION, m_HitFraction);
	DDV_MinMaxFloat(pDX, m_HitFraction, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_LIFETIME_MAX, m_LifetimeMax);
	DDV_MinMaxInt(pDX, m_LifetimeMax, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_LIFETIME_MIN, m_LifetimeMin);
	DDV_MinMaxInt(pDX, m_LifetimeMin, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_N_CROSSOVER, m_NCrossover);
	DDV_MinMaxInt(pDX, m_NCrossover, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_N_INVERSION, m_NInversion);
	DDV_MinMaxInt(pDX, m_NInversion, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_N_MUTATION, m_NMutation);
	DDV_MinMaxInt(pDX, m_NMutation, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_P_CROSSOVER, m_PCrossover);
	DDV_MinMaxFloat(pDX, m_PCrossover, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_P_INVERSION, m_PInversion);
	DDV_MinMaxFloat(pDX, m_PInversion, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_P_MUTATION, m_PMutation);
	DDV_MinMaxFloat(pDX, m_PMutation, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_SEED, m_Seed);
	DDX_Text(pDX, IDC_EDIT_SIZE_KEEP, m_SizeKeep);
	DDV_MinMaxInt(pDX, m_SizeKeep, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_SIZE_POPULATION, m_SizePopulation);
	DDV_MinMaxInt(pDX, m_SizePopulation, 1, 2147483647);
	DDX_Text(pDX, IDC_EDIT_TEMPERATURE_DELTA, m_TempDelta);
	DDV_MinMaxFloat(pDX, m_TempDelta, 0.f, 1.e+006f);
	DDX_Text(pDX, IDC_EDIT_TEMPERATURE_START, m_TempStart);
	DDV_MinMaxFloat(pDX, m_TempStart, 0.f, 1.e+006f);
	DDX_Text(pDX, IDC_EDIT_TEMPERATURE_STOP, m_TempStop);
	DDV_MinMaxFloat(pDX, m_TempStop, 0.f, 1.e+006f);
	DDX_Control(pDX, IDC_RADIO_VARIATION_SIMPLE, m_RadioVariationSimple);
	DDX_Control(pDX, IDC_RADIO_VARIATION_MODIFIED, m_RadioVariationModified);
	DDX_Control(pDX, IDC_RADIO_VARIATION_VARIABLE, m_RadioVariationVariable);
	DDX_Text(pDX, IDC_EDIT_KEEPSTEPS, m_KeepSteps);
	DDV_MinMaxInt(pDX, m_KeepSteps, 0, 2147483647);
	DDX_Check(pDX, IDC_CHECK_APPROXIMATE, m_ApproximateSolutions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSAVGeneticParameters, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogSAVGeneticParameters)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_BOLTZMANNSCALING, OnCheckBoltzmannscaling)
	ON_BN_CLICKED(IDC_CHECK_COSTS, OnCheckCosts)
	ON_BN_CLICKED(IDC_RADIO_VARIATION_MODIFIED, OnRadioVariationModified)
	ON_BN_CLICKED(IDC_RADIO_VARIATION_SIMPLE, OnRadioVariationSimple)
	ON_BN_CLICKED(IDC_RADIO_VARIATION_VARIABLE, OnRadioVariationVariable)
	ON_BN_CLICKED(IDC_CHECK_APPROXIMATE, OnCheckApproximate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSAVGeneticParameters message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_CostFile = fileDlg.GetPathName();
	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogSAVGeneticParameters::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(SAVGENETICREDUCER)) { 
		Message::Error("Algorithm is not a SAVGeneticReducer");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to a SAVGeneticReducer.
	m_Reducer = dynamic_cast(SAVGeneticReducer *, m_Algorithm.GetPointer());
		
	// Initialize field values.
	m_RadioVariationSimple.SetCheck(m_Reducer->GetVariation() == SAVGeneticReducer::VARIATION_SIMPLE);
	m_RadioVariationModified.SetCheck(m_Reducer->GetVariation() == SAVGeneticReducer::VARIATION_MODIFIED);
	m_RadioVariationVariable.SetCheck(m_Reducer->GetVariation() == SAVGeneticReducer::VARIATION_VARIABLE);
	m_LifetimeMin = m_Reducer->GetMinimumLifetime();
	m_LifetimeMax = m_Reducer->GetMaximumLifetime();

	m_BoltzmannScaling = m_Reducer->ScaleFitness() && m_Reducer->GetScaling() == SAVGeneticReducer::SCALING_BOLTZMANN;
	m_TempStart = m_Reducer->GetStartTemperature();
	m_TempStop = m_Reducer->GetStopTemperature();
	m_TempDelta = m_Reducer->GetDeltaTemperature();

	m_Bias = m_Reducer->GetBias();
	m_Costs = m_Reducer->UseCosts();
	m_CostFile = m_Reducer->GetCostFilename().GetBuffer();

	m_GenerationGap = m_Reducer->GetGenerationGap();
	m_StopFitness = m_Reducer->UseFitnessStopping();
	m_StopKeep = m_Reducer->UseKeepListStopping();

	m_PCrossover = m_Reducer->GetCrossoverProbability();
	m_PMutation = m_Reducer->GetMutationProbability();
	m_PInversion = m_Reducer->GetInversionProbability();

	m_NCrossover = m_Reducer->GetNoCrossoverPoints();
	m_NMutation = m_Reducer->GetNoMutations();
	m_NInversion = m_Reducer->GetNoTranspositions();

	m_Replacement = m_Reducer->ReplacementSampling();
	m_Elitism = m_Reducer->UseElitism();

	m_SizeKeep = m_Reducer->GetKeepSize();
	m_SizePopulation = m_Reducer->GetPopulationSize();

	m_Seed = m_Reducer->GetSeed();

	m_ApproximateSolutions = m_Reducer->ApproximateSolutions();
	m_HitFraction = m_Reducer->GetHittingFraction();
	m_KeepSteps = m_Reducer->GetNoKeepSteps();

	// Some things are not implemented yet.
	m_RadioVariationSimple.EnableWindow(FALSE);
	//m_CheckCosts.EnableWindow(FALSE);
	//m_CheckApproximateSolutions.EnableWindow(FALSE);

	// Initialize field states.
	m_EditLifetimeMin.EnableWindow(m_Reducer->GetVariation() == SAVGeneticReducer::VARIATION_VARIABLE);
	m_EditLifetimeMax.EnableWindow(m_Reducer->GetVariation() == SAVGeneticReducer::VARIATION_VARIABLE);
	m_EditTemperatureStart.EnableWindow(m_BoltzmannScaling);
	m_EditTemperatureStop.EnableWindow(m_BoltzmannScaling);
	m_EditTemperatureDelta.EnableWindow(m_BoltzmannScaling);
	m_EditCostFile.EnableWindow(m_Costs);
	m_ButtonBrowse.EnableWindow(m_Costs);
	m_EditHitFraction.EnableWindow(m_ApproximateSolutions);
	m_EditKeepSteps.EnableWindow(m_ApproximateSolutions);

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnCheckBoltzmannscaling() 
{
	UpdateData(TRUE);
	m_EditTemperatureStart.EnableWindow(m_BoltzmannScaling);
	m_EditTemperatureStop.EnableWindow(m_BoltzmannScaling);
	m_EditTemperatureDelta.EnableWindow(m_BoltzmannScaling);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnCheckCosts() 
{
	UpdateData(TRUE);
	m_EditCostFile.EnableWindow(m_Costs);
	m_ButtonBrowse.EnableWindow(m_Costs);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnRadioVariationModified() 
{
	m_EditLifetimeMin.EnableWindow(FALSE);
	m_EditLifetimeMax.EnableWindow(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnRadioVariationSimple() 
{
	m_EditLifetimeMin.EnableWindow(FALSE);
	m_EditLifetimeMax.EnableWindow(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnRadioVariationVariable() 
{
	m_EditLifetimeMin.EnableWindow(TRUE);
	m_EditLifetimeMax.EnableWindow(TRUE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnOK() 
{

	// Transfer data from UI fields to variables.
	UpdateData(TRUE);

	// Set parameters.
	if (m_RadioVariationSimple.GetCheck())
		m_Reducer->SetVariation(SAVGeneticReducer::VARIATION_SIMPLE);
	else if (m_RadioVariationModified.GetCheck())
		m_Reducer->SetVariation(SAVGeneticReducer::VARIATION_MODIFIED);
	else if (m_RadioVariationVariable.GetCheck())
		m_Reducer->SetVariation(SAVGeneticReducer::VARIATION_VARIABLE);

	m_Reducer->SetMinimumLifetime(m_LifetimeMin);
	m_Reducer->SetMaximumLifetime(m_LifetimeMax);

	m_Reducer->ScaleFitness(m_BoltzmannScaling);
	m_Reducer->SetScaling(SAVGeneticReducer::SCALING_BOLTZMANN);
	m_Reducer->SetStartTemperature(m_TempStart);
	m_Reducer->SetStopTemperature(m_TempStop);
	m_Reducer->SetDeltaTemperature(m_TempDelta);

	m_Reducer->SetBias(m_Bias);
	m_Reducer->UseCosts(m_Costs);
	m_Reducer->SetCostFilename(m_CostFile.GetBuffer(0));

	m_Reducer->SetGenerationGap(m_GenerationGap);
	m_Reducer->UseFitnessStopping(m_StopFitness);
	m_Reducer->UseKeepListStopping(m_StopKeep);

	m_Reducer->SetCrossoverProbability(m_PCrossover);
	m_Reducer->SetMutationProbability(m_PMutation);
	m_Reducer->SetInversionProbability(m_PInversion);
	m_Reducer->SetNoCrossoverPoints(m_NCrossover);
	m_Reducer->SetNoMutations(m_NMutation);
	m_Reducer->SetNoTranspositions(m_NInversion);

	m_Reducer->ReplacementSampling(m_Replacement);
	m_Reducer->UseElitism(m_Elitism);

	m_Reducer->SetPopulationSize(m_SizePopulation);
	m_Reducer->SetKeepSize(m_SizeKeep);

	m_Reducer->SetSeed(m_Seed);

	m_Reducer->ApproximateSolutions(m_ApproximateSolutions);
	m_Reducer->SetHittingFraction(m_HitFraction);
	m_Reducer->SetNoKeepSteps(m_KeepSteps);

	CDialogAlgorithm::OnOK();

}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogSAVGeneticParameters::OnCheckApproximate() 
{
	UpdateData(TRUE);
	m_EditHitFraction.EnableWindow(m_ApproximateSolutions);
	m_EditKeepSteps.EnableWindow(m_ApproximateSolutions);
}

 
