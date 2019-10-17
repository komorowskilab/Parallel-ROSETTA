// MainFrm.cpp : implementation of the CMainFrame class

// Revisions.....: 970315 Made all Dialogbars non-dockable. KMR
//===================================================================


#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\mainfrm.h>
#include <frontend\splash.h>

#include <kernel\basic\message.h>

#include <common\messagehelper.h>
#include <common\objectmanager.h>
#include <common\modemanager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DATE, OnUpdateDate)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME, OnUpdateTime)
	ON_COMMAND_EX(CG_ID_VIEW_PROGRESS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_PROGRESS, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_HELP_FINDER, OnRosettaHelp)
	ON_COMMAND(ID_HELP, OnRosettaHelp)
	ON_COMMAND(ID_CONTEXT_HELP, OnRosettaHelp)
	ON_COMMAND(ID_DEFAULT_HELP, OnRosettaHelp)
	/*
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	*/

	ON_COMMAND(IDC_BUTTON_MESSAGEBAR_COPY, OnButtonMessageBarCopy)
	ON_COMMAND(IDC_BUTTON_MESSAGEBAR_CLEAR, OnButtonMessageBarClear)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here


}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

CMainFrame::~CMainFrame()
{
}

//-------------------------------------------------------------------
// Method........: OnCreate
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	{
		// Initialize dialog bar m_DialogMessages
		if (!m_DialogMessages.Create(this, IDD_MESSAGEBAR, 0, CG_ID_VIEW_PROGRESS))
		{
			TRACE0("Failed to create Dialog Bar\n");
			return -1;		// fail to create
		}

		m_DialogMessages.SetWindowText("Messages");

	  //m_DialogProgress.SetBarStyle(m_DialogProgress.GetBarStyle() |
		//CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

		m_DialogMessages.EnableDocking(0); // KMR CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP);
		EnableDocking(CBRS_ALIGN_ANY);
		ShowControlBar(&m_DialogMessages, FALSE, FALSE);
		FloatControlBar(&m_DialogMessages,CPoint(300,300));	// KMR

		// Aš: Add bitmaps to buttons.
		CButton *button_copy  = (CButton *) m_DialogMessages.GetDlgItem(IDC_BUTTON_MESSAGEBAR_COPY);
		CButton *button_clear = (CButton *) m_DialogMessages.GetDlgItem(IDC_BUTTON_MESSAGEBAR_CLEAR);
		
		if (button_copy) {
			HBITMAP bmp_copy = (HBITMAP) LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_COPY), IMAGE_BITMAP, 0, 0, LR_COLOR);
			button_copy->SetBitmap(bmp_copy);
		}
		
		if (button_clear) {
			HBITMAP bmp_clear = (HBITMAP) LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CUT), IMAGE_BITMAP, 0, 0, LR_COLOR);
			button_clear->SetBitmap(bmp_clear);
		}

	}


	CWndSplash::ShowSplashScreen(this);
	{
		// Find out the size of the static variable 'indicators' defined
		// by AppWizard and copy it
		int nOrigSize = sizeof(indicators) / sizeof(UINT);

		UINT* pIndicators = new UINT[nOrigSize + 2];
		memcpy(pIndicators, indicators, sizeof(indicators));

		// Call the Status Bar Component's status bar creation function
		if (!InitStatusBar(pIndicators, nOrigSize, 60))
		{
			TRACE0("Failed to initialize Status Bar\n");
			return -1;
		}
		delete[] pIndicators;
	}

	return 0;
}

//-------------------------------------------------------------------
// Method........: PreCreateWindow
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

//-------------------------------------------------------------------
// Method........: XAddString
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Use CListBox insead of CEdit due to potential
//                 massive overhead for string appends if floaded with
//                 messages.
// Revisions.....: 
//-------------------------------------------------------------------

void CMainFrame::WarningAddString(const CString &message)
{

	/*
  CEdit *edit = (CEdit *) m_DialogMessages.GetDlgItem(IDC_WARNINGHISTORY);

	if (edit != NULL) {
		CString buffer;
		edit->GetWindowText(buffer);
		if (!buffer.IsEmpty())
			buffer += "\r\n";
		buffer += message;
		edit->SetWindowText(buffer);
		edit->LineScroll(edit->GetLineCount(), 0);
	}
	*/

	CListBox *listbox = (CListBox *) m_DialogMessages.GetDlgItem(IDC_WARNINGHISTORY);

	if (listbox != NULL) {
		listbox->AddString(message);
		listbox->SetCurSel(listbox->GetCount() - 1);
		listbox->UpdateWindow();
	}

}

void CMainFrame::ProgressAddString(const CString &message)
{

	/*
  CEdit *edit = (CEdit *) m_DialogMessages.GetDlgItem(IDC_NOTIFYHISTORY);

	if (edit != NULL) {
		CString buffer;
		edit->GetWindowText(buffer);
		if (!buffer.IsEmpty())
			buffer += "\r\n";
		buffer += message;
		edit->SetWindowText(buffer);
		edit->LineScroll(edit->GetLineCount(), 0);
	}
	*/

	CListBox *listbox = (CListBox *) m_DialogMessages.GetDlgItem(IDC_NOTIFYHISTORY);

	if (listbox != NULL) {
		listbox->AddString(message);
		listbox->SetCurSel(listbox->GetCount() - 1);
		listbox->UpdateWindow();
	}
  
}

//-------------------------------------------------------------------
// Method........: OnButtonMessageBarCopy
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CMainFrame::OnButtonMessageBarCopy()
{

	Message message;

	CListBox *listbox1 = (CListBox *) m_DialogMessages.GetDlgItem(IDC_NOTIFYHISTORY);
	CListBox *listbox2 = (CListBox *) m_DialogMessages.GetDlgItem(IDC_WARNINGHISTORY);

	// Open clipboard.
  if (!OpenClipboard()) {
		message.Error("Cannot open the Clipboard.");
    return;
  }

  // Remove the current Clipboard contents
  if (!EmptyClipboard()) {
		message.Error("Cannot empty the Clipboard.");
		CloseClipboard();
    return;
  }

	CString text, line;

  // Create a string with the current data.
	if (listbox1) {
		for (int i = 0; i < listbox1->GetCount(); i++) {
			listbox1->GetText(i, line);
			text += line;
			text += '\n';
		}
		if (listbox1->GetCount() > 0)
			text += '\n';
	}

	if (listbox2) {
		for (int i = 0; i < listbox2->GetCount(); i++) {
			listbox2->GetText(i, line);
			text += line;
			text += '\n';
		}
	}

	// Prepare for copying.
	HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.GetLength() + 1);
	char *buffer = (char *) GlobalLock(clipbuffer);
	strcpy(buffer, LPCSTR(text));
	GlobalUnlock(clipbuffer);

	// Copy text to clipboard.
	if (!SetClipboardData(CF_TEXT, clipbuffer)) {
		message.Error("Cannot copy text to Clipboard.");
		CloseClipboard();
		return;
	}

	// Close clipboard.
  CloseClipboard();

	message.Notify("Messages copied to Clipboard.");

}

//-------------------------------------------------------------------
// Method........: OnButtonMessageBarClear
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CMainFrame::OnButtonMessageBarClear()
{

	/*
	CEdit *edit;

	edit = (CEdit *) m_DialogMessages.GetDlgItem(IDC_NOTIFYHISTORY);
	if (edit != NULL)
		edit->SetWindowText("");

	edit = (CEdit *) m_DialogMessages.GetDlgItem(IDC_WARNINGHISTORY);
	if (edit != NULL)
		edit->SetWindowText("");
	*/

	CListBox *listbox;

	listbox = (CListBox *) m_DialogMessages.GetDlgItem(IDC_NOTIFYHISTORY);
	if (listbox != NULL)
		listbox->ResetContent();

	listbox = (CListBox *) m_DialogMessages.GetDlgItem(IDC_WARNINGHISTORY);
	if (listbox != NULL)
		listbox->ResetContent();

}

//-------------------------------------------------------------------
// Method........: OnUpdateDate
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CMainFrame::OnUpdateDate(CCmdUI* pCmdUI)
{

	// Get current date and format it
	CTime time = CTime::GetCurrentTime();
	CString strDate = time.Format(_T("%A, %B %d, %y "));

	CSize size;
	{
		HGDIOBJ hOldFont = NULL;
		HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
		CClientDC dc(NULL);
		if (hFont != NULL) 
			hOldFont = dc.SelectObject(hFont);
		size = dc.GetTextExtent(strDate);
		if (hOldFont != NULL) 
			dc.SelectObject(hOldFont);
	}

	// Update the pane to reflect the current date
	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(m_nDatePaneNo, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(m_nDatePaneNo, nID, nStyle, size.cx);
	pCmdUI->SetText(strDate);
	pCmdUI->Enable(TRUE);

}

//-------------------------------------------------------------------
// Method........: OnUpdateTime
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

void CMainFrame::OnUpdateTime(CCmdUI* pCmdUI)
{

	// Get current date and format it
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("%X"));

	CSize size;
	{
		HGDIOBJ hOldFont = NULL;
		HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
		CClientDC dc(NULL);
		if (hFont != NULL) 
			hOldFont = dc.SelectObject(hFont);
		size = dc.GetTextExtent(strTime);
		if (hOldFont != NULL) 
			dc.SelectObject(hOldFont);
	}

	// Update the pane to reflect the current time
	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(m_nTimePaneNo, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(m_nTimePaneNo, nID, nStyle, size.cx);
	pCmdUI->SetText(strTime);
	pCmdUI->Enable(TRUE);

}

//-------------------------------------------------------------------
// Method........: InitStatusBar
// Author........: J—rn E. Nygjerd
// Date..........: 960417
// Description...: 
// Comments......: 
// Revisions.....:
//-------------------------------------------------------------------

BOOL CMainFrame::InitStatusBar(UINT *pIndicators, int nSize, int nSeconds)
{

	// Create an index for the DATE pane
	m_nDatePaneNo = nSize++;
	pIndicators[m_nDatePaneNo] = ID_INDICATOR_DATE;
	// Create an index for the TIME pane
	m_nTimePaneNo = nSize++;
	nSeconds = 1;
	pIndicators[m_nTimePaneNo] = ID_INDICATOR_TIME;

	// TODO: Select an appropriate time interval for updating
	// the status bar when idling.
	m_wndStatusBar.SetTimer(0x1000, nSeconds * 1000, NULL);

	return m_wndStatusBar.SetIndicators(pIndicators, nSize);

}

//-------------------------------------------------------------------
// Method........: OnRosettaHelp
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Deflects request for help, see macros at the top.
// Comments......: 
// Revisions.....: RA: 050321
//                 Start explorer with rosetta homepage
//-------------------------------------------------------------------

void CMainFrame::OnRosettaHelp()
{

	if (system("explorer \"http://rosetta.lcb.uu.se/\"") == -1) {
	
		String text = "The help is available on the web.\n"
			          "Point your browser to:\n\n"
					  "http://rosetta.lcb.uu.se/";

		MessageHelper::Query(text, "Help", MessageHelper::BUTTONS_OK, MessageHelper::ICON_INFORMATION);
	}

}

//-------------------------------------------------------------------
// Method........: StatusBarSetText/StatusBarGetText
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: Aš 9807: Added UpdateWindow() call.
//                 Aš 9905: Trim leading spaces.
//-------------------------------------------------------------------

void CMainFrame::StatusBarSetText(const CString &message) 
{
	if (!message.IsEmpty()) {
		if (message[0] == ' ') {
			CString trimmed(message);
			trimmed.TrimLeft();
			m_wndStatusBar.SetPaneText(0, trimmed);
		}
		else {
			m_wndStatusBar.SetPaneText(0, message);
		}
		m_wndStatusBar.UpdateWindow();
	}
}

CString CMainFrame::StatusBarGetText() const 
{
	return m_wndStatusBar.GetPaneText(0);
}


/////////////////////////////

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle){
	
	CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
	if (ModeManager::IsMode(ModeManager::GENOMIC)){
		HICON genIcon = AfxGetApp()->LoadIcon(IDI_GENOMIC);
		SetTitle("Genomic Rosetta");
		SetIcon(genIcon,FALSE);
	}
} 
