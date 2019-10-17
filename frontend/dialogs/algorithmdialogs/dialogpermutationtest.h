// DialogBinarySplitter.h : header file
//
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\genomics\algorithms\permutationtest.h>


/////////////////////////////////////////////////////////////////////////////
// CDialogPermutationTest dialog

class CDialogPermutationTest : public CDialogAlgorithm
{
// Construction
public:
	CDialogPermutationTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPermutationTest)
	enum { IDD = IDD_PERMUTATIONTEST };
	CButton	m_Append;
	CButton m_Radio1;
	CButton m_Radio2;
	CButton m_ButtonLog;
	CEdit	m_EditLength;

	CEdit	m_EditSeed;
	CEdit m_EditNFolds;
	CEdit m_EditNPermutations;
	CEdit m_EditLogFilename;
	CEdit	m_EditCmdFilename;
	int		m_Seed;

	CString	m_CmdFilename;
	CString	m_LogFilename;
	int m_NFolds;
	int m_Length;
	int m_NPermutations;
	//}}AFX_DATA

private:
	
	Handle<PermutationTest> m_alg;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPermutationTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPermutationTest)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonBrowseCmd();
	afx_msg void OnButtonBrowseLog();
	afx_msg void OnRadio1();	
	afx_msg void OnRadio2();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
