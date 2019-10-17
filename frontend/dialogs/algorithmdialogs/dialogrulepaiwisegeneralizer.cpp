// DialogRulePairwiseGeneralizer.cpp : implementation file
//
//Author: Ewa Makosa
#include <stdafx.h>
#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogrulepairwisegeneralizer.h>

#include <kernel\algorithms\rulepairwisegeneralizer.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRulePairwiseGeneralizer dialog


CDialogRulePairwiseGeneralizer::CDialogRulePairwiseGeneralizer(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogRulePairwiseGeneralizer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRulePairwiseGeneralizer)
	m_Threshold = 0.0f;
	m_Filename = _T("");
	m_AttributeDifference = 0;
	//}}AFX_DATA_INIT
}


void CDialogRulePairwiseGeneralizer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRulePairwiseGeneralizer)
	DDX_Control(pDX, IDC_CHECK_USEIDG, m_CheckUseIDG);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_Threshold);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	DDX_Text(pDX, IDC_EDIT_ATTRIBUTE_DIFFERENCE, m_AttributeDifference);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRulePairwiseGeneralizer, CDialog)
	//{{AFX_MSG_MAP(CDialogRulePairwiseGeneralizer)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_CHECK_USEIDG, OnCheckUseIDG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRulePairwiseGeneralizer message handlers

void CDialogRulePairwiseGeneralizer::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_Filename = fileDlg.GetPathName();
	UpdateData(FALSE);		
}

void CDialogRulePairwiseGeneralizer::OnCheckUseIDG() 
{
	UpdateData(TRUE);
	m_EditFilename.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckUseIDG.GetCheck());	
}

void CDialogRulePairwiseGeneralizer::OnOK() 
{

	UpdateData(TRUE);

	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULEPAIRWISEGENERALIZER)) { 
		if (m_Algorithm.IsNULL()){
			Message::Error("Algorithm not present");
		}else
			Message::Error("Algorithm not present or not a RulePairwiseGeneralizer.");
		EndDialog(IDABORT);
		return;
	}

	// Cast to verified type.
	Handle<RulePairwiseGeneralizer> ruleGeneralizer = dynamic_cast(RulePairwiseGeneralizer *, m_Algorithm.GetPointer());


	// Set various.
	ruleGeneralizer->SetThreshold(m_Threshold);
	ruleGeneralizer->SetAttributeDifference(m_AttributeDifference);

	// Set IDG stuff.
	ruleGeneralizer->UseIDG(m_CheckUseIDG.GetCheck());
	ruleGeneralizer->SetIDGFilename(m_Filename.GetBuffer(0));

	
	CDialogAlgorithm::OnOK();
}

BOOL CDialogRulePairwiseGeneralizer::OnInitDialog() 
{	
	CDialogAlgorithm::OnInitDialog();
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULEPAIRWISEGENERALIZER)) { 
		if (m_Algorithm.IsNULL()){
			Message::Error("Algorithm not present");
		}else
			Message::Error("Algorithm not present or not a RulePairwiseGeneralizer.");
		CDialog::OnCancel();
		return FALSE;
	}

	// Cast to verified type.
	Handle<RulePairwiseGeneralizer> ruleGeneralizer = dynamic_cast(RulePairwiseGeneralizer *, m_Algorithm.GetPointer());

	
	// Initialize edit fields.
	m_Threshold = ruleGeneralizer->GetThreshold();
	m_AttributeDifference = ruleGeneralizer->GetAttributeDifference();

	// Update data.
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}
 
