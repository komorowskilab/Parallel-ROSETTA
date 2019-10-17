// dialogfilename.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogfilename.h>

#include <kernel\algorithms\algorithm.h>
#include <kernel\algorithms\keyword.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogFilename dialog


CDialogFilename::CDialogFilename(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogFilename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFilename)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogFilename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFilename)
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogFilename, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogFilename)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogFilename message handlers

//-------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFilename::OnButtonBrowse() 
{

	// Set up a filedialog.
	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	// The user canceled.
	if (fileDlg.DoModal() != IDOK)
		return;

  // Get the path.
  CString fullpath = fileDlg.GetPathName();

	// Insert the path in the editbox.
	m_EditFilename.SetWindowText(fullpath);

	UpdateData(FALSE);
	
}

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Use the generic SetParameter function so that
//                 this dialog can be used by any algorithm that requires
//                 a filename.
// Revisions.....: 
//===================================================================

void CDialogFilename::OnOK() 
{

	CString filename;

	// Get the contents of the filename edit control.
	m_EditFilename.GetWindowText(filename);

	// Set the parameter.
	if (!m_Algorithm->SetParameter(Keyword::Filename(), filename.GetBuffer(0)))
		Message::Error("The algorithm could not handle the setting of the filename parameter.");

	// Do stuff higher up.
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

BOOL CDialogFilename::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Clear the path in the editbox.
	m_EditFilename.SetWindowText("");

	if (m_Algorithm == NULL) {
		EndDialog(IDABORT);
		return FALSE;
	}

	// Set window text.
	SetWindowText(IdHolder::GetDescription(m_Algorithm->GetId()).GetBuffer());

	String parameters = m_Algorithm->GetParameters();

	// Does the parameter string contain a "FILENAME = ...;" substring? If so, extract filename.
	if (parameters.Contains(Keyword::Filename())) {
		parameters = parameters.After(Keyword::Filename());
		parameters = parameters.After(Keyword::Assignment());
		if (parameters.Contains(Keyword::Separator()))
			parameters = parameters.Before(Keyword::Separator());
		parameters.Trim("\t ");
		m_EditFilename.SetWindowText(parameters.GetBuffer());
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
