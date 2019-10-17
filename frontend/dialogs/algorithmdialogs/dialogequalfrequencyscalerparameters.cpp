// DialogEqualFrequencyScalerParameters.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogequalfrequencyscalerparameters.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEqualFrequencyScalerParameters dialog


CDialogEqualFrequencyScalerParameters::CDialogEqualFrequencyScalerParameters(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogEqualFrequencyScalerParameters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogEqualFrequencyScalerParameters)
	m_Intervals = 0;
	//}}AFX_DATA_INIT
}


void CDialogEqualFrequencyScalerParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEqualFrequencyScalerParameters)
	DDX_Text(pDX, IDC_EDIT_INTERVALS, m_Intervals);
	DDV_MinMaxInt(pDX, m_Intervals, 1, 2147483647);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogEqualFrequencyScalerParameters, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogEqualFrequencyScalerParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEqualFrequencyScalerParameters message handlers

//------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogEqualFrequencyScalerParameters::OnOK() 
{

	// Transfer data from UI fields to variables.
	UpdateData(TRUE);

	// Set parameters.
	m_Scaler->SetNoIntervals(m_Intervals);
	
	CDialogAlgorithm::OnOK();

}

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogEqualFrequencyScalerParameters::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(EQUALFREQUENCYSCALER)) { 
		Message::Error("Algorithm is not a EqualFrequencyScaler.");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to a EqualFrequencyScaler.
	m_Scaler = dynamic_cast(EqualFrequencyScaler *, m_Algorithm.GetPointer());
		
	// Initialize field values.
	m_Intervals = m_Scaler->GetNoIntervals();

	// Update data fields.
	UpdateData(FALSE);

	CDialogAlgorithm::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
