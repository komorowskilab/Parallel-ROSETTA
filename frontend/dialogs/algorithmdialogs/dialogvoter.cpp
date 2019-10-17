// dialogvoter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogvoter.h>

#include <kernel\algorithms\voter.h>

#include <kernel\structures\rules.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogVoter dialog


CDialogVoter::CDialogVoter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogVoter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogVoter)
	m_Fraction = 0.0f;
	//}}AFX_DATA_INIT

	m_EnableRules = TRUE;
	m_EnableAdvanced = TRUE;

}


void CDialogVoter::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogVoter)
	DDX_Control(pDX, IDC_COMBO_RULES, m_ComboRules);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_Fraction);
	DDV_MinMaxFloat(pDX, m_Fraction, 0.f, 1.f);
	DDX_Control(pDX, IDC_BUTTON_ADVANCEDPARAMS, m_ButtonAdvancedParams);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_Filename);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	DDX_Control(pDX, IDC_CHECK_USEIDG, m_CheckUseIDG);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogVoter, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogVoter)
	ON_CBN_SELCHANGE(IDC_COMBO_RULES, OnSelchangeComboRules)
	ON_EN_KILLFOCUS(IDC_EDIT_TOLERANCE, OnKillfocusEditFraction)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCEDPARAMS, OnButtonAdvancedparameters)
	ON_BN_CLICKED(IDC_CHECK_USEIDG, OnCheckUseIDG)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogVoter message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogVoter::OnOK() 
{

	UpdateData(TRUE);

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(VOTER)) {
		Message::Error("Algorithm not present or of non-expected type.");
		EndDialog(IDABORT);
		return;
  }

	Handle<Voter> voter = dynamic_cast(Voter *, m_Algorithm.GetPointer());

	// Get selected rules.
	int selection = m_ComboRules.GetCurSel();
	if (selection == CB_ERR) {
		Message::Error("No rules selected.");
		EndDialog(IDABORT);
		return;
	}

	// Check validity.
	if (!m_Available[selection]->IsA(RULES)) {
		Message::Error("Selection is not a rule set.");
		EndDialog(IDABORT);
		return;
	}

	// Set rules.
	if (!voter->SetRules(dynamic_cast(Rules *, m_Available[selection].GetPointer())))
		Message::Error("An error occurred when assigning a rule set to the classifier.");

	// Set IDG stuff.
	voter->UseIDG(m_CheckUseIDG.GetState());
	voter->SetIDGFilename(m_Filename.GetBuffer(0));

	// Set fraction.
	if (!voter->SetFraction(m_Fraction))
		Message::Error("An error occurred when setting the fraction.");
	
	// Clean up set rule vector (built in OnInitDialog) so as to not keep
	// any rule sets "alive" unnecessarily.
	m_Available.erase(m_Available.begin(), m_Available.end());
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogVoter::OnInitDialog() 
{

	int i;

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(VOTER)) { 
		Message::Error("Algorithm not present or of non-expected type.");
		EndDialog(IDABORT);
		return FALSE;
  }

	Handle<Voter> voter = dynamic_cast(Voter *, m_Algorithm.GetPointer());

	SetWindowText(IdHolder::GetDescription(voter->GetId()).GetBuffer());

	// Check if the project is set.
	if (m_Project.IsNULL()) { 
		Message::Error("Project not set.");
		EndDialog(IDABORT);
		return FALSE;
  }
  
	// Get all available rule sets.
	m_Project->GetAllIdentifiers(RULES, m_Available);

	bool all = true;

	// Make sure they are all "real" rules.
	for (i = m_Available.size() - 1; i >= 0; i--) {
		if (!dynamic_cast(Rules *, m_Available[i].GetPointer())->HasRHS(all))
			m_Available.erase(m_Available.begin() + i);
	}

	if (m_Available.empty()) {
		Message::Error("No rule sets available.");
		EndDialog(IDABORT);
		return FALSE;
	}

	int selection = 0;

	// Build the combo box list.
	for (i = 0; i < m_Available.size(); i++) {
		String name = dynamic_cast(Rules *, m_Available[i].GetPointer())->GetName();
		m_ComboRules.AddString(name.GetBuffer());
		if (m_Available[i].GetPointer() == voter->GetRules())
			selection = i;
	}
	m_ComboRules.SetCurSel(selection);

	// Initialize IDG stuff
	m_CheckUseIDG.SetCheck(voter->UseIDG());
	m_Filename = (voter->GetIDGFilename()).GetBuffer();

	// Initialize fraction.
	m_Fraction = voter->GetFraction();

	// Enable stuff.
	m_ComboRules.EnableWindow(m_EnableRules);
	m_ButtonAdvancedParams.EnableWindow(HasAdvancedParameters() && m_EnableAdvanced);

	m_EditFilename.EnableWindow(voter->UseIDG());
	m_ButtonBrowse.EnableWindow(voter->UseIDG());

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnSelchangeComboRules
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogVoter::OnSelchangeComboRules() 
{
}

//-------------------------------------------------------------------
// Method........: OnKillfocusEditFraction
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogVoter::OnKillfocusEditFraction() 
{
}

//-------------------------------------------------------------------
// Method........: OnCheckUseIDG
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogVoter::OnCheckUseIDG() 
{
	UpdateData(TRUE);
	m_EditFilename.EnableWindow(m_CheckUseIDG.GetCheck());
	m_ButtonBrowse.EnableWindow(m_CheckUseIDG.GetCheck());
}

//-------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogVoter::OnButtonBrowse() 
{
	CFileDialog fileDlg(TRUE, NULL, "*.*");
	if (fileDlg.DoModal() == IDOK)
		m_Filename = fileDlg.GetPathName();
	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for voters with special options.
// Revisions.....:
//===================================================================

bool CDialogVoter::HasAdvancedParameters() const
{
	return false;
}

//------------------------------------------------------------------
// Method........: OnButtonAdvancedparameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for voters with special options.
// Revisions.....:
//===================================================================

void CDialogVoter::OnButtonAdvancedparameters() 
{
}
 
