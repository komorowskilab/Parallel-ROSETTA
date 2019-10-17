// CG: This file was added by the Splash Screen component.

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CWndSplash : public CWnd
{
// Construction
protected:

	CWndSplash();

// Attributes:
public:

	CBitmap m_bitmap;

// Operations
public:

	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static void PreTranslateAppMessage(MSG* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndSplash)
	//}}AFX_VIRTUAL

// Implementation
public:

	~CWndSplash();
	virtual void PostNcDestroy();

protected:

	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CWndSplash* c_pSplashWnd;

// Generated message map functions
protected:

	//{{AFX_MSG(CWndSplash)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
 
