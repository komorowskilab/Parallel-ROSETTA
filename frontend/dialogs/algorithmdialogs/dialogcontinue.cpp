
#include <stdafx.h>
#include <resource.h>

#include <frontend\dialogs\algorithmdialogs\dialogcontinue.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogContinue dialog

CDialogContinue::CDialogContinue(UINT nCaptionID)
{
	m_nCaptionID = CG_IDS_PROGRESS_CAPTION;
	if (nCaptionID != 0)
		m_nCaptionID = nCaptionID;

  m_bCancel=FALSE;
  //{{AFX_DATA_INIT(CDialogContinue)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_bParentDisabled = FALSE;
}

CDialogContinue::~CDialogContinue()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CDialogContinue::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CDialogContinue::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CDialogContinue::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CDialogContinue::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CDialogContinue::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDialogContinue)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogContinue, CDialog)
    //{{AFX_MSG_MAP(CDialogContinue)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString CDialogContinue::GetStatus() const {

    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(IDC_SIMPLEPROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);

		CString text;

    pWndStatus->GetWindowText(text);

		return text;

}

void CDialogContinue::SetStatus(LPCTSTR lpszMessage)
{
    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(IDC_SIMPLEPROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);
    pWndStatus->SetWindowText(lpszMessage);
}

void CDialogContinue::OnCancel()
{
    m_bCancel=TRUE;
}


void CDialogContinue::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

BOOL CDialogContinue::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;

    return bResult;
}
    
/////////////////////////////////////////////////////////////////////////////
// CDialogContinue message handlers

BOOL CDialogContinue::OnInitDialog() 
{
    CDialog::OnInitDialog();

	CString strCaption;
	VERIFY(strCaption.LoadString(m_nCaptionID));
    SetWindowText(strCaption);

    return TRUE;  
}
 
