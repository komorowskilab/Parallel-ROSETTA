// dialogreductsstatistics.h : header file
//

#include <frontend\dialogs\mylistctrl.h>

#include <kernel\structures\reducts.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReductsStatistics dialog

class CDialogReductsStatistics : public CDialog
{
// Attributes
private:

  Handle<Reducts> m_Reducts;

// Operations
private:

  void CalculateStatistics();

public:

  void SetReducts(Reducts *reducts);

// Construction
public:
	CDialogReductsStatistics(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogReductsStatistics)
	enum { IDD = IDD_REDUCTS_STATISTICS };
	CMyListCtrl	m_HistogramAttribute;
	CMyListCtrl	m_HistogramLength;
	CMyListCtrl	m_HistogramStrength;
	CString	m_Size;
	CString	m_Name;
	CString m_MeanLength;
	CString	m_StdDevLength;
	CString	m_MedianLength;
	CString	m_MinimumLength;
	CString	m_MaximumLength;
	CString m_MeanStrength;
	CString	m_StdDevStrength;
	CString	m_MedianStrength;
	CString	m_MinimumStrength;
	CString	m_MaximumStrength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogReductsStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogReductsStatistics)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
