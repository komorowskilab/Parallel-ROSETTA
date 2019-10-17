// dialogreductperformancepruner.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogreductperformancepruner.h>

#include <kernel\algorithms\reductperformancefilter.h>
#include <kernel\algorithms\rulegenerator.h>
#include <kernel\algorithms\batchclassifier.h>

#include <common\objectmanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReductPerformancePruner dialog


CDialogReductPerformancePruner::CDialogReductPerformancePruner(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogReductPerformancePruner::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogReductPerformancePruner)
	m_Invert = FALSE;
	m_Index = 0;
	m_Threshold = 0.0f;
	m_Log = FALSE;
	m_Filename = _T("");
	//}}AFX_DATA_INIT

	m_SelectedRG   = 0;
	m_SelectedBC   = 0;
	m_SelectedDTRG = 0;
	m_SelectedDTBC = 0;

}


void CDialogReductPerformancePruner::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogReductPerformancePruner)
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Control(pDX, IDC_EDIT_INDEX, m_EditIndex);
	DDX_Control(pDX, IDC_COMBO_TABLE_RG, m_ComboTableRG);
	DDX_Control(pDX, IDC_COMBO_TABLE_BC, m_ComboTableBC);
	DDX_Control(pDX, IDC_COMBO_RG, m_ComboRG);
	DDX_Control(pDX, IDC_COMBO_BC, m_ComboBC);
	DDX_Control(pDX, IDC_BUTTON_RGPARAMS, m_ButtonRGParams);
	DDX_Control(pDX, IDC_BUTTON_BCPARAMS, m_ButtonBCParams);
	DDX_Control(pDX, IDC_RADIO_DIAGONAL, m_RadioDiagonal);
	DDX_Control(pDX, IDC_RADIO_ROW, m_RadioRow);
	DDX_Control(pDX, IDC_RADIO_COL, m_RadioCol);
	DDX_Check(pDX, IDC_CHECK_INVERT, m_Invert);
	DDX_Text(pDX, IDC_EDIT_INDEX, m_Index);
	DDV_MinMaxInt(pDX, m_Index, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_Threshold);
	DDX_Check(pDX, IDC_CHECK_LOGFILE, m_Log);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogReductPerformancePruner, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogReductPerformancePruner)
	ON_BN_CLICKED(IDC_BUTTON_BCPARAMS, OnButtonBcparams)
	ON_BN_CLICKED(IDC_BUTTON_RGPARAMS, OnButtonRgparams)
	ON_CBN_SELCHANGE(IDC_COMBO_BC, OnSelchangeComboBc)
	ON_CBN_SELCHANGE(IDC_COMBO_RG, OnSelchangeComboRg)
	ON_BN_CLICKED(IDC_RADIO_COL, OnRadioCol)
	ON_BN_CLICKED(IDC_RADIO_DIAGONAL, OnRadioDiagonal)
	ON_BN_CLICKED(IDC_RADIO_ROW, OnRadioRow)
	ON_CBN_SELCHANGE(IDC_COMBO_TABLE_RG, OnSelchangeComboRgTable)
	ON_CBN_SELCHANGE(IDC_COMBO_TABLE_BC, OnSelchangeComboBcTable)
	ON_BN_CLICKED(IDC_CHECK_LOGFILE, OnCheckLogfile)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogReductPerformancePruner message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogReductPerformancePruner::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REDUCTPERFORMANCEFILTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL()) {
		Message::Error("No structure set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Project.IsNULL()) {
		Message::Error("No project set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<ReductPerformanceFilter> filter = dynamic_cast(ReductPerformanceFilter *, m_Algorithm.GetPointer());

	// Are any rule generators available?
	if (ObjectManager::Count(RULEGENERATOR) == 0) {
		Message::Error("No rule generators available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Are any batch classifiers available?
	if (ObjectManager::Count(BATCHCLASSIFIER) == 0) {
		Message::Error("No rule generators available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Get parent DT, if any.
	Handle<Structure> parentdt = m_Structure->FindParent(DECISIONTABLE);

	// Acquire all available DTs.
	m_Project->GetAllIdentifiers(DECISIONTABLE, m_AvailableDT);

	if (m_AvailableDT.empty()) {
		Message::Error("No decision tables available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int parentdt_index = 0;
	int i;

	// Determine index of parent DT, if any.
	for (i = 0; i < m_AvailableDT.size(); i++) {
		if (m_AvailableDT[i].GetPointer() == parentdt.GetPointer())
			parentdt_index = i;
	}

	m_SelectedDTRG = parentdt_index;

	// Fill RG DT combo box.
	for (i = 0; i < m_AvailableDT.size(); i++) {
		String name = dynamic_cast(DecisionTable *, m_AvailableDT[i].GetPointer())->GetName();
		m_ComboTableRG.AddString(name.GetBuffer());
		if (m_AvailableDT[i].GetPointer() == filter->GetRGDecisionTable().GetPointer())
			m_SelectedDTRG = i;
	}

	m_ComboTableRG.SetCurSel(m_SelectedDTRG);

	m_SelectedDTBC = parentdt_index;

	// Fill BC DT combo box.
	for (i = 0; i < m_AvailableDT.size(); i++) {
		String name = dynamic_cast(DecisionTable *, m_AvailableDT[i].GetPointer())->GetName();
		m_ComboTableBC.AddString(name.GetBuffer());
		if (m_AvailableDT[i].GetPointer() == filter->GetBCDecisionTable().GetPointer())
			m_SelectedDTBC = i;
	}

	m_ComboTableBC.SetCurSel(m_SelectedDTBC);

	int current_count = 0;
	
	// Fill combo list with available rule generators.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (!algorithm->IsA(RULEGENERATOR))
			continue;
		m_AvailableRG.push_back(algorithm.GetPointer());
		if (filter->GetRuleGenerator() != NULL) {
			if (filter->GetRuleGenerator()->GetId() == algorithm->GetId())
				m_SelectedRG = current_count;
		}
		String description = IdHolder::GetDescription(algorithm->GetId());
		m_ComboRG.AddString(description.GetBuffer());
		current_count++;
	}

	m_ComboRG.SetCurSel(m_SelectedRG);

	current_count = 0;

	// Fill combo list with available batch classifiers.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (!algorithm->IsA(BATCHCLASSIFIER))
			continue;
		m_AvailableBC.push_back(algorithm.GetPointer());
		if (filter->GetBatchClassifier() != NULL) {
			if (filter->GetBatchClassifier()->GetId() == algorithm->GetId())
				m_SelectedBC = current_count;
		}
		String description = IdHolder::GetDescription(algorithm->GetId());
		m_ComboBC.AddString(description.GetBuffer());
		current_count++;
	}

	m_ComboBC.SetCurSel(m_SelectedBC);

	// Enable/disable parameter buttons.
	m_ButtonRGParams.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableRG[m_SelectedRG]->GetId()) != NULL);
	m_ButtonBCParams.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableBC[m_SelectedBC]->GetId()) != NULL);

	// Initialize radio buttons.
	m_RadioDiagonal.SetCheck(filter->GetCriterion() == ReductPerformanceFilter::CRITERION_DIAGONAL);
	m_RadioRow.SetCheck(filter->GetCriterion() == ReductPerformanceFilter::CRITERION_ROW);
	m_RadioCol.SetCheck(filter->GetCriterion() == ReductPerformanceFilter::CRITERION_COLUMN);

	// Initialize threshold field.
	m_Threshold = filter->GetThreshold();

	// Initialize index field.
	m_Index = filter->GetIndex();

	// Enable/disable index field.
	m_EditIndex.EnableWindow(filter->GetCriterion() != ReductPerformanceFilter::CRITERION_DIAGONAL);

	// Initialize logfile stuff.
	m_Log      = filter->GetFilename() != Undefined::String();
	m_Filename = filter->GetFilename().GetBuffer();

	m_ButtonBrowse.EnableWindow(m_Log);
	m_EditFilename.EnableWindow(m_Log);

	// Initialize invert flag.
	m_Invert = filter->InvertLogic();

	// Update data.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnOK() 
{

	// Update data.
	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REDUCTPERFORMANCEFILTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return;
	}

	Handle<ReductPerformanceFilter> filter = dynamic_cast(ReductPerformanceFilter *, m_Algorithm.GetPointer());

	// Set RG decision table.
	if (!filter->SetRGDecisionTable(dynamic_cast(DecisionTable *, m_AvailableDT[m_SelectedDTRG].GetPointer()))) {
		Message::Error("Error assigning RG decision table to performance filter.");
		EndDialog(IDABORT);
		return;
	}

	// Set BC decision table.
	if (!filter->SetBCDecisionTable(dynamic_cast(DecisionTable *, m_AvailableDT[m_SelectedDTBC].GetPointer()))) {
		Message::Error("Error assigning BC decision table to performance filter.");
		EndDialog(IDABORT);
		return;
	}

	// Set RG algorithm.
	if (!filter->SetRuleGenerator(dynamic_cast(RuleGenerator *, m_AvailableRG[m_SelectedRG].GetPointer()))) {
		Message::Error("Error assigning rule generator to performance filter.");
		EndDialog(IDABORT);
		return;
	}

	// Set BC algorithm.
	if (!filter->SetBatchClassifier(dynamic_cast(BatchClassifier *, m_AvailableBC[m_SelectedBC].GetPointer()))) {
		Message::Error("Error assigning batch classifier to performance filter.");
		EndDialog(IDABORT);
		return;
	}

	// Set criterion.
	if (m_RadioDiagonal.GetCheck())
		filter->SetCriterion(ReductPerformanceFilter::CRITERION_DIAGONAL);
	else if (m_RadioRow.GetCheck())
		filter->SetCriterion(ReductPerformanceFilter::CRITERION_ROW);
	else if (m_RadioCol.GetCheck())
		filter->SetCriterion(ReductPerformanceFilter::CRITERION_COLUMN);

	// Set index.
	if (!filter->SetIndex(m_Index)) {
		Message::Error("Error assigning row/column index to performance filter.");
		EndDialog(IDABORT);
		return;
	}

	// Set threshold.
	if (!filter->SetThreshold(m_Threshold)) {
		Message::Error("Error assigning threshold to performance filter.");
		EndDialog(IDABORT);
		return;
	}

	// Set logfile stuff.
	if (!m_Log)
		filter->SetFilename(Undefined::String());
	else
		filter->SetFilename(m_Filename.GetBuffer(0));

	// Set invert flag
	if (!filter->InvertLogic(m_Invert)) {
		Message::Error("Error assigning invert flag to performance filter.");
		EndDialog(IDABORT);
		return;
	}
	
	// Erase vectors (built in OnInitDialog) so as to not keep
	// any objects "alive" unnecessarily.
	m_AvailableBC.erase(m_AvailableBC.begin(), m_AvailableBC.end());
	m_AvailableRG.erase(m_AvailableRG.begin(), m_AvailableRG.end());
	m_AvailableDT.erase(m_AvailableDT.begin(), m_AvailableDT.end());

	m_SelectedBC   = 0;
	m_SelectedRG   = 0;
	m_SelectedDTRG = 0;
	m_SelectedDTBC = 0;

	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnButtonBcparams
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnButtonBcparams() 
{

	Handle<Algorithm> selected = m_AvailableBC[m_SelectedBC];

	CDialogAlgorithm *dialog = ObjectManager::GetIdentifiedDialog(selected->GetId());

	if (dialog == NULL)
		return;

	dialog->SetAlgorithm(selected.GetPointer(), dynamic_cast(Structure *, m_AvailableDT[m_SelectedDTBC].GetPointer()), m_Project.GetPointer());

	dialog->DoModal();
	
}

//-------------------------------------------------------------------
// Method........: OnButtonRgparams
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnButtonRgparams() 
{

	Handle<Algorithm> selected = m_AvailableRG[m_SelectedRG];

	CDialogAlgorithm *dialog = ObjectManager::GetIdentifiedDialog(selected->GetId());

	if (dialog == NULL)
		return;

	dialog->SetAlgorithm(selected.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog->DoModal();
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboRgTable
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================


void CDialogReductPerformancePruner::OnSelchangeComboRgTable() 
{

	UpdateData(TRUE);

	// Determine which DT is selected.
	int selected = m_ComboTableRG.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedDTRG = selected;
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboBcTable
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================


void CDialogReductPerformancePruner::OnSelchangeComboBcTable() 
{

	UpdateData(TRUE);

	// Determine which DT is selected.
	int selected = m_ComboTableBC.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedDTBC = selected;
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboBc
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnSelchangeComboBc() 
{

	UpdateData(TRUE);

	// Determine which BC  is selected.
	int selected = m_ComboBC.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedBC = selected;

	m_ButtonBCParams.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableBC[m_SelectedBC]->GetId()) != NULL);
	
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboRg
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnSelchangeComboRg() 
{

	UpdateData(TRUE);

	// Determine which RG is selected.
	int selected = m_ComboRG.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedRG = selected;

	m_ButtonRGParams.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableRG[m_SelectedRG]->GetId()) != NULL);
	
}

//-------------------------------------------------------------------
// Method........: OnRadioCol
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnRadioCol() 
{

	m_EditIndex.EnableWindow(TRUE);
	
}

//-------------------------------------------------------------------
// Method........: OnRadioDiagonal
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnRadioDiagonal() 
{

	m_EditIndex.EnableWindow(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnRadioRow
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnRadioRow() 
{

	m_EditIndex.EnableWindow(TRUE);
	
}

//-------------------------------------------------------------------
// Method........: OnCheckLogfile
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnCheckLogfile() 
{

	UpdateData(TRUE);

	m_ButtonBrowse.EnableWindow(m_Log);
	m_EditFilename.EnableWindow(m_Log);

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductPerformancePruner::OnButtonBrowse() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_Filename = fullpath;
		m_EditFilename.SetWindowText(m_Filename);
	}
	
}
 
