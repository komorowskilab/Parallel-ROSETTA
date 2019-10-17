#if !defined(AFX_DIALOGOBJECTSELECTOR_H__9A919A31_E350_11D1_8FA7_00A0C925D262__INCLUDED_)
#define AFX_DIALOGOBJECTSELECTOR_H__9A919A31_E350_11D1_8FA7_00A0C925D262__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogobjectselector.h : header file
//

#include <kernel/algorithms/objectselector.h>
#include <kernel/structures/decisiontable.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogObjectSelector dialog

class CDialogObjectSelector : public CDialog
{
// Construction
public:
	CDialogObjectSelector(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogObjectSelector)
	enum { IDD = IDD_OBJECTSELECTOR };
	CEdit	m_EditFilename;
	CComboBox	m_ComboValue;
	CComboBox	m_ComboIndex;
	CComboBox	m_ComboAttribute;
	CButton	m_ButtonBrowse;
	CButton	m_RadioAll;
	CButton	m_RadioIndex;
	CButton	m_RadioValue;
	CButton	m_RadioFile;
	//}}AFX_DATA

	ObjectSelector        m_Selector;
	Handle<DecisionTable> m_Table;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogObjectSelector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdateValueCombo(const CComboBox &attribute_combo, CComboBox &value_combo);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogObjectSelector)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowse();
	afx_msg void OnSelchangeComboAttribute();
	afx_msg void OnRadioAll();
	afx_msg void OnRadioFile();
	afx_msg void OnRadioIndex();
	afx_msg void OnRadioValue();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGOBJECTSELECTOR_H__9A919A31_E350_11D1_8FA7_00A0C925D262__INCLUDED_)
 
