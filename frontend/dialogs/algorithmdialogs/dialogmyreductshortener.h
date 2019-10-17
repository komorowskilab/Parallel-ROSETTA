// dialogmyreductshortener.h : header file
//

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogMyReductShortener dialog

class CDialogMyReductShortener : public CDialogAlgorithm
{
// Construction
public:
	CDialogMyReductShortener(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogMyReductShortener)
	enum { IDD = IDD_MYREDUCTSHORTENER };
	CButton	m_CheckPercentage;
	CButton	m_CheckAttributeIf;
	CButton	m_CheckAttribute;
	CComboBox	m_ComboAttributeSecondary;
	CEdit	m_EditUpperPercentage;
	CEdit	m_EditLowerPercentage;
	CButton	m_CheckInvert;
	CComboBox	m_ComboAttributePrimary;
	CComboBox	m_ComboAttribute;
	//}}AFX_DATA

	BOOL m_Masked; // Attribute indices relative to masked or unmasked table?

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMyReductShortener)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogMyReductShortener)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckAttribute();
	afx_msg void OnCheckAttributeIf();
	afx_msg void OnCheckPercentage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
