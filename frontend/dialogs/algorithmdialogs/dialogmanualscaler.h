// dialogmanualscaler.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <frontend\dialogs\mylistctrl.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogManualScaler dialog

class CDialogManualScaler : public CDialogAlgorithm
{
// Construction
public:
	CDialogManualScaler(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogManualScaler)
	enum { IDD = IDD_MANUALSCALER };
	CButton	m_ButtonDeleteAll;
	CEdit	m_EditValue;
	CEdit	m_EditInterval;
	CButton	m_ButtonDelete;
	CButton	m_ButtonAdd;
	CButton	m_CheckViaDictionary;
	CMyListCtrl	m_ListMapping;
	CComboBox	m_ComboAttributes;
	//}}AFX_DATA

	BOOL m_Masked; // Operate on a masked table?

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogManualScaler)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogManualScaler)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnSelchangeComboAttributes();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickListMapping(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDeleteall();
	afx_msg void OnCheckViadictionary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
