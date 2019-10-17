#if !defined(AFX_DIALOGBATCHCLASSIFIERCALRES_H__31CB7081_97EA_11D1_8FA2_00A0C925D262__INCLUDED_)
#define AFX_DIALOGBATCHCLASSIFIERCALRES_H__31CB7081_97EA_11D1_8FA2_00A0C925D262__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogbatchclassifiercalres.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogBatchClassifierCalRes dialog

class CDialogBatchClassifierCalRes : public CDialogAlgorithm
{
// Construction
public:
	CDialogBatchClassifierCalRes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogBatchClassifierCalRes)
	enum { IDD = IDD_BATCHCLASSIFIER_CALRES };
	CEdit	m_EditGroups;
	CEdit	m_EditFilenameROC;
	CEdit	m_EditFilenameCal;
	CComboBox	m_ComboROCClass;
	CComboBox	m_ComboCalClass;
	CButton	m_CheckROC;
	CButton	m_CheckCal;
	CButton	m_ButtonBrowseROC;
	CButton	m_ButtonBrowseCal;
	CButton	m_ButtonMaximize;
	int		m_Groups;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogBatchClassifierCalRes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogBatchClassifierCalRes)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowseCal();
	afx_msg void OnButtonBrowseRoc();
	afx_msg void OnCheckCalibration();
	afx_msg void OnCheckRoc();
	virtual void OnOK();
	afx_msg void OnButtonMaximize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGBATCHCLASSIFIERCALRES_H__31CB7081_97EA_11D1_8FA2_00A0C925D262__INCLUDED_)
 
