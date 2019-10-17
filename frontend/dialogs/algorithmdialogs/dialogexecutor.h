// dialogexecutor.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogExecutor dialog

class CDialogExecutor : public CDialogAlgorithm
{
// Construction
public:
	CDialogExecutor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogExecutor)
	enum { IDD = IDD_EXECUTOR };
	CComboBox	m_ComboReturn;
	BOOL	m_Return;
	CString	m_CmdFilename;
	CString	m_LogFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogExecutor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogExecutor)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonBrowseCmd();
	afx_msg void OnButtonBrowseLog();
	afx_msg void OnCheckReturn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
