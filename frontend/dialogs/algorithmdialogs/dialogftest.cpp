// dialogftest.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend/dialogs/algorithmdialogs/dialogftest.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogFTest dialog

//-------------------------------------------------------------------
// Method........: Constructor.
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================
CDialogFTest::CDialogFTest(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogFTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFTest)
	m_Attributes = 1;
	m_FThreshold = 0.0f;
	m_PThres = 0.1f;
	m_NBoot = 100;
	//}}AFX_DATA_INIT
}


void CDialogFTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFTest)
	DDX_Control(pDX, IDC_CHECK_APPENDT, m_Append);
	DDX_Control(pDX, IDC_BOOTCHECK, m_DoBoot);
	DDX_Control(pDX, IDC_EDIT_FTHRESHOLD, m_EditFThreshold);
	DDX_Control(pDX, IDC_EDIT_MAX_ATTR, m_EditMaxAttr);
	
	DDX_Control(pDX, IDC_NBOOTSTRAPS, m_EditNBoot);
	DDX_Control(pDX, IDC_PBOOT, m_EditPThres);
	DDX_Control(pDX, IDC_SEED, m_EditSeed);
	DDX_Control(pDX, IDC_SEEDCHECK, m_SeedCheck);
	
	DDX_Text(pDX, IDC_SEED, m_Seed);
	DDX_Text(pDX, IDC_EDIT_MAX_ATTR, m_Attributes);
	DDX_Text(pDX, IDC_EDIT_FTHRESHOLD, m_FThreshold);
	
	DDX_Text(pDX, IDC_NBOOTSTRAPS, m_NBoot);
	DDX_Text(pDX, IDC_PBOOT, m_PThres);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogFTest, CDialog)
	//{{AFX_MSG_MAP(CDialogFTest)
	ON_BN_CLICKED(IDC_BOOTCHECK, OnCheckBoot)
		ON_BN_CLICKED(IDC_SEEDCHECK, OnCheckSeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogFTest message handlers
//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Daniel Remmem
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 960731
//===================================================================

BOOL CDialogFTest::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(FTEST)) { 
		Message::Error("Algorithm was not f test.");
		CDialog::OnCancel();
		return FALSE;
  }
  
	// Cast to a t tester.
	m_alg = dynamic_cast(FTest *, m_Algorithm.GetPointer());
	
	// Initialize checkboxes.
	m_Append.SetCheck(m_alg->AppendAsChildren());
	
	// Initialize editvalues.
	m_Attributes   = m_alg->GetAttributes();
	m_FThreshold = m_alg->GetThreshold(); 

	m_DoBoot.SetCheck(FALSE);
	
		m_EditNBoot.EnableWindow(false);
	  m_EditPThres.EnableWindow(false);
			m_EditSeed.EnableWindow(false);
	  m_SeedCheck.EnableWindow(false);
		

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
void CDialogFTest::OnOK() 
{
	
	UpdateData(TRUE);

	if (!m_alg->SetAttributes(m_Attributes)) {
		Message::Error("Could not set random seed");
		return;
	}
	if (!m_alg->SetSeed(m_Seed)) {
		Message::Error("Could not set random seed");
		return;
	}
		if (!m_alg->SetGenerateSeed(	m_SeedCheck.GetCheck()))
	 {
		Message::Error("Could not choose to generete seed");
		return;
	}


	if (!m_alg->SetBootStrap(	m_DoBoot.GetCheck()))
	 {
		Message::Error("Could not select bootstrap");
		return;
	}

	
	if (!m_alg->SetThreshold(m_FThreshold)) {
		Message::Error("Could not set splitting factor");
		return;
	}
	if (!m_alg->SetPThreshold(m_PThres)) {
		Message::Error("Could not set p threshold");
		return;
	}
	if (!m_alg->SetStraps(m_NBoot)) {
		Message::Error("Could not set number of bootstraps");
		return;
	}

	if (!m_alg->AppendAsChildren(m_Append.GetCheck())) {
		Message::Error("Could not set the bool value AppendAsChildren");
		return;
	}

	CDialog::OnOK();
}
void CDialogFTest::OnCheckBoot() 
{
	if (m_DoBoot.GetCheck()) {
		// m_EditTThreshold.EnableWindow(false);
		m_EditNBoot.EnableWindow(true);
	  m_EditPThres.EnableWindow(true);
		
				if (m_SeedCheck.GetCheck()) 
	
			{		m_EditSeed.EnableWindow(false);
			}
			else 
			{
					m_EditSeed.EnableWindow(true);
			}

	  m_SeedCheck.EnableWindow(true);
		m_EditFThreshold.EnableWindow(false);
		

	} else {
		// m_EditTThreshold.EnableWindow(true);
		m_EditNBoot.EnableWindow(false);
	  m_EditPThres.EnableWindow(false);
		m_EditSeed.EnableWindow(false);
	  m_SeedCheck.EnableWindow(false);
		m_EditFThreshold.EnableWindow(true);
	}
}
void CDialogFTest::OnCheckSeed() 
{
	if (m_SeedCheck.GetCheck()) 
		{
				m_EditSeed.EnableWindow(false);
	}

	else
		{
				m_EditSeed.EnableWindow(true);
	}
} 
