// dialogfilescaler.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogfilescaler.h>

#include <kernel\algorithms\scaler.h>
#include <kernel\algorithms\keyword.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogFileScaler dialog


CDialogFileScaler::CDialogFileScaler(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogFileScaler::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFileScaler)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogFileScaler::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFileScaler)
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDX_Control(pDX, IDC_CHECK_MASKSYMBOLIC, m_CheckMaskSymbolic);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_ButtonBrowse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogFileScaler, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogFileScaler)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogFileScaler message handlers

//-------------------------------------------------------------------
// Method........: OnButtonBrowse
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

void CDialogFileScaler::OnButtonBrowse() 
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
//                 this dialog can be used by any FileScaler algorithm.
// Revisions.....: 
//===================================================================

void CDialogFileScaler::OnOK() 
{

	CString filename;
	BOOL    masksymbolic;

	// Get the contents.
	m_EditFilename.GetWindowText(filename);
	masksymbolic = m_CheckMaskSymbolic.GetCheck();

	// Set the parameters.
	if (!m_Algorithm->SetParameter(Keyword::Filename(), filename.GetBuffer(0)))
		Message::Error("Setting of filename parameter failed.");
	if (!m_Algorithm->SetParameter(Keyword::Mask(), String::Format(masksymbolic)))
		Message::Error("Setting of masking parameter failed.");

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

BOOL CDialogFileScaler::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	m_EditFilename.SetWindowText("");
	m_CheckMaskSymbolic.SetCheck(TRUE);

	if (m_Algorithm->IsA(SCALER)) {
		Handle<Scaler> scaler = dynamic_cast(Scaler *, m_Algorithm.GetPointer());
		if (scaler->GetFilename() != Undefined::String())
			m_EditFilename.SetWindowText(scaler->GetFilename().GetBuffer());
		m_CheckMaskSymbolic.SetCheck(scaler->MaskSymbolic());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
 
