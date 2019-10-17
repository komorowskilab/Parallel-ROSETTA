// dialogapproximator.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <kernel\structures\reduct.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogApproximator dialog

class CDialogApproximator : public CDialogAlgorithm
{
// Construction
public:
	CDialogApproximator(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogApproximator)
	enum { IDD = IDD_APPROXIMATOR };
	CComboBox	m_ComboDecision;
	CButton	m_ButtonAttributes;
	float	m_Precision;
	//}}AFX_DATA

	BOOL      m_Masked;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogApproximator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogApproximator)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonPartition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
