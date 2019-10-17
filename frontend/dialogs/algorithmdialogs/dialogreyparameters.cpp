// dialogreyparameters.cpp : implementation file
//

#include <stdafx.h>
#include <frontend\rosetta.h>
#include <frontend\dialogs\algorithmdialogs\dialogreyparameters.h>
#include <kernel\utilities\mathkit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReyParameters dialog


CDialogReyParameters::CDialogReyParameters(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogReyParameters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogReyParameters)
	m_CheckApproximate = FALSE;
	m_EditApproximate = 0.0f;
	//}}AFX_DATA_INIT
}


void CDialogReyParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogReyParameters)
	DDX_Control(pDX, IDC_EDIT_APPROXIMATE, m_CEditApproximate);
	DDX_Control(pDX, IDC_SLIDER_APPROXIMATE, m_SliderApproximate);
	DDX_Check(pDX, IDC_CHECK_APPROXIMATE, m_CheckApproximate);
	DDX_Text(pDX, IDC_EDIT_APPROXIMATE, m_EditApproximate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogReyParameters, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogReyParameters)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_APPROXIMATE, OnCustomdrawSliderApproximate)
	ON_EN_UPDATE(IDC_EDIT_APPROXIMATE, OnUpdateEditApproximate)
	ON_BN_CLICKED(IDC_CHECK_APPROXIMATE, OnCheckApproximate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogReyParameters message handlers

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogReyParameters::OnInitDialog() 
{
	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(REYREDUCER)) { 
		Message::Error("Algorithm is not a ReyReducer");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to a ReyReducer.
	m_Reducer = dynamic_cast(ReyReducer *, m_Algorithm.GetPointer());
		
	// Initialize field values.
	m_CheckApproximate = m_Reducer->GetApproximate();
	m_EditApproximate = m_Reducer->GetApproximateFraction();

	//Initialize slider values
	m_SliderApproximate.SetRange(0,100);
	m_SliderApproximate.SetPos(MathKit::Round(m_EditApproximate * static_cast(float, m_SliderApproximate.GetRangeMax())));

	// Initialize field and slider states.
	m_CEditApproximate.EnableWindow(m_CheckApproximate);
	m_SliderApproximate.EnableWindow(m_CheckApproximate);

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogReyParameters::OnOK() 
{
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REYREDUCER)) {
		Message::Error("Algorithm not present or not a ReyReducer.");
		EndDialog(IDABORT);
		return;
	}

	Handle<ReyReducer> alg = dynamic_cast(ReyReducer *, m_Algorithm.GetPointer());

	UpdateData(TRUE);

	alg->SetApproximate(m_CheckApproximate);
	if (m_CheckApproximate)
		alg->SetApproximateFraction(m_EditApproximate);
	
	CDialogAlgorithm::OnOK();
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogReyParameters::OnCustomdrawSliderApproximate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	float app = static_cast(float, m_SliderApproximate.GetPos()) / static_cast(float, m_SliderApproximate.GetRangeMax());
	
	if (app < 0)
		app = 0.0f;

	if (app > 1)
		app = 1.0f;

	m_EditApproximate = app;

	*pResult = 0;

	UpdateData(FALSE);
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogReyParameters::OnUpdateEditApproximate() 
{
	UpdateData(TRUE);
	m_SliderApproximate.SetPos(MathKit::Round(m_EditApproximate * static_cast(float, m_SliderApproximate.GetRangeMax())));
}

//------------------------------------------------------------------
// Method........: 
// Author........: Robin Andersson
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogReyParameters::OnCheckApproximate() 
{
	UpdateData(TRUE);

	m_CEditApproximate.EnableWindow(m_CheckApproximate);
	m_SliderApproximate.EnableWindow(m_CheckApproximate);

	UpdateData(FALSE);
}
 
