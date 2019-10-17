#if !defined(AFX_DIALOGINDISCERNIBILITYGRAPHEXPORTER_H__6675D2A1_E419_11D1_8FA7_00A0C925D262__INCLUDED_)
#define AFX_DIALOGINDISCERNIBILITYGRAPHEXPORTER_H__6675D2A1_E419_11D1_8FA7_00A0C925D262__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogindiscernibilitygraphexporter.h : header file
//

#include <frontend/dialogs/algorithmdialogs/dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogIndiscernibilityGraphExporter dialog

class CDialogIndiscernibilityGraphExporter : public CDialogAlgorithm
{
// Construction
public:
	CDialogIndiscernibilityGraphExporter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogIndiscernibilityGraphExporter)
	enum { IDD = IDD_INDISCERNIBILITYGRAPHEXPORTER };
	CButton	m_CheckReflexive;
	CStatic	m_NameText;
	CStatic	m_IDGText;
	CStatic	m_CardText2;
	CStatic	m_CardText1;
	CEdit	m_EditCard;
	CButton	m_CheckCard;
	CButton	m_CheckDegree;
	CButton	m_CheckAPSP;
	CEdit	m_EditFilenameIDG;
	CComboBox	m_ComboNames;
	CButton	m_CheckUseNames;
	CButton	m_CheckUseIDG;
	CButton	m_CheckModuloDecision;
	CButton	m_ButtonBrowseIDG;
	CString	m_FilenameIDG;
	CString	m_FilenameOutput;
	int		m_Card;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogIndiscernibilityGraphExporter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogIndiscernibilityGraphExporter)
	afx_msg void OnButtonOutputbrowse();
	afx_msg void OnButtonIdgbrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckIdg();
	afx_msg void OnCheckUsenames();
	virtual void OnOK();
	afx_msg void OnCheckCard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGINDISCERNIBILITYGRAPHEXPORTER_H__6675D2A1_E419_11D1_8FA7_00A0C925D262__INCLUDED_)
 
