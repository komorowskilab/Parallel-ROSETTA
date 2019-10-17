// dialogjohnsonparameters.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogjohnsonparameters.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogJohnsonParameters dialog


CDialogJohnsonParameters::CDialogJohnsonParameters(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogJohnsonParameters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogJohnsonParameters)
	m_HitFraction = 0.0f;
	m_ApproximateSolutions = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogJohnsonParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogJohnsonParameters)
	DDX_Control(pDX, IDC_CHECK_APPROXIMATE, m_CheckApproximateSolutions);
	DDX_Control(pDX, IDC_EDIT_HITFRACTION, m_EditHitFraction);
	DDX_Check(pDX, IDC_CHECK_APPROXIMATE, m_ApproximateSolutions);
	DDX_Text(pDX, IDC_EDIT_HITFRACTION, m_HitFraction);
	DDV_MinMaxFloat(pDX, m_HitFraction, 0.f, 1.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogJohnsonParameters, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogJohnsonParameters)
	ON_BN_CLICKED(IDC_CHECK_APPROXIMATE, OnCheckApproximate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogJohnsonParameters message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogJohnsonParameters::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(JOHNSONREDUCER)) { 
		Message::Error("Algorithm is not a JohnsonReducer");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to a JohnsonReducer.
	m_Reducer = dynamic_cast(JohnsonReducer *, m_Algorithm.GetPointer());
		
	// Initialize field values.
	m_ApproximateSolutions = m_Reducer->ApproximateSolutions();
	m_HitFraction = m_Reducer->GetHittingFraction();

	// Initialize field states.
	m_EditHitFraction.EnableWindow(m_ApproximateSolutions);

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogJohnsonParameters::OnOK() 
{

	// Transfer data from UI fields to variables.
	UpdateData(TRUE);

	// Set parameters.
	m_Reducer->ApproximateSolutions(m_ApproximateSolutions);
	m_Reducer->SetHittingFraction(m_HitFraction);

	CDialogAlgorithm::OnOK();

}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogJohnsonParameters::OnCheckApproximate() 
{
	UpdateData(TRUE);
	m_EditHitFraction.EnableWindow(m_ApproximateSolutions);
}

 
