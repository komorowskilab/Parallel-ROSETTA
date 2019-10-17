#if !defined(AFX_DIALOGQUALITYRULEFILTERLOOP_H__7E481A82_8394_11D2_9580_006008A066CE__INCLUDED_)
#define AFX_DIALOGQUALITYRULEFILTERLOOP_H__7E481A82_8394_11D2_9580_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogqualityrulefilterloop.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogQualityRuleFilterLoop dialog

class CDialogQualityRuleFilterLoop : public CDialogAlgorithm
{
// Construction
public:
	CDialogQualityRuleFilterLoop(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogQualityRuleFilterLoop)
	enum { IDD = IDD_RULEFILTER_QUALITY_LOOP };
	CButton	m_ButtonClassifierParams;
	CEdit	m_EditFixed;
	CEdit	m_EditDynamic;
	CComboBox	m_ComboROCFocus;
	CComboBox	m_ComboDecisionTable;
	CComboBox	m_ComboClassifier;
	float	m_Dynamic;
	float	m_Fallback;
	CString	m_Filename;
	int		m_Fixed;
	CButton m_RadioFixed;
	CButton m_RadioDynamic;
	int		m_ResolutionLimit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogQualityRuleFilterLoop)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	Identifier::Handles   m_AvailableDT;  // Available decision tables.
	int                   m_SelectedDT;   // Index of selected decision table.
	Algorithm::Handles    m_AvailableC;   // Available classifiers.
	int                   m_SelectedC;    // Index of selected classifier.

	void UpdateROCCombo();

	// Generated message map functions
	//{{AFX_MSG(CDialogQualityRuleFilterLoop)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonClassificationParameters();
	afx_msg void OnButtonQmeasure();
	afx_msg void OnSelchangeComboClassifier();
	afx_msg void OnSelchangeComboDecisiontable();
	afx_msg void OnRadioDynamic();
	afx_msg void OnRadioFixed();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGQUALITYRULEFILTERLOOP_H__7E481A82_8394_11D2_9580_006008A066CE__INCLUDED_)
 
