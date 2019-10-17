// dialogreductpruner.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogReductPruner dialog

class CDialogReductPruner : public CDialogAlgorithm
{
// Construction
public:
	CDialogReductPruner(CWnd* pParent = NULL);   // standard constructor

	// Are attribute indices relative to masked or unmasked table?
	BOOL m_Masked;

// Dialog Data
	//{{AFX_DATA(CDialogReductPruner)
	enum { IDD = IDD_REDUCTPRUNER };
	CButton	m_CheckStrength;
	CButton	m_CheckLength;
	CButton	m_CheckAttribute;
	CButton	m_CheckInvertLogic;
	CButton	m_RadioAnd;
	CButton	m_RadioOr;
	CEdit	m_EditStrengthUpper;
	CEdit	m_EditStrengthLower;
	CEdit	m_EditLengthUpper;
	CEdit	m_EditLengthLower;
	CComboBox	m_ComboAttribute;
	BOOL	m_InvertLogic;
	int		m_LengthLower;
	int		m_LengthUpper;
	int		m_StrengthLower;
	int		m_StrengthUpper;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogReductPruner)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogReductPruner)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckInvertlogic();
	virtual void OnOK();
	afx_msg void OnCheckAttribute();
	afx_msg void OnCheckLength();
	afx_msg void OnCheckStrength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
