// DialogRuleGroupGeneralizer.cpp : implementation file
//
//Author: Ewa Makosa
#include <stdafx.h>
#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogrulegroupgeneralizer.h>

#include <kernel\algorithms\rulegroupgeneralizer.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleGroupGeneralizer dialog
// Ewa Makosa


CDialogRuleGroupGeneralizer::CDialogRuleGroupGeneralizer(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogRuleGroupGeneralizer ::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRuleGroupGeneralizer)
	m_Threshold = 0.0f;
	m_Filename = _T("");
	m_Coverage = 0;
	//}}AFX_DATA_INIT
}


void CDialogRuleGroupGeneralizer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRuleGroupGeneralizer)
	DDX_Control(pDX, IDC_CHECK_USEIDG, m_CheckUseIDG);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_Threshold);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	DDX_Text(pDX, IDC_EDIT_COVERAGE, m_Coverage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRuleGroupGeneralizer, CDialog)
	//{{AFX_MSG_MAP(CDialogRuleGroupGeneralizer)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_USEIDG, OnCheckUseIDG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleGroupGeneralizer message handlers

void CDialogRuleGroupGeneralizer::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_Filename = fileDlg.GetPathName();
	UpdateData(FALSE);		
}

void CDialogRuleGroupGeneralizer::OnCheckUseIDG() 
{
	UpdateData(TRUE);
	m_EditFilename.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckUseIDG.GetCheck());	
}

void CDialogRuleGroupGeneralizer::OnOK() 
{

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULEGROUPGENERALIZER)) { 
		if (m_Algorithm.IsNULL()){
			Message::Error("Algorithm not present");
		}else
			Message::Error("Algorithm not present or not a RuleGroupGeneralizer.");
		EndDialog(IDABORT);
		return;
	}

	// Cast to verified type.
	Handle<RuleGroupGeneralizer> ruleGeneralizer = dynamic_cast(RuleGroupGeneralizer *, m_Algorithm.GetPointer());


	// Set various.
	ruleGeneralizer->SetThreshold(m_Threshold);
	ruleGeneralizer->SetCoverageThreshold(m_Coverage);

	// Set IDG stuff.
	ruleGeneralizer->UseIDG(m_CheckUseIDG.GetCheck());
	ruleGeneralizer->SetIDGFilename(m_Filename.GetBuffer(0));

	
	CDialogAlgorithm::OnOK();
}

BOOL CDialogRuleGroupGeneralizer::OnInitDialog() 
{	
	CDialogAlgorithm::OnInitDialog();
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULEGROUPGENERALIZER)) { 
		if (m_Algorithm.IsNULL()){
			Message::Error("Algorithm not present");
		}else
			Message::Error("Algorithm not present or not a RuleGroupGeneralizer.");
		CDialog::OnCancel();
		return FALSE;
	}

	// Cast to verified type.
	Handle<RuleGroupGeneralizer> ruleGeneralizer = dynamic_cast(RuleGroupGeneralizer *, m_Algorithm.GetPointer());

	
	// Initialize edit fields.
	m_Threshold = ruleGeneralizer->GetThreshold();
	m_Coverage = ruleGeneralizer->GetCoverageThreshold();

	// Update data.
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}
 
