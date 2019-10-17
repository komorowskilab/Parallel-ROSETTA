#if !defined(AFX_DIALOGRULEPAIRWISEGENERALIZER_H__0F34868C_AD4A_4976_8B40_B7A105178CFC__INCLUDED_)
#define AFX_DIALOGRULEPAIRWISEGENERALIZER_ATR_H__0F34868C_AD4A_4976_8B40_B7A105178CFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogRuleGeneralizer.h : header file
// Ewa Makosa

/////////////////////////////////////////////////////////////////////////////
// CDialogRulePairwiseGeneralizer dialog
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

class CDialogRulePairwiseGeneralizer  : public CDialogAlgorithm
{
// Construction
public:
	CDialogRulePairwiseGeneralizer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRulePairwiseGeneralizer)
	enum { IDD = IDD_RULEPAIRWISEGENERALIZER };
	CButton	m_CheckUseIDG;
	CEdit	m_EditFilename;
	CButton	m_ButtonBrowse;
	float	m_Threshold;
	CString	m_Filename;
	int	m_AttributeDifference;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRulePairwiseGeneralizer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRulePairwiseGeneralizer)
	afx_msg void OnButtonBrowse();
	afx_msg void OnCheckUseIDG();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGRULEPAIRWISEGENERALIZER_ATR_H__0F34868C_AD4A_4976_8B40_B7A105178CFC__INCLUDED_)
 
