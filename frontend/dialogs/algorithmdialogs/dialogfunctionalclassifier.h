#if !defined(AFX_DIALOGFUNCTIONALCLASSIFIER_H__49974CF3_6738_4197_BB68_EDB1D88820FD__INCLUDED_)
#define AFX_DIALOGFUNCTIONALCLASSIFIER_H__49974CF3_6738_4197_BB68_EDB1D88820FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogfunctionalclassifier.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogFunctionalClassifier dialog

class CDialogFunctionalClassifier : public CDialogAlgorithm
{
// Construction
public:
	CDialogFunctionalClassifier(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogFunctionalClassifier)
	enum { IDD = IDD_FUNCTIONAL_CLASSIFIER };
	CButton	m_ButtonAdvancedParams;
	CButton	m_ButtonTemplateDefs;
	CButton	m_CCheckRand;
	CStatic	m_StaticRulesFile;
	CStatic	m_StaticRandFile;
	CStatic	m_StaticClassificationFile;
	CEdit	m_CEditRand;
	CEdit	m_EditFilenameRand;
	CButton	m_ButtonBrowseRand;
	CButton	m_Help;
	CStatic	m_CVRep;
	CStatic	m_CVNum;
	CStatic	m_CVFile;
	CEdit	m_CEditRNG;
	CEdit	m_CEditCV;
	CEdit	m_EditFilenameCV;
	CButton	m_ButtonBrowseCV;
	CButton	m_ButtonBrowseRules;
	CEdit	m_EditFilenameRules;
	CButton	m_ButtonBrowseClassification;
	CEdit	m_EditFilenameClassification;
	BOOL	m_CheckCV;
	int		m_EditCV;
	int		m_EditRNG;
	BOOL	m_CheckRandomization;
	BOOL	m_CheckClassify;
	int		m_EditRand;
	BOOL	m_CheckRand;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFunctionalClassifier)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogFunctionalClassifier)
	virtual void OnOK();
	afx_msg void OnButtonBrowseClassification();
	afx_msg void OnButtonBrowseRules();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowseCV();
	afx_msg void OnCheckCV();
	afx_msg void OnHelp();
	afx_msg void OnButtonBrowseRand();
	afx_msg void OnCheckClassify();
	afx_msg void OnCheckRand();
	afx_msg void OnButtonTemplateDefs();
	afx_msg void OnButtonAdvancedParams();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGFUNCTIONALCLASSIFIER_H__49974CF3_6738_4197_BB68_EDB1D88820FD__INCLUDED_)
 
