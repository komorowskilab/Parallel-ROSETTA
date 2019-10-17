// dialogparametersgeneral.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogparametersgeneral.h>

#include <kernel\algorithms\keyword.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogParametersGeneral dialog


CDialogParametersGeneral::CDialogParametersGeneral(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogParametersGeneral::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogParametersGeneral)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogParametersGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogParametersGeneral)
	DDX_Control(pDX, IDC_EDIT_PARAMS, m_EditParams);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogParametersGeneral, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogParametersGeneral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogParametersGeneral message handlers

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CDialogParametersGeneral::OnInitDialog() 
{

	CDialogAlgorithm::OnInitDialog();
	
	// Check that algorithm is set.
	if (m_Algorithm.IsNULL()) {
		Message::Error("Algorithm not set.");
		EndDialog(IDABORT);
		return FALSE;
	}

	// Get algorithm name.
	String name = IdHolder::GetDescription(m_Algorithm->GetId());

	// Set dialog title.
	SetWindowText(name.GetBuffer());

	// Get current parameters.
	String params = m_Algorithm->GetParameters();

	// Get field separator.
	String separator = Keyword::Separator();

	// Insert carriage returns after each field separator.
	params.Replace(separator, separator + "\r\n");

	// Fill the edit control.
	m_EditParams.SetWindowText(params.GetBuffer());

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

void CDialogParametersGeneral::OnOK() 
{

	CString parameters;

	// Get contents of edit control.
	m_EditParams.GetWindowText(parameters);

	// Convert from CString to String.
	String params = parameters.GetBuffer(0);

	// Remove carriage returns.
	while (params.Contains('\n'))
		params.Delete('\n');
	while (params.Contains('\r'))
		params.Delete('\r');

	// Parse contents.
	if (!m_Algorithm->SetParameters(params)) {
		Message::Error("Parsing of parameters failed.");
		EndDialog(IDABORT);
		return;
	}
	
	CDialogAlgorithm::OnOK();

}
 
