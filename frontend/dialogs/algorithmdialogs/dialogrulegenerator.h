#if !defined(AFX_DIALOGRULEGENERATOR_H__D1571963_434F_11D3_95C0_006008A066CE__INCLUDED_)
#define AFX_DIALOGRULEGENERATOR_H__D1571963_434F_11D3_95C0_006008A066CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogrulegenerator.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogRuleGenerator dialog

class CDialogRuleGenerator : public CDialogAlgorithm
{
// Construction
public:
	CDialogRuleGenerator(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRuleGenerator)
	enum { IDD = IDD_RULEGENERATOR };
	CButton	m_ButtonAdvancedParams;
	CButton	m_ButtonGroup;
	CEdit	m_EditFilename;
	CButton	m_CheckUseIDG;
	CButton	m_ButtonBrowse;
	CString	m_Filename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRuleGenerator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRuleGenerator)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckUseidg();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void OnButtonAdvancedparams();

	virtual bool HasIDGSupport() const;
	virtual bool HasAdvancedParameters() const;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGRULEGENERATOR_H__D1571963_434F_11D3_95C0_006008A066CE__INCLUDED_)
 
