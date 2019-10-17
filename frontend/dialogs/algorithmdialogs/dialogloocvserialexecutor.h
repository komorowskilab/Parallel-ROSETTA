// dialogcvserialexecutor.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogLeaveOneOutCVSerialExecutor dialog

class CDialogLeaveOneOutCVSerialExecutor : public CDialogAlgorithm
{
// Construction
public:
	CDialogLeaveOneOutCVSerialExecutor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogLeaveOneOutCVSerialExecutor)
	enum { IDD = IDD_EXECUTOR_LOOCV };
	BOOL	m_InvertSampling;
	CButton m_EditCheck;
	CEdit m_EditInterations;
	int		m_CVIterations;
	int		m_Seed;
	int		m_TrLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogLeaveOneOutCVSerialExecutor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogLeaveOneOutCVSerialExecutor)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCmdfile();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
