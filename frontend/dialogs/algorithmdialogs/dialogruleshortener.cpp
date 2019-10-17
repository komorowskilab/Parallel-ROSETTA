// dialogruleshortener.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogruleshortener.h>

#include <kernel\algorithms\ruleshortener.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleShortener dialog

CDialogRuleShortener::CDialogRuleShortener(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogRuleShortener::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRuleShortener)
	m_Threshold = 0.0f;
	m_Filename = _T("");
	//}}AFX_DATA_INIT
}


void CDialogRuleShortener::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRuleShortener)
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_CHECK_USEIDG, m_CheckUseIDG);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_Threshold);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRuleShortener, CDialog)
	//{{AFX_MSG_MAP(CDialogRuleShortener)
	ON_BN_CLICKED(IDC_CHECK_USEIDG, OnCheckUseIDG)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleShortener message handlers

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Ewa Makosa
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogRuleShortener::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULESHORTENER)) { 
		if (m_Algorithm.IsNULL()){
			Message::Error("Algorithm not present");
		}else
			Message::Error("Algorithm not present or not a RuleShortener.");
		CDialog::OnCancel();
		return FALSE;
	}

	// Cast to verified type.
	Handle<RuleShortener> ruleTuner = dynamic_cast(RuleShortener *, m_Algorithm.GetPointer());

	
	// Initialize edit fields.
	m_Threshold = ruleTuner->GetThreshold();

	// Update data.
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//------------------------------------------------------------------
// Method........: OnOK
// Author........: Ewa Makosa
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogRuleShortener::OnOK() 
{

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULESHORTENER)) {
		if (m_Algorithm.IsNULL()){
			Message::Error("Algorithm not present");
		}else
			Message::Error("Algorithm not present or not a RuleShortener.");
		EndDialog(IDABORT);
		return;
	}

	// Cast to verified type.
	Handle<RuleShortener> ruleTuner = dynamic_cast(RuleShortener *, m_Algorithm.GetPointer());

	// Set various.
	ruleTuner->SetThreshold(m_Threshold);

	// Set IDG stuff.
	ruleTuner->UseIDG(m_CheckUseIDG.GetCheck());
	ruleTuner->SetIDGFilename(m_Filename.GetBuffer(0));

	
	CDialogAlgorithm::OnOK();
}

void CDialogRuleShortener::OnCheckUseIDG() 
{
	UpdateData(TRUE);
	m_EditFilename.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckUseIDG.GetCheck());	
}

void CDialogRuleShortener::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_Filename = fileDlg.GetPathName();
	UpdateData(FALSE);	
}
 
