// dialogrulegenerator.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogrulegenerator.h>

#include <kernel\algorithms\rulegenerator.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleGenerator dialog


CDialogRuleGenerator::CDialogRuleGenerator(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogRuleGenerator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRuleGenerator)
	m_Filename = _T("");
	//}}AFX_DATA_INIT
}


void CDialogRuleGenerator::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRuleGenerator)
	DDX_Control(pDX, IDC_BUTTON_ADVANCEDPARAMS, m_ButtonAdvancedParams);
	DDX_Control(pDX, IDC_STATIC_GROUP, m_ButtonGroup);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_CHECK_USEIDG, m_CheckUseIDG);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRuleGenerator, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogRuleGenerator)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCEDPARAMS, OnButtonAdvancedparams)
	ON_BN_CLICKED(IDC_CHECK_USEIDG, OnCheckUseidg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleGenerator message handlers

//------------------------------------------------------------------
// Method........: HasIDGSupport
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for algorithms with no IDG support.
// Revisions.....:
//===================================================================

bool CDialogRuleGenerator::HasIDGSupport() const
{
	return true;
}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for algorithms with such.
// Revisions.....:
//===================================================================

bool CDialogRuleGenerator::HasAdvancedParameters() const
{
	return false;
}

//------------------------------------------------------------------
// Method........: OnButtonAdvancedparams
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this, if relevant.
// Revisions.....:
//===================================================================

void CDialogRuleGenerator::OnButtonAdvancedparams() 
{
	Message::Warning("No further parameters to set.");
}

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogRuleGenerator::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(RULEGENERATOR)) { 
		Message::Error("Algorithm not present or not a RuleGenerator.");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to verified type.
	Handle<RuleGenerator> generator = dynamic_cast(RuleGenerator *, m_Algorithm.GetPointer());

	// Set initial data.
	m_CheckUseIDG.SetCheck(generator->UseIDG());
	m_Filename = (generator->GetIDGFilename()).GetBuffer();

	// Disable stuff?
	m_ButtonGroup.EnableWindow(HasIDGSupport());
	m_CheckUseIDG.EnableWindow(HasIDGSupport());
	m_EditFilename.EnableWindow(HasIDGSupport() && generator->UseIDG());
	m_ButtonBrowse.EnableWindow(HasIDGSupport() && generator->UseIDG());
	m_ButtonAdvancedParams.EnableWindow(HasAdvancedParameters());

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogRuleGenerator::OnOK() 
{

	// Cast to verified type.
	Handle<RuleGenerator> generator = dynamic_cast(RuleGenerator *, m_Algorithm.GetPointer());

	// Set IDG stuff.
	if (!generator->UseIDG(m_CheckUseIDG.GetCheck()) || !generator->SetIDGFilename(m_Filename.GetBuffer(0)))
		Message::Warning("Failed to set IDG data.");
	
	CDialogAlgorithm::OnOK();

}

//------------------------------------------------------------------
// Method........: OnCheckUseidg
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogRuleGenerator::OnCheckUseidg() 
{
	UpdateData(TRUE);
	m_EditFilename.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckUseIDG.GetCheck());
}

//------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogRuleGenerator::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_Filename = fileDlg.GetPathName();
	UpdateData(FALSE);
}
 
