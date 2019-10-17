// dialogfilename.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogFilename dialog

class CDialogFilename : public CDialogAlgorithm
{
// Construction
public:
	CDialogFilename(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogFilename)
	enum { IDD = IDD_FILENAME };
	CEdit	m_EditFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFilename)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogFilename)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
