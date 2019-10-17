// dialogcvserialexecutor.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogcvserialexecutor.h>
#include <frontend\dialogs\algorithmdialogs\dialogexecutor.h>

#include <kernel\algorithms\cvserialexecutor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogCVSerialExecutor dialog


CDialogCVSerialExecutor::CDialogCVSerialExecutor(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogCVSerialExecutor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogCVSerialExecutor)
	m_InvertSampling = FALSE;
	m_CVIterations = 0;
	m_Seed = 0;
	m_TrLength = 0;
	//}}AFX_DATA_INIT
}


void CDialogCVSerialExecutor::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogCVSerialExecutor)
	DDX_Check(pDX, IDC_CHECK_INVERTSAMPLING, m_InvertSampling);
	DDX_Text(pDX, IDC_EDIT_CVITERATIONS, m_CVIterations);
	DDV_MinMaxInt(pDX, m_CVIterations, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_SEED, m_Seed);
	DDX_Text(pDX, IDC_EDIT_TRLENGTH, m_TrLength);
	DDV_MinMaxInt(pDX, m_TrLength, 0, 2147483647);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogCVSerialExecutor, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogCVSerialExecutor)
	ON_BN_CLICKED(IDC_BUTTON_CMDFILE, OnButtonCmdfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogCVSerialExecutor message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogCVSerialExecutor::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is of the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(CVSERIALEXECUTOR)) { 
		Message::Error("Algorithm not present or not a CVSerialExecutor.");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to verified type.
	Handle<CVSerialExecutor> executor = dynamic_cast(CVSerialExecutor *, m_Algorithm.GetPointer());

	// Initialize fields.
	m_CVIterations   = executor->GetN();
	m_InvertSampling = executor->InvertSampling();
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

void CDialogCVSerialExecutor::OnButtonCmdfile() 
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

void CDialogCVSerialExecutor::OnOK() 
{

	// Check if the algorithm is of the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(CVSERIALEXECUTOR)) { 
		Message::Error("Algorithm not present or not a CVSerialExecutor.");
		EndDialog(IDABORT);
		return;
  }

	// Cast to verified type.
	Handle<CVSerialExecutor> executor = dynamic_cast(CVSerialExecutor *, m_Algorithm.GetPointer());

	UpdateData(TRUE);

	// Set parameters.
	executor->SetN(m_CVIterations);
	executor->InvertSampling(m_InvertSampling);
	executor->SetLength(m_TrLength);
	executor->SetSeed(m_Seed);
	
	CDialogAlgorithm::OnOK();

}
 
