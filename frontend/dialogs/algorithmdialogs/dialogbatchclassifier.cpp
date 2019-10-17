// dialogbatchclassifier.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogbatchclassifier.h>
#include <frontend\dialogs\algorithmdialogs\dialogbatchclassifiercalres.h>

#include <kernel\algorithms\classifier.h>
#include <kernel\algorithms\batchclassifier.h>

#include <kernel\structures\decisiontable.h>

#include <kernel\basic\message.h>

#include <common\objectmanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogBatchClassifier dialog


CDialogBatchClassifier::CDialogBatchClassifier(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogBatchClassifier::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogBatchClassifier)
	m_PriThreshold = 0.0f;
	m_FBThresh = 0.0f;
	//}}AFX_DATA_INIT

	m_Selected          = 0;
	m_Masked            = TRUE;
	m_DecisionAttribute = Undefined::Integer();

}


void CDialogBatchClassifier::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBatchClassifier)
	DDX_Control(pDX, IDC_EDIT_FBTHRESH, m_EditFBThresh);
	DDX_Control(pDX, IDC_CHECK_FALLBACK, m_CheckFallback);
	DDX_Control(pDX, IDC_EDIT_PRITHRESH, m_EditPriThreshold);
	DDX_Control(pDX, IDC_COMBO_PRICLASS, m_ComboPriClass);
	DDX_Control(pDX, IDC_COMBO_FALLBACK, m_ComboFallback);
	DDX_Control(pDX, IDC_RADIO_BEST, m_RadioBest);
	DDX_Control(pDX, IDC_RADIO_PRIORITIZE, m_RadioPrioritize);
	DDX_Control(pDX, IDC_RADIO_REFRAIN, m_RadioRefrain);
	DDX_Control(pDX, IDC_CHECK_VERBOSE, m_CheckVerbose);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_CHECK_LOG, m_CheckLog);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Control(pDX, IDC_COMBO_CLASSIFIER, m_ComboClassifier);
	DDX_Control(pDX, IDC_BUTTON_PARAMETERS, m_ButtonParameters);
	DDX_Text(pDX, IDC_EDIT_PRITHRESH, m_PriThreshold);
	DDV_MinMaxFloat(pDX, m_PriThreshold, 0.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_FBTHRESH, m_FBThresh);
	DDV_MinMaxFloat(pDX, m_FBThresh, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogBatchClassifier, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogBatchClassifier)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETERS, OnButtonParameters)
	ON_CBN_SELCHANGE(IDC_COMBO_CLASSIFIER, OnSelchangeComboClassifier)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_LOG, OnCheckLog)
	ON_BN_CLICKED(IDC_CHECK_FALLBACK, OnCheckFallback)
	ON_BN_CLICKED(IDC_RADIO_BEST, OnRadioBest)
	ON_BN_CLICKED(IDC_RADIO_PRIORITIZE, OnRadioPrioritize)
	ON_BN_CLICKED(IDC_RADIO_REFRAIN, OnRadioRefrain)
	ON_BN_CLICKED(IDC_BUTTON_CALRES, OnButtonCalres)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogBatchClassifier message handlers

//-------------------------------------------------------------------
// Method........: OnButtonParameters
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnButtonParameters() 
{

	UpdateData(TRUE);

	Handle<Algorithm> selected = m_Available[m_Selected];

	CDialogAlgorithm *dialog = ObjectManager::GetIdentifiedDialog(selected->GetId());

	if (dialog == NULL)
		return;

	dialog->SetAlgorithm(selected.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog->DoModal();
	
}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogBatchClassifier::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(BATCHCLASSIFIER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE)) {
		Message::Error("Structure not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *, m_Algorithm.GetPointer());
	Handle<DecisionTable>   decisiontable   = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Are any classifier algorithms available?
	if (ObjectManager::Count(CLASSIFIER) == 0) {
		Message::Error("No classifier algorithms available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int current_count = 0;
	int i;
	
	// Fill combo list with available classifier algorithms.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		//if (!algorithm->IsA(CLASSIFIER))
		if (!algorithm->IsA(STANDARDVOTER))
			continue;
		m_Available.push_back(algorithm.GetPointer());
		if (batchclassifier->GetClassifier() != NULL) {
			if (batchclassifier->GetClassifier()->GetId() == algorithm->GetId())
				m_Selected = current_count;
		}
		String description = IdHolder::GetDescription(algorithm->GetId());
		m_ComboClassifier.AddString(description.GetBuffer());
		current_count++;
	}

	m_ComboClassifier.SetCurSel(m_Selected);

	// Get index of decision attribute.
	m_DecisionAttribute = decisiontable->GetDecisionAttribute(m_Masked);

	if (m_DecisionAttribute == Undefined::Integer()) {
		Message::Error("Decision table has no decision attribute set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Vector(int) frequencies; // Dummy, not in use.

	// Get value set.
	if (!decisiontable->GetValueSet(m_DecisionValues, frequencies, m_DecisionAttribute, m_Masked)) {
		Message::Error("Failed to get value set of decision attribute.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int fallback_selected_if_none_other = 0;

	// Get index of name that has code '1', if any.
	for (i = 0; i < m_DecisionValues.size(); i++) {
		if (m_DecisionValues[i] == 1) {
			fallback_selected_if_none_other = i;
			break;
		}
	}

	int fallback_selected = fallback_selected_if_none_other;

	// Initialize fallback stuff.
	for (i = 0; i < m_DecisionValues.size(); i++) {
		String name = decisiontable->GetDictionaryEntry(m_DecisionAttribute, m_DecisionValues[i], m_Masked);
		m_ComboFallback.AddString(name.GetBuffer());
		if (name == batchclassifier->GetFallbackClass())
			fallback_selected = i;
	}

	m_ComboFallback.SetCurSel(fallback_selected);

	m_FBThresh = batchclassifier->GetFallbackCertainty();

	m_CheckFallback.SetCheck(batchclassifier->AssignFallback());
	m_ComboFallback.EnableWindow(batchclassifier->AssignFallback());
	m_EditFBThresh.EnableWindow(batchclassifier->AssignFallback());

	// Initialize radio buttons.
	m_RadioBest.SetCheck(batchclassifier->GetMultipleStrategy()            == BatchClassifier::MULTIPLE_BEST);
	m_RadioPrioritize.SetCheck(batchclassifier->GetMultipleStrategy()      == BatchClassifier::MULTIPLE_PRIORITIZE);
	m_RadioRefrain.SetCheck(batchclassifier->GetMultipleStrategy()         == BatchClassifier::MULTIPLE_REFRAIN);

	int pri_selected = 0;

	// Initialize prioritization stuff.
	for (i = 0; i < m_DecisionValues.size(); i++) {
		String name = decisiontable->GetDictionaryEntry(m_DecisionAttribute, m_DecisionValues[i], m_Masked);
		m_ComboPriClass.AddString(name.GetBuffer());
		if (name == batchclassifier->GetPriorityClass())
			pri_selected = i;
	}

	m_ComboPriClass.SetCurSel(pri_selected);
	m_PriThreshold = batchclassifier->GetPriorityThreshold();

	m_ComboPriClass.EnableWindow(batchclassifier->GetMultipleStrategy()    == BatchClassifier::MULTIPLE_PRIORITIZE);
	m_EditPriThreshold.EnableWindow(batchclassifier->GetMultipleStrategy() == BatchClassifier::MULTIPLE_PRIORITIZE);

	// Log details.
	m_CheckLog.SetCheck(batchclassifier->LogDetails());
	m_EditFilename.SetWindowText((batchclassifier->GetLogFilename()).GetBuffer());
	m_CheckVerbose.SetCheck(batchclassifier->VerboseLog());

	m_EditFilename.EnableWindow(batchclassifier->LogDetails());
	m_ButtonBrowse.EnableWindow(batchclassifier->LogDetails());
	m_CheckVerbose.EnableWindow(batchclassifier->LogDetails());

	// Enable/disable parameter button.
	m_ButtonParameters.EnableWindow(ObjectManager::GetIdentifiedDialog(m_Available[m_Selected]->GetId()) != NULL);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboClassifier
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnSelchangeComboClassifier() 
{

	UpdateData(TRUE);

	// Determine which classifier is selected.
	int selected = m_ComboClassifier.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}

	m_Selected = selected;

	m_ButtonParameters.EnableWindow(ObjectManager::GetIdentifiedDialog(m_Available[m_Selected]->GetId()) != NULL);

}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================


void CDialogBatchClassifier::OnOK() 
{

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(BATCHCLASSIFIER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return;
	}

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE)) {
		Message::Error("Structure not present or of unexpected type.");
		EndDialog(IDABORT);
		return;
	}

	Handle<BatchClassifier> batchclassifier = dynamic_cast(BatchClassifier *, m_Algorithm.GetPointer());
	Handle<DecisionTable>   decisiontable   = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	// Get selected classifier.
	Handle<Classifier> classifier = dynamic_cast(Classifier *, m_Available[m_Selected].GetPointer());

	// Set selected classifier.
	if (!batchclassifier->SetClassifier(classifier.GetPointer())) {
		Message::Error("Error assigning single-object classifier to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	// Set fallback flag.
	if (!batchclassifier->AssignFallback(m_CheckFallback.GetCheck())) {
		Message::Error("Error assigning fallback flag to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	CString fallback_class;

	// Determine which fallback class is selected.
	int selected = m_ComboFallback.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}
	m_ComboFallback.GetLBText(selected, fallback_class);

	// Set selected fallback class.
	if (!batchclassifier->SetFallbackClass(fallback_class.GetBuffer(0))) {
		Message::Error("Error assigning fallback class to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	// Set selected fallback certainty threshold.
	if (!batchclassifier->SetFallbackCertainty(m_FBThresh)) {
		Message::Error("Error assigning fallback certainty threshold to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	BatchClassifier::MultipleStrategy strategy;

	// Get multiple strategy handling stuff.
	if (m_RadioBest.GetCheck())
		strategy = BatchClassifier::MULTIPLE_BEST;
	else if (m_RadioPrioritize.GetCheck())
		strategy = BatchClassifier::MULTIPLE_PRIORITIZE;
	else if (m_RadioRefrain.GetCheck())
		strategy = BatchClassifier::MULTIPLE_REFRAIN;
	else {
		Message::Error("Undefined multiple strategy.");
		EndDialog(IDABORT);
		return;
	}

	// Set multiple strategy handling stuff.
	if (!batchclassifier->SetMultipleStrategy(strategy)) {
		Message::Error("Error assigning multiple strategy to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	CString pri_class;

	// Determine which priority class is selected.
	selected = m_ComboPriClass.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}
	m_ComboPriClass.GetLBText(selected, pri_class);

	// Set selected priority class.
	if (!batchclassifier->SetPriorityClass(pri_class.GetBuffer(0))) {
		Message::Error("Error assigning priority class to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	// Set priority threshold.
	if (!batchclassifier->SetPriorityThreshold(m_PriThreshold)) {
		Message::Error("Error assigning prioritization threshold to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	CString filename;

	// Set log options.
	if (!batchclassifier->LogDetails(m_CheckLog.GetCheck())) {
		Message::Error("Error assigning log flag to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	m_EditFilename.GetWindowText(filename);

	if (!batchclassifier->SetLogFilename(filename.GetBuffer(0))) {
		Message::Error("Error assigning filename to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	if (!batchclassifier->VerboseLog(m_CheckVerbose.GetCheck())) {
		Message::Error("Error assigning verbose flag to batch classifier.");
		EndDialog(IDABORT);
		return;
	}
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnButtonBrowse() 
{
	
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilename.SetWindowText(fullpath);    
	}

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnCheckLog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnCheckLog() 
{

	UpdateData(TRUE);

	m_EditFilename.EnableWindow(m_CheckLog.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckLog.GetCheck());
	m_CheckVerbose.EnableWindow(m_CheckLog.GetCheck());

	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnCheckFallback
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnCheckFallback() 
{

	UpdateData(TRUE);

	m_ComboFallback.EnableWindow(m_CheckFallback.GetCheck());
	m_EditFBThresh.EnableWindow(m_CheckFallback.GetCheck());
	
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnRadioBest
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnRadioBest() 
{

	UpdateData(TRUE);

	m_ComboPriClass.EnableWindow(FALSE);
	m_EditPriThreshold.EnableWindow(FALSE);

	// Hack: The !#&%"%± resource should do this...
	m_RadioBest.SetCheck(TRUE);
	m_RadioPrioritize.SetCheck(FALSE);
	m_RadioRefrain.SetCheck(FALSE);

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnRadioPrioritize
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnRadioPrioritize() 
{

	UpdateData(TRUE);

	m_ComboPriClass.EnableWindow(TRUE);
	m_EditPriThreshold.EnableWindow(TRUE);
	
	// Hack: The !#&%"%± resource should do this...
	m_RadioBest.SetCheck(FALSE);
	m_RadioPrioritize.SetCheck(TRUE);
	m_RadioRefrain.SetCheck(FALSE);

	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnRadioRefrain
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnRadioRefrain() 
{

	UpdateData(TRUE);

	m_ComboPriClass.EnableWindow(FALSE);
	m_EditPriThreshold.EnableWindow(FALSE);
	
	// Hack: The !#&%"%± resource should do this...
	m_RadioBest.SetCheck(FALSE);
	m_RadioPrioritize.SetCheck(FALSE);
	m_RadioRefrain.SetCheck(TRUE);

	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnButtonCalres
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifier::OnButtonCalres() 
{

	UpdateData(TRUE);

	CDialogBatchClassifierCalRes dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();
	
}
 
