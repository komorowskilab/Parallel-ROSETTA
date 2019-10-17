// dialogstandardvoterparameters.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\basic\identifier.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogStandardVoterParameters dialog

class CDialogStandardVoterParameters : public CDialogAlgorithm
{
// Construction
public:
	CDialogStandardVoterParameters(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogStandardVoterParameters)
	enum { IDD = IDD_STANDARDVOTER_PARAMS };
	CButton	m_CheckMostSpecificOnly;
	CButton m_RadioVotingSupport;
	CButton m_RadioVotingSimple;
	CButton m_RadioNormFiring;
	CButton m_RadioNormAll;
	//}}AFX_DATA

	BOOL                m_EnableGeneralization; // Enable generalization option?

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogStandardVoterParameters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogStandardVoterParameters)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
