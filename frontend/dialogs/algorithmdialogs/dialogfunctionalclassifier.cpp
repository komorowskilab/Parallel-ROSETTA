// dialogfunctionalclassifier.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>
#include <frontend\dialogs\algorithmdialogs\dialogfunctionalclassifier.h>
#include <frontend\dialogs\algorithmdialogs\dialogfunctionalclassifiertemplatedefs.h>
#include <frontend\dialogs\algorithmdialogs\dialogfunctionalclassifieradvancedparams.h>
#include <kernel\genomics\algorithms\functionalclassifier.h>

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifier dialog


CDialogFunctionalClassifier::CDialogFunctionalClassifier(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogFunctionalClassifier::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFunctionalClassifier)
	//}}AFX_DATA_INIT
}


void CDialogFunctionalClassifier::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFunctionalClassifier)
	DDX_Control(pDX, IDC_BUTTON_ADVANCED_PARAMS, m_ButtonAdvancedParams);
	DDX_Control(pDX, IDC_BUTTON_TEMPLATE_DEFS, m_ButtonTemplateDefs);
	DDX_Control(pDX, IDC_CHECK_RAND, m_CCheckRand);
	DDX_Control(pDX, IDC_STATIC_RULESFILE, m_StaticRulesFile);
	DDX_Control(pDX, IDC_STATIC_RANDFILE, m_StaticRandFile);
	DDX_Control(pDX, IDC_STATIC_CLASSIFICATIONFILE, m_StaticClassificationFile);
	DDX_Control(pDX, IDC_EDIT_RAND, m_CEditRand);
	DDX_Control(pDX, IDC_EDIT_FILENAME_RAND, m_EditFilenameRand);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_RAND, m_ButtonBrowseRand);
	DDX_Control(pDX, IDHELP, m_Help);
	DDX_Control(pDX, IDC_CVREP, m_CVRep);
	DDX_Control(pDX, IDC_CVNUM, m_CVNum);
	DDX_Control(pDX, IDC_CVFILE, m_CVFile);
	DDX_Control(pDX, IDC_EDIT_RNG, m_CEditRNG);
	DDX_Control(pDX, IDC_EDIT_CV, m_CEditCV);
	DDX_Control(pDX, IDC_EDIT_FILENAME_CV, m_EditFilenameCV);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_CV, m_ButtonBrowseCV);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_RULES, m_ButtonBrowseRules);
	DDX_Control(pDX, IDC_EDIT_FILENAME_RULES, m_EditFilenameRules);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_CLASSIFICATION, m_ButtonBrowseClassification);
	DDX_Control(pDX, IDC_EDIT_FILENAME_CLASSIFICATION, m_EditFilenameClassification);
	DDX_Check(pDX, IDC_CHECK_CV, m_CheckCV);
	DDX_Text(pDX, IDC_EDIT_CV, m_EditCV);
	DDX_Text(pDX, IDC_EDIT_RNG, m_EditRNG);
	DDV_MinMaxInt(pDX, m_EditRNG, 0, 1000);
	DDX_Check(pDX, IDC_CHECK_CLASSIFY, m_CheckClassify);
	DDX_Text(pDX, IDC_EDIT_RAND, m_EditRand);
	DDX_Check(pDX, IDC_CHECK_RAND, m_CheckRand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogFunctionalClassifier, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogFunctionalClassifier)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_CLASSIFICATION, OnButtonBrowseClassification)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_RULES, OnButtonBrowseRules)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_CV, OnButtonBrowseCV)
	ON_BN_CLICKED(IDC_CHECK_CV, OnCheckCV)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_RAND, OnButtonBrowseRand)
	ON_BN_CLICKED(IDC_CHECK_CLASSIFY, OnCheckClassify)
	ON_BN_CLICKED(IDC_CHECK_RAND, OnCheckRand)
	ON_BN_CLICKED(IDC_BUTTON_TEMPLATE_DEFS, OnButtonTemplateDefs)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED_PARAMS, OnButtonAdvancedParams)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifier message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Torgeir Hvidsten
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: Robin Andersson
//===================================================================

void CDialogFunctionalClassifier::OnOK() 
{
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(FUNCTIONALCLASSIFIER)) {
		Message::Error("Algorithm not present or not a FunctionalClassifier.");
		EndDialog(IDABORT);
		return;
	}

	Handle <FunctionalClassifier> executor = dynamic_cast(FunctionalClassifier*, m_Algorithm.GetPointer());

	CString filename;

	UpdateData(TRUE);

	// Set parameters
	// --------------

	// Classification
	executor->SetClassify(m_CheckClassify);
	m_EditFilenameClassification.GetWindowText(filename);
	executor->SetLogFile(filename.GetBuffer(0));
	m_EditFilenameRules.GetWindowText(filename);
	executor->SetRuleLogFile(filename.GetBuffer(0));

	// CV
	executor->SetCVLog(m_CheckCV);
	m_EditFilenameCV.GetWindowText(filename);
	executor->SetCVLogFile(filename.GetBuffer(0));
	executor->SetCV(m_EditCV);
	executor->SetRNG(m_EditRNG);
	
	// Randomization
	if (m_CheckRand) 
		executor->SetRandomizations(m_EditRand);
	else
		executor->SetRandomizations(0);

	m_EditFilenameRand.GetWindowText(filename);
	executor->SetRandLogFile(filename.GetBuffer(0));

	CDialogAlgorithm::OnOK();
}

//------------------------------------------------------------------
// Method........: 
// Author........: Torgeir Hvidsten
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnButtonBrowseClassification() 
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilenameClassification.SetWindowText(fullpath);    
	}

	UpdateData(FALSE);
	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Torgeir Hvidsten
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnButtonBrowseRules() 
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilenameRules.SetWindowText(fullpath);    
	}

	UpdateData(FALSE);
	
}

//------------------------------------------------------------------
// Method........: 
// Author........: Torgeir Hvidsten
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: Robin Andersson
//===================================================================

BOOL CDialogFunctionalClassifier::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(FUNCTIONALCLASSIFIER)) { 
		Message::Error("Algorithm is not a FunctionalClassifier");
		CDialog::OnCancel();
		return FALSE;
	}
	
	//cast to verified type
	Handle<FunctionalClassifier> alg = dynamic_cast(FunctionalClassifier*, m_Algorithm.GetPointer());

	m_CheckCV = alg->GetCVLog();
	m_EditCV = alg->GetCV();
	m_EditRNG = alg->GetRNG();

	m_EditRand = alg->GetRandomizations();
	m_CheckRand = m_EditRand > 0;
	m_CheckClassify = alg->GetClassify();

	m_EditFilenameCV.SetWindowText(alg->GetCVLogFile().GetBuffer());
	m_EditFilenameRules.SetWindowText(alg->GetRuleLogFile().GetBuffer());
	m_EditFilenameClassification.SetWindowText(alg->GetLogFile().GetBuffer());
	m_EditFilenameRand.SetWindowText(alg->GetRandLogFile().GetBuffer());

	m_CVFile.EnableWindow(m_CheckCV);
	m_CVNum.EnableWindow(m_CheckCV);
	m_CVRep.EnableWindow(m_CheckCV);
	m_EditFilenameCV.EnableWindow(m_CheckCV);
	m_ButtonBrowseCV.EnableWindow(m_CheckCV);
	m_CEditCV.EnableWindow(m_CheckCV);
	m_CEditRNG.EnableWindow(m_CheckCV);
	m_CCheckRand.EnableWindow(m_CheckCV);

	m_ButtonBrowseRand.EnableWindow(m_CheckRand);
	m_EditFilenameRand.EnableWindow(m_CheckRand);
	m_StaticRandFile.EnableWindow(m_CheckRand);
	m_CEditRand.EnableWindow(m_CheckRand);

	m_EditFilenameRules.EnableWindow(m_CheckClassify);
	m_EditFilenameClassification.EnableWindow(m_CheckClassify);
	m_StaticClassificationFile.EnableWindow(m_CheckClassify);
	m_StaticRulesFile.EnableWindow(m_CheckClassify);
	m_ButtonBrowseRules.EnableWindow(m_CheckClassify);
	m_ButtonBrowseClassification.EnableWindow(m_CheckClassify);

	//Help
	m_Help.SetIcon(LoadIcon(NULL,IDI_QUESTION)); 

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: 
// Author........: Torgeir Hvidsten
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnButtonBrowseCV() 
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilenameCV.SetWindowText(fullpath);    
	}

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

void CDialogFunctionalClassifier::OnCheckCV() 
{

	UpdateData(TRUE);

	bool enable = m_CheckCV;

	//Edit fields:
	m_EditFilenameCV.EnableWindow(enable);
	m_ButtonBrowseCV.EnableWindow(enable);
	m_CEditCV.EnableWindow(enable);
	m_CEditRNG.EnableWindow(enable);
	m_CCheckRand.EnableWindow(enable);

	//Static texts:
	m_CVFile.EnableWindow(enable);
	m_CVRep.EnableWindow(enable);
	m_CVNum.EnableWindow(enable);

	if (!enable){
		m_CheckRand = false;
		m_ButtonBrowseRand.EnableWindow(enable);
		m_EditFilenameRand.EnableWindow(enable);
		m_StaticRandFile.EnableWindow(enable);
		m_CEditRand.EnableWindow(enable);
	}

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

void CDialogFunctionalClassifier::OnHelp() 
{
	if (system("explorer \"http://rosetta.lcb.uu.se/\"") == -1)
		Message::Error("Internet explorer not found, please consult the online manual on http://rosetta.lcb.uu.se/");
}

//------------------------------------------------------------------
// Method........: 
// Author........: Torgeir Hvidsten
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnButtonBrowseRand() 
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK) {
    CString fullpath = fileDlg.GetPathName();
		m_EditFilenameRand.SetWindowText(fullpath);    
	}

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

void CDialogFunctionalClassifier::OnCheckRand() 
{
	UpdateData(TRUE);

	bool enable = m_CheckRand;

	m_ButtonBrowseRand.EnableWindow(enable);
	m_EditFilenameRand.EnableWindow(enable);
	m_StaticRandFile.EnableWindow(enable);
	m_CEditRand.EnableWindow(enable);

	UpdateData(TRUE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnCheckClassify() 
{
	UpdateData(TRUE);

	bool enable = m_CheckClassify;

	m_EditFilenameRules.EnableWindow(enable);
	m_EditFilenameClassification.EnableWindow(enable);
	m_StaticClassificationFile.EnableWindow(enable);
	m_StaticRulesFile.EnableWindow(enable);
	m_ButtonBrowseRules.EnableWindow(enable);
	m_ButtonBrowseClassification.EnableWindow(enable);

	UpdateData(TRUE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnButtonTemplateDefs() 
{
	CDialogFunctionalClassifierTemplateDefs dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();

}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFunctionalClassifier::OnButtonAdvancedParams() 
{
	CDialogFunctionalClassifierAdvancedParams dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();
	
}
 
