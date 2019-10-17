#if !defined(AFX_DIALOGDISCERNIBILITYFUNCTIONEXPORTER_H__6675D2A2_E419_11D1_8FA7_00A0C925D262__INCLUDED_)
#define AFX_DIALOGDISCERNIBILITYFUNCTIONEXPORTER_H__6675D2A2_E419_11D1_8FA7_00A0C925D262__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogdiscernibilityfunctionexporter.h : header file
//

#include <frontend/dialogs/algorithmdialogs/dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogDiscernibilityFunctionExporter dialog

class CDialogDiscernibilityFunctionExporter : public CDialogAlgorithm
{
// Construction
public:
	CDialogDiscernibilityFunctionExporter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDiscernibilityFunctionExporter)
	enum { IDD = IDD_DISCERNIBILITYFUNCTIONEXPORTER };
	CButton	m_RadioDiscernibilityFull;
	CButton	m_RadioDiscernibilityObject;
	CEdit	m_EditFilenameIDG;
	CComboBox	m_ComboNames;
	CButton	m_ButtonObjectSelection;
	CButton	m_CheckUseNames;
	CButton	m_CheckUseIDG;
	CButton	m_CheckModuloDecision;
	CButton	m_CheckSimplify;
	CButton	m_ButtonBrowseIDG;
	CString	m_FilenameIDG;
	CString	m_FilenameOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDiscernibilityFunctionExporter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDiscernibilityFunctionExporter)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioDiscernibilityFull();
	afx_msg void OnRadioDiscernibilityObject();
	afx_msg void OnButtonObjectselection();
	afx_msg void OnButtonOutputbrowse();
	afx_msg void OnButtonIdgbrowse();
	afx_msg void OnCheckIdg();
	afx_msg void OnCheckUsenames();
	afx_msg void OnCheckSimplify();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDISCERNIBILITYFUNCTIONEXPORTER_H__6675D2A2_E419_11D1_8FA7_00A0C925D262__INCLUDED_)
 
