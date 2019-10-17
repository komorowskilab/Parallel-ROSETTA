// dialogattributestatistics.h : header file
//

#include <frontend\dialogs\mylistctrl.h>

#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>

#include <kernel\utilities\mathkit.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeStatistics dialog

class CDialogAttributeStatistics : public CDialog
{
// Construction
public:
	CDialogAttributeStatistics(CWnd* pParent = NULL);   // standard constructor

private:

	Handle<DecisionTable> m_Table;

public:

	int  m_NoAttribute;
	int  m_NoCorrelation;

	BOOL m_Masked; // Operate on a masked table?

// Dialog Data
	//{{AFX_DATA(CDialogAttributeStatistics)
	enum { IDD = IDD_ATTRIBUTE_STATISTICS };
	CComboBox	m_NameCorrelation;
	CListBox	m_NameMain;
	CMyListCtrl	m_Histogram;
	CString	m_Mean;
	CString	m_Correlation;
	CString	m_Median;
	CString	m_StDev;
	CString	m_Status;
	CString	m_Type;
	CString	m_NumberAttributes;
	CString	m_NumberObjects;
	CString	m_Unit;
	CString	m_Name;
	CString	m_MissingValuesWarning;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAttributeStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:
	
	void CalculateStatistics();

public:
	
	void SetDecisionTable(DecisionTable *table);



protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAttributeStatistics)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeMain();
	afx_msg void OnSelchangeCorrelation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
