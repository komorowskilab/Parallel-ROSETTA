// dialogexecutor.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogexecutor.h>

#include <kernel\algorithms\executor.h>

#include <common\objectmanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogExecutor dialog


CDialogExecutor::CDialogExecutor(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogExecutor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogExecutor)
	m_Return = FALSE;
	m_CmdFilename = _T("");
	m_LogFilename = _T("");
	//}}AFX_DATA_INIT
}


void CDialogExecutor::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogExecutor)
	DDX_Control(pDX, IDC_COMBO_RETURN, m_ComboReturn);
	DDX_Check(pDX, IDC_CHECK_RETURN, m_Return);
	DDX_Text(pDX, IDC_EDIT_COMMANDFILE, m_CmdFilename);
	DDX_Text(pDX, IDC_EDIT_LOGFILE, m_LogFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogExecutor, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogExecutor)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_CMD, OnButtonBrowseCmd)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_LOG, OnButtonBrowseLog)
	ON_BN_CLICKED(IDC_CHECK_RETURN, OnCheckReturn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogExecutor message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

BOOL CDialogExecutor::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check if the algorithm is of the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(EXECUTOR)) { 
		Message::Error("Algorithm not present or not a Executor.");
		CDialog::OnCancel();
		return FALSE;
  }

	// Cast to verified type.
	Handle<Executor> executor = dynamic_cast(Executor *, m_Algorithm.GetPointer());

	// Initialize filenames.
	m_CmdFilename = executor->GetCommandFilename().GetBuffer();
	m_LogFilename = executor->GetLogFilename().GetBuffer();

	// Initialize check.
	m_Return = (executor->GetOutputType() != Undefined::Id());

	CString selected = "";
	int     i;

	// Fill combo box.
	for (i = 0; i < ObjectManager::GetNoStructures(); i++) {
		Handle<Structure> structure = ObjectManager::GetEnumeratedStructure(i);
		CString description = IdHolder::GetDescription(structure->GetId()).GetBuffer();
		if (structure->GetId() == executor->GetOutputType())
			selected = description;
		else if (selected == "" && structure->IsA(executor->GetOutputType()))
			selected = description;
		m_ComboReturn.AddString(description);
	}

	m_ComboReturn.SetCurSel(m_ComboReturn.FindString(-1, selected));

	// Enable/disable combo box.
	m_ComboReturn.EnableWindow(m_Return);

	// Update data.
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogExecutor::OnOK() 
{

	// Check if the algorithm is of the correct type.
	if (m_Algorithm.IsNULL() || !m_Algorithm->IsA(EXECUTOR)) { 
		Message::Error("Algorithm not present or not a Executor.");
		EndDialog(IDABORT);
		return;
  }

	// Cast to verified type.
	Handle<Executor> executor = dynamic_cast(Executor *, m_Algorithm.GetPointer());

	UpdateData(TRUE);

	// Set filenames.
	executor->SetCommandFilename(m_CmdFilename.GetBuffer(0));
	executor->SetLogFilename(m_LogFilename.GetBuffer(0));

	// Set return type.
	if (!m_Return) {
		executor->SetOutputType(Undefined::Id());
	}
	else {
		CString description;
		m_ComboReturn.GetLBText(m_ComboReturn.GetCurSel(), description);
		executor->SetOutputType(IdHolder::GetId(description.GetBuffer(0)));
	}
	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowseCmd
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogExecutor::OnButtonBrowseCmd() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() != IDOK)
		return;

  m_CmdFilename = fileDlg.GetPathName();
	
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnButtonBrowseLog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogExecutor::OnButtonBrowseLog() 
{

	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() != IDOK)
		return;

  m_LogFilename = fileDlg.GetPathName();
	
	UpdateData(FALSE);

}

//-------------------------------------------------------------------
// Method........: OnCheckReturn
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogExecutor::OnCheckReturn() 
{

	UpdateData(TRUE);

	m_ComboReturn.EnableWindow(m_Return);

	UpdateData(FALSE);

}
 
