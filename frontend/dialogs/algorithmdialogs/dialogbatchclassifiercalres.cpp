// dialogbatchclassifiercalres.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogbatchclassifiercalres.h>

#include <kernel\algorithms\batchclassifier.h>

#include <kernel\structures\decisiontable.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogBatchClassifierCalRes dialog


CDialogBatchClassifierCalRes::CDialogBatchClassifierCalRes(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogBatchClassifierCalRes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogBatchClassifierCalRes)
	m_Groups = 0;
	//}}AFX_DATA_INIT
}


void CDialogBatchClassifierCalRes::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBatchClassifierCalRes)
	DDX_Control(pDX, IDC_EDIT_GROUPS, m_EditGroups);
	DDX_Control(pDX, IDC_EDIT_FILENAME_ROC, m_EditFilenameROC);
	DDX_Control(pDX, IDC_EDIT_FILENAME_CAL, m_EditFilenameCal);
	DDX_Control(pDX, IDC_COMBO_ROCCLASS, m_ComboROCClass);
	DDX_Control(pDX, IDC_COMBO_CALCLASS, m_ComboCalClass);
	DDX_Control(pDX, IDC_CHECK_ROC, m_CheckROC);
	DDX_Control(pDX, IDC_CHECK_CALIBRATION, m_CheckCal);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_ROC, m_ButtonBrowseROC);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_CAL, m_ButtonBrowseCal);
	DDX_Control(pDX, IDC_BUTTON_MAXIMIZE, m_ButtonMaximize);
	DDX_Text(pDX, IDC_EDIT_GROUPS, m_Groups);
	DDV_MinMaxInt(pDX, m_Groups, 1, 2147483647);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogBatchClassifierCalRes, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogBatchClassifierCalRes)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_CAL, OnButtonBrowseCal)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_ROC, OnButtonBrowseRoc)
	ON_BN_CLICKED(IDC_CHECK_CALIBRATION, OnCheckCalibration)
	ON_BN_CLICKED(IDC_CHECK_ROC, OnCheckRoc)
	ON_BN_CLICKED(IDC_BUTTON_MAXIMIZE, OnButtonMaximize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogBatchClassifierCalRes message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogBatchClassifierCalRes::OnInitDialog() 
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

	bool masked = true;

	// Get index of decision attribute.
	int decisionattribute = decisiontable->GetDecisionAttribute(masked);

	if (decisionattribute == Undefined::Integer()) {
		Message::Error("Decision table has no decision attribute set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	Vector(int) decisionvalues;
	Vector(int) frequencies; // Dummy, not in use.

	// Get value set.
	if (!decisiontable->GetValueSet(decisionvalues, frequencies, decisionattribute, masked)) {
		Message::Error("Failed to get value set of decision attribute.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int selected_class_if_none_other = 0;
	int i;

	// Get index of decision class that has code '1', if any.
	for (i = 0; i < decisionvalues.size(); i++) {
		if (decisionvalues[i] == 1) {
			selected_class_if_none_other = i;
			break;
		}
	}

	// Initialize calibration stuff.
	m_CheckCal.SetCheck(batchclassifier->GenerateCalibration());

	int cal_selected = selected_class_if_none_other;

	for (i = 0; i < decisionvalues.size(); i++) {
		String name = decisiontable->GetDictionaryEntry(decisionattribute, decisionvalues[i], masked);
		m_ComboCalClass.AddString(name.GetBuffer());
		if (name == batchclassifier->GetCalibrationClass())
			cal_selected = i;
	}

	m_ComboCalClass.SetCurSel(cal_selected);
	m_EditFilenameCal.SetWindowText((batchclassifier->GetCalibrationFilename()).GetBuffer());
	m_Groups = batchclassifier->GetNoCalibrationGroups();

	m_ComboCalClass.EnableWindow(batchclassifier->GenerateCalibration());
	m_EditFilenameCal.EnableWindow(batchclassifier->GenerateCalibration());
	m_ButtonBrowseCal.EnableWindow(batchclassifier->GenerateCalibration());
	m_ButtonMaximize.EnableWindow(batchclassifier->GenerateCalibration());
	m_EditGroups.EnableWindow(batchclassifier->GenerateCalibration());

	// Initialize ROC stuff.
	m_CheckROC.SetCheck(batchclassifier->GenerateROC());

	int roc_selected = selected_class_if_none_other;

	for (i = 0; i < decisionvalues.size(); i++) {
		String name = decisiontable->GetDictionaryEntry(decisionattribute, decisionvalues[i], masked);
		m_ComboROCClass.AddString(name.GetBuffer());
		if (name == batchclassifier->GetROCClass())
			roc_selected = i;
	}

	m_ComboROCClass.SetCurSel(roc_selected);
	m_EditFilenameROC.SetWindowText((batchclassifier->GetROCFilename()).GetBuffer());

	m_ComboROCClass.EnableWindow(batchclassifier->GenerateROC());
	m_EditFilenameROC.EnableWindow(batchclassifier->GenerateROC());
	m_ButtonBrowseROC.EnableWindow(batchclassifier->GenerateROC());

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifierCalRes::OnButtonBrowseCal() 
{

	UpdateData(TRUE);
	
	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilenameCal.SetWindowText(fullpath);    
	}

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifierCalRes::OnButtonBrowseRoc() 
{

	UpdateData(TRUE);
	
	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilenameROC.SetWindowText(fullpath);    
	}

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifierCalRes::OnCheckCalibration() 
{
 
	UpdateData(TRUE);

	m_ComboCalClass.EnableWindow(m_CheckCal.GetCheck());
	m_EditFilenameCal.EnableWindow(m_CheckCal.GetCheck());
	m_ButtonBrowseCal.EnableWindow(m_CheckCal.GetCheck());
	m_ButtonMaximize.EnableWindow(m_CheckCal.GetCheck());
	m_EditGroups.EnableWindow(m_CheckCal.GetCheck());

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifierCalRes::OnCheckRoc() 
{

	UpdateData(TRUE);

	m_ComboROCClass.EnableWindow(m_CheckROC.GetCheck());
	m_EditFilenameROC.EnableWindow(m_CheckROC.GetCheck());
	m_ButtonBrowseROC.EnableWindow(m_CheckROC.GetCheck());
	
	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifierCalRes::OnOK() 
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

	// Set calibration options.
	if (!batchclassifier->GenerateCalibration(m_CheckCal.GetCheck())) {
		Message::Error("Error assigning calibration flag to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

  CString cal_class, filename;

	int selected = m_ComboCalClass.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}
	m_ComboCalClass.GetLBText(selected, cal_class);

	if (!batchclassifier->SetCalibrationClass(cal_class.GetBuffer(0))) {
		Message::Error("Error assigning calibration class to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	m_EditFilenameCal.GetWindowText(filename);

	if (!batchclassifier->SetCalibrationFilename(filename.GetBuffer(0))) {
		Message::Error("Error assigning calibration filename to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	if (!batchclassifier->SetNoCalibrationGroups(m_Groups)) {
		Message::Error("Error assigning number of calibration groups to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	// Set ROC options.
	if (!batchclassifier->GenerateROC(m_CheckROC.GetCheck())) {
		Message::Error("Error assigning ROC flag to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

  CString roc_class;

	selected = m_ComboROCClass.GetCurSel();
	if (selected == CB_ERR) {
		Message::Debug("CComboBox::GetCurSel() returned CB_ERR");
		return;
	}
	m_ComboROCClass.GetLBText(selected, roc_class);

	if (!batchclassifier->SetROCClass(roc_class.GetBuffer(0))) {
		Message::Error("Error assigning ROC class to batch classifier.");
		EndDialog(IDABORT);
		return;
	}

	m_EditFilenameROC.GetWindowText(filename);

	if (!batchclassifier->SetROCFilename(filename.GetBuffer(0))) {
		Message::Error("Error assigning ROC filename to batch classifier.");
		EndDialog(IDABORT);
		return;
	}
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogBatchClassifierCalRes::OnButtonMaximize() 
{

	if (m_Structure.IsNULL() || !m_Structure->IsA(DECISIONTABLE))
		return;

	Handle<DecisionTable> decisiontable = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	bool masked = true;

	m_Groups = decisiontable->GetNoObjects(masked);
	
	UpdateData(FALSE);

}
 
