// dialogqualityrulefilterloop.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogqualityrulefilterloop.h>
#include <frontend\dialogs\algorithmdialogs\dialogqualityrulefilter.h>
#include <frontend\dialogs\algorithmdialogs\dialogstandardvoter.h>

#include <kernel\algorithms\qualityrulefilterloop.h>
#include <kernel\algorithms\standardvoter.h>

#include <common\objectmanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogQualityRuleFilterLoop dialog


CDialogQualityRuleFilterLoop::CDialogQualityRuleFilterLoop(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogQualityRuleFilterLoop::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogQualityRuleFilterLoop)
	m_Dynamic = 0.0f;
	m_Fallback = 0.0f;
	m_Filename = _T("");
	m_Fixed = 0;
	m_ResolutionLimit = 0;
	//}}AFX_DATA_INIT

	m_SelectedDT = 0;
	m_SelectedC  = 0;

}


void CDialogQualityRuleFilterLoop::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogQualityRuleFilterLoop)
	DDX_Control(pDX, IDC_BUTTON_CLASSIFICATION_PARAMETERS, m_ButtonClassifierParams);
	DDX_Control(pDX, IDC_EDIT_FIXED, m_EditFixed);
	DDX_Control(pDX, IDC_EDIT_DYNAMIC, m_EditDynamic);
	DDX_Control(pDX, IDC_COMBO_ROCFOCUS, m_ComboROCFocus);
	DDX_Control(pDX, IDC_COMBO_DECISIONTABLE, m_ComboDecisionTable);
	DDX_Control(pDX, IDC_COMBO_CLASSIFIER, m_ComboClassifier);
	DDX_Text(pDX, IDC_EDIT_DYNAMIC, m_Dynamic);
	DDV_MinMaxFloat(pDX, m_Dynamic, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_FALLBACK, m_Fallback);
	DDV_MinMaxFloat(pDX, m_Fallback, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	DDX_Text(pDX, IDC_EDIT_FIXED, m_Fixed);
	DDV_MinMaxInt(pDX, m_Fixed, 1, 2147483647);
	DDX_Control(pDX, IDC_RADIO_FIXED, m_RadioFixed);
	DDX_Control(pDX, IDC_RADIO_DYNAMIC, m_RadioDynamic);
	DDX_Text(pDX, IDC_EDIT_RESOLUTION_LIMIT, m_ResolutionLimit);
	DDV_MinMaxInt(pDX, m_ResolutionLimit, 1, 2147483647);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogQualityRuleFilterLoop, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogQualityRuleFilterLoop)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_CLASSIFICATION_PARAMETERS, OnButtonClassificationParameters)
	ON_BN_CLICKED(IDC_BUTTON_QMEASURE, OnButtonQmeasure)
	ON_CBN_SELCHANGE(IDC_COMBO_CLASSIFIER, OnSelchangeComboClassifier)
	ON_CBN_SELCHANGE(IDC_COMBO_DECISIONTABLE, OnSelchangeComboDecisiontable)
	ON_BN_CLICKED(IDC_RADIO_DYNAMIC, OnRadioDynamic)
	ON_BN_CLICKED(IDC_RADIO_FIXED, OnRadioFixed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogQualityRuleFilterLoop message handlers

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogQualityRuleFilterLoop::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
		
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(QUALITYRULEFILTERLOOP)) {
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

	Handle<QualityRuleFilterLoop> filter = dynamic_cast(QualityRuleFilterLoop *, m_Algorithm.GetPointer());

	// Are any classifiers available?
	if (ObjectManager::Count(VOTER) == 0) { // Hack. Generalize later on.
		Message::Error("No classifiers of right type available.");
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
		if (m_AvailableDT[i].GetPointer() == parentdt.GetPointer()) {
			parentdt_index = i;
			break;
		}
	}

	m_SelectedDT = parentdt_index;

	// Fill DT combo box.
	for (i = 0; i < m_AvailableDT.size(); i++) {
		String name = dynamic_cast(DecisionTable *, m_AvailableDT[i].GetPointer())->GetName();
		m_ComboDecisionTable.AddString(name.GetBuffer());
		if (m_AvailableDT[i].GetPointer() == filter->GetDecisionTable().GetPointer())
			m_SelectedDT = i;
	}

	m_ComboDecisionTable.SetCurSel(m_SelectedDT);

	int current_count = 0;

	m_SelectedC = 0;

	// Fill combo list with available classifiers.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (!algorithm->IsA(VOTER)) // Hack. Generalize later on.
			continue;
		m_AvailableC.push_back(algorithm.GetPointer());
		if (filter->GetClassifier() != NULL) {
			if (filter->GetClassifier()->GetId() == algorithm->GetId())
				m_SelectedC = current_count;
		}
		String description = IdHolder::GetDescription(algorithm->GetId());
		m_ComboClassifier.AddString(description.GetBuffer());
		current_count++;
	}

	m_ComboClassifier.SetCurSel(m_SelectedC);

	// Enable/disable parameter buttons.
	m_ButtonClassifierParams.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableC[m_SelectedC]->GetId()) != NULL);

	// Initialize logfile stuff.
	m_Filename = filter->GetFilename().GetBuffer();

	// Initialize resolution stuff.
	m_RadioFixed.SetCheck(filter->GetResolution()   == QualityRuleFilterLoop::RESOLUTION_FIXED);
	m_RadioDynamic.SetCheck(filter->GetResolution() == QualityRuleFilterLoop::RESOLUTION_DYNAMIC);

	m_Fixed   = filter->GetFixedResolution();
	m_Dynamic = filter->GetDynamicResolution();

	m_ResolutionLimit = filter->GetResolutionLimit();

	m_EditFixed.EnableWindow(m_RadioFixed.GetCheck());
	m_EditDynamic.EnableWindow(m_RadioDynamic.GetCheck());

	// Initialize ROC stuff.
	m_Fallback = filter->GetROCFallbackCertainty();
	UpdateROCCombo();

	// Update data.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::OnButtonBrowse() 
{

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_Filename = fullpath;
	}

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

void CDialogQualityRuleFilterLoop::OnButtonClassificationParameters() 
{

	Handle<Algorithm> selected = m_AvailableC[m_SelectedC];

	CDialogAlgorithm *dialog = ObjectManager::GetIdentifiedDialog(selected->GetId());

	if (dialog == NULL)
		return;

	dialog->SetAlgorithm(selected.GetPointer(), dynamic_cast(Structure *, m_AvailableDT[m_SelectedDT].GetPointer()), m_Project.GetPointer());

	BOOL oldER;
	BOOL oldEG;

	// Ugly, ugly hack.
	if (selected->IsA(VOTER)) {
		oldER = dynamic_cast(CDialogVoter *, dialog)->m_EnableRules;
		dynamic_cast(CDialogVoter *, dialog)->m_EnableRules = FALSE;
	}

	// Ugly, ugly hack.
	if (selected->IsA(STANDARDVOTER)) {
		oldEG = dynamic_cast(CDialogStandardVoter *, dialog)->m_EnableGeneralization;
		dynamic_cast(CDialogStandardVoter *, dialog)->m_EnableGeneralization = FALSE;
	}

	dialog->DoModal();

	// Ugly, ugly hack.
	if (selected->IsA(VOTER)) {
		dynamic_cast(CDialogVoter *, dialog)->m_EnableRules = oldER;
	}

	// Ugly, ugly hack.
	if (selected->IsA(STANDARDVOTER)) {
		dynamic_cast(CDialogStandardVoter *, dialog)->m_EnableGeneralization = oldEG;
	}
	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::OnButtonQmeasure() 
{

	CDialogQualityRuleFilter dialog;

	dialog.m_EnableOnlyQualitySelection = TRUE;
	dialog.m_EnableUndefinedSelection   = TRUE;

	dialog.m_Algorithm = m_Algorithm;
	dialog.m_Structure = m_Structure;
	dialog.m_Project   = m_Project;

	dialog.DoModal();

}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::OnSelchangeComboClassifier() 
{

	UpdateData(TRUE);

	// Determine which Classifier is selected.
	int selected = m_ComboClassifier.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedC = selected;

	m_ButtonClassifierParams.EnableWindow(ObjectManager::GetIdentifiedDialog(m_AvailableC[m_SelectedC]->GetId()) != NULL);
	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::OnSelchangeComboDecisiontable() 
{

	UpdateData(TRUE);

	// Determine which DT is selected.
	int selected = m_ComboDecisionTable.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_SelectedDT = selected;

	// Update ROC combo.
	UpdateROCCombo();
	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::OnRadioDynamic() 
{
	m_EditFixed.EnableWindow(FALSE);
	m_EditDynamic.EnableWindow(TRUE);
}

void CDialogQualityRuleFilterLoop::OnRadioFixed() 
{
	m_EditFixed.EnableWindow(TRUE);
	m_EditDynamic.EnableWindow(FALSE);	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::OnOK() 
{

	UpdateData(TRUE);

	Handle<QualityRuleFilterLoop> filter = dynamic_cast(QualityRuleFilterLoop *, m_Algorithm.GetPointer());

	filter->SetFilename(m_Filename.GetBuffer(0));

	if (m_RadioFixed.GetCheck())
		filter->SetResolution(QualityRuleFilterLoop::RESOLUTION_FIXED);
	else if (m_RadioDynamic.GetCheck())
		filter->SetResolution(QualityRuleFilterLoop::RESOLUTION_DYNAMIC);

	filter->SetFixedResolution(m_Fixed);
	filter->SetDynamicResolution(m_Dynamic);
	filter->SetResolutionLimit(m_ResolutionLimit);

	filter->SetDecisionTable(dynamic_cast(DecisionTable *, m_AvailableDT[m_SelectedDT].GetPointer()));
	filter->SetClassifier(dynamic_cast(Voter *, m_AvailableC[m_SelectedC].GetPointer())); // Hack. Generalize later on.

	CString classname;
	m_ComboROCFocus.GetWindowText(classname);

	filter->SetROCFocusClass(classname.GetBuffer(0));
	filter->SetROCFallbackCertainty(m_Fallback);
	
	CDialogAlgorithm::OnOK();

}

//------------------------------------------------------------------
// Method........: UpdateROCCombo
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogQualityRuleFilterLoop::UpdateROCCombo() {

	m_ComboROCFocus.ResetContent();

	Handle<DecisionTable>         table  = dynamic_cast(DecisionTable *,         m_AvailableDT[m_SelectedDT].GetPointer());
	Handle<QualityRuleFilterLoop> filter = dynamic_cast(QualityRuleFilterLoop *, m_Algorithm.GetPointer());

	bool masked = true;

	// Get index of decision attribute.
	int decisionattribute = table->GetDecisionAttribute(masked);

	if (decisionattribute == Undefined::Integer()) {
		Message::Error("Decision table has no decision attribute set.");
		return;
	}

	Vector(int) decisionvalues;
	Vector(int) frequencies; // Dummy, not in use.

	// Get value set.
	if (!table->GetValueSet(decisionvalues, frequencies, decisionattribute, masked)) {
		Message::Error("Failed to get value set of decision attribute.");
		return;
	}

	int roc_selected_if_none_other = 0;
	int i;

	// Get index of name that has code '1', if any.
	for (i = 0; i < decisionvalues.size(); i++) {
		if (decisionvalues[i] == 1) {
			roc_selected_if_none_other = i;
			break;
		}
	}

	int roc_selected = roc_selected_if_none_other;

	for (i = 0; i < decisionvalues.size(); i++) {
		String name = table->GetDictionaryEntry(decisionattribute, decisionvalues[i], masked);
		m_ComboROCFocus.AddString(name.GetBuffer());
		if (name == filter->GetROCFocusClass())
			roc_selected = i;
	}

	m_ComboROCFocus.SetCurSel(roc_selected);

}
 
