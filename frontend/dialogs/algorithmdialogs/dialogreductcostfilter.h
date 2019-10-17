// dialogreductcostfilter.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReductCostFilter dialog

class CDialogReductCostFilter : public CDialogAlgorithm
{
// Construction
public:
	CDialogReductCostFilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogReductCostFilter)
	enum { IDD = IDD_REDUCTCOSTFILTER };
	BOOL	m_Invert;
	CString	m_CostFilename;
	CString	m_LogFilename;
	float	m_Threshold;
	float	m_DefaultCost;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogReductCostFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogReductCostFilter)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonBrowseCost();
	afx_msg void OnButtonBrowseLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
