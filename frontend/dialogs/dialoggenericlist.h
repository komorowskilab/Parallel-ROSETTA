// dialoggenericlist.h : header file
//

#ifndef __DIALOG__GENERICLIST___
#define __DIALOG__GENERICLIST___

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\basic\macros.h>
#include <kernel\basic\string.h>
#include <kernel\basic\vector.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogGenericList dialog

class CDialogGenericList : public CDialogAlgorithm
{
// Construction
public:
	CDialogGenericList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogGenericList)
	enum { IDD = IDD_DIALOG_GENERICLIST };
	CListCtrl	m_ListEntries;
	CString	m_StaticHeader;
	//}}AFX_DATA

	// Dialog "input":
	Vector(String)      m_Entries;   // List entries (texts).
	String              m_Header;    // List header.
	String              m_Title;     // Dialog title.
	bool                m_CanEdit;   // Enable editing by single-click?
	bool                m_CanSelect; // Enable selection by double-click?

	// Dialog "output":
	int                 m_Selection; // Index of selected entry.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGenericList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogGenericList)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListEntries(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditListEntries(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListEntries(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
 
