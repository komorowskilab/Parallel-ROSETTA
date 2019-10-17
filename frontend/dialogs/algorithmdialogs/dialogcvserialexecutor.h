// dialogcvserialexecutor.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogCVSerialExecutor dialog

class CDialogCVSerialExecutor : public CDialogAlgorithm
{
// Construction
public:
	CDialogCVSerialExecutor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogCVSerialExecutor)
	enum { IDD = IDD_EXECUTOR_CV };
	BOOL	m_InvertSampling;
	int		m_CVIterations;
	int		m_Seed;
	int		m_TrLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogCVSerialExecutor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogCVSerialExecutor)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCmdfile();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
