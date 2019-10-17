// dialogbinarysplitter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogpermutationtest.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPermutationTest dialog

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================
CDialogPermutationTest::CDialogPermutationTest(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogPermutationTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPermutationTest)
	 m_Seed = 0;
		 m_NPermutations=100;
	 m_NFolds=10;
	 m_Length=1;
	
	//}}AFX_DATA_INIT
	 
}


void CDialogPermutationTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPermutationTest)
	DDX_Control(pDX, IDC_CHECK_APPEND, m_Append);
	DDX_Control(pDX, IDC_RADIO1, m_Radio1);
		DDX_Control(pDX, IDC_RADIO2, m_Radio2);
DDX_Control(pDX, IDC_EDITNFOLDS, m_EditNFolds);
DDX_Control(pDX, IDC_EDITLENGTH, m_EditLength);

DDX_Control(pDX, IDC_EDIT_LOGFILE, m_EditLogFilename);
DDX_Control(pDX, IDC_BUTTON_BROWSE_LOG, m_ButtonLog);
	DDX_Control(pDX, IDC_EDIT_COMMANDFILE, m_EditCmdFilename);
DDX_Control(pDX, IDC_EDITNPERMUTATIONS, m_EditNPermutations);


	DDX_Control(pDX, IDC_EDIT_RANDOM_SEED, m_EditSeed);
	DDX_Text(pDX, IDC_EDIT_RANDOM_SEED, m_Seed);

		DDX_Text(pDX, IDC_EDIT_COMMANDFILE, m_CmdFilename);
	DDX_Text(pDX, IDC_EDIT_LOGFILE, m_LogFilename);
	DDX_Text(pDX, IDC_EDITNFOLDS, m_NFolds);
	DDX_Text(pDX, IDC_EDITLENGTH, m_Length);
		DDX_Text(pDX, IDC_EDITNPERMUTATIONS, m_NPermutations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPermutationTest, CDialog)
	//{{AFX_MSG_MAP(CDialogPermutationTest)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_CMD, OnButtonBrowseCmd)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_LOG, OnButtonBrowseLog)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPermutationTest message handlers
//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================

BOOL CDialogPermutationTest::OnInitDialog() 
{
	CDialog::OnInitDialog();

		m_Radio1.SetCheck(FALSE);
	 m_Radio2.SetCheck(TRUE);
	m_EditNFolds.EnableWindow(false);

	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(PERMUTATIONTEST)) { 
		Message::Error("Algorithm was not a permutation test.");
		CDialog::OnCancel();
		return FALSE;
  }
  
	// Cast to a binary splitter.
	m_alg = dynamic_cast(PermutationTest *, m_Algorithm.GetPointer());
	
	// Initialize checkboxes.
	m_Append.SetCheck(m_alg->AppendAsChildren());
	
	// Initialize editvalues.
//	m_Seed   = m_alg->GetSeed();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================
void CDialogPermutationTest::OnOK() 
{
	
	UpdateData(TRUE);

	if (!m_alg->SetSeed(m_Seed)) {
		Message::Error("Could not set random seed");
		return;
	}

	if (!m_alg->AppendAsChildren(m_Append.GetCheck())) {
		Message::Error("Could not set the bool value AppendAsChildren");
		return;
	}

		if (!m_alg->SetCommandFilename(m_CmdFilename.GetBuffer(0))) {
		Message::Error("Could not set command file name");
		return;
	}
			if (!m_alg->SetLogFilename(m_LogFilename.GetBuffer(0))) {
		Message::Error("Could not set log filename");
		return;
	}
	
				if (!m_alg->SetNFolds(m_NFolds)) {
		Message::Error("Could not number of folds");
		return;
	}
			if (!m_alg->SetNPermutations(m_NPermutations)) {
		Message::Error("Could not number of permutations");
		return;
	}
	if (!m_alg->SetLength(m_Length)) {
		Message::Error("Could not set length of training pipeline");
		return;
	}
	if (m_Radio2.GetCheck()) {
				if (!m_alg->SetAlgorithm(0)) {
		Message::Error("Could not set evaluation algorithm");
		return;
	}
	}
	else{
				if (!m_alg->SetAlgorithm(1)) {
		Message::Error("Could not set evaluation algorithm");
		return;
				}
		}
	/*

 int                 GetAlgorithm() const {return algorithm_;}
	bool                SetAlgorithm(int algorithm) {algorithm_ = algorithm; return true;}
*/


	CDialog::OnOK();
}

//-------------------------------------------------------------------
// Method........: OnButtonBrowseCmd
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogPermutationTest::OnButtonBrowseCmd() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() != IDOK)
		return;

  m_CmdFilename = fileDlg.GetPathName();
	
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowseLog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogPermutationTest::OnButtonBrowseLog() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() != IDOK)
		return;

  m_LogFilename = fileDlg.GetPathName();
	
	UpdateData(FALSE);
}

void CDialogPermutationTest::OnRadio1() 
{
m_EditNFolds.EnableWindow(true);
}

void CDialogPermutationTest::OnRadio2() 
{
m_EditNFolds.EnableWindow(false);
}

 
