// dialogattributename.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeName dialog

class CDialogAttributeName : public CDialog
{
// Construction
public:
	CString m_Unit;
	CString m_Name;
	CDialogAttributeName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAttributeName)
	enum { IDD = IDD_ATTRIBUTE_NAME };
	CEdit	m_EditUnit;
	CEdit	m_EditName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAttributeName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAttributeName)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
