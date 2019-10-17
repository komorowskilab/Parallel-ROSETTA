// dialogstandardvoterparameters.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogstandardvoterparameters.h>

#include <kernel\algorithms\standardvoter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogStandardVoterParameters dialog


CDialogStandardVoterParameters::CDialogStandardVoterParameters(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogStandardVoterParameters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogStandardVoterParameters)
	//}}AFX_DATA_INIT

	m_EnableGeneralization = TRUE;

}


void CDialogStandardVoterParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogStandardVoterParameters)
	DDX_Control(pDX, IDC_CHECK_MOST_SPECIFIC_ONLY, m_CheckMostSpecificOnly);
	DDX_Control(pDX, IDC_RADIO_VOTING_SUPPORT, m_RadioVotingSupport);
	DDX_Control(pDX, IDC_RADIO_VOTING_SIMPLE, m_RadioVotingSimple);
	DDX_Control(pDX, IDC_RADIO_NORM_FIRING, m_RadioNormFiring);
	DDX_Control(pDX, IDC_RADIO_NORM_ALL, m_RadioNormAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogStandardVoterParameters, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogStandardVoterParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogStandardVoterParameters message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogStandardVoterParameters::OnOK() 
{

	UpdateData(TRUE);

	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(STANDARDVOTER)) { 
		Message::Error("Algorithm not present or of non-expected type.");
		EndDialog(IDABORT);
		return;
  }

	Handle<StandardVoter> voter = dynamic_cast(StandardVoter *, m_Algorithm.GetPointer());

	// Set generalization stuff.
	BOOL exclude = m_CheckMostSpecificOnly.GetCheck();
	if (!voter->ExcludeGeneralizations(exclude))
		Message::Error("An error occurred when setting the generalization flag.");

	StandardVoter::Voting voting;

	// Set voting stuff.
	if (m_RadioVotingSupport.GetCheck())
		voting = StandardVoter::VOTING_SUPPORT;
	else if (m_RadioVotingSimple.GetCheck())
		voting = StandardVoter::VOTING_SIMPLE;
	else
		Message::Error("Unknown voting strategy.");

	if (!voter->SetVotingStrategy(voting))
		Message::Error("An error occurred when setting the voting strategy.");

	StandardVoter::Normalization normalization;

	// Set normalization stuff.
	if (m_RadioNormFiring.GetCheck())
		normalization = StandardVoter::NORMALIZATION_FIRING;
	else if (m_RadioNormAll.GetCheck())
		normalization = StandardVoter::NORMALIZATION_ALL;
	else
		Message::Error("Unknown normalization strategy.");

	if (!voter->SetNormalizationStrategy(normalization))
		Message::Error("An error occurred when setting the normalization strategy.");
		
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

BOOL CDialogStandardVoterParameters::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(STANDARDVOTER)) { 
		Message::Error("Algorithm not present or of non-expected type.");
		EndDialog(IDABORT);
		return FALSE;
  }

	Handle<StandardVoter> voter = dynamic_cast(StandardVoter *, m_Algorithm.GetPointer());

	SetWindowText(IdHolder::GetDescription(voter->GetId()).GetBuffer());

	// Initialize generalization stuff.
	m_CheckMostSpecificOnly.SetCheck(voter->ExcludeGeneralizations());

	// Initialize voting stuff.
	m_RadioVotingSupport.SetCheck(voter->GetVotingStrategy() == StandardVoter::VOTING_SUPPORT);
	m_RadioVotingSimple.SetCheck(voter->GetVotingStrategy() == StandardVoter::VOTING_SIMPLE);

	// Initialize norm. stuff.
	m_RadioNormFiring.SetCheck(voter->GetNormalizationStrategy() == StandardVoter::NORMALIZATION_FIRING);
	m_RadioNormAll.SetCheck(voter->GetNormalizationStrategy() == StandardVoter::NORMALIZATION_ALL);

	// Enable stuff.
	m_CheckMostSpecificOnly.EnableWindow(m_EnableGeneralization);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
