// dialogdynamicreducer.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogdynamicreducer.h>

#include <kernel\algorithms\dynamicreducer.h>

#include <common\objectmanager.h>
#include <kernel\utilities\mathkit.h>

//#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogDynamicReducer dialog


CDialogDynamicReducer::CDialogDynamicReducer(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogDynamicReducer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDynamicReducer)
	m_Seed = 0;
	//}}AFX_DATA_INIT

	m_SelectedRed = 0;
}


void CDialogDynamicReducer::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDynamicReducer)
	DDX_Control(pDX, IDC_SPIN_SEED, m_SpinSeed);
	DDX_Control(pDX, IDC_SPIN_SAMPLES, m_SpinSamples);
	DDX_Control(pDX, IDC_SPIN_LEVELS, m_SpinLevels);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD, m_CSliderThreshold);
	DDX_Control(pDX, IDC_SLIDER_UPPER_PERCENTAGE, m_CSliderUpperPercentage);
	DDX_Control(pDX, IDC_SLIDER_LOWER_PERCENTAGE, m_CSliderLowerPercentage);
	DDX_Control(pDX, IDC_PARAMETERS, m_Parameters);
	DDX_Control(pDX, IDC_COMBO_REDUCER, m_CReducer);
	DDX_Text(pDX, IDC_EDIT_LEVELS, m_Levels);
	DDX_Text(pDX, IDC_EDIT_LOWER_PERCENTAGE, m_LowerPercentage);
	DDX_Text(pDX, IDC_EDIT_SAMPLES, m_Samples);
	DDX_Text(pDX, IDC_EDIT_UPPER_PERCENTAGE, m_UpperPercentage);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_Threshold);
	DDX_Check(pDX, IDC_CHECK_GENERALIZED, m_CheckGeneralized);
	DDX_Text(pDX, IDC_EDIT_SEED, m_Seed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDynamicReducer, CDialog)
	//{{AFX_MSG_MAP(CDialogDynamicReducer)
	ON_BN_CLICKED(IDC_PARAMETERS, OnParameters)
	ON_CBN_SELCHANGE(IDC_COMBO_REDUCER, OnSelchangeComboReducer)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LOWER_PERCENTAGE, OnCustomdrawSliderLowerPercentage)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_UPPER_PERCENTAGE, OnCustomdrawSliderUpperPercentage)
	ON_EN_UPDATE(IDC_EDIT_LOWER_PERCENTAGE, OnUpdateEditLowerPercentage)
	ON_EN_UPDATE(IDC_EDIT_UPPER_PERCENTAGE, OnUpdateEditUpperPercentage)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THRESHOLD, OnCustomdrawSliderThreshold)
	ON_EN_UPDATE(IDC_EDIT_THRESHOLD, OnUpdateEditThreshold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDynamicReducer message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogDynamicReducer::OnInitDialog() 
{	
	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(DYNAMICREDUCER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<DynamicReducer> dynamicreducer = dynamic_cast(DynamicReducer *, m_Algorithm.GetPointer());

	
	// Are any non-dynamic reducers available?
	if (ObjectManager::Count(REDUCER) - 1 /*The dynamic reducer*/ == 0) {
		Message::Error("No non-dynamic reducers available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int current_count = 0;
	int i;

	// Fill combo list with available reducers.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (!algorithm->IsA(REDUCER) || algorithm->IsA(DYNAMICREDUCER))
			continue;
		m_AvailableRed.push_back(algorithm.GetPointer());
		if (dynamicreducer->GetReducer() != NULL) {
			if (dynamicreducer->GetReducer()->GetId() == algorithm->GetId())
				m_SelectedRed = current_count;
		}
		String description = IdHolder::GetDescription(algorithm->GetId());
		m_CReducer.AddString(description.GetBuffer());
		current_count++;
	}

	m_CReducer.SetCurSel(m_SelectedRed);

	// Enable/disable parameter buttons.
	m_Parameters.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableRed[m_SelectedRed]->GetId()) != NULL);

	// Initialize data fields.
	m_Levels			= dynamicreducer->GetNoLevels();
	m_Samples			= dynamicreducer->GetNoSamples();
	m_LowerPercentage   = dynamicreducer->GetLowerPercentage();
	m_UpperPercentage   = dynamicreducer->GetUpperPercentage();
	m_Seed				= dynamicreducer->GetSeed();
	m_CheckGeneralized	= dynamicreducer->Generalized();
	m_Threshold			= dynamicreducer->GetThreshold();

	// Initialize sliders
	m_CSliderUpperPercentage.SetRange(1,100);
	m_CSliderLowerPercentage.SetRange(1,100);
	m_CSliderUpperPercentage.SetPos(m_UpperPercentage);
	m_CSliderLowerPercentage.SetPos(m_LowerPercentage);

	m_CSliderThreshold.SetRange(0,100);
	m_CSliderThreshold.SetPos(MathKit::Round(m_Threshold * (float)m_CSliderThreshold.GetRangeMax()));

	// Initialize spins
	m_SpinLevels.SetRange(1,1000);
	m_SpinLevels.SetPos(m_Levels);
	m_SpinSamples.SetRange(1,1000);
	m_SpinSamples.SetPos(m_Samples);
	m_SpinSeed.SetRange32(0,100000);
	m_SpinSeed.SetPos(m_Seed);

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnOK() 
{
	// Update data.
	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(DYNAMICREDUCER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return;
	}

	Handle<DynamicReducer> dynamicreducer = dynamic_cast(DynamicReducer *, m_Algorithm.GetPointer());

	// Set reducer.
	if (!dynamicreducer->SetReducer(dynamic_cast(Reducer *, m_AvailableRed[m_SelectedRed].GetPointer()))) {
		Message::Error("Error assigning non-dynamic reducer to dynamic reducer.");
		EndDialog(IDABORT);
		return;
	}

	// Set parameters.
	dynamicreducer->SetNoLevels(m_Levels);
	dynamicreducer->SetNoSamples(m_Samples);
	dynamicreducer->SetLowerPercentage(m_LowerPercentage);
	dynamicreducer->SetUpperPercentage(m_UpperPercentage);
	dynamicreducer->SetSeed(m_Seed);
	dynamicreducer->SetThreshold(m_Threshold);
	dynamicreducer->Generalized(m_CheckGeneralized);

	// Erase vector (built in OnInitDialog) so as to not keep
	// any objects "alive" unnecessarily.
	m_AvailableRed.erase(m_AvailableRed.begin(), m_AvailableRed.end());

	m_SelectedRed = 0;

	CDialogAlgorithm::OnOK();
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnSelchangeComboReducer() 
{
	UpdateData(TRUE);

	// Determine which reducer is selected.
	int selected = m_CReducer.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedRed = selected;
	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnParameters() 
{
	Handle<Algorithm> selected = m_AvailableRed[m_SelectedRed];

	CDialogAlgorithm *dialog = ObjectManager::GetIdentifiedDialog(selected->GetId());

	if (dialog == NULL)
		return;

	dialog->SetAlgorithm(selected.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog->DoModal();
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnCustomdrawSliderLowerPercentage(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	m_LowerPercentage = m_CSliderLowerPercentage.GetPos();

	if (m_LowerPercentage > m_UpperPercentage) {
		m_UpperPercentage = m_LowerPercentage;
		m_CSliderUpperPercentage.SetPos(m_LowerPercentage);
	}

	*pResult = 0;

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnCustomdrawSliderUpperPercentage(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_UpperPercentage = m_CSliderUpperPercentage.GetPos();

	if (m_UpperPercentage < m_LowerPercentage) {
		m_LowerPercentage = m_UpperPercentage;
		m_CSliderLowerPercentage.SetPos(m_UpperPercentage);
	}

	*pResult = 0;

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnUpdateEditLowerPercentage() 
{
	UpdateData(TRUE);
	m_CSliderLowerPercentage.SetPos(m_LowerPercentage);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnUpdateEditUpperPercentage() 
{
	UpdateData(TRUE);
	m_CSliderUpperPercentage.SetPos(m_UpperPercentage);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnCustomdrawSliderThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	float thres = (float)m_CSliderThreshold.GetPos() / (float)m_CSliderThreshold.GetRangeMax();
	
	if (thres < 0)
		thres = 0.0f;

	if (thres > 1)
		thres = 1.0f;

	m_Threshold = thres;

	*pResult = 0;

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogDynamicReducer::OnUpdateEditThreshold() 
{
	UpdateData(TRUE);
	m_CSliderThreshold.SetPos(MathKit::Round(m_Threshold * (float)m_CSliderThreshold.GetRangeMax()));
}

 
