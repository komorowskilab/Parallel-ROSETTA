// dialogreductcostfilter.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogreductcostfilter.h>

#include <kernel\algorithms\reductcostfilter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogReductCostFilter dialog


CDialogReductCostFilter::CDialogReductCostFilter(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogReductCostFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogReductCostFilter)
	m_Invert = FALSE;
	m_CostFilename = _T("");
	m_LogFilename = _T("");
	m_Threshold = 0.0f;
	m_DefaultCost = 0.0f;
	//}}AFX_DATA_INIT
}


void CDialogReductCostFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogReductCostFilter)
	DDX_Check(pDX, IDC_CHECK_INVERT, m_Invert);
	DDX_Text(pDX, IDC_EDIT_COSTFILE, m_CostFilename);
	DDX_Text(pDX, IDC_EDIT_LOGFILE, m_LogFilename);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_Threshold);
	DDX_Text(pDX, IDC_EDIT_DEFAULTCOST, m_DefaultCost);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogReductCostFilter, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogReductCostFilter)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_COST, OnButtonBrowseCost)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_LOG, OnButtonBrowseLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogReductCostFilter message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

BOOL CDialogReductCostFilter::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check algorithm validity.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REDUCTCOSTFILTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Cast to verified type.
	Handle<ReductCostFilter> filter = dynamic_cast(ReductCostFilter *, m_Algorithm.GetPointer());

	// Get parameters from filter.
	m_CostFilename = filter->GetCostFilename().GetBuffer();
	m_LogFilename  = filter->GetLogFilename().GetBuffer();
	m_DefaultCost  = filter->GetDefaultCost();
	m_Threshold    = filter->GetThreshold();
	m_Invert       = filter->InvertLogic();

	// Update data fields.
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductCostFilter::OnOK() 
{

	UpdateData(TRUE);
	
	// Check algorithm validity.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(REDUCTCOSTFILTER)) {
		Message::Error("Algorithm not present or of unexpected type.");
		EndDialog(IDABORT);
		return;
	}

	// Cast to verified type.
	Handle<ReductCostFilter> filter = dynamic_cast(ReductCostFilter *, m_Algorithm.GetPointer());

	// Set parameters.
	filter->SetCostFilename(m_CostFilename.GetBuffer(0));
	filter->SetLogFilename(m_LogFilename.GetBuffer(0));
	filter->SetDefaultCost(m_DefaultCost);
	filter->SetThreshold(m_Threshold);
	filter->InvertLogic(m_Invert);

	//Message::Information("m_DefaultCost = " + String::Format(m_DefaultCost));
	//Message::Information("filter->GetDefaultCost() = " + String::Format(filter->GetDefaultCost()));

	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowseCost
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductCostFilter::OnButtonBrowseCost() 
{

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() != IDOK)
		return;

  m_CostFilename = fileDlg.GetPathName();
	
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowseLog
// Author........: Aleksander šhrn
// Description...: 
// Comments......: 
// Revisions.....: 
// Date..........: 
//===================================================================

void CDialogReductCostFilter::OnButtonBrowseLog() 
{

	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() != IDOK)
		return;

  m_LogFilename = fileDlg.GetPathName();
	
	UpdateData(FALSE);
	
}
 
