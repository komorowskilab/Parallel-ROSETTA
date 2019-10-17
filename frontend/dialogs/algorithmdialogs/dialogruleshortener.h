#if !defined(AFX_DIALOGRULESHORTENER_H__D612F3FF_29C0_49CE_855C_A1D025DA7E29__INCLUDED_)
#define AFX_DIALOGRULESHORTENER_H__D612F3FF_29C0_49CE_855C_A1D025DA7E29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// Ewa Makosa
// CDialogRuleShortener dialog

class CDialogRuleShortener : public CDialogAlgorithm
{
// Construction
public:
	CDialogRuleShortener(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRuleShortener)
	enum { IDD = IDD_RULESHORTENER};
	CButton	m_ButtonBrowse;
	CEdit	m_EditFilename;
	CButton	m_CheckUseIDG;
	float	m_Threshold;
	CString	m_Filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRuleShortener)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRuleShortener)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckUseIDG();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDIALOGRULESHORTENER_H__D612F3FF_29C0_49CE_855C_A1D025DA7E29__INCLUDED_)
 
