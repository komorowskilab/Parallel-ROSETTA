// dialogorthogonalscaler.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogorthogonalscaler.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogOrthogonalScaler dialog


CDialogOrthogonalScaler::CDialogOrthogonalScaler(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogOrthogonalScaler::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogOrthogonalScaler)
	//}}AFX_DATA_INIT
}


void CDialogOrthogonalScaler::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOrthogonalScaler)
	DDX_Control(pDX, IDC_BUTTON_ADVANCEDPARAMS, m_ButtonAdvancedParams);
	DDX_Control(pDX, IDC_CHECK_MASKSYMBOLIC, m_MaskSymbolic);
	DDX_Control(pDX, IDC_EDIT_FILENAME_SAVE, m_EditFilenameSave);
	DDX_Control(pDX, IDC_BUTTON_BROWSE_SAVE, m_BrowseButtonSave);
	DDX_Control(pDX, IDC_RADIO_DISCARD, m_RadioDiscard);
	DDX_Control(pDX, IDC_RADIO_SAVE, m_RadioSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogOrthogonalScaler, CDialog)
	//{{AFX_MSG_MAP(CDialogOrthogonalScaler)
	ON_BN_CLICKED(IDC_RADIO_DISCARD, OnRadioDiscard)
	ON_BN_CLICKED(IDC_RADIO_SAVE, OnRadioSave)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_SAVE, OnButtonBrowseSave)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCEDPARAMS, OnButtonAdvancedparameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogOrthogonalScaler message handlers

//------------------------------------------------------------------
// Method........: OnRadio...
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogOrthogonalScaler::OnRadioDiscard() 
{
	m_BrowseButtonSave.EnableWindow(FALSE);
	m_EditFilenameSave.EnableWindow(FALSE);
	
	m_Mode = Scaler::MODE_DISCARD;

}

void CDialogOrthogonalScaler::OnRadioSave() 
{
	m_BrowseButtonSave.EnableWindow(TRUE);
	m_EditFilenameSave.EnableWindow(TRUE);

	m_Mode = Scaler::MODE_SAVE;

}

//------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogOrthogonalScaler::OnInitDialog() 
{

	CDialog::OnInitDialog();

	// Check if the algorithm is the correct type.
	if (!m_Algorithm->IsA(ORTHOGONALSCALER)) { 
		Message::Error("Algorithm was not a OrthogonalScaler");
		CDialog::OnCancel();
		return FALSE;
  }
  
	// Cast to a OrthogonalScaler.
	m_alg = dynamic_cast(OrthogonalScaler *, m_Algorithm.GetPointer());

	// Set window text.
	SetWindowText(IdHolder::GetDescription(m_alg->GetId()).GetBuffer());

	// Initialize radiobuttonstates.
	m_Mode = m_alg->GetMode();
	m_RadioDiscard.SetCheck(m_Mode == Scaler::MODE_DISCARD);
	m_RadioSave.SetCheck(m_Mode == Scaler::MODE_SAVE);

	m_MaskSymbolic.SetCheck(m_alg->MaskSymbolic());

	// Hack: Disable MODE_DISCARD to force dictionary to get built.
	m_RadioDiscard.EnableWindow(FALSE);
	
	// Initialize windowstates
	m_EditFilenameSave.EnableWindow(m_Mode == Scaler::MODE_SAVE);
	m_BrowseButtonSave.EnableWindow(m_Mode == Scaler::MODE_SAVE);
		
	String text;

	// Initialize filename for saving
	text = m_alg->GetFilename();
	if (text != Undefined::String())
		m_EditFilenameSave.SetWindowText(text.GetBuffer());

	// Initialize advanced stuff.
	m_ButtonAdvancedParams.EnableWindow(HasAdvancedParameters());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogOrthogonalScaler::OnOK() 
{
	
	CString filename;
				
	// Set the filename for saving.
	m_EditFilenameSave.GetWindowText(filename);
	if (!filename.IsEmpty()) 
		m_alg->SetFilename(filename.GetBuffer(0));
	else if (m_Mode == Scaler::MODE_SAVE) {
		Message::Error("You must supply a filename.");
		return;
	}
	
	// Set chosen mode.
	m_alg->SetMode(m_Mode);

	// Set mask symbolic flag.
	m_alg->MaskSymbolic(m_MaskSymbolic.GetCheck());
	
	CDialog::OnOK();
}

//------------------------------------------------------------------
// Method........: 
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogOrthogonalScaler::OnButtonBrowseSave() 
{
	
	CFileDialog fileDlg(TRUE, NULL, "*.*");
  
	if (fileDlg.DoModal() == IDOK)
  {
    // Get the path.
    CString fullpath = fileDlg.GetPathName();
		// Insert the path in the editbox.
		m_EditFilenameSave.SetWindowText(fullpath);

	}
	
}

//------------------------------------------------------------------
// Method........: HasAdvancedParameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for scalers with special options.
// Revisions.....:
//===================================================================

bool CDialogOrthogonalScaler::HasAdvancedParameters() const
{
	return false;
}

//------------------------------------------------------------------
// Method........: OnButtonAdvancedparameters
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Overload this for scalers with special options.
// Revisions.....:
//===================================================================

void CDialogOrthogonalScaler::OnButtonAdvancedparameters() 
{
}
 
