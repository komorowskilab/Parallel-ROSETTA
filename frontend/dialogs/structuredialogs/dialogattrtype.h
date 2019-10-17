// dialogattrtype.h : header file
//

#include <kernel/structures/attribute.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogAttributeType dialog

class CDialogAttributeType : public CDialog
{
// Construction
public:
	CDialogAttributeType(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAttributeType)
	enum { IDD = IDD_ATTRIBUTE_TYPE };
	CEdit	m_EditExponent;
	CButton	m_RadioString;
	CButton	m_RadioInteger;
	CButton	m_RadioFloat;
	int		m_Exponent;
	//}}AFX_DATA

	Attribute::Type m_Type;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAttributeType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAttributeType)
	afx_msg void OnRadioFloat();
	afx_msg void OnRadioInteger();
	afx_msg void OnRadioString();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
 
