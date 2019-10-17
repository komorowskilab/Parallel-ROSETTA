// dialogrulepruner.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogRulePruner dialog

class CDialogRulePruner : public CDialogAlgorithm
{
// Construction
public:
	CDialogRulePruner(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogRulePruner)
	enum { IDD = IDD_RULEPRUNER };
	CEdit	m_EditCovUpper;
	CEdit	m_EditCovLower;
	CButton	m_CheckCoverage;
	CEdit	m_EditStabUpper;
	CEdit	m_EditStabLower;
	CButton	m_CheckStab;
	CComboBox	m_ComboValue;
	CComboBox	m_ComboAttribute;
	CButton	m_CheckValue;
	CButton	m_CheckDominating;
	CEdit	m_EditLengthUpper;
	CEdit	m_EditLengthLower;
	CButton	m_CheckLength;
	CEdit	m_EditSuppUpper;
	CEdit	m_EditSuppLower;
	CEdit	m_EditProbUpper;
	CEdit	m_EditProbLower;
	CButton	m_CheckSupp;
	CButton	m_CheckProb;
	CButton	m_CheckDec;
	CButton	m_RadioAnd;
	CButton	m_RadioOr;
	CComboBox	m_ComboDecision;
	BOOL	m_InvertLogic;
	float	m_ProbLower;
	float	m_ProbUpper;
	int		m_SuppLower;
	int		m_SuppUpper;
	int		m_LengthLower;
	int		m_LengthUpper;
	double	m_StabLower;
	double	m_StabUpper;
	float	m_CovLower;
	float	m_CovUpper;
	//}}AFX_DATA

	BOOL m_Masked; // Operate relative to a masked table.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogRulePruner)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogRulePruner)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckDecision();
	afx_msg void OnCheckAccuracy();
	afx_msg void OnCheckSupport();
	afx_msg void OnCheckLength();
	afx_msg void OnCheckValue();
	afx_msg void OnSelchangeComboAttribute();
	afx_msg void OnCheckStability();
	afx_msg void OnCheckCoverage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
