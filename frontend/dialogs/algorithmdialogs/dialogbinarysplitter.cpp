// dialogbinarysplitter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogbinarysplitter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogBinarySplitter dialog

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================
CDialogBinarySplitter::CDialogBinarySplitter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogBinarySplitter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogBinarySplitter)
	m_Seed = 0;
	m_Factor = 0.0f;
	//}}AFX_DATA_INIT
}


void CDialogBinarySplitter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogBinarySplitter)
	DDX_Control(pDX, IDC_CHECK_APPEND, m_Append);
	DDX_Control(pDX, IDC_EDIT_SPLITTING_FACTOR, m_EditFactor);
	DDX_Control(pDX, IDC_EDIT_RANDOM_SEED, m_EditSeed);
	DDX_Text(pDX, IDC_EDIT_RANDOM_SEED, m_Seed);
	DDX_Text(pDX, IDC_EDIT_SPLITTING_FACTOR, m_Factor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogBinarySplitter, CDialog)
	//{{AFX_MSG_MAP(CDialogBinarySplitter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogBinarySplitter message handlers
//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================

BOOL CDialogBinarySplitter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(BINARYSPLITTER)) { 
		Message::Error("Algorithm was not a binary splitter.");
		CDialog::OnCancel();
		return FALSE;
  }
  
	// Cast to a binary splitter.
	m_alg = dynamic_cast(BinarySplitter *, m_Algorithm.GetPointer());
	
	// Initialize checkboxes.
	m_Append.SetCheck(m_alg->AppendAsChildren());
	
	// Initialize editvalues.
	m_Seed   = m_alg->GetSeed();
	m_Factor = m_alg->GetSplittingFactor(); 

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
void CDialogBinarySplitter::OnOK() 
{
	
	UpdateData(TRUE);

	if (!m_alg->SetSeed(m_Seed)) {
		Message::Error("Could not set random seed");
		return;
	}
	
	if (!m_alg->SetSplittingFactor(m_Factor)) {
		Message::Error("Could not set splitting factor");
		return;
	}

	if (!m_alg->AppendAsChildren(m_Append.GetCheck())) {
		Message::Error("Could not set the bool value AppendAsChildren");
		return;
	}

	CDialog::OnOK();
}
 
