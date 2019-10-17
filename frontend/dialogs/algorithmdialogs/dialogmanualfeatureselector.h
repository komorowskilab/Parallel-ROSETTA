// #if !defined(AFX_ATTRIBUTESELECTOR_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_)
// #define AFX_ATTRIBUTESELECTOR_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogattributeselector.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogManualFeatureSelector dialog

class CDialogManualFeatureSelector : public CDialogAlgorithm
{
// Construction
public:
	CDialogManualFeatureSelector(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogManualFeatureSelector)
	enum { IDD = IDD_MANUALFEATURESELECTOR };
	CCheckListBox	m_ListAttributes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogManualFeatureSelector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogManualFeatureSelector)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonInvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// #endif // !defined(AFX_ATTRIBUTESELECTOR_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_)
 
