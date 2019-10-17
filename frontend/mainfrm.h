
#ifndef __MAINFRM_H__
#define __MAINFRM_H__

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

private:

	BOOL InitStatusBar(UINT *pIndicators, int nSize, int nSeconds);

public:

	CMainFrame();

	int         m_nDatePaneNo;
	int         m_nTimePaneNo;

	CDialogBar  m_DialogMessages;

	void    WarningAddString(const CString &message);
	void    ProgressAddString(const CString &message);

	void    StatusBarSetText(const CString &message);
	CString StatusBarGetText() const;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//RA 0406:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle); 
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

	afx_msg void OnUpdateDate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTime(CCmdUI* pCmdUI);
	afx_msg void OnButtonMessageBarCopy();
	afx_msg void OnButtonMessageBarClear();
	afx_msg void OnRosettaHelp();

	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
 
