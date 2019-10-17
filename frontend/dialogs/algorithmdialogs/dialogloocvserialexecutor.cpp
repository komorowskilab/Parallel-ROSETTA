// dialogcvserialexecutor.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogloocvserialexecutor.h>
#include <frontend\dialogs\algorithmdialogs\dialogexecutor.h>

#include <kernel\genomics\algorithms\loocvserialexecutor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogLeaveOneOutCVSerialExecutor dialog


CDialogLeaveOneOutCVSerialExecutor::CDialogLeaveOneOutCVSerialExecutor(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogLeaveOneOutCVSerialExecutor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLeaveOneOutCVSerialExecutor)
	m_InvertSampling = FALSE;
	m_CVIterations = 0;
	m_Seed = 0;
	m_TrLength = 0;
	//}}AFX_DATA_INIT
}


void CDialogLeaveOneOutCVSerialExecutor::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLeaveOneOutCVSerialExecutor)
	
		DDX_Text(pDX, IDC_EDIT_SEED, m_Seed);
	DDX_Text(pDX, IDC_EDIT_TRLENGTH, m_TrLength);
	DDV_MinMaxInt(pDX, m_TrLength, 0, 2147483647);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLeaveOneOutCVSerialExecutor, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogLeaveOneOutCVSerialExecutor)
	ON_BN_CLICKED(IDC_BUTTON_CMDFILE, OnButtonCmdfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogLeaveOneOutCVSerialExecutor message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogLeaveOneOutCVSerialExecutor::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is of the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(LOOCVSERIALEXECUTOR)) { 
		Message::Error("Algorithm not present or not a LeaveOneOutCVSerialExecutor.");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to verified type.
	Handle<LeaveOneOutCVSerialExecutor> executor = dynamic_cast(LeaveOneOutCVSerialExecutor *, m_Algorithm.GetPointer());

	Handle<DecisionTable> thetable= dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	
	// Initialize fields.
	m_CVIterations   = thetable->GetNoObjects(true);
	m_InvertSampling = false;
	m_TrLength       = executor->GetLength();
	m_Seed           = executor->GetSeed();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnButtonCmdfile
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogLeaveOneOutCVSerialExecutor::OnButtonCmdfile() 
{

	CDialogExecutor dialog;

	dialog.SetAlgorithm(m_Algorithm.GetPointer(), m_Structure.GetPointer(), m_Project.GetPointer());

	dialog.DoModal();
	
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogLeaveOneOutCVSerialExecutor::OnOK() 
{

	// Check if the algorithm is of the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(LOOCVSERIALEXECUTOR)) { 
		Message::Error("Algorithm not present or not a leave-one-out CVSerialExecutor.");
		EndDialog(IDABORT);
		return;
  }

	// Cast to verified type.
	Handle<LeaveOneOutCVSerialExecutor> executor = dynamic_cast(LeaveOneOutCVSerialExecutor *, m_Algorithm.GetPointer());

	UpdateData(TRUE);

	// Set parameters.
	executor->SetN(m_CVIterations);
	executor->InvertSampling(m_InvertSampling);
	executor->SetLength(m_TrLength);
	executor->SetSeed(m_Seed);
	
	CDialogAlgorithm::OnOK();

}
 
