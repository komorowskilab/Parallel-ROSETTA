#if !defined(AFX_DIALOGQUALITYRULEFILTER_H__7E481A81_8394_11D2_9580_006008A066CE__INCLUDED_)
#define AFX_DIALOGQUALITYRULEFILTER_H__7E481A81_8394_11D2_9580_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogqualityrulefilter.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogQualityRuleFilter dialog

class CDialogQualityRuleFilter : public CDialogAlgorithm
{
// Construction
public:
	CDialogQualityRuleFilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogQualityRuleFilter)
	enum { IDD = IDD_RULEFILTER_QUALITY };
	CButton	m_CheckRemoveUndefined;
	CButton	m_CheckUseUpper;
	CButton	m_CheckUseLower;
	CButton	m_CheckInvert;
	CEdit	m_EditWeight;
	CEdit	m_EditUpper;
	CEdit	m_EditLower;
	CComboBox	m_ComboNormalization;
	BOOL	m_Invert;
	BOOL	m_UseLower;
	BOOL	m_UseUpper;
	float	m_Lower;
	float	m_Upper;
	float	m_Weight;
	CButton m_RadioMichalski;
	CButton m_RadioTorgo;
	CButton m_RadioBrazdil;
	CButton m_RadioPearson;
	CButton m_RadioG2;
	CButton m_RadioJ;
	CButton m_RadioCohen;
	CButton m_RadioColeman;
	CButton m_RadioKononenko;
	BOOL	m_RemoveUndefined;
	//}}AFX_DATA

	BOOL m_EnableOnlyQualitySelection; // Disable everything except quality measure selection?
	BOOL m_EnableUndefinedSelection;   // ..with the possible exception of the "remove undefined" option?

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogQualityRuleFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogQualityRuleFilter)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckUselower();
	afx_msg void OnCheckUseupper();
	afx_msg void OnRadioBrazdil();
	afx_msg void OnRadioCohen();
	afx_msg void OnRadioColeman();
	afx_msg void OnRadioG2();
	afx_msg void OnRadioJ();
	afx_msg void OnRadioKononenko();
	afx_msg void OnRadioMichalski();
	afx_msg void OnRadioPearson();
	afx_msg void OnRadioTorgo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGQUALITYRULEFILTER_H__7E481A81_8394_11D2_9580_006008A066CE__INCLUDED_)
 
