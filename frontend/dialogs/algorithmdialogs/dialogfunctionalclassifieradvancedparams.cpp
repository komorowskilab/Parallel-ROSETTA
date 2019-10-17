// dialogfunctionalclassifieradvancedparams.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>
#include <frontend\dialogs\algorithmdialogs\dialogfunctionalclassifieradvancedparams.h>
#include <frontend\dialogs\algorithmdialogs\dialogreducer.h>
#include <kernel\genomics\algorithms\functionalclassifier.h>
#include <common\objectmanager.h>
#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifierAdvancedParams dialog


CDialogFunctionalClassifierAdvancedParams::CDialogFunctionalClassifierAdvancedParams(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogFunctionalClassifierAdvancedParams::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFunctionalClassifierAdvancedParams)
	//}}AFX_DATA_INIT

	m_SelectedRed = 0;
}


void CDialogFunctionalClassifierAdvancedParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFunctionalClassifierAdvancedParams)
	DDX_Control(pDX, IDC_COMBO_REDUCER, m_CReducer);
	DDX_Control(pDX, IDC_SLIDER_PRUNE_COV, m_SliderPruneCov);
	DDX_Control(pDX, IDC_SLIDER_PRUNE_ACC, m_SliderPruneAcc);
	DDX_Control(pDX, IDC_PARAMETERS, m_Parameters);
	DDX_Control(pDX, IDC_ACC, m_Acc);
	DDX_Control(pDX, IDC_COV, m_Cov);
	DDX_Control(pDX, IDC_EDIT_PRUNE_COV, m_CEditPruneCov);
	DDX_Control(pDX, IDC_EDIT_PRUNE_ACC, m_CEditPruneAcc);
	DDX_Text(pDX, IDC_EDIT_WEIGTH, m_Weight);
	DDX_Check(pDX, IDC_CHECK_PRUNING, m_CheckPruning);
	DDX_Text(pDX, IDC_EDIT_PRUNE_ACC, m_EditPruneAcc);
	DDX_Text(pDX, IDC_EDIT_PRUNE_COV, m_EditPruneCov);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogFunctionalClassifierAdvancedParams, CDialog)
	//{{AFX_MSG_MAP(CDialogFunctionalClassifierAdvancedParams)
	ON_BN_CLICKED(IDC_CHECK_PRUNING, OnCheckPruning)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRUNE_ACC, OnCustomdrawSliderPruneAcc)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRUNE_COV, OnCustomdrawSliderPruneCov)
	ON_BN_CLICKED(IDC_PARAMETERS, OnParameters)
	ON_CBN_SELCHANGE(IDC_COMBO_REDUCER, OnSelchangeComboReducer)
	ON_EN_UPDATE(IDC_EDIT_PRUNE_ACC, OnUpdateEditPruneAcc)
	ON_EN_UPDATE(IDC_EDIT_PRUNE_COV, OnUpdateEditPruneCov)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifierAdvancedParams message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierAdvancedParams::OnCheckPruning() 
{
	UpdateData(TRUE);

	bool enable = m_CheckPruning;

	m_CEditPruneAcc.EnableWindow(enable);
	m_CEditPruneCov.EnableWindow(enable);
	m_Acc.EnableWindow(enable);
	m_Cov.EnableWindow(enable);

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

void CDialogFunctionalClassifierAdvancedParams::OnOK() 
{

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(FUNCTIONALCLASSIFIER)) {
		Message::Error("Algorithm not present or not a FunctionalClassifier.");
		EndDialog(IDABORT);
		return;
	}

	Handle <FunctionalClassifier> executor = dynamic_cast(FunctionalClassifier*, m_Algorithm.GetPointer());

	UpdateData(TRUE);

	// Set reducer 
	if (!executor->SetReducer(dynamic_cast(Reducer *, m_AvailableRed[m_SelectedRed].GetPointer()))) {
		Message::Error("Error assigning reducer to functional classifier.");
		EndDialog(IDABORT);
		return;
	}

	// Weight on false positives:
	executor->SetWeight(m_Weight);

	//Rule pruning
	executor->SetRulePruning(m_CheckPruning);
	executor->SetRulePruningAcc(m_EditPruneAcc);
	executor->SetRulePruningCov(m_EditPruneCov);
	
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

BOOL CDialogFunctionalClassifierAdvancedParams::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();

	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(FUNCTIONALCLASSIFIER)) { 
		Message::Error("Algorithm is not a FunctionalClassifier");
		CDialog::OnCancel();
		return FALSE;
	}

	// Cast to verified type
	Handle<FunctionalClassifier> alg = dynamic_cast(FunctionalClassifier*, m_Algorithm.GetPointer());

	// Are any non-dynamic reducers available?
	if (ObjectManager::Count(REDUCER) == 0) {
		Message::Error("No reducers available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int current_count = 0;
	int i;

	// Fill combo list with available reducers.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (!algorithm->IsA(REDUCER))
			continue;
		// Check if needed parameter settings are available
		if (!algorithm->SetParameters("DISCERNIBILITY=Object; MODULO.DECISION=T; SELECTION=All", false))
			continue;
		m_AvailableRed.push_back(algorithm.GetPointer());
		if (alg->GetReducer() != NULL) {
			if (alg->GetReducer()->GetId() == algorithm->GetId())
				m_SelectedRed = current_count;
		}
		String description = IdHolder::GetDescription(algorithm->GetId());
		m_CReducer.AddString(description.GetBuffer());
		current_count++;
	}

	m_CReducer.SetCurSel(m_SelectedRed);

	// Enable parameters button?
	m_Parameters.EnableWindow(ObjectManager::HasIdentifiedDialog(m_AvailableRed[m_SelectedRed]->GetId()));

	// Init values
	m_CheckPruning = alg->GetRulePruning();
	m_EditPruneAcc = alg->GetRulePruningAcc();
	m_EditPruneCov = alg->GetRulePruningCov();	
	m_Weight = alg->GetWeight();

	m_SliderPruneCov.SetRange(0,100);
	m_SliderPruneCov.SetPos(MathKit::Round(m_EditPruneCov * static_cast(float, m_SliderPruneCov.GetRangeMax())));
	m_SliderPruneAcc.SetRange(0,100);
	m_SliderPruneAcc.SetPos(MathKit::Round(m_EditPruneAcc * static_cast(float, m_SliderPruneAcc.GetRangeMax())));

	// Enable windows
	m_CEditPruneAcc.EnableWindow(m_CheckPruning);
	m_CEditPruneCov.EnableWindow(m_CheckPruning);
	m_Acc.EnableWindow(m_CheckPruning);
	m_Cov.EnableWindow(m_CheckPruning);

	// Update data fields.
	UpdateData(FALSE);

	return TRUE;
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierAdvancedParams::OnCustomdrawSliderPruneAcc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	float acc = static_cast(float, m_SliderPruneAcc.GetPos()) / static_cast(float, m_SliderPruneAcc.GetRangeMax());
	
	if (acc < 0)
		acc = 0.0f;

	if (acc > 1)
		acc = 1.0f;

	m_EditPruneAcc = acc;

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

void CDialogFunctionalClassifierAdvancedParams::OnCustomdrawSliderPruneCov(NMHDR* pNMHDR, LRESULT* pResult) 
{
	float cov = static_cast(float, m_SliderPruneCov.GetPos()) / static_cast(float, m_SliderPruneCov.GetRangeMax());
	
	if (cov < 0)
		cov = 0.0f;

	if (cov > 1)
		cov = 1.0f;

	m_EditPruneCov = cov;

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

void CDialogFunctionalClassifierAdvancedParams::OnParameters() 
{
	Handle<Algorithm> selected = m_AvailableRed[m_SelectedRed];

	CDialogAlgorithm* dialog = ObjectManager::GetIdentifiedDialog(selected->GetId());

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

void CDialogFunctionalClassifierAdvancedParams::OnSelchangeComboReducer() 
{
	UpdateData(TRUE);

	// Determine which reducer is selected.
	int selected = m_CReducer.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedRed = selected;	

	m_Parameters.EnableWindow(ObjectManager::HasIdentifiedDialog(m_AvailableRed[m_SelectedRed]->GetId()));
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierAdvancedParams::OnUpdateEditPruneAcc() 
{
	UpdateData(TRUE);
	m_SliderPruneAcc.SetPos(MathKit::Round(m_EditPruneAcc * static_cast(float, m_SliderPruneAcc.GetRangeMax())));
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifierAdvancedParams::OnUpdateEditPruneCov() 
{
	UpdateData(TRUE);
	m_SliderPruneCov.SetPos(MathKit::Round(m_EditPruneCov * static_cast(float, m_SliderPruneCov.GetRangeMax())));

}
 
